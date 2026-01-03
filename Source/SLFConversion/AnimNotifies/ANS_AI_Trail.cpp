// ANS_AI_Trail.cpp
// C++ Animation Notify implementation for ANS_AI_Trail
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - activates trail effects on AI weapons during attacks

#include "AnimNotifies/ANS_AI_Trail.h"
#include "Components/SkeletalMeshComponent.h"

UANS_AI_Trail::UANS_AI_Trail()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(100, 200, 255, 255); // Light blue for trail
#endif
}

void UANS_AI_Trail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors and find child components with "Trail" tag
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor || !AttachedActor->GetRootComponent())
		{
			continue;
		}

		// Check if attached at one of our weapon sockets
		TArray<USceneComponent*> ChildComponents;
		AttachedActor->GetRootComponent()->GetChildrenComponents(true, ChildComponents);

		for (USceneComponent* Child : ChildComponents)
		{
			FName SocketName = Child->GetAttachSocketName();
			if (WeaponSockets.Contains(SocketName) || Child->ComponentHasTag(FName("Trail")))
			{
				// Activate trail components with "Trail" tag
				if (Child->ComponentHasTag(FName("Trail")))
				{
					Child->SetActive(true);
					UE_LOG(LogTemp, Log, TEXT("UANS_AI_Trail::NotifyBegin - Trail ON for %s on %s"),
						*Child->GetName(), *Owner->GetName());
				}
			}
		}
	}
}

void UANS_AI_Trail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic - trail stays active during notify duration
}

void UANS_AI_Trail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Deactivate trail components
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor || !AttachedActor->GetRootComponent())
		{
			continue;
		}

		TArray<USceneComponent*> ChildComponents;
		AttachedActor->GetRootComponent()->GetChildrenComponents(true, ChildComponents);

		for (USceneComponent* Child : ChildComponents)
		{
			FName SocketName = Child->GetAttachSocketName();
			if (WeaponSockets.Contains(SocketName) || Child->ComponentHasTag(FName("Trail")))
			{
				if (Child->ComponentHasTag(FName("Trail")))
				{
					Child->SetActive(false);
					UE_LOG(LogTemp, Log, TEXT("UANS_AI_Trail::NotifyEnd - Trail OFF for %s on %s"),
						*Child->GetName(), *Owner->GetName());
				}
			}
		}
	}
}

FString UANS_AI_Trail::GetNotifyName_Implementation() const
{
	return TEXT("AI Trail");
}
