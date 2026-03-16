// SLFAnimNotifyStateWeaponTrace.cpp
#include "SLFAnimNotifyStateWeaponTrace.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_StatusEffectManager.h"
#include "Components/StatusEffectManagerComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

FString USLFAnimNotifyStateWeaponTrace::GetNotifyName_Implementation() const
{
	return TEXT("Weapon Trace");
}

void USLFAnimNotifyStateWeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	// Clear hit actors list at start of trace
	HitActors.Empty();
	HitResetTimer = 0.0f;
	PreviousStartPos = FVector::ZeroVector;
	bHasPreviousPos = false;

	// Log socket availability and mode for debugging
	bool bHasStart = MeshComp->DoesSocketExist(StartSocketName);
	bool bHasEnd = MeshComp->DoesSocketExist(EndSocketName);
	bool bReachMode = (WeaponReach > 0.0f);
	UE_LOG(LogTemp, Warning, TEXT("[ANS_WeaponTrace] BEGIN on %s - Duration: %.2fs, Sockets: %s=%s %s=%s, Radius: %.0f, Mode: %s%s"),
		*Owner->GetName(), TotalDuration,
		*StartSocketName.ToString(), bHasStart ? TEXT("YES") : TEXT("NO"),
		*EndSocketName.ToString(), bHasEnd ? TEXT("YES") : TEXT("NO"),
		TraceRadius,
		bReachMode ? TEXT("Reach") : TEXT("TwoSocket"),
		bReachMode ? *FString::Printf(TEXT(" (%.0fcm, Axis=%d, Negate=%s)"),
			WeaponReach, (int32)WeaponDirectionAxis.GetValue(), bNegateDirection ? TEXT("Y") : TEXT("N")) : TEXT(""));

	// Spawn slash VFX at weapon socket
	UNiagaraSystem* VFXToUse = SlashVFX;
	if (!VFXToUse)
	{
		VFXToUse = LoadObject<UNiagaraSystem>(nullptr,
			TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_RibbonTrail.NS_RibbonTrail"));
	}
	if (VFXToUse && bHasStart)
	{
		ActiveSlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			VFXToUse,
			MeshComp,
			StartSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true // bAutoDestroy
		);
		if (ActiveSlashComponent)
		{
			ActiveSlashComponent->SetRelativeScale3D(FVector(SlashVFXScale));
			ActiveSlashComponent->SetColorParameter(FName(TEXT("Color")), SlashColor);
		}
	}
}

void USLFAnimNotifyStateWeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	// Periodically clear hit list for multi-hit attacks (whirlwinds, combos)
	if (HitResetInterval > 0.0f)
	{
		HitResetTimer += FrameDeltaTime;
		if (HitResetTimer >= HitResetInterval)
		{
			HitActors.Empty();
			HitResetTimer = 0.0f;
		}
	}

	// Get socket positions for trace
	FVector StartPos = FVector::ZeroVector;
	FVector EndPos = FVector::ZeroVector;

	// Resolve start position: try socket first, then bone names
	bool bHasStartSocket = MeshComp->DoesSocketExist(StartSocketName);
	bool bHasEndSocket = MeshComp->DoesSocketExist(EndSocketName);

	// Try to find a valid start bone if socket doesn't exist
	FName ResolvedStartBone = NAME_None;
	if (!bHasStartSocket)
	{
		// Check StartSocketName as a bone
		if (MeshComp->GetBoneIndex(StartSocketName) != INDEX_NONE)
		{
			ResolvedStartBone = StartSocketName;
		}
		else
		{
			TArray<FName> BoneNames = {
				FName("R_Hand"), FName("hand_r"), FName("weapon_r"),
				FName("Hand_R"), FName("RightHand"), FName("R_Sword")
			};
			for (const FName& BN : BoneNames)
			{
				if (MeshComp->GetBoneIndex(BN) != INDEX_NONE)
				{
					ResolvedStartBone = BN;
					break;
				}
			}
		}
	}

	bool bHasStart = bHasStartSocket || !ResolvedStartBone.IsNone();

	if (bHasStart && WeaponReach > 0.0f)
	{
		// Directional reach mode: trace from hand/weapon bone along weapon direction
		StartPos = bHasStartSocket
			? MeshComp->GetSocketLocation(StartSocketName)
			: MeshComp->GetBoneLocation(ResolvedStartBone);

		FVector Direction;
		bool bUsedBoneDirection = false;

		// Bone-to-bone direction: from DirectionBone THROUGH start point toward blade tip
		if (!DirectionBoneName.IsNone())
		{
			FVector DirBonePos = MeshComp->GetBoneLocation(DirectionBoneName);
			if (DirBonePos.IsNearlyZero())
			{
				// Try as socket
				if (MeshComp->DoesSocketExist(DirectionBoneName))
					DirBonePos = MeshComp->GetSocketLocation(DirectionBoneName);
			}
			FVector BoneToStart = StartPos - DirBonePos;
			if (BoneToStart.SizeSquared() > 1.0f)
			{
				Direction = BoneToStart.GetSafeNormal();
				bUsedBoneDirection = true;
			}
		}

		// Fallback: actor forward vector (always reliable)
		if (!bUsedBoneDirection)
		{
			Direction = Owner->GetActorForwardVector();
		}
		if (bNegateDirection) Direction = -Direction;

		EndPos = StartPos + Direction * WeaponReach;
	}
	else if (bHasStartSocket && bHasEndSocket)
	{
		// Two-socket mode
		StartPos = MeshComp->GetSocketLocation(StartSocketName);
		EndPos = MeshComp->GetSocketLocation(EndSocketName);
	}
	else if (bHasStart)
	{
		// Have a start bone but no reach/end — use actor forward
		StartPos = bHasStartSocket
			? MeshComp->GetSocketLocation(StartSocketName)
			: MeshComp->GetBoneLocation(ResolvedStartBone);
		FVector Forward = Owner->GetActorForwardVector();
		EndPos = StartPos + (Forward * 200.0f);
	}
	else
	{
		// Last resort: no bones or sockets found — trace IN FRONT of enemy
		FVector Forward = Owner->GetActorForwardVector();
		StartPos = Owner->GetActorLocation() + FVector(0, 0, 80.0f) + (Forward * 100.0f);
		EndPos = StartPos + (Forward * 200.0f);
	}

	// --- Swing-arc trace: also sweep between previous and current hand position ---
	// This catches horizontal slashes, overhead swings, and upcuts that the
	// forward reach trace would miss.
	if (bHasPreviousPos)
	{
		float SwingDist = FVector::Dist(PreviousStartPos, StartPos);
		if (SwingDist > 5.0f) // Hand moved enough to constitute a swing
		{
			// Interpolate the reach endpoint along the arc too
			FVector PrevEndPos = PreviousStartPos + (EndPos - StartPos);
			// We'll trace: PrevStart→Start (hand arc) and PrevEnd→End (weapon tip arc)
			// Combined with the forward trace below, this covers the full swept volume.
			// Use the midpoints for a single arc trace that covers the swing path:
			FVector ArcStart = PreviousStartPos;
			FVector ArcEnd = EndPos;  // Previous hand → current weapon tip = full swing diagonal
			StartPos = ArcStart;
			EndPos = ArcEnd;
		}
	}
	PreviousStartPos = bHasStart
		? (bHasStartSocket ? MeshComp->GetSocketLocation(StartSocketName) : MeshComp->GetBoneLocation(ResolvedStartBone))
		: StartPos;
	bHasPreviousPos = true;

	// Setup trace parameters - use SphereTraceMultiForObjects (by object type)
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	// Also ignore attached weapons (so we don't hit our own equipment)
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);
	for (AActor* Attached : AttachedActors)
	{
		ActorsToIgnore.Add(Attached);
	}

	// Trace against Pawns, WorldDynamic, and Destructible objects
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Destructible));

	EDrawDebugTrace::Type DebugDraw = bDrawDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;

	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
		Owner,
		StartPos,
		EndPos,
		TraceRadius,
		ObjectTypes,
		false, // bTraceComplex
		ActorsToIgnore,
		DebugDraw,
		HitResults,
		true // bIgnoreSelf
	);

	if (bHit)
	{
		// Get combat manager and equipment manager for damage calculation
		UAC_CombatManager* CombatManager = Owner->FindComponentByClass<UAC_CombatManager>();
		UAC_EquipmentManager* EquipmentManager = Owner->FindComponentByClass<UAC_EquipmentManager>();

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			// Skip if already hit this actor during this trace
			if (HitActors.Contains(HitActor)) continue;

			// Add to hit list
			HitActors.Add(HitActor);

			// Get weapon damage info from equipment manager or AI overrides
			double Damage = 50.0;  // Default damage (10% of typical 500 HP)
			double PoiseDamage = 25.0;  // Default poise damage
			TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffectsLegacy;  // For legacy API
			TMap<UPrimaryDataAsset*, FSLFStatusEffectApplication> WeaponStatusEffects;  // From weapon
			USoundBase* GuardSound = nullptr;
			USoundBase* PerfectGuardSound = nullptr;

			if (EquipmentManager)
			{
				// Get damage values from equipped weapon (player)
				Damage = EquipmentManager->GetWeaponDamage();
				PoiseDamage = EquipmentManager->GetWeaponPoiseDamage();
				WeaponStatusEffects = EquipmentManager->GetWeaponStatusEffects();
			}
			else
			{
				// AI attacker: use per-montage overrides if set, otherwise defaults
				if (OverrideDamage >= 0.0f) Damage = OverrideDamage;
				if (OverridePoiseDamage >= 0.0f) PoiseDamage = OverridePoiseDamage;

				// Get status effects from AI's DefaultAttackStatusEffects
				UAICombatManagerComponent* OwnerAICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
				if (OwnerAICombatManager)
				{
					WeaponStatusEffects = OwnerAICombatManager->DefaultAttackStatusEffects;
					UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] AI attacker - Damage=%.0f Poise=%.0f StatusEffects=%d"),
						Damage, PoiseDamage, WeaponStatusEffects.Num());
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("[ANS_WeaponTrace] HIT: %s -> %s | Dmg=%.0f Poise=%.0f at %s"),
				*Owner->GetName(), *HitActor->GetName(), Damage, PoiseDamage, *Hit.ImpactPoint.ToString());

			// Try player combat manager first (UAC_CombatManager)
			UAC_CombatManager* TargetCombatManager = HitActor->FindComponentByClass<UAC_CombatManager>();
			if (TargetCombatManager)
			{
				// Apply damage to player target
				TargetCombatManager->HandleIncomingWeaponDamage(
					Owner,
					GuardSound,
					PerfectGuardSound,
					Hit,
					Damage,
					PoiseDamage,
					StatusEffectsLegacy
				);
			}
			else
			{
				// Try AI combat manager (UAICombatManagerComponent - used by enemies)
				UAICombatManagerComponent* AICombatManager = HitActor->FindComponentByClass<UAICombatManagerComponent>();
				if (AICombatManager)
				{
					// Apply damage to AI target
					AICombatManager->HandleIncomingWeaponDamage_AI(Owner, Damage, PoiseDamage, Hit);
				}
			}

			// Apply weapon status effects to target
			// Find target's status effect manager and apply each effect with proper BuildupAmount
			if (WeaponStatusEffects.Num() > 0)
			{
				// Try legacy AC_StatusEffectManager first
				UAC_StatusEffectManager* TargetStatusManager = HitActor->FindComponentByClass<UAC_StatusEffectManager>();
				if (TargetStatusManager)
				{
					for (const auto& EffectPair : WeaponStatusEffects)
					{
						UPrimaryDataAsset* StatusEffectAsset = EffectPair.Key;
						const FSLFStatusEffectApplication& Application = EffectPair.Value;

						if (IsValid(StatusEffectAsset))
						{
							UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Applying status effect %s to %s: Rank=%d, BuildupAmount=%.1f"),
								*StatusEffectAsset->GetName(), *HitActor->GetName(),
								Application.Rank, Application.BuildupAmount);

							TargetStatusManager->AddOneShotBuildup(StatusEffectAsset, Application.Rank, Application.BuildupAmount);
						}
					}
				}
				else
				{
					// Try new StatusEffectManagerComponent
					UStatusEffectManagerComponent* TargetStatusComp = HitActor->FindComponentByClass<UStatusEffectManagerComponent>();
					if (TargetStatusComp)
					{
						for (const auto& EffectPair : WeaponStatusEffects)
						{
							UPrimaryDataAsset* StatusEffectAsset = EffectPair.Key;
							const FSLFStatusEffectApplication& Application = EffectPair.Value;

							if (IsValid(StatusEffectAsset))
							{
								UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Applying status effect %s to %s: Rank=%d, BuildupAmount=%.1f"),
									*StatusEffectAsset->GetName(), *HitActor->GetName(),
									Application.Rank, Application.BuildupAmount);

								// Cast to UDataAsset for the function signature
								TargetStatusComp->AddOneShotBuildup(Cast<UDataAsset>(StatusEffectAsset), Application.Rank, Application.BuildupAmount);
							}
						}
					}
				}
			}
		}
	}
}

void USLFAnimNotifyStateWeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	UE_LOG(LogTemp, Warning, TEXT("[ANS_WeaponTrace] END - Hit %d actors total"), HitActors.Num());

	// Deactivate slash VFX (auto-destroy handles cleanup)
	if (ActiveSlashComponent)
	{
		ActiveSlashComponent->Deactivate();
		ActiveSlashComponent = nullptr;
	}

	// Clear hit actors list
	HitActors.Empty();
}
