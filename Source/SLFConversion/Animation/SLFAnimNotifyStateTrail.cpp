// SLFAnimNotifyStateTrail.cpp
#include "SLFAnimNotifyStateTrail.h"

FString USLFAnimNotifyStateTrail::GetNotifyName_Implementation() const
{
	return TEXT("Trail Effect");
}

void USLFAnimNotifyStateTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_Trail] Begin - Activating weapon trail"));
	// TODO: Get equipped weapon, activate TrailEffect niagara system
}

void USLFAnimNotifyStateTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_Trail] End - Deactivating weapon trail"));
	// TODO: Deactivate trail effect
}
