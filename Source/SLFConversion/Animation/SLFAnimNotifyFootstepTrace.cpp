// SLFAnimNotifyFootstepTrace.cpp
#include "SLFAnimNotifyFootstepTrace.h"

FString USLFAnimNotifyFootstepTrace::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Footstep: %s"), *FootBoneName.ToString());
}

void USLFAnimNotifyFootstepTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	FVector FootLocation = MeshComp->GetSocketLocation(FootBoneName);

	UE_LOG(LogTemp, Log, TEXT("[AN_FootstepTrace] Footstep at %s"), *FootLocation.ToString());

	// TODO: Trace down, get physical material, play appropriate sound/effect
}
