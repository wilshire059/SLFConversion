// SLFActionJump.cpp
#include "SLFActionJump.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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

			UE_LOG(LogTemp, Log, TEXT("[ActionJump] Character jumping (Vel: %.0f, %.0f, %.0f)"),
				Character->GetVelocity().X, Character->GetVelocity().Y, Character->GetVelocity().Z);
		}
	}
}
