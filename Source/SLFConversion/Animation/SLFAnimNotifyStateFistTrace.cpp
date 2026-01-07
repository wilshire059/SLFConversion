// SLFAnimNotifyStateFistTrace.cpp
#include "SLFAnimNotifyStateFistTrace.h"
#include "Components/AC_CombatManager.h"
#include "Engine/World.h"

FString USLFAnimNotifyStateFistTrace::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Fist Trace: %s"), *FistSocketName.ToString());
}

void USLFAnimNotifyStateFistTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// Clear hit actors list at start of trace
	HitActors.Empty();

	if (MeshComp && MeshComp->GetOwner())
	{
		UE_LOG(LogTemp, Log, TEXT("[ANS_FistTrace] Begin on %s - Duration: %.2fs"), *MeshComp->GetOwner()->GetName(), TotalDuration);
	}
}

void USLFAnimNotifyStateFistTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UWorld* World = Owner->GetWorld();
	if (!World) return;

	// Get fist socket location
	FVector FistLocation = MeshComp->GetSocketLocation(FistSocketName);

	// Setup trace parameters
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = true;

	// Sphere overlap at fist location
	bool bHit = World->SweepMultiByChannel(
		HitResults,
		FistLocation,
		FistLocation,  // Same location - just an overlap check
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		QueryParams
	);

	if (bHit)
	{
		// Get combat manager for damage calculation
		UAC_CombatManager* CombatManager = Owner->FindComponentByClass<UAC_CombatManager>();

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			// Skip if already hit this actor during this trace
			if (HitActors.Contains(HitActor)) continue;

			// Add to hit list
			HitActors.Add(HitActor);

			UE_LOG(LogTemp, Log, TEXT("[ANS_FistTrace] Hit: %s at %s"),
				*HitActor->GetName(), *Hit.ImpactPoint.ToString());

			// Get the target's combat manager to apply damage
			UAC_CombatManager* TargetCombatManager = HitActor->FindComponentByClass<UAC_CombatManager>();
			if (TargetCombatManager)
			{
				// Unarmed base damage values
				double Damage = 5.0;
				double PoiseDamage = 3.0;
				TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;
				USoundBase* GuardSound = nullptr;
				USoundBase* PerfectGuardSound = nullptr;

				// Apply damage to target
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
		}
	}
}

void USLFAnimNotifyStateFistTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_FistTrace] End - Hit %d actors"), HitActors.Num());

	// Clear hit actors list
	HitActors.Empty();
}
