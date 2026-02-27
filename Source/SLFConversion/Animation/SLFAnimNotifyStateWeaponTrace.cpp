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

	// Log socket availability and mode for debugging
	bool bHasStart = MeshComp->DoesSocketExist(StartSocketName);
	bool bHasEnd = MeshComp->DoesSocketExist(EndSocketName);
	bool bReachMode = (WeaponReach > 0.0f);
	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Begin on %s - Duration: %.2fs, Sockets: %s=%s %s=%s, Radius: %.0f, Mode: %s%s"),
		*Owner->GetName(), TotalDuration,
		*StartSocketName.ToString(), bHasStart ? TEXT("YES") : TEXT("NO"),
		*EndSocketName.ToString(), bHasEnd ? TEXT("YES") : TEXT("NO"),
		TraceRadius,
		bReachMode ? TEXT("Reach") : TEXT("TwoSocket"),
		bReachMode ? *FString::Printf(TEXT(" (%.0fcm, Axis=%d, Negate=%s)"),
			WeaponReach, (int32)WeaponDirectionAxis.GetValue(), bNegateDirection ? TEXT("Y") : TEXT("N")) : TEXT(""));
}

void USLFAnimNotifyStateWeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	// Get socket positions for trace
	FVector StartPos = FVector::ZeroVector;
	FVector EndPos = FVector::ZeroVector;

	bool bHasStartSocket = MeshComp->DoesSocketExist(StartSocketName);
	bool bHasEndSocket = MeshComp->DoesSocketExist(EndSocketName);

	if (WeaponReach > 0.0f && bHasStartSocket)
	{
		// Directional reach mode: trace from socket origin along weapon direction.
		FTransform SocketTransform = MeshComp->GetSocketTransform(StartSocketName);
		StartPos = SocketTransform.GetLocation();

		FVector Direction;
		bool bUsedBoneDirection = false;

		// Bone-to-bone direction: compute from DirectionBone THROUGH StartSocket
		// e.g., lowerarm_r -> hand_r = "from elbow through hand toward blade tip"
		if (!DirectionBoneName.IsNone())
		{
			FVector DirBonePos = MeshComp->GetBoneLocation(DirectionBoneName);
			FVector BoneToSocket = StartPos - DirBonePos;
			if (BoneToSocket.SizeSquared() > 1.0f)
			{
				Direction = BoneToSocket.GetSafeNormal();
				bUsedBoneDirection = true;
			}
		}

		// Fallback: socket local axis
		if (!bUsedBoneDirection)
		{
			switch (WeaponDirectionAxis)
			{
				case EAxis::X: Direction = SocketTransform.GetUnitAxis(EAxis::X); break;
				case EAxis::Y: Direction = SocketTransform.GetUnitAxis(EAxis::Y); break;
				case EAxis::Z: Direction = SocketTransform.GetUnitAxis(EAxis::Z); break;
				default:       Direction = SocketTransform.GetUnitAxis(EAxis::X); break;
			}
		}
		if (bNegateDirection) Direction = -Direction;

		EndPos = StartPos + Direction * WeaponReach;
	}
	else if (bHasStartSocket && bHasEndSocket)
	{
		// Two-socket mode: trace directly between two sockets
		StartPos = MeshComp->GetSocketLocation(StartSocketName);
		EndPos = MeshComp->GetSocketLocation(EndSocketName);
	}
	else
	{
		// Fallback: Try common socket names (hand_r, weapon_r)
		FName FallbackSocket = NAME_None;
		TArray<FName> CommonSockets = { FName("hand_r"), FName("weapon_r"), FName("RightHand"), FName("Hand_R") };
		for (const FName& SocketName : CommonSockets)
		{
			if (MeshComp->DoesSocketExist(SocketName))
			{
				FallbackSocket = SocketName;
				break;
			}
		}

		if (!FallbackSocket.IsNone())
		{
			StartPos = MeshComp->GetSocketLocation(FallbackSocket);
			FVector Forward = Owner->GetActorForwardVector();
			EndPos = StartPos + (Forward * 150.0f);
		}
		else
		{
			StartPos = Owner->GetActorLocation() + FVector(0, 0, 80.0f);
			FVector Forward = Owner->GetActorForwardVector();
			EndPos = StartPos + (Forward * 150.0f);
		}
	}

	// Setup trace parameters - use SphereTraceMultiForObjects (by object type)
	// instead of SweepMultiByChannel which depends on collision channel responses
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

			UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Hit: %s at %s"),
				*HitActor->GetName(), *Hit.ImpactPoint.ToString());

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

	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] End - Hit %d actors"), HitActors.Num());

	// Clear hit actors list
	HitActors.Empty();
}
