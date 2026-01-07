// SLFAnimNotifyStateInvincibility.cpp
#include "SLFAnimNotifyStateInvincibility.h"
#include "Components/AC_CombatManager.h"

FString USLFAnimNotifyStateInvincibility::GetNotifyName_Implementation() const
{
	return TEXT("Invincibility Frame");
}

void USLFAnimNotifyStateInvincibility::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAC_CombatManager* CombatManager = Owner->FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->SetInvincibility(true);
		UE_LOG(LogTemp, Log, TEXT("[ANS_Invincibility] Begin - I-frames active"));
	}
}

void USLFAnimNotifyStateInvincibility::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UAC_CombatManager* CombatManager = Owner->FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->SetInvincibility(false);
		UE_LOG(LogTemp, Log, TEXT("[ANS_Invincibility] End - I-frames removed"));
	}
}
