// B_Action_Dodge.cpp
// C++ implementation for Blueprint B_Action_Dodge
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action_Dodge.json

#include "Blueprints/B_Action_Dodge.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFPrimaryDataAssets.h"

UB_Action_Dodge::UB_Action_Dodge()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Initialized"));
}

void UB_Action_Dodge::ExecuteAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionDodge] ExecuteAction"));

	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	// Read DodgeMontages directly from the C++ property on UPDA_ActionBase
	// (Migration script sets this property, not RelevantData)
	if (Action)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionDodge] Action class: %s"), *Action->GetClass()->GetName());

		// Cast to UPDA_ActionBase - works because PDA_ActionBase is reparented to C++
		if (UPDA_ActionBase* ActionData = Cast<UPDA_ActionBase>(Action))
		{
			// Read directly from DodgeMontages property (set by migration script)
			DodgeMontages = ActionData->DodgeMontages;
			UE_LOG(LogTemp, Log, TEXT("[ActionDodge] DodgeMontages: Forward=%s, Backward=%s, Backstep=%s"),
				DodgeMontages.Forward ? *DodgeMontages.Forward->GetName() : TEXT("NULL"),
				DodgeMontages.Backward ? *DodgeMontages.Backward->GetName() : TEXT("NULL"),
				DodgeMontages.Backstep ? *DodgeMontages.Backstep->GetName() : TEXT("NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[ActionDodge] Cast<UPDA_ActionBase> FAILED! Class: %s"),
				*Action->GetClass()->GetPathName());
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

UAnimMontage* UB_Action_Dodge::GetDirectionalDodgeMontage()
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
