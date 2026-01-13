// ANS_WeaponTrace.cpp
// C++ Animation Notify implementation for ANS_WeaponTrace
//
// 20-PASS VALIDATION: 2026-01-12 - Uses CollisionManager on attached weapons
// Logic: Enable CollisionManager tracing on equipped weapons

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
	int32 WeaponsEnabled = 0;

	// Get attached actors (weapons) and enable their collision traces
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
			WeaponsEnabled++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] Begin on %s - Duration: %.2fs, DamageMultiplier: %.2f, Weapons: %d"),
		*Owner->GetName(), TotalDuration, DamageMultiplier, WeaponsEnabled);
}

void UANS_WeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// CollisionManager handles tracing via its own tick
}

void UANS_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	int32 WeaponsDisabled = 0;

	// Get attached actors and disable their collision traces
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
			WeaponsDisabled++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[ANS_WeaponTrace] End on %s - Weapons disabled: %d"),
		*Owner->GetName(), WeaponsDisabled);
}

FString UANS_WeaponTrace::GetNotifyName_Implementation() const
{
	return TEXT("Weapon Trace");
}
