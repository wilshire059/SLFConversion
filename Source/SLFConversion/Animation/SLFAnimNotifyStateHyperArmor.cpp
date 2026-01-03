// SLFAnimNotifyStateHyperArmor.cpp
#include "SLFAnimNotifyStateHyperArmor.h"

FString USLFAnimNotifyStateHyperArmor::GetNotifyName_Implementation() const
{
	return TEXT("Hyper Armor");
}

void USLFAnimNotifyStateHyperArmor::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_HyperArmor] Begin - Stagger immunity active"));
	// TODO: Set CombatManager->bHasHyperArmor = true
}

void USLFAnimNotifyStateHyperArmor::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Log, TEXT("[ANS_HyperArmor] End - Stagger immunity removed"));
	// TODO: Set CombatManager->bHasHyperArmor = false
}
