// AN_TryGuard.cpp
// C++ Animation Notify implementation for AN_TryGuard
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - queues guard action on input buffer if player wants to guard

#include "AnimNotifies/AN_TryGuard.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/InputBufferComponent.h"

UAN_TryGuard::UAN_TryGuard()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 150, 50, 255); // Orange for guard check
#endif
}

void UAN_TryGuard::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get CombatManager component and check WantsToGuard?
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (!CombatManager)
	{
		UE_LOG(LogTemp, Log, TEXT("UAN_TryGuard::Notify - No CombatManager on %s"), *Owner->GetName());
		return;
	}

	// Check if player wants to guard (is holding guard key)
	if (!CombatManager->bWantsToGuard)
	{
		// Player doesn't want to guard - nothing to do
		return;
	}

	// From Blueprint: Get InputBuffer component and queue guard action
	UInputBufferComponent* InputBuffer = Owner->FindComponentByClass<UInputBufferComponent>();
	if (!InputBuffer)
	{
		UE_LOG(LogTemp, Log, TEXT("UAN_TryGuard::Notify - No InputBuffer on %s"), *Owner->GetName());
		return;
	}

	// Queue guard action with tag SoulslikeFramework.Action.GuardStart
	FGameplayTag GuardTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.GuardStart"), false);
	if (GuardTag.IsValid())
	{
		InputBuffer->QueueAction(GuardTag);
		UE_LOG(LogTemp, Log, TEXT("UAN_TryGuard::Notify - Queued guard action on %s"), *Owner->GetName());
	}
}

FString UAN_TryGuard::GetNotifyName_Implementation() const
{
	return TEXT("Check for Guard");
}
