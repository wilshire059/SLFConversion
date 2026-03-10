// SLFActionSwim.cpp
#include "SLFActionSwim.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USLFActionSwim::USLFActionSwim()
{
	SwimIdleMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_SwimIdle.AM_SwimIdle")));
	SwimForwardMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_SwimForward.AM_SwimForward")));
	SwimLeftMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_SwimLeft.AM_SwimLeft")));
	SwimRightMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_SwimRight.AM_SwimRight")));
}

bool USLFActionSwim::CanExecuteAction_Implementation()
{
	return bIsInWater && OwnerActor != nullptr;
}

void USLFActionSwim::ExecuteAction_Implementation()
{
	// Swimming is driven by EnterWater/ExitWater + UpdateSwimAnimation, not single-fire
}

void USLFActionSwim::EnterWater()
{
	if (bIsInWater) return;
	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	bIsInWater = true;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->SetMovementMode(MOVE_Swimming);
		MoveComp->MaxSwimSpeed = SwimSpeed;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionSwim] Entered water — swimming mode"));

	// Start with idle (treading water)
	UAnimMontage* Montage = SwimIdleMontage.LoadSynchronous();
	PlaySwimMontage(Montage);
}

void USLFActionSwim::ExitWater()
{
	if (!bIsInWater) return;
	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	bIsInWater = false;
	bIsSwimming = false;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	// Stop swim montage
	if (ActiveSwimMontage && Character->GetMesh() && Character->GetMesh()->GetAnimInstance())
	{
		Character->GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, ActiveSwimMontage);
	}
	ActiveSwimMontage = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[ActionSwim] Exited water — walking mode"));
}

void USLFActionSwim::UpdateSwimAnimation(const FVector2D& MovementInput)
{
	if (!bIsInWater || !OwnerActor) return;

	UAnimMontage* DesiredMontage = nullptr;

	if (MovementInput.Size() < 0.1f)
	{
		// No input — tread water
		DesiredMontage = SwimIdleMontage.LoadSynchronous();
		bIsSwimming = false;
	}
	else if (FMath::Abs(MovementInput.X) > FMath::Abs(MovementInput.Y))
	{
		// Primarily lateral movement
		if (MovementInput.X > 0.0f)
		{
			DesiredMontage = SwimRightMontage.LoadSynchronous();
		}
		else
		{
			DesiredMontage = SwimLeftMontage.LoadSynchronous();
		}
		bIsSwimming = true;
	}
	else
	{
		// Forward/backward — use forward swim for both
		DesiredMontage = SwimForwardMontage.LoadSynchronous();
		bIsSwimming = true;
	}

	if (DesiredMontage && DesiredMontage != ActiveSwimMontage)
	{
		PlaySwimMontage(DesiredMontage);
	}
}

void USLFActionSwim::PlaySwimMontage(UAnimMontage* Montage)
{
	if (!Montage || !OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character || !Character->GetMesh() || !Character->GetMesh()->GetAnimInstance()) return;

	UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance();

	// Don't restart same montage
	if (ActiveSwimMontage == Montage && AnimInst->Montage_IsPlaying(Montage))
	{
		return;
	}

	AnimInst->Montage_Play(Montage, 1.0f);
	// Loop swim montages
	AnimInst->Montage_SetNextSection(FName("Default"), FName("Default"), Montage);
	ActiveSwimMontage = Montage;
}
