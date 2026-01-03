// SLFAnimNotifyStateInputBuffer.cpp
#include "SLFAnimNotifyStateInputBuffer.h"

FString USLFAnimNotifyStateInputBuffer::GetNotifyName_Implementation() const
{
	return TEXT("Input Buffer");
}

void USLFAnimNotifyStateInputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	UE_LOG(LogTemp, Log, TEXT("[ANS_InputBuffer] Begin - Opening buffer"));

	// TODO: Get InputBufferComponent->ToggleBuffer(true)
}

void USLFAnimNotifyStateInputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	UE_LOG(LogTemp, Log, TEXT("[ANS_InputBuffer] End - Closing buffer"));

	// TODO: Get InputBufferComponent->ToggleBuffer(false)
}
