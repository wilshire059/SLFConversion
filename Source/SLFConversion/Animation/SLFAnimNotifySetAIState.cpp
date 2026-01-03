// SLFAnimNotifySetAIState.cpp
#include "SLFAnimNotifySetAIState.h"

FString USLFAnimNotifySetAIState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Set AI State: %s"), *NewState.ToString());
}

void USLFAnimNotifySetAIState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_SetAIState] Setting state to %s"), *NewState.ToString());
	// TODO: Get AI_BehaviorManager->SetState(NewState)
}
