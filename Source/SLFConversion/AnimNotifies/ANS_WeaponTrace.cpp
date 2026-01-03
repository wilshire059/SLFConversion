// ANS_WeaponTrace.cpp
// C++ Animation Notify implementation for ANS_WeaponTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables weapon collision tracing

#include "AnimNotifies/ANS_WeaponTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CollisionManagerComponent.h"

UANS_WeaponTrace::UANS_WeaponTrace()
	: TraceType(ESLFTraceType::RightHand)
	, DamageMultiplier(1.0)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 0, 0, 255); // Red for weapon trace
#endif
}

void UANS_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors (weapons) and enable their collision traces
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		// Get CollisionManager component on attached weapon
		UCollisionManagerComponent* CollisionManager = AttachedActor->FindComponentByClass<UCollisionManagerComponent>();
		if (CollisionManager)
		{
			// Enable weapon trace and set damage multiplier
			CollisionManager->SetMultipliers(DamageMultiplier, 1.0);
			CollisionManager->ToggleTrace(true);
			UE_LOG(LogTemp, Log, TEXT("UANS_WeaponTrace::NotifyBegin - Enabled trace on %s (Damage: %.2f)"),
				*AttachedActor->GetName(), DamageMultiplier);
		}
	}
}

void UANS_WeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic in Blueprint - collision manager handles its own tick
}

void UANS_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors and disable their collision traces
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		// Get CollisionManager component on attached weapon
		UCollisionManagerComponent* CollisionManager = AttachedActor->FindComponentByClass<UCollisionManagerComponent>();
		if (CollisionManager)
		{
			// Disable weapon trace
			CollisionManager->ToggleTrace(false);
			UE_LOG(LogTemp, Log, TEXT("UANS_WeaponTrace::NotifyEnd - Disabled trace on %s"), *AttachedActor->GetName());
		}
	}
}

FString UANS_WeaponTrace::GetNotifyName_Implementation() const
{
	return TEXT("Weapon Trace");
}
