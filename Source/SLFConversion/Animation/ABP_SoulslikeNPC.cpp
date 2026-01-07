// ABP_SoulslikeNPC.cpp
// C++ AnimInstance implementation for ABP_SoulslikeNPC
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Animation/ABP_SoulslikeNPC.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UABP_SoulslikeNPC::UABP_SoulslikeNPC()
{
	OwnerCharacter = nullptr;
}

void UABP_SoulslikeNPC::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache owner reference
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	UE_LOG(LogTemp, Log, TEXT("UABP_SoulslikeNPC::NativeInitializeAnimation - Owner: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

void UABP_SoulslikeNPC::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}

	// Update SoulslikeNpc/SoulslikeCharacter reference (owning actor)
	SoulslikeNpc = OwnerCharacter;
	SoulslikeCharacter = OwnerCharacter;

	// Get movement component reference
	MovementComponent = OwnerCharacter->GetCharacterMovement();

	// Update velocity from owner
	Velocity = OwnerCharacter->GetVelocity();

	// Calculate ground speed (2D velocity magnitude)
	GroundSpeed = Velocity.Size2D();

	// ShouldMove if ground speed > 3.0
	ShouldMove = GroundSpeed > 3.0;

	// IsFalling from movement component
	IsFalling = MovementComponent ? MovementComponent->IsFalling() : false;

	// Calculate direction relative to actor rotation
	Direction = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());

	// Update look-at data if AI Combat Manager is set
	if (ACAICombatManager)
	{
		// Get look-at target from combat manager via interface
		// HasLookAtTarget and LookAtLocation would be set by the combat manager
		if (HasLookAtTarget)
		{
			DistanceToLookAtTarget = FVector::Dist(OwnerCharacter->GetActorLocation(), LookAtLocation);
		}
	}
}

FVector UABP_SoulslikeNPC::GetOwnerVelocity() const
{
	return OwnerCharacter ? OwnerCharacter->GetVelocity() : FVector::ZeroVector;
}

FRotator UABP_SoulslikeNPC::GetOwnerRotation() const
{
	return OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;
}

// AnimGraph function removed - conflicts with UE's internal AnimGraph function name
