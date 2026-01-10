// ABP_SoulslikeCharacter_Additive.cpp
// C++ AnimInstance implementation for ABP_SoulslikeCharacter_Additive
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Animation/ABP_SoulslikeCharacter_Additive.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UABP_SoulslikeCharacter_Additive::UABP_SoulslikeCharacter_Additive()
{
	OwnerCharacter = nullptr;
}

void UABP_SoulslikeCharacter_Additive::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache owner reference
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	UE_LOG(LogTemp, Log, TEXT("UABP_SoulslikeCharacter_Additive::NativeInitializeAnimation - Owner: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"));
}

void UABP_SoulslikeCharacter_Additive::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}

	// Update animation variables based on character state

	// Crouch state
	IsCrouched = OwnerCharacter->bIsCrouched;

	// Velocity and movement
	Velocity = OwnerCharacter->GetVelocity();
	Velocity2D = FVector(Velocity.X, Velocity.Y, 0.0);
	Speed = Velocity2D.Size();
	Direction = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());

	// Location and rotation
	WorldLocation = OwnerCharacter->GetActorLocation();
	WorldRotation = OwnerCharacter->GetActorRotation();

	// Falling state
	if (UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement())
	{
		bIsFalling = MovementComp->IsFalling();
		Acceleration = MovementComp->GetCurrentAcceleration();
		Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.0);
		bIsAccelerating = Acceleration2D.Size() > 0.1f;
	}

	// Cache and use CombatManager for blocking state (on Character)
	if (!CombatManager)
	{
		CombatManager = OwnerCharacter->FindComponentByClass<UAC_CombatManager>();
	}
	if (CombatManager)
	{
		bIsBlocking = CombatManager->IsGuarding;
	}

	// Cache and use ActionManager for resting state (on Character)
	if (!ActionManager)
	{
		ActionManager = OwnerCharacter->FindComponentByClass<UAC_ActionManager>();
	}
	if (ActionManager)
	{
		IsResting = ActionManager->IsResting;
	}

	// Cache and use EquipmentManager for overlay states (on Controller, NOT Character!)
	if (!EquipmentManager)
	{
		if (AController* Controller = OwnerCharacter->GetController())
		{
			EquipmentManager = Controller->FindComponentByClass<UAC_EquipmentManager>();
			if (EquipmentManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("[AnimBP] Found EquipmentManager on Controller: %s"), *Controller->GetName());
			}
		}
	}
	// Read overlay states from EquipmentManager
	if (EquipmentManager)
	{
		LeftHandOverlayState = EquipmentManager->LeftHandOverlayState;
		RightHandOverlayState = EquipmentManager->RightHandOverlayState;
		ActiveOverlayState = EquipmentManager->ActiveOverlayState;
	}

	// DEBUG: Log animation state periodically
	static int32 DebugLogCounter = 0;
	if (++DebugLogCounter % 60 == 0)  // Log every ~1 second at 60fps
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimBP DEBUG] Speed=%.1f, LeftOverlay=%d, RightOverlay=%d, ActiveOverlay=%d, EquipMgr=%s"),
			Speed,
			(int32)LeftHandOverlayState,
			(int32)RightHandOverlayState,
			(int32)ActiveOverlayState,
			EquipmentManager ? TEXT("YES") : TEXT("NO"));
	}
}

FVector UABP_SoulslikeCharacter_Additive::GetOwnerVelocity() const
{
	return OwnerCharacter ? OwnerCharacter->GetVelocity() : FVector::ZeroVector;
}

FRotator UABP_SoulslikeCharacter_Additive::GetOwnerRotation() const
{
	return OwnerCharacter ? OwnerCharacter->GetActorRotation() : FRotator::ZeroRotator;
}
