// SLFAnimNotifyStateWeaponTrace.cpp
#include "SLFAnimNotifyStateWeaponTrace.h"
#include "Components/AC_CombatManager.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/AC_EquipmentManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

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

	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Begin on %s - Duration: %.2fs"), *Owner->GetName(), TotalDuration);
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

	// Check if sockets exist, otherwise use fallback
	bool bHasStartSocket = MeshComp->DoesSocketExist(StartSocketName);
	bool bHasEndSocket = MeshComp->DoesSocketExist(EndSocketName);

	if (bHasStartSocket && bHasEndSocket)
	{
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
			// Use found socket as start, extend forward for end
			StartPos = MeshComp->GetSocketLocation(FallbackSocket);
			FVector Forward = Owner->GetActorForwardVector();
			EndPos = StartPos + (Forward * 150.0f); // 150 units forward (weapon reach)
		}
		else
		{
			// Last resort: trace from character center forward
			StartPos = Owner->GetActorLocation() + FVector(0, 0, 80.0f); // Chest height
			FVector Forward = Owner->GetActorForwardVector();
			EndPos = StartPos + (Forward * 150.0f);
		}
	}

	// Setup trace parameters
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = true;

	// Sphere trace between weapon sockets
	bool bHit = World->SweepMultiByChannel(
		HitResults,
		StartPos,
		EndPos,
		FQuat::Identity,
		ECC_Pawn,  // Trace against pawns
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
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

			// Get weapon damage info from equipment manager
			double Damage = 50.0;  // Default damage (10% of typical 500 HP)
			double PoiseDamage = 25.0;  // Default poise damage
			TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;
			USoundBase* GuardSound = nullptr;
			USoundBase* PerfectGuardSound = nullptr;

			if (EquipmentManager)
			{
				// Get damage values from equipped weapon
				Damage = EquipmentManager->GetWeaponDamage();
				PoiseDamage = EquipmentManager->GetWeaponPoiseDamage();
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
					StatusEffects
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
