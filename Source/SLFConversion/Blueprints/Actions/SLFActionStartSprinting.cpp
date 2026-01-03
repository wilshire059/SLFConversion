// SLFActionStartSprinting.cpp
#include "SLFActionStartSprinting.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USLFActionStartSprinting::USLFActionStartSprinting()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Initialized"));
}

void USLFActionStartSprinting::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] ExecuteAction"));

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			// TODO: Set sprint speed from SpeedAsset
			Movement->MaxWalkSpeed = 600.0f;  // Sprint speed
			UE_LOG(LogTemp, Log, TEXT("[ActionStartSprinting] Sprint speed set to %.0f"), Movement->MaxWalkSpeed);
		}
	}
}
