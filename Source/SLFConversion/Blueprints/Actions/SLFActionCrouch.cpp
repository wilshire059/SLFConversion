// SLFActionCrouch.cpp
#include "SLFActionCrouch.h"
#include "GameFramework/Character.h"

USLFActionCrouch::USLFActionCrouch()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionCrouch] Initialized"));
}

void USLFActionCrouch::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionCrouch] ExecuteAction"));

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
			UE_LOG(LogTemp, Log, TEXT("[ActionCrouch] Uncrouching"));
		}
		else
		{
			Character->Crouch();
			UE_LOG(LogTemp, Log, TEXT("[ActionCrouch] Crouching"));
		}
	}
}
