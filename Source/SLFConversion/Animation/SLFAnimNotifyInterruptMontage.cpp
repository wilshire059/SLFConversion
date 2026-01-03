// SLFAnimNotifyInterruptMontage.cpp
#include "SLFAnimNotifyInterruptMontage.h"
#include "GameFramework/Character.h"

FString USLFAnimNotifyInterruptMontage::GetNotifyName_Implementation() const
{
	return TEXT("Interrupt Montage");
}

void USLFAnimNotifyInterruptMontage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		Character->StopAnimMontage();
		UE_LOG(LogTemp, Log, TEXT("[AN_InterruptMontage] Stopping montage with blend %.2fs"), BlendOutTime);
	}
}
