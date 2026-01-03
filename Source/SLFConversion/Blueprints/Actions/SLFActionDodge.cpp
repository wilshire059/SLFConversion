// SLFActionDodge.cpp
// C++ implementation for B_Action_Dodge
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - B_Action_Dodge
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         1/1 (DodgeMontages)
// Functions:         1/1 (ExecuteAction)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "SLFActionDodge.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USLFActionDodge::USLFActionDodge()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Initialized"));
}

void USLFActionDodge::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] ExecuteAction"));

	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	UAnimMontage* MontageToPlay = GetDirectionalDodgeMontage();
	if (MontageToPlay)
	{
		Character->PlayAnimMontage(MontageToPlay);
		UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Playing montage: %s"), *MontageToPlay->GetName());
	}
}

UAnimMontage* USLFActionDodge::GetDirectionalDodgeMontage()
{
	if (!OwnerActor) return DodgeMontages.Fwd;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return DodgeMontages.Fwd;

	// Get movement input direction
	FVector InputVector = Character->GetCharacterMovement()->GetLastInputVector();

	if (InputVector.IsNearlyZero())
	{
		return DodgeMontages.Bwd;  // Default to backstep
	}

	// Transform to local space
	FVector LocalInput = Character->GetActorRotation().UnrotateVector(InputVector);
	LocalInput.Normalize();

	// Determine direction
	if (FMath::Abs(LocalInput.X) > FMath::Abs(LocalInput.Y))
	{
		return LocalInput.X > 0 ? DodgeMontages.Fwd : DodgeMontages.Bwd;
	}
	else
	{
		return LocalInput.Y > 0 ? DodgeMontages.Right : DodgeMontages.Left;
	}
}
