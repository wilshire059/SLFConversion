// SLFActionJump.cpp
#include "SLFActionJump.h"
#include "GameFramework/Character.h"

USLFActionJump::USLFActionJump()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJump] Initialized"));
}

void USLFActionJump::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionJump] ExecuteAction"));

	if (!OwnerActor) return;

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		if (Character->CanJump())
		{
			Character->Jump();
			UE_LOG(LogTemp, Log, TEXT("[ActionJump] Character jumping"));
		}
	}
}
