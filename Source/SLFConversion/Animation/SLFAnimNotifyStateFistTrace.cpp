// SLFAnimNotifyStateFistTrace.cpp
#include "SLFAnimNotifyStateFistTrace.h"

FString USLFAnimNotifyStateFistTrace::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Fist Trace: %s"), *FistSocketName.ToString());
}

void USLFAnimNotifyStateFistTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UE_LOG(LogTemp, Log, TEXT("[ANS_FistTrace] Begin"));
}

void USLFAnimNotifyStateFistTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	FVector FistLocation = MeshComp->GetSocketLocation(FistSocketName);
	// TODO: Sphere trace at fist location, process hits
}

void USLFAnimNotifyStateFistTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UE_LOG(LogTemp, Log, TEXT("[ANS_FistTrace] End"));
}
