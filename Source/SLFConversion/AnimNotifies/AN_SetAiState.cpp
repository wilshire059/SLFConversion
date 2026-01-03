// AN_SetAiState.cpp
// C++ Animation Notify implementation for AN_SetAiState
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - sets AI state via AI_BehaviorManager component

#include "AnimNotifies/AN_SetAiState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AC_AI_BehaviorManager.h"

UAN_SetAiState::UAN_SetAiState()
	: State(ESLFAIStates::Idle)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(100, 200, 255, 255); // Light blue for AI state
#endif
}

void UAN_SetAiState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get AC_AI_BehaviorManager component
	UAC_AI_BehaviorManager* BehaviorManager = Owner->FindComponentByClass<UAC_AI_BehaviorManager>();
	if (!BehaviorManager)
	{
		UE_LOG(LogTemp, Log, TEXT("UAN_SetAiState::Notify - No AI_BehaviorManager on %s"), *Owner->GetName());
		return;
	}

	// From Blueprint: Call SetState with NewState and Data
	BehaviorManager->SetState(State, Data);
	UE_LOG(LogTemp, Log, TEXT("UAN_SetAiState::Notify - Set AI state %d on %s"), (int32)State, *Owner->GetName());
}

FString UAN_SetAiState::GetNotifyName_Implementation() const
{
	return TEXT("Switch AI State");
}
