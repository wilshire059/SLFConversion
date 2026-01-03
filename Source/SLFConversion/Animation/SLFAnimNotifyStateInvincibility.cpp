// SLFAnimNotifyStateInvincibility.cpp
#include "SLFAnimNotifyStateInvincibility.h"

FString USLFAnimNotifyStateInvincibility::GetNotifyName_Implementation() const
{
	return TEXT("Invincibility Frame");
}

void USLFAnimNotifyStateInvincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_Invincibility] Begin - I-frames active"));
	// TODO: Set CombatManager->bIsInvincible = true
}

void USLFAnimNotifyStateInvincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_Invincibility] End - I-frames removed"));
	// TODO: Set CombatManager->bIsInvincible = false
}
