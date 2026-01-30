// ABP_SoulslikeCharacter_Additive.cpp
// C++ AnimInstance implementation for ABP_SoulslikeCharacter_Additive
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Animation/ABP_SoulslikeCharacter_Additive.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Blueprint.h"

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
	//
	// DA_ExampleAnimSetup is a Blueprint data asset instance (PDA_AnimData_C class)
	// containing the TwoHanded animation sequences
	if (!AnimDataAsset)
	{
		// Try multiple path formats for Blueprint data asset loading
		static const TCHAR* Paths[] = {
			TEXT("/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleAnimSetup.DA_ExampleAnimSetup"),
			TEXT("/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleAnimSetup"),
			TEXT("/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleAnimSetup.DA_ExampleAnimSetup_C"),
		};

		for (const TCHAR* Path : Paths)
		{
			// LoadObject works better for UDataAsset/UObject than StaticLoadObject
			AnimDataAsset = LoadObject<UPDA_AnimData>(nullptr, Path);
			if (AnimDataAsset)
			{
				UE_LOG(LogTemp, Warning, TEXT("[AnimBP] Loaded AnimDataAsset from path: %s"), Path);
				break;
			}
		}

		if (AnimDataAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AnimBP] AnimDataAsset: %s (Class: %s)"),
				*AnimDataAsset->GetName(),
				*AnimDataAsset->GetClass()->GetName());

			// Debug: Check TwoHanded animation values
			UE_LOG(LogTemp, Warning, TEXT("[AnimBP] TwoHandedWeapon_Right: %s"),
				AnimDataAsset->TwoHandedWeapon_Right ? *AnimDataAsset->TwoHandedWeapon_Right->GetName() : TEXT("NULL"));
			UE_LOG(LogTemp, Warning, TEXT("[AnimBP] TwoHandedWeapon_Left: %s"),
				AnimDataAsset->TwoHandedWeapon_Left ? *AnimDataAsset->TwoHandedWeapon_Left->GetName() : TEXT("NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[AnimBP] FAILED to load AnimDataAsset from any path!"));
		}
	}

	// ALWAYS log AnimDataAsset state (regardless of how it was loaded)
	UE_LOG(LogTemp, Warning, TEXT("UABP_SoulslikeCharacter_Additive::NativeInitializeAnimation - Owner: %s, AnimData: %s"),
		OwnerCharacter ? *OwnerCharacter->GetName() : TEXT("None"),
		AnimDataAsset ? *AnimDataAsset->GetName() : TEXT("None"));

	// ALWAYS log TwoHanded animation state for debugging
	if (AnimDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimBP INIT] TwoHandedWeapon_Right: %s"),
			AnimDataAsset->TwoHandedWeapon_Right ? *AnimDataAsset->TwoHandedWeapon_Right->GetName() : TEXT("NULL"));
		UE_LOG(LogTemp, Warning, TEXT("[AnimBP INIT] TwoHandedWeapon_Left: %s"),
			AnimDataAsset->TwoHandedWeapon_Left ? *AnimDataAsset->TwoHandedWeapon_Left->GetName() : TEXT("NULL"));
	}
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
		// Also check the Blueprint-level enum values by looking at our class
		UClass* MyClass = GetClass();
		FString ParentName = MyClass->GetSuperClass() ? MyClass->GetSuperClass()->GetName() : TEXT("None");

		UE_LOG(LogTemp, Warning, TEXT("[AnimBP DEBUG] Class=%s, Parent=%s, Speed=%.1f, bIsAccel=%s, LeftOverlay=%d, RightOverlay=%d, ActiveOverlay=%d, EquipMgr=%s, AnimData=%s"),
			*MyClass->GetName(),
			*ParentName,
			Speed,
			bIsAccelerating ? TEXT("TRUE") : TEXT("FALSE"),
			(int32)LeftHandOverlayState,
			(int32)RightHandOverlayState,
			(int32)ActiveOverlayState,
			EquipmentManager ? TEXT("YES") : TEXT("NO"),
			AnimDataAsset ? *AnimDataAsset->GetName() : TEXT("NULL"));

		// Log the actual enum value in hex to see if there's any conversion issue
		UE_LOG(LogTemp, Warning, TEXT("[AnimBP DEBUG] LeftOverlay raw bytes: 0x%02X, RightOverlay raw bytes: 0x%02X"),
			(uint8)LeftHandOverlayState,
			(uint8)RightHandOverlayState);
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
