// SLFActionSlide.cpp
#include "SLFActionSlide.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

USLFActionSlide::USLFActionSlide()
{
	SlideMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_Slide.AM_Slide")));
}

bool USLFActionSlide::CanExecuteAction_Implementation()
{
	if (!OwnerActor || bIsSliding || bOnCooldown) return false;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return false;

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (!MoveComp) return false;

	// Must be on ground and moving
	return !MoveComp->IsFalling() && MoveComp->Velocity.Size2D() > 100.0f;
}

void USLFActionSlide::ExecuteAction_Implementation()
{
	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	UE_LOG(LogTemp, Log, TEXT("[ActionSlide] Starting slide"));

	bIsSliding = true;

	// Apply slide impulse in movement direction
	FVector SlideDir = Character->GetVelocity().GetSafeNormal2D();
	if (SlideDir.IsNearlyZero())
	{
		SlideDir = Character->GetActorForwardVector();
	}

	float SlideSpeed = SlideDistance / SlideDuration;
	Character->GetCharacterMovement()->Velocity = SlideDir * SlideSpeed;
	Character->GetCharacterMovement()->GroundFriction = 0.0f; // Reduce friction during slide

	// Crouch during slide
	Character->Crouch();

	// Play slide montage
	UAnimMontage* Montage = SlideMontage.LoadSynchronous();
	if (Montage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0f, 0.0f, NAME_None);
	}

	// End slide after duration
	if (UWorld* World = Character->GetWorld())
	{
		World->GetTimerManager().SetTimer(SlideEndHandle, this,
			&USLFActionSlide::EndSlide, SlideDuration, false);
	}
}

void USLFActionSlide::EndSlide()
{
	bIsSliding = false;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (Character)
	{
		Character->GetCharacterMovement()->GroundFriction = 8.0f; // Restore default
		Character->UnCrouch();
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionSlide] Slide ended, starting cooldown"));

	// Start cooldown
	bOnCooldown = true;
	if (OwnerActor)
	{
		if (UWorld* World = OwnerActor->GetWorld())
		{
			World->GetTimerManager().SetTimer(CooldownHandle, this,
				&USLFActionSlide::ClearCooldown, SlideCooldown, false);
		}
	}
}

void USLFActionSlide::ClearCooldown()
{
	bOnCooldown = false;
}
