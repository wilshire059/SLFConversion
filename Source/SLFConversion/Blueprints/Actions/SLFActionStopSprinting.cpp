// SLFActionStopSprinting.cpp
#include "SLFActionStopSprinting.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USLFActionStopSprinting::USLFActionStopSprinting()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] Initialized"));
}

void USLFActionStopSprinting::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] ExecuteAction"));

	if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = 400.0f;  // Normal walk speed
			UE_LOG(LogTemp, Log, TEXT("[ActionStopSprinting] Walk speed set to %.0f"), Movement->MaxWalkSpeed);
		}
	}
}
