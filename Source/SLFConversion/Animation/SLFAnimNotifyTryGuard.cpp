// SLFAnimNotifyTryGuard.cpp
#include "SLFAnimNotifyTryGuard.h"
#include "Components/InputBufferComponent.h"
#include "GameplayTagsManager.h"

FString USLFAnimNotifyTryGuard::GetNotifyName_Implementation() const
{
	return TEXT("Try Guard");
}

void USLFAnimNotifyTryGuard::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_TryGuard] Notify on %s"), *Owner->GetName());

	// Get InputBufferComponent and queue guard action
	if (UInputBufferComponent* InputBuffer = Owner->FindComponentByClass<UInputBufferComponent>())
	{
		// Use Guard action tag
		FGameplayTag GuardTag = FGameplayTag::RequestGameplayTag(FName("Action.Guard"), false);
		if (GuardTag.IsValid())
		{
			InputBuffer->QueueAction(GuardTag);
		}
		else
		{
			// Try alternate tag format
			GuardTag = FGameplayTag::RequestGameplayTag(FName("Action.GuardStart"), false);
			if (GuardTag.IsValid())
			{
				InputBuffer->QueueAction(GuardTag);
			}
		}
	}
}
