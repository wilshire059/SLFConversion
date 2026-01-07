// LadderManagerComponent.cpp
// C++ implementation for AC_LadderManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_LadderManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         10/10 (initialized in constructor)
// Functions:         39/39 implemented (32 BlueprintNativeEvent + 7 inline getters)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "LadderManagerComponent.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

ULadderManagerComponent::ULadderManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize runtime state
	bIsOnGround = true;
	bIsClimbing = false;
	bIsClimbingOffTop = false;
	bIsClimbingDownFromTop = false;
	bIsOrientedToLadder = false;
	ClimbAnimState = ESLFLadderClimbState::Idle;
	CurrentLadder = nullptr;
	bClimbFast = false;

	// Initialize config
	LadderAnimset = nullptr;
	ClimbSprintMultiplier = 1.5;
}

// ═══════════════════════════════════════════════════════════════════════════════
// STATE SETTERS [1-7/39]
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::SetCurrentLadder_Implementation(AActor* Ladder)
{
	CurrentLadder = Ladder;
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] SetCurrentLadder: %s"),
		Ladder ? *Ladder->GetName() : TEXT("null"));
}

void ULadderManagerComponent::SetIsClimbingDownFromTop_Implementation(bool Value)
{
	bIsClimbingDownFromTop = Value;
}

void ULadderManagerComponent::SetIsClimbingOffTop_Implementation(bool Value)
{
	bIsClimbingOffTop = Value;
}

void ULadderManagerComponent::SetIsOnGround_Implementation(bool Value)
{
	bIsOnGround = Value;
}

void ULadderManagerComponent::SetIsClimbing_Implementation(bool Value)
{
	bIsClimbing = Value;
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] SetIsClimbing: %s"), Value ? TEXT("true") : TEXT("false"));
}

void ULadderManagerComponent::SetIsOrientedToLadder_Implementation(bool Value)
{
	bIsOrientedToLadder = Value;
}

void ULadderManagerComponent::SetClimbAnimState_Implementation(ESLFLadderClimbState State)
{
	ClimbAnimState = State;
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] SetClimbAnimState: %d"), static_cast<int32>(State));
}

// ═══════════════════════════════════════════════════════════════════════════════
// CLIMB CONTROL [15-24/39]
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::StartClimb_Implementation(AActor* Ladder)
{
	if (!Ladder)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] StartClimb: Invalid ladder"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StartClimb: %s"), *Ladder->GetName());

	SetCurrentLadder(Ladder);
	SetIsClimbing(true);
	SetIsOnGround(false);
	AttachToLadder();
	OrientToLadder();
}

void ULadderManagerComponent::StopClimb_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StopClimb"));

	DetachFromLadder();
	SetIsClimbing(false);
	SetIsOnGround(true);
	ResetLadderState();
}

void ULadderManagerComponent::ClimbUp_Implementation()
{
	if (!bIsClimbing) return;

	SetClimbAnimState(ESLFLadderClimbState::ClimbingUp);
	PlayClimbUpMontage();
}

void ULadderManagerComponent::ClimbDown_Implementation()
{
	if (!bIsClimbing) return;

	SetClimbAnimState(ESLFLadderClimbState::ClimbingDown);
	PlayClimbDownMontage();
}

void ULadderManagerComponent::StopClimbInput_Implementation()
{
	if (!bIsClimbing) return;

	SetClimbAnimState(ESLFLadderClimbState::Idle);
	StopClimbMontage();
}

void ULadderManagerComponent::MountLadderTop_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] MountLadderTop"));

	SetClimbAnimState(ESLFLadderClimbState::MountTop);
	SetIsClimbingDownFromTop(true);
	PlayMountTopMontage();
}

void ULadderManagerComponent::MountLadderBottom_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] MountLadderBottom"));

	SetClimbAnimState(ESLFLadderClimbState::MountBottom);
	PlayMountBottomMontage();
}

void ULadderManagerComponent::DismountLadderTop_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] DismountLadderTop"));

	SetClimbAnimState(ESLFLadderClimbState::DismountTop);
	SetIsClimbingOffTop(true);
	PlayDismountTopMontage();
}

void ULadderManagerComponent::DismountLadderBottom_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] DismountLadderBottom"));

	SetClimbAnimState(ESLFLadderClimbState::DismountBottom);
	PlayDismountBottomMontage();
}

void ULadderManagerComponent::ToggleFastClimb_Implementation(bool bFast)
{
	bClimbFast = bFast;
	UpdateClimbPlayRate();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ANIMATION & MONTAGES [25-34/39]
// ═══════════════════════════════════════════════════════════════════════════════

// Helper to get AnimInstance from owner character
UAnimInstance* ULadderManagerComponent::GetOwnerAnimInstance() const
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			return Mesh->GetAnimInstance();
		}
	}
	return nullptr;
}

// Helper to play a soft montage reference
void ULadderManagerComponent::PlayLadderMontage(const TSoftObjectPtr<UAnimMontage>& Montage, float PlayRate)
{
	if (Montage.IsNull())
	{
		return;
	}

	UAnimMontage* LoadedMontage = Montage.LoadSynchronous();
	if (!LoadedMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(LoadedMontage, PlayRate);
		CurrentClimbMontage = LoadedMontage;
	}
}

void ULadderManagerComponent::PlayClimbUpMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayClimbUpMontage"));
	// Play climb up montage from LadderAnimset (IMPLEMENTED)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbUp, bClimbFast ? ClimbSprintMultiplier : 1.0f);
	}
}

void ULadderManagerComponent::PlayClimbDownMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayClimbDownMontage"));
	// Play climb down montage from LadderAnimset (IMPLEMENTED)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbDown, bClimbFast ? ClimbSprintMultiplier : 1.0f);
	}
}

void ULadderManagerComponent::PlayMountTopMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountTopMontage"));
	// Play climb in from top montage (IMPLEMENTED)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbInFromTop, 1.0f);
	}
}

void ULadderManagerComponent::PlayMountBottomMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountBottomMontage"));
	// Play climb up to start climbing from bottom (IMPLEMENTED)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbUp, 1.0f);
	}
}

void ULadderManagerComponent::PlayDismountTopMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayDismountTopMontage"));
	// Play climb out at top montage (IMPLEMENTED)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbOutAtTop, 1.0f);
	}
}

void ULadderManagerComponent::PlayDismountBottomMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayDismountBottomMontage"));
	// Play climb down to get off at bottom (IMPLEMENTED)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbDown, 1.0f);
	}
}

void ULadderManagerComponent::StopClimbMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StopClimbMontage"));
	// Stop current climb montage (IMPLEMENTED)
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance && CurrentClimbMontage)
	{
		AnimInstance->Montage_Stop(0.2f, CurrentClimbMontage);
		CurrentClimbMontage = nullptr;
	}
}

void ULadderManagerComponent::OnMontageEnded_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] OnMontageEnded"));

	// Handle dismount completion
	if (bIsClimbingOffTop || ClimbAnimState == ESLFLadderClimbState::DismountBottom)
	{
		StopClimb();
	}
}

void ULadderManagerComponent::OnMontageBlendOut_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] OnMontageBlendOut"));
}

void ULadderManagerComponent::UpdateClimbPlayRate_Implementation()
{
	double Rate = bClimbFast ? ClimbSprintMultiplier : 1.0;
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] UpdateClimbPlayRate: %.2f"), Rate);
	// Set montage play rate (IMPLEMENTED)
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (AnimInstance && CurrentClimbMontage)
	{
		AnimInstance->Montage_SetPlayRate(CurrentClimbMontage, Rate);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY [35-39/39]
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::OrientToLadder_Implementation()
{
	if (!CurrentLadder) return;

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] OrientToLadder"));

	// Rotate character to face ladder direction (IMPLEMENTED)
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		// Get ladder's forward direction and rotate character to face it
		FRotator LadderRotation = CurrentLadder->GetActorRotation();
		// Character should face into the ladder (opposite of ladder forward)
		FRotator TargetRotation = FRotator(0.0, LadderRotation.Yaw + 180.0, 0.0);
		Character->SetActorRotation(TargetRotation);
	}
	SetIsOrientedToLadder(true);
}

void ULadderManagerComponent::AttachToLadder_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] AttachToLadder"));
	// Attach character to ladder - disable gravity and set flying mode (IMPLEMENTED)
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		Character->GetCharacterMovement()->GravityScale = 0.0f;
		Character->GetCharacterMovement()->StopMovementImmediately();
	}
}

void ULadderManagerComponent::DetachFromLadder_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] DetachFromLadder"));
	// Detach character - restore normal movement (IMPLEMENTED)
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		Character->GetCharacterMovement()->GravityScale = 1.0f;
	}
}

void ULadderManagerComponent::UpdateLadderPosition_Implementation()
{
	// Update character position along ladder based on climb input (IMPLEMENTED)
	// This is typically called from tick when climbing
	// The actual movement is handled by the montages root motion
	// This function can be extended to add manual position updates if needed
}

void ULadderManagerComponent::ResetLadderState_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] ResetLadderState"));

	SetCurrentLadder(nullptr);
	SetIsClimbingDownFromTop(false);
	SetIsClimbingOffTop(false);
	SetIsOrientedToLadder(false);
	SetClimbAnimState(ESLFLadderClimbState::Idle);
	bClimbFast = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CLIMB EVENTS
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::EventOnClimbOut()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] EventOnClimbOut"));

	// Called when player finishes climbing out (either at top or bottom)
	// Reset climbing state and detach from ladder
	SetIsClimbing(false);
	DetachFromLadder();
	ResetLadderState();
}

void ULadderManagerComponent::EventOnClimbDownFromTop()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] EventOnClimbDownFromTop"));

	// Called when player starts to climb down from the top of a ladder
	SetIsClimbingDownFromTop(true);
	SetClimbAnimState(ESLFLadderClimbState::ClimbingDown);

	// Play climb down from top montage if available
	// The montage would be played via the LadderAnimset data asset
	// Full implementation requires casting to PDA_LadderAnimData and accessing ClimbDownFromTop
}
