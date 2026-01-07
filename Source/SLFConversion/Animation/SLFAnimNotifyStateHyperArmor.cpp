// SLFAnimNotifyStateHyperArmor.cpp
#include "SLFAnimNotifyStateHyperArmor.h"
#include "Components/AC_CombatManager.h"

FString USLFAnimNotifyStateHyperArmor::GetNotifyName_Implementation() const
{
	return TEXT("Hyper Armor");
}

void USLFAnimNotifyStateHyperArmor::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAC_CombatManager* CombatManager = Owner->FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->SetHyperArmor(true);
		UE_LOG(LogTemp, Log, TEXT("[ANS_HyperArmor] Begin - Stagger immunity active"));
	}
}

void USLFAnimNotifyStateHyperArmor::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAC_CombatManager* CombatManager = Owner->FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->SetHyperArmor(false);
		UE_LOG(LogTemp, Log, TEXT("[ANS_HyperArmor] End - Stagger immunity removed"));
	}
}
