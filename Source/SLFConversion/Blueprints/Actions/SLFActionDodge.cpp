// SLFActionDodge.cpp
// C++ implementation for B_Action_Dodge

#include "SLFActionDodge.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFPrimaryDataAssets.h"

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

	// Get DodgeMontages directly from C++ property (no reflection, no FInstancedStruct)
	if (Action)
	{
		if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
		{
			// Direct property access - montages migrated from Blueprint FInstancedStruct to C++ property
			DodgeMontages = ActionData->DodgeMontages;
			UE_LOG(LogTemp, Log, TEXT("[ActionDodge] DodgeMontages: Forward=%s"),
				DodgeMontages.Forward ? *DodgeMontages.Forward->GetName() : TEXT("NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ActionDodge] Cast<UPDA_ActionBase> FAILED!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] Action is null"));
	}

	UAnimMontage* MontageToPlay = GetDirectionalDodgeMontage();
	if (MontageToPlay)
	{
		Character->PlayAnimMontage(MontageToPlay);
		UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Playing montage: %s"), *MontageToPlay->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionDodge] No montage found for direction"));
	}
}

UAnimMontage* USLFActionDodge::GetDirectionalDodgeMontage()
{
	if (!OwnerActor) return DodgeMontages.Forward;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return DodgeMontages.Forward;

	// Get movement input direction
	FVector InputVector = Character->GetCharacterMovement()->GetLastInputVector();

	if (InputVector.IsNearlyZero())
	{
		return DodgeMontages.Backstep;  // Default to backstep
	}

	// Transform to local space
	FVector LocalInput = Character->GetActorRotation().UnrotateVector(InputVector);
	LocalInput.Normalize();

	// Determine direction
	if (FMath::Abs(LocalInput.X) > FMath::Abs(LocalInput.Y))
	{
		return LocalInput.X > 0 ? DodgeMontages.Forward : DodgeMontages.Backward;
	}
	else
	{
		return LocalInput.Y > 0 ? DodgeMontages.Right : DodgeMontages.Left;
	}
}
