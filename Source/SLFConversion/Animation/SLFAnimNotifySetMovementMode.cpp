// SLFAnimNotifySetMovementMode.cpp
#include "SLFAnimNotifySetMovementMode.h"
#include "GameFramework/Character.h"

FString USLFAnimNotifySetMovementMode::GetNotifyName_Implementation() const
{
	return TEXT("Set Movement Mode");
}

void USLFAnimNotifySetMovementMode::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->SetMovementMode(NewMovementMode);
			UE_LOG(LogTemp, Log, TEXT("[AN_SetMovementMode] Set to %d"), (int32)NewMovementMode);
		}
	}
}
