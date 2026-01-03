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

	// TODO: Migrate EventGraph logic here
	// Update animation variables based on character state

	// Example: Update speed/direction from velocity
	// FVector Velocity = OwnerCharacter->GetVelocity();
	// Speed = Velocity.Size();
	// Direction = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());
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
