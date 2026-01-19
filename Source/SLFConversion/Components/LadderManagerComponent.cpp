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
#include "Components/AC_ActionManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

ULadderManagerComponent::ULadderManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;  // Enable tick for input handling

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

	// Initialize input tracking
	LastClimbInputY = 0.0f;
}

// ═══════════════════════════════════════════════════════════════════════════════
// LIFECYCLE - BeginPlay and Tick
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// CRITICAL: Load LadderAnimset if not set (lost during Blueprint reparenting)
	if (!LadderAnimset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] BeginPlay - LadderAnimset is null, attempting to load default..."));

		// Try to load the default ladder animation data asset
		LadderAnimset = LoadObject<UDataAsset>(nullptr, TEXT("/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleLadderAnimSetup.DA_ExampleLadderAnimSetup"));

		if (LadderAnimset)
		{
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] BeginPlay - Loaded default LadderAnimset: %s"), *LadderAnimset->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[LadderManager] BeginPlay - FAILED to load default LadderAnimset! Climbing will not work."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] BeginPlay - LadderAnimset already set: %s"), *LadderAnimset->GetName());
	}

	// Find and cache IA_Move input action
	CachedMoveAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/SoulslikeFramework/Input/Actions/IA_Move.IA_Move"));
	if (!CachedMoveAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] BeginPlay - Could not find IA_Move input action!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] BeginPlay - IA_Move cached successfully"));
	}

	// Bind to IA_Move for climbing input
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		APlayerController* PC = Cast<APlayerController>(Character->GetController());
		if (PC)
		{
			if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PC->InputComponent))
			{
				if (CachedMoveAction)
				{
					EnhancedInputComp->BindAction(CachedMoveAction, ETriggerEvent::Triggered, this, &ULadderManagerComponent::OnMoveInput);
					UE_LOG(LogTemp, Log, TEXT("[LadderManager] BeginPlay - Bound to IA_Move Triggered"));
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] BeginPlay - Input handling enabled"));
}

void ULadderManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Input is now handled via OnMoveInput callback (Enhanced Input)
	// Tick is kept for potential future use (position updates, etc.)
}

void ULadderManagerComponent::OnMoveInput(const FInputActionValue& Value)
{
	// Only process when climbing
	if (!bIsClimbing)
	{
		return;
	}

	// Get the Vector2D value from IA_Move (matches bp_only's ActionValue)
	FVector2D MoveInput = Value.Get<FVector2D>();

	UE_LOG(LogTemp, Verbose, TEXT("[LadderManager] OnMoveInput: X=%.2f, Y=%.2f"), MoveInput.X, MoveInput.Y);

	// Handle climbing input
	HandleClimbingInput(MoveInput);
}

void ULadderManagerComponent::HandleClimbingInput(const FVector2D& InputVector)
{
	// bp_only logic uses CONTINUOUS input processing with a state machine:
	// - IA_Move Triggered fires continuously while key is held
	// - Switch on ClimbAnimState to route input
	// - Allow re-triggering climb when montage finishes and input still held

	float InputY = InputVector.Y;
	const float DeadZone = 0.1f;

	bool bWantsClimbUp = InputY > DeadZone;
	bool bWantsClimbDown = InputY < -DeadZone;
	bool bInputInDeadzone = !bWantsClimbUp && !bWantsClimbDown;

	// Check if current montage is still playing
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	bool bMontageActive = AnimInstance && CurrentClimbMontage && AnimInstance->Montage_IsPlaying(CurrentClimbMontage);

	// STATE MACHINE: Route based on current climb state
	switch (ClimbAnimState)
	{
		case ESLFLadderClimbState::Idle:
		{
			// In Idle - start climbing in requested direction
			if (bWantsClimbDown)
			{
				UE_LOG(LogTemp, Log, TEXT("[LadderManager] Idle -> ClimbDown (Y=%.2f)"), InputY);
				ClimbDown();
			}
			else if (bWantsClimbUp)
			{
				UE_LOG(LogTemp, Log, TEXT("[LadderManager] Idle -> ClimbUp (Y=%.2f)"), InputY);
				ClimbUp();
			}
			break;
		}

		case ESLFLadderClimbState::ClimbingUp:
		case ESLFLadderClimbState::ClimbingDown:
		{
			// Currently climbing - check if montage finished and input still held
			if (!bMontageActive)
			{
				// Montage finished - go back to Idle, then process input
				SetClimbAnimState(ESLFLadderClimbState::Idle);

				if (bWantsClimbDown)
				{
					UE_LOG(LogTemp, Log, TEXT("[LadderManager] Montage done, continuing ClimbDown (Y=%.2f)"), InputY);
					ClimbDown();
				}
				else if (bWantsClimbUp)
				{
					UE_LOG(LogTemp, Log, TEXT("[LadderManager] Montage done, continuing ClimbUp (Y=%.2f)"), InputY);
					ClimbUp();
				}
				else if (bInputInDeadzone)
				{
					UE_LOG(LogTemp, Log, TEXT("[LadderManager] Montage done, input released -> Idle"));
				}
			}
			// If montage still playing, let it continue - don't spam calls
			break;
		}

		case ESLFLadderClimbState::MountTop:
		case ESLFLadderClimbState::MountBottom:
		case ESLFLadderClimbState::DismountTop:
		case ESLFLadderClimbState::DismountBottom:
		{
			// Mounting/dismounting - don't process climb input, wait for animation
			break;
		}
	}

	LastClimbInputY = InputY;
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

void ULadderManagerComponent::StartClimb_Implementation(AActor* Ladder, bool bIsTopdown)
{
	if (!Ladder)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] StartClimb: Invalid ladder"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StartClimb: %s, IsTopdown: %s, LadderAnimset: %s"),
		*Ladder->GetName(),
		bIsTopdown ? TEXT("true") : TEXT("false"),
		LadderAnimset ? *LadderAnimset->GetName() : TEXT("NULL"));

	SetCurrentLadder(Ladder);
	SetIsClimbing(true);

	// CRITICAL: Reset zone flags at START of climb to clear any stale state
	// These will be set fresh as the character moves into zones
	SetIsOnGround(false);
	SetIsClimbingOffTop(false);

	// Set topdown flag BEFORE orient so OrientToLadder knows approach direction
	if (bIsTopdown)
	{
		SetIsClimbingDownFromTop(true);
		// If mounting from top, we start IN the top zone
		SetIsClimbingOffTop(true);
	}
	else
	{
		SetIsClimbingDownFromTop(false);
		// If mounting from bottom, we start IN the bottom zone
		SetIsOnGround(true);
	}

	AttachToLadder();
	OrientToLadder();

	// Play the appropriate mount montage based on approach direction
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StartClimb - About to play mount montage (topdown=%s)"),
		bIsTopdown ? TEXT("true") : TEXT("false"));

	if (bIsTopdown)
	{
		MountLadderTop();
	}
	else
	{
		MountLadderBottom();
	}

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StartClimb - Complete. ClimbAnimState: %d"),
		static_cast<int32>(ClimbAnimState));
}

void ULadderManagerComponent::StopClimb_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] StopClimb"));

	// CRITICAL: Stop the climbing montage FIRST before detaching
	// Without this, the character stays stuck in the climbing animation pose
	StopClimbMontage();

	DetachFromLadder();
	SetIsClimbing(false);
	SetIsOnGround(true);
	ResetLadderState();
}

void ULadderManagerComponent::ClimbUp_Implementation()
{
	if (!bIsClimbing) return;

	// bp_only: Check zones BEFORE starting animation, not after
	// If already in top zone, dismount instead of climbing
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] ClimbUp - bIsClimbingOffTop: %s"),
		bIsClimbingOffTop ? TEXT("true") : TEXT("false"));

	if (bIsClimbingOffTop)
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] ClimbUp -> At top zone, dismounting top"));
		DismountLadderTop();
		return;
	}

	SetClimbAnimState(ESLFLadderClimbState::ClimbingUp);
	PlayClimbUpMontage();
}

void ULadderManagerComponent::ClimbDown_Implementation()
{
	if (!bIsClimbing) return;

	// bp_only: Check zones BEFORE starting animation, not after
	// If already in bottom zone, dismount instead of climbing
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	FVector CharPos = Character ? Character->GetActorLocation() : FVector::ZeroVector;
	FRotator CharRot = Character ? Character->GetActorRotation() : FRotator::ZeroRotator;

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] ClimbDown - bIsOnGround: %s, CharZ: %.1f, CharRot: %.1f"),
		bIsOnGround ? TEXT("true") : TEXT("false"), CharPos.Z, CharRot.Yaw);

	if (bIsOnGround)
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] ClimbDown -> At bottom zone, dismounting bottom"));
		DismountLadderBottom();
		return;
	}

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
void ULadderManagerComponent::PlayLadderMontage(const TSoftObjectPtr<UAnimMontage>& Montage, float PlayRate, bool bBindNotifyCallback)
{
	if (Montage.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] PlayLadderMontage - Montage reference is NULL"));
		return;
	}

	UAnimMontage* LoadedMontage = Montage.LoadSynchronous();
	if (!LoadedMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] PlayLadderMontage - Failed to load montage"));
		return;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LadderManager] PlayLadderMontage - No AnimInstance found!"));
		return;
	}

	float Duration = AnimInstance->Montage_Play(LoadedMontage, PlayRate);
	CurrentClimbMontage = LoadedMontage;

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayLadderMontage - Playing %s (Duration: %.2f, Rate: %.2f)"),
		*LoadedMontage->GetName(), Duration, PlayRate);

	// bp_only uses OnNotifyBegin callback, NOT OnCompleted/OnMontageEnded
	// Montages don't auto-blend-out, so OnCompleted never fires
	// Instead, montages have an AnimNotify "Loop_End" that fires when the loop completes
	if (bBindNotifyCallback)
	{
		// Unbind first to avoid duplicate bindings
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ULadderManagerComponent::HandleMontageNotifyBegin);
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ULadderManagerComponent::HandleMontageNotifyBegin);

		// Also bind OnMontageBlendingOut as fallback for montages without Loop_End notify
		AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &ULadderManagerComponent::HandleMontageBlendingOut);
		AnimInstance->OnMontageBlendingOut.AddDynamic(this, &ULadderManagerComponent::HandleMontageBlendingOut);

		UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayLadderMontage - Bound OnPlayMontageNotifyBegin + OnMontageBlendingOut for %s"), *LoadedMontage->GetName());
	}
}

void ULadderManagerComponent::PlayClimbUpMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayClimbUpMontage - LadderAnimset: %s"),
		LadderAnimset ? *LadderAnimset->GetName() : TEXT("NULL"));

	// Play climb up montage - bind callback to transition to Idle when done (allows continuous climbing)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbUp, bClimbFast ? ClimbSprintMultiplier : 1.0f, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LadderManager] PlayClimbUpMontage - FAILED: LadderAnimset is null or cast failed!"));
		SetClimbAnimState(ESLFLadderClimbState::Idle);
	}
}

void ULadderManagerComponent::PlayClimbDownMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayClimbDownMontage - LadderAnimset: %s"),
		LadderAnimset ? *LadderAnimset->GetName() : TEXT("NULL"));

	// Play climb down montage - bind callback to transition to Idle when done (allows continuous climbing)
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbDown, bClimbFast ? ClimbSprintMultiplier : 1.0f, true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LadderManager] PlayClimbDownMontage - FAILED: LadderAnimset is null or cast failed!"));
		SetClimbAnimState(ESLFLadderClimbState::Idle);
	}
}

void ULadderManagerComponent::PlayMountTopMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountTopMontage - LadderAnimset: %s"),
		LadderAnimset ? *LadderAnimset->GetName() : TEXT("NULL"));

	// Play climb in from top montage - bind callback to transition to Idle when done
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountTopMontage - AnimData valid, ClimbInFromTop: %s"),
			AnimData->ClimbInFromTop.IsNull() ? TEXT("NULL") : TEXT("Valid"));
		PlayLadderMontage(AnimData->ClimbInFromTop, 1.0f, true);  // true = bind end callback
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LadderManager] PlayMountTopMontage - FAILED: LadderAnimset cast failed or null!"));
		// Fallback: transition to Idle immediately so player isn't stuck
		SetClimbAnimState(ESLFLadderClimbState::Idle);
	}
}

void ULadderManagerComponent::PlayMountBottomMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountBottomMontage - LadderAnimset: %s"),
		LadderAnimset ? *LadderAnimset->GetName() : TEXT("NULL"));

	// Play climb up to start climbing from bottom - bind callback to transition to Idle when done
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayMountBottomMontage - AnimData valid, ClimbUp: %s"),
			AnimData->ClimbUp.IsNull() ? TEXT("NULL") : TEXT("Valid"));
		PlayLadderMontage(AnimData->ClimbUp, 1.0f, true);  // true = bind end callback
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LadderManager] PlayMountBottomMontage - FAILED: LadderAnimset cast failed or null!"));
		// Fallback: transition to Idle immediately so player isn't stuck
		SetClimbAnimState(ESLFLadderClimbState::Idle);
	}
}

void ULadderManagerComponent::PlayDismountTopMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayDismountTopMontage"));
	// Play climb out at top montage - bind end callback to trigger StopClimb
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbOutAtTop, 1.0f, true);  // true = bind end callback
	}
}

void ULadderManagerComponent::PlayDismountBottomMontage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] PlayDismountBottomMontage"));
	// Play climb down to get off at bottom - bind end callback to trigger StopClimb
	if (UPDA_LadderAnimData* AnimData = Cast<UPDA_LadderAnimData>(LadderAnimset))
	{
		PlayLadderMontage(AnimData->ClimbDown, 1.0f, true);  // true = bind end callback
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

void ULadderManagerComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// NOTE: This callback often doesn't fire because ladder montages don't auto-blend-out
	// bp_only uses OnNotifyBegin with "Loop_End" notify instead - see HandleMontageNotifyBegin
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageEnded - Montage: %s, Interrupted: %s, ClimbAnimState: %d"),
		Montage ? *Montage->GetName() : TEXT("null"),
		bInterrupted ? TEXT("true") : TEXT("false"),
		static_cast<int32>(ClimbAnimState));

	if (!bInterrupted)
	{
		// Check what type of montage just finished
		if (ClimbAnimState == ESLFLadderClimbState::DismountBottom ||
			ClimbAnimState == ESLFLadderClimbState::DismountTop)
		{
			// DISMOUNT finished - exit ladder completely
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageEnded - Dismount complete, calling StopClimb"));
			StopClimb();
		}
		else if (ClimbAnimState == ESLFLadderClimbState::MountTop ||
				 ClimbAnimState == ESLFLadderClimbState::MountBottom)
		{
			// MOUNT finished - transition to Idle so player can start climbing
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageEnded - Mount complete, transitioning to Idle"));
			SetClimbAnimState(ESLFLadderClimbState::Idle);
		}
		else if (ClimbAnimState == ESLFLadderClimbState::ClimbingUp ||
				 ClimbAnimState == ESLFLadderClimbState::ClimbingDown)
		{
			// CLIMB loop finished - transition to Idle (input handler will restart if key held)
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageEnded - Climb loop done, transitioning to Idle"));
			SetClimbAnimState(ESLFLadderClimbState::Idle);
		}
	}

	CurrentClimbMontage = nullptr;
}

void ULadderManagerComponent::HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	// bp_only uses OnNotifyBegin with "Loop_End" notify to detect when montage loop completes
	// This is because ladder montages don't auto-blend-out, so OnCompleted never fires
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageNotifyBegin - NotifyName: '%s', ClimbAnimState: %d, bIsClimbingOffTop: %s, bIsOnGround: %s"),
		*NotifyName.ToString(), static_cast<int32>(ClimbAnimState),
		bIsClimbingOffTop ? TEXT("true") : TEXT("false"),
		bIsOnGround ? TEXT("true") : TEXT("false"));

	// Check for "Loop_End" OR "Climb_End" notify - different montages may use different names
	bool bIsEndNotify = (NotifyName == FName(TEXT("Loop_End")) ||
	                     NotifyName == FName(TEXT("Climb_End")) ||
	                     NotifyName == FName(TEXT("End")));

	if (bIsEndNotify)
	{
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageNotifyBegin - End notify detected! (%s)"), *NotifyName.ToString());

		// Handle state transition based on current state
		if (ClimbAnimState == ESLFLadderClimbState::DismountBottom ||
			ClimbAnimState == ESLFLadderClimbState::DismountTop)
		{
			// DISMOUNT finished - exit ladder completely
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] Loop_End -> Dismount complete, calling StopClimb"));
			StopClimb();
		}
		else if (ClimbAnimState == ESLFLadderClimbState::MountTop ||
				 ClimbAnimState == ESLFLadderClimbState::MountBottom)
		{
			// MOUNT finished - transition to Idle so player can start climbing
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] Loop_End -> Mount complete, transitioning to Idle"));

			// CRITICAL: After MountTop completes, snap character to correct position and rotation
			// The ClimbInFromTop montage's root motion may have displaced the character from the ladder
			// We need to ensure they are positioned correctly for ClimbDown to work
			if (ClimbAnimState == ESLFLadderClimbState::MountTop && CurrentLadder)
			{
				ACharacter* Character = Cast<ACharacter>(GetOwner());
				if (Character)
				{
					FVector OldCharPos = Character->GetActorLocation();
					FVector LadderPos = CurrentLadder->GetActorLocation();
					FRotator LadderRotation = CurrentLadder->GetActorRotation();
					FVector LadderForward = LadderRotation.Vector();

					// Face INTO the ladder (opposite of ladder forward)
					FRotator NewRotation = FRotator(0.0, LadderRotation.Yaw + 180.0, 0.0);
					Character->SetActorRotation(NewRotation);

					// Snap position to be directly in front of the ladder at the current Z height
					// This ensures the character is within range of the ladder's collision boxes
					double ClimbOffset = 50.0;
					FVector NewCharPos = LadderPos + (LadderForward * ClimbOffset);
					NewCharPos.Z = OldCharPos.Z;  // Keep current Z (montage should have set this correctly)
					Character->SetActorLocation(NewCharPos);

					UE_LOG(LogTemp, Log, TEXT("[LadderManager] MountTop complete - Snapped from (%.1f, %.1f, %.1f) to (%.1f, %.1f, %.1f), Yaw: %.1f"),
						OldCharPos.X, OldCharPos.Y, OldCharPos.Z, NewCharPos.X, NewCharPos.Y, NewCharPos.Z, NewRotation.Yaw);
				}
			}

			SetClimbAnimState(ESLFLadderClimbState::Idle);
		}
		else if (ClimbAnimState == ESLFLadderClimbState::ClimbingUp ||
				 ClimbAnimState == ESLFLadderClimbState::ClimbingDown)
		{
			// CLIMB loop finished - transition to Idle
			// bp_only: Zone checks happen BEFORE starting the next animation (in ClimbUp/ClimbDown)
			// NOT here after the animation ends
			ACharacter* Character = Cast<ACharacter>(GetOwner());
			FVector CharPos = Character ? Character->GetActorLocation() : FVector::ZeroVector;
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] Loop_End -> Climb done (CharZ: %.1f), transitioning to Idle"), CharPos.Z);
			SetClimbAnimState(ESLFLadderClimbState::Idle);
		}
	}
}

void ULadderManagerComponent::HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	// Fallback for montages that don't have Loop_End notify (like dismount montages)
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] HandleMontageBlendingOut - Montage: %s, Interrupted: %s, ClimbAnimState: %d"),
		Montage ? *Montage->GetName() : TEXT("null"),
		bInterrupted ? TEXT("true") : TEXT("false"),
		static_cast<int32>(ClimbAnimState));

	if (!bInterrupted)
	{
		// Only handle dismount states here - climb states use Loop_End notify
		if (ClimbAnimState == ESLFLadderClimbState::DismountBottom ||
			ClimbAnimState == ESLFLadderClimbState::DismountTop)
		{
			// DISMOUNT finished - exit ladder completely
			UE_LOG(LogTemp, Log, TEXT("[LadderManager] BlendingOut -> Dismount complete, calling StopClimb"));
			StopClimb();
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// UTILITY [35-39/39]
// ═══════════════════════════════════════════════════════════════════════════════

void ULadderManagerComponent::OrientToLadder_Implementation()
{
	if (!CurrentLadder) return;

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] OrientToLadder - IsClimbingDownFromTop: %s"),
		bIsClimbingDownFromTop ? TEXT("true") : TEXT("false"));

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;

	// Get ladder's transform
	FVector LadderLocation = CurrentLadder->GetActorLocation();
	FRotator LadderRotation = CurrentLadder->GetActorRotation();
	FVector LadderForward = LadderRotation.Vector();

	FRotator TargetRotation;
	FVector TargetLocation;
	double ClimbOffset = 50.0;  // Distance in front of ladder to stand

	// Position character in front of the ladder
	TargetLocation = LadderLocation + (LadderForward * ClimbOffset);

	// Keep the character's current Z (vertical position) - mounting animation handles Z
	TargetLocation.Z = Character->GetActorLocation().Z;

	if (bIsClimbingDownFromTop)
	{
		// TOP-DOWN: Character faces AWAY from ladder (same direction as ladder forward)
		// This is the natural position when standing on a ledge looking down at where the ladder goes
		// The MountTop montage will turn them around to face the ladder
		// Don't change position - character stays on ledge, montage handles stepping onto ladder
		TargetRotation = FRotator(0.0, LadderRotation.Yaw, 0.0);
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] OrientToLadder - TopDown: LadderYaw: %.1f, setting CharYaw: %.1f"),
			LadderRotation.Yaw, TargetRotation.Yaw);
		Character->SetActorRotation(TargetRotation);
	}
	else
	{
		// BOTTOM-UP: Character faces INTO the ladder immediately
		TargetRotation = FRotator(0.0, LadderRotation.Yaw + 180.0, 0.0);
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] OrientToLadder - BottomUp: LadderYaw: %.1f, CharYaw: %.1f"),
			LadderRotation.Yaw, TargetRotation.Yaw);
		Character->SetActorRotation(TargetRotation);
		Character->SetActorLocation(TargetLocation);
	}

	SetIsOrientedToLadder(true);
}

void ULadderManagerComponent::AttachToLadder_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] AttachToLadder"));

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Character->GetCharacterMovement())
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();

	// Save original rotation settings to restore later
	bSavedUseControllerRotationYaw = Character->bUseControllerRotationYaw;
	bSavedOrientRotationToMovement = MoveComp->bOrientRotationToMovement;

	// Stop all current movement
	MoveComp->StopMovementImmediately();
	MoveComp->Velocity = FVector::ZeroVector;

	// Use Flying mode - this completely disables gravity and ground checks
	// NOTE: Do NOT set MaxFlySpeed=0 - this blocks root motion from moving the character
	// bp_only uses Flying mode without restricting MaxFlySpeed, relying on BrakingDeceleration
	MoveComp->SetMovementMode(EMovementMode::MOVE_Flying);
	MoveComp->GravityScale = 0.0f;
	MoveComp->BrakingDecelerationFlying = 5000.0f;  // Matches bp_only - high decel prevents WASD drift

	// CRITICAL: Disable ALL rotation influences while climbing
	Character->bUseControllerRotationYaw = false;
	Character->bUseControllerRotationPitch = false;
	Character->bUseControllerRotationRoll = false;
	MoveComp->bOrientRotationToMovement = false;

	// CRITICAL: Set IsOnLadder in AC_ActionManager to block all other actions (dodge, attack, etc.)
	if (UAC_ActionManager* ActionManager = Character->FindComponentByClass<UAC_ActionManager>())
	{
		ActionManager->IsOnLadder = true;
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] AttachToLadder - Set IsOnLadder=true (actions blocked)"));
	}

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] AttachToLadder - Flying mode, MaxFlySpeed=0, rotation disabled"));
}

void ULadderManagerComponent::DetachFromLadder_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[LadderManager] DetachFromLadder"));

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !Character->GetCharacterMovement())
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();

	// Restore normal movement settings
	MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
	MoveComp->GravityScale = 1.0f;
	MoveComp->GroundFriction = 8.0f;  // Restore default
	MoveComp->BrakingDecelerationWalking = 2048.0f;  // Restore default
	MoveComp->MaxWalkSpeed = 600.0f;  // Restore default (typical UE value)
	MoveComp->MaxFlySpeed = 600.0f;  // Restore default (in case Flying mode is used later)

	// Restore saved rotation settings
	Character->bUseControllerRotationYaw = bSavedUseControllerRotationYaw;
	Character->bUseControllerRotationPitch = false;  // Usually false
	Character->bUseControllerRotationRoll = false;  // Usually false
	MoveComp->bOrientRotationToMovement = bSavedOrientRotationToMovement;

	// CRITICAL: Clear IsOnLadder in AC_ActionManager to re-enable all actions
	if (UAC_ActionManager* ActionManager = Character->FindComponentByClass<UAC_ActionManager>())
	{
		ActionManager->IsOnLadder = false;
		UE_LOG(LogTemp, Log, TEXT("[LadderManager] DetachFromLadder - Set IsOnLadder=false (actions enabled)"));
	}

	UE_LOG(LogTemp, Log, TEXT("[LadderManager] DetachFromLadder - Movement restored, Walking mode, rotation settings restored"));
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
