// SLFAnimNotifyStateInputBuffer.cpp
#include "SLFAnimNotifyStateInputBuffer.h"
#include "Components/AC_InputBuffer.h"

FString USLFAnimNotifyStateInputBuffer::GetNotifyName_Implementation() const
{
	return TEXT("Input Buffer");
}

void USLFAnimNotifyStateInputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAC_InputBuffer* InputBuffer = Owner->FindComponentByClass<UAC_InputBuffer>();
	if (InputBuffer)
	{
		InputBuffer->ToggleBuffer(true);
		UE_LOG(LogTemp, Log, TEXT("[ANS_InputBuffer] Begin - Buffer opened"));
	}
}

void USLFAnimNotifyStateInputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAC_InputBuffer* InputBuffer = Owner->FindComponentByClass<UAC_InputBuffer>();
	if (InputBuffer)
	{
		InputBuffer->ToggleBuffer(false);
		UE_LOG(LogTemp, Log, TEXT("[ANS_InputBuffer] End - Buffer closed"));
	}
}
