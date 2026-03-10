// SLFActionDoubleJump.cpp
#include "SLFActionDoubleJump.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "SLFSoulslikeCharacter.h"

USLFActionDoubleJump::USLFActionDoubleJump()
{
	DoubleJumpMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_DoubleJump.AM_DoubleJump")));
}

bool USLFActionDoubleJump::CanExecuteAction_Implementation()
{
	if (!OwnerActor) return false;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return false;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp) return false;

	// Must be airborne (falling) — this is the SECOND jump
	return MoveComp->IsFalling();
}

void USLFActionDoubleJump::ExecuteAction_Implementation()
{
	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp) return;

	// Preserve horizontal momentum and boost it, cancel downward velocity, add upward impulse
	FVector Velocity = MoveComp->Velocity;
	FVector HorizontalVel(Velocity.X, Velocity.Y, 0.0f);
	float HorizontalSpeed = HorizontalVel.Size();

	// If player has any horizontal momentum, boost it significantly
	if (HorizontalSpeed > 10.0f)
	{
		FVector HorizontalDir = HorizontalVel.GetSafeNormal();
		float BoostedSpeed = FMath::Max(HorizontalSpeed, DoubleJumpImpulse * 0.8f);
		Velocity.X = HorizontalDir.X * BoostedSpeed;
		Velocity.Y = HorizontalDir.Y * BoostedSpeed;
	}

	Velocity.Z = FMath::Max(Velocity.Z, 0.0f);
	Velocity.Z += DoubleJumpImpulse;

	// Temporarily boost MaxWalkSpeed so CalcVelocity doesn't clamp horizontal velocity
	float OrigMaxWalkSpeed = MoveComp->MaxWalkSpeed;
	float BoostedHorizSpeed = FVector2D(Velocity.X, Velocity.Y).Size();
	MoveComp->MaxWalkSpeed = FMath::Max(OrigMaxWalkSpeed, BoostedHorizSpeed + 100.0f);

	// Set velocity directly
	MoveComp->Velocity = Velocity;

	UE_LOG(LogTemp, Log, TEXT("[ActionDoubleJump] Executing double jump (Vel: %.0f,%.0f,%.0f HorizSpeed: %.0f->%.0f MaxWalk: %.0f->%.0f)"),
		Velocity.X, Velocity.Y, Velocity.Z, HorizontalSpeed, BoostedHorizSpeed, OrigMaxWalkSpeed, MoveComp->MaxWalkSpeed);

	// Reset fall tracking so the double jump height doesn't trigger heavy landing reactions
	if (ASLFSoulslikeCharacter* SoulsChar = Cast<ASLFSoulslikeCharacter>(Character))
	{
		SoulsChar->FallStartZ = Character->GetActorLocation().Z;
		SoulsChar->bTrackingFall = true;
	}

	// Play double jump montage
	UAnimMontage* Montage = DoubleJumpMontage.LoadSynchronous();
	if (Montage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0f, 0.0f, NAME_None);
	}

	// Disable root motion so the montage doesn't override our velocity
	if (UAnimInstance* AnimInst = Character->GetMesh()->GetAnimInstance())
	{
		AnimInst->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	// Restore MaxWalkSpeed and root motion after a fixed delay (safely, no raw pointers)
	// 2 seconds is enough for any double jump arc to complete
	TWeakObjectPtr<ACharacter> WeakChar = Character;
	float SavedMaxWalkSpeed = OrigMaxWalkSpeed;
	FTimerHandle RestoreHandle;
	Character->GetWorldTimerManager().SetTimer(RestoreHandle, [WeakChar, SavedMaxWalkSpeed]()
	{
		if (ACharacter* Char = WeakChar.Get())
		{
			if (UCharacterMovementComponent* MC = Char->GetCharacterMovement())
			{
				MC->MaxWalkSpeed = SavedMaxWalkSpeed;
			}
			if (UAnimInstance* AnimInst = Char->GetMesh()->GetAnimInstance())
			{
				AnimInst->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
			}
		}
	}, 2.0f, false);
}
