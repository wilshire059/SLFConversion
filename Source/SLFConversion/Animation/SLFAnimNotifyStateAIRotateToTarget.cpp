// SLFAnimNotifyStateAIRotateToTarget.cpp
#include "SLFAnimNotifyStateAIRotateToTarget.h"
#include "AIController.h"
#include "GameFramework/Character.h"

FString USLFAnimNotifyStateAIRotateToTarget::GetNotifyName_Implementation() const
{
	return TEXT("AI Rotate To Target");
}

void USLFAnimNotifyStateAIRotateToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character) return;

	AAIController* AIController = Cast<AAIController>(Character->GetController());
	if (!AIController) return;

	// TODO: Get focus actor from blackboard, lerp rotation toward it
}
