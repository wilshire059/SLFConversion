// ANS_AI_WeaponTrace.cpp
// C++ Animation Notify implementation for ANS_AI_WeaponTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables AI weapon collision tracing

#include "AnimNotifies/ANS_AI_WeaponTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CollisionManagerComponent.h"

UANS_AI_WeaponTrace::UANS_AI_WeaponTrace()
	: DamageMultiplier(1.0)
	, TraceSizeMultiplier(1.0)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 50, 50, 255); // Red for weapon trace
#endif
}

void UANS_AI_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors (weapons) and enable trace on their CollisionManagers
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		// Check if this actor has children components at one of our traced sockets
		TArray<USceneComponent*> ChildComponents;
		AttachedActor->GetRootComponent()->GetChildrenComponents(false, ChildComponents);

		for (USceneComponent* Child : ChildComponents)
		{
			FName SocketName = Child->GetAttachSocketName();
			if (TracedWeaponSockets.Contains(SocketName))
			{
				// Find CollisionManager on this attached actor
				UCollisionManagerComponent* CollisionManager = AttachedActor->FindComponentByClass<UCollisionManagerComponent>();
				if (CollisionManager)
				{
					CollisionManager->SetMultipliers(DamageMultiplier, TraceSizeMultiplier);
					CollisionManager->ToggleTrace(true);
					UE_LOG(LogTemp, Log, TEXT("UANS_AI_WeaponTrace::NotifyBegin - Trace ON for %s on %s"),
						*AttachedActor->GetName(), *Owner->GetName());
				}
				break;
			}
		}
	}
}

void UANS_AI_WeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// Tick handled by component
}

void UANS_AI_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors and disable trace on their CollisionManagers
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		TArray<USceneComponent*> ChildComponents;
		AttachedActor->GetRootComponent()->GetChildrenComponents(false, ChildComponents);

		for (USceneComponent* Child : ChildComponents)
		{
			FName SocketName = Child->GetAttachSocketName();
			if (TracedWeaponSockets.Contains(SocketName))
			{
				UCollisionManagerComponent* CollisionManager = AttachedActor->FindComponentByClass<UCollisionManagerComponent>();
				if (CollisionManager)
				{
					CollisionManager->ToggleTrace(false);
					UE_LOG(LogTemp, Log, TEXT("UANS_AI_WeaponTrace::NotifyEnd - Trace OFF for %s on %s"),
						*AttachedActor->GetName(), *Owner->GetName());
				}
				break;
			}
		}
	}
}

FString UANS_AI_WeaponTrace::GetNotifyName_Implementation() const
{
	return TEXT("AI Weapon Trace");
}
