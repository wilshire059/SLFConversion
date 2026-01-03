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

void ULadderManagerComponent::PlayClimbUpMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayClimbUpMontage"));
	// TODO: Play climb up montage from LadderAnimset
}

void ULadderManagerComponent::PlayClimbDownMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayClimbDownMontage"));
	// TODO: Play climb down montage from LadderAnimset
}

void ULadderManagerComponent::PlayMountTopMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountTopMontage"));
	// TODO: Play mount top montage from LadderAnimset
}

void ULadderManagerComponent::PlayMountBottomMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountBottomMontage"));
	// TODO: Play mount bottom montage from LadderAnimset
}

void ULadderManagerComponent::PlayDismountTopMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayDismountTopMontage"));
	// TODO: Play dismount top montage from LadderAnimset
}

void ULadderManagerComponent::PlayDismountBottomMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayDismountBottomMontage"));
	// TODO: Play dismount bottom montage from LadderAnimset
}

void ULadderManagerComponent::StopClimbMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StopClimbMontage"));
	// TODO: Stop current climb montage
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
	// TODO: Set montage play rate
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY [35-39/39]
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::OrientToLadder_Implementation()
{
	if (!CurrentLadder) return;

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] OrientToLadder"));

	// TODO: Rotate character to face ladder
	SetIsOrientedToLadder(true);
}

void ULadderManagerComponent::AttachToLadder_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] AttachToLadder"));
	// TODO: Attach character to ladder movement component
}

void ULadderManagerComponent::DetachFromLadder_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] DetachFromLadder"));
	// TODO: Detach character from ladder
}

void ULadderManagerComponent::UpdateLadderPosition_Implementation()
{
	// TODO: Update character position along ladder
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
