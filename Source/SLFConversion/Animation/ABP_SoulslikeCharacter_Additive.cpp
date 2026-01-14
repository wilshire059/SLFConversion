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

	// Load AnimDataAsset if not already set (required for LL implementation graphs)
	// The AnimGraph's LinkedAnimLayer nodes (LL_OneHanded_Right, etc.) read animation
	// sequences from this data asset via property access nodes
	if (!AnimDataAsset)
	{
		static const TCHAR* AnimDataPath = TEXT("/Game/SoulslikeFramework/Data/_AnimationData/PDA_AnimData.PDA_AnimData");
		AnimDataAsset = Cast<UPDA_AnimData>(StaticLoadObject(UPDA_AnimData::StaticClass(), nullptr, AnimDataPath));

		if (AnimDataAsset)
		{
			UE_LOG(LogTemp, Log, TEXT("[AnimBP] Loaded AnimDataAsset: %s"), *AnimDataAsset->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AnimBP] Failed to load AnimDataAsset from %s"), AnimDataPath);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UABP_SoulslikeCharacter_Additive::NativeInitializeAnimation - Owner: %s, AnimData: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"),
		AnimDataAsset ? *AnimDataAsset->GetName() : TEXT("None"));
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

	// Falling state and movement
	if (UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement())
	{
		bIsFalling = MovementComp->IsFalling();
		Acceleration = MovementComp->GetCurrentAcceleration();
		Acceleration2D = FVector(Acceleration.X, Acceleration.Y, 0.0);

		// FIX: bIsAccelerating drives IDLE<->CYCLE transition
		// Must use SPEED, not acceleration - at constant velocity, acceleration is 0
		// but the character is still moving and should be in CYCLE state
		bIsAccelerating = Speed > 3.0f;  // Small threshold to filter micro-movements
	}

	// Cache and use CombatManager for blocking state and IK data (on Character)
	if (!CombatManager)
	{
		CombatManager = OwnerCharacter->FindComponentByClass<UAC_CombatManager>();
	}
	if (CombatManager)
	{
		// Use GetIsGuarding() to include grace period (not direct IsGuarding access)
		bIsBlocking = CombatManager->GetIsGuarding();
		IkWeight = CombatManager->IKWeight;
		ActiveHitNormal = CombatManager->CurrentHitNormal;
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
	// Read overlay states, guard sequence, and tags from EquipmentManager
	// AnimGraph BlendListByEnum nodes read directly from these C++ UPROPERTY variables
	// (No reflection needed - Blueprint variables were renamed to match C++ property names)
	if (EquipmentManager)
	{
		LeftHandOverlayState = EquipmentManager->LeftHandOverlayState;
		RightHandOverlayState = EquipmentManager->RightHandOverlayState;
		ActiveOverlayState = EquipmentManager->ActiveOverlayState;
		ActiveGuardSequence = EquipmentManager->ActiveBlockSequence;
		GrantedTags = EquipmentManager->GrantedTags;

	}

	// DEBUG: Log animation state periodically
	static int32 DebugLogCounter = 0;
	if (++DebugLogCounter % 60 == 0)  // Log every ~1 second at 60fps
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimBP DEBUG] Speed=%.1f, bIsAccelerating=%s, LeftOverlay=%d, RightOverlay=%d, ActiveOverlay=%d, EquipMgr=%s"),
			Speed,
			bIsAccelerating ? TEXT("TRUE") : TEXT("FALSE"),
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
