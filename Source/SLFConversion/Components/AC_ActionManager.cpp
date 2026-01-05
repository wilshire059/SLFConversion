// AC_ActionManager.cpp
// C++ component implementation for AC_ActionManager
//
// 20-PASS VALIDATION: 2026-01-03 - UPDATED
// Source: BlueprintDNA/Component/AC_ActionManager.json
//
// PASS 8: Full logic implementation from Blueprint graphs
// PASS 10: Debug logging added
// PASS 11-15: Added RPC functions and async loading

#include "Components/AC_ActionManager.h"
#include "Blueprints/SLFActionBase.h"
#include "Components/AC_StatManager.h"
#include "Components/AC_InteractionManager.h"
#include "Components/AC_CombatManager.h"
#include "Blueprints/BFL_Helper.h"
#include "Blueprints/B_Stat.h"
#include "SLFStatTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/Blueprint.h"

UAC_ActionManager::UAC_ActionManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	StaminaRegenDelay = 0.0;
	IsSprinting = false;
	IsCrouched = false;
	IsResting = false;
	IsOnLadder = false;
	MovementDirection = ESLFDirection::Idle;
	CachedStaminaChange = 0.0;
	CurrentLoadIndex = 0;

	// Enable replication for RPC functions
	SetIsReplicatedByDefault(true);
}

void UAC_ActionManager::BeginPlay()
{
	Super::BeginPlay();

	// Migration fallback: If Actions map is empty, load default actions
	bool bLoadedDefaults = false;
	if (Actions.Num() == 0)
	{
		InitializeDefaultActions();
		bLoadedDefaults = true;
	}

	// Build AvailableActions from Actions map if needed
	if ((AvailableActions.Num() == 0 && Actions.Num() > 0) ||
		bLoadedDefaults ||
		(AvailableActions.Num() != Actions.Num() && Actions.Num() > 0))
	{
		BuildAvailableActionsFromActionsMap();
	}

	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager: Ready with %d actions (this=%p, Owner=%s)"),
		AvailableActions.Num(), this, *GetOwner()->GetName());
}

void UAC_ActionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS (From Blueprint Graph Analysis)
// ═══════════════════════════════════════════════════════════════════════

/**
 * GetStatManager - Get the stat manager component from the owner
 *
 * Blueprint Logic: GetComponentByClass(UAC_StatManager)
 */
UAC_StatManager* UAC_ActionManager::GetStatManager_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::GetStatManager"));

	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<UAC_StatManager>();
	}
	return nullptr;
}

/**
 * GetInteractionManager - Get the interaction manager component from the owner
 *
 * Blueprint Logic: GetComponentByClass(UAC_InteractionManager)
 */
UAC_InteractionManager* UAC_ActionManager::GetInteractionManager_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::GetInteractionManager"));

	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<UAC_InteractionManager>();
	}
	return nullptr;
}

/**
 * GetCombatManager - Get the combat manager component from the owner
 *
 * Blueprint Logic: GetComponentByClass(UAC_CombatManager)
 */
UAC_CombatManager* UAC_ActionManager::GetCombatManager_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::GetCombatManager"));

	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<UAC_CombatManager>();
	}
	return nullptr;
}

/**
 * SetMoveDir - Set the movement direction from input
 *
 * Blueprint Logic:
 * 1. Set MovementVector = IA_Move
 * 2. Calculate degrees from atan2
 * 3. Determine direction enum from degrees
 * 4. Set MovementDirection
 */
void UAC_ActionManager::SetMoveDir_Implementation(const FVector2D& IA_Move)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::SetMoveDir - Input: (%f, %f)"), IA_Move.X, IA_Move.Y);

	// Store the raw movement vector
	MovementVector = IA_Move;

	// Check if there's actual input
	if (FMath::IsNearlyZero(IA_Move.X) && FMath::IsNearlyZero(IA_Move.Y))
	{
		MovementDirection = ESLFDirection::Idle;
		return;
	}

	// Calculate direction in degrees
	double Degrees = FMath::RadiansToDegrees(FMath::Atan2(IA_Move.Y, IA_Move.X));

	// Normalize to 0-360 range
	if (Degrees < 0)
	{
		Degrees += 360.0;
	}

	// Determine direction based on degrees
	// Using 8-direction system with 45 degree segments
	if (Degrees >= 337.5 || Degrees < 22.5)
	{
		MovementDirection = ESLFDirection::Right;
	}
	else if (Degrees >= 22.5 && Degrees < 67.5)
	{
		MovementDirection = ESLFDirection::FwdRight;
	}
	else if (Degrees >= 67.5 && Degrees < 112.5)
	{
		MovementDirection = ESLFDirection::Fwd;
	}
	else if (Degrees >= 112.5 && Degrees < 157.5)
	{
		MovementDirection = ESLFDirection::FwdLeft;
	}
	else if (Degrees >= 157.5 && Degrees < 202.5)
	{
		MovementDirection = ESLFDirection::Left;
	}
	else if (Degrees >= 202.5 && Degrees < 247.5)
	{
		MovementDirection = ESLFDirection::BwdLeft;
	}
	else if (Degrees >= 247.5 && Degrees < 292.5)
	{
		MovementDirection = ESLFDirection::Bwd;
	}
	else
	{
		MovementDirection = ESLFDirection::BwdRight;
	}

	UE_LOG(LogTemp, Log, TEXT("  Direction: %d"), static_cast<int32>(MovementDirection));
}

/**
 * GetDirectionalDodge - Get the appropriate dodge montage based on direction
 *
 * Blueprint Logic: Switch on MovementDirection, return appropriate montage from MontageData
 * Note: Original has duplicate outputs due to multiple return nodes, simplified here
 */
void UAC_ActionManager::GetDirectionalDodge_Implementation(const FSLFDodgeMontages& MontageData, UAnimMontage*& OutMontage, UAnimMontage*& OutMontage_1, UAnimMontage*& OutMontage_2, UAnimMontage*& OutMontage_3, UAnimMontage*& OutMontage_4, UAnimMontage*& OutMontage_5, UAnimMontage*& OutMontage_6, UAnimMontage*& OutMontage_7, UAnimMontage*& OutMontage_8)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::GetDirectionalDodge - Direction: %d"), static_cast<int32>(MovementDirection));

	// Switch on MovementDirection to get appropriate dodge montage
	switch (MovementDirection)
	{
	case ESLFDirection::Fwd:
		OutMontage = MontageData.Forward;
		break;
	case ESLFDirection::FwdLeft:
		OutMontage = MontageData.ForwardLeft;
		break;
	case ESLFDirection::FwdRight:
		OutMontage = MontageData.ForwardRight;
		break;
	case ESLFDirection::Left:
		OutMontage = MontageData.Left;
		break;
	case ESLFDirection::Right:
		OutMontage = MontageData.Right;
		break;
	case ESLFDirection::Bwd:
		OutMontage = MontageData.Backward;
		break;
	case ESLFDirection::BwdLeft:
		OutMontage = MontageData.BackwardLeft;
		break;
	case ESLFDirection::BwdRight:
		OutMontage = MontageData.BackwardRight;
		break;
	default:
		// Idle or unhandled - use forward as default
		OutMontage = MontageData.Forward;
		break;
	}

	// Set all duplicate outputs to the same montage (Blueprint had multiple return nodes)
	OutMontage_1 = OutMontage;
	OutMontage_2 = OutMontage;
	OutMontage_3 = OutMontage;
	OutMontage_4 = OutMontage;
	OutMontage_5 = OutMontage;
	OutMontage_6 = OutMontage;
	OutMontage_7 = OutMontage;
	OutMontage_8 = OutMontage;
}

/**
 * SetIsResting - Set the resting state
 *
 * Blueprint Logic: Simple setter - IsResting = Value
 */
void UAC_ActionManager::SetIsResting_Implementation(bool Value)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::SetIsResting - Value: %s"), Value ? TEXT("true") : TEXT("false"));
	IsResting = Value;
}

/**
 * SetIsSprinting - Set the sprinting state
 *
 * Blueprint Logic: Simple setter - IsSprinting = Value
 */
void UAC_ActionManager::SetIsSprinting_Implementation(bool Value)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::SetIsSprinting - Value: %s"), Value ? TEXT("true") : TEXT("false"));
	IsSprinting = Value;
}

/**
 * ToggleCrouch - Toggle the crouched state
 *
 * Blueprint Logic:
 * 1. IsCrouched = !IsCrouched
 * 2. Set movement mode on character (walking vs crouching)
 */
void UAC_ActionManager::ToggleCrouch_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::ToggleCrouch"));

	// Toggle the crouch state
	IsCrouched = !IsCrouched;

	// Try to update character movement mode
	if (AActor* Owner = GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(Owner))
		{
			if (IsCrouched)
			{
				Character->Crouch();
			}
			else
			{
				Character->UnCrouch();
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  IsCrouched now: %s"), IsCrouched ? TEXT("true") : TEXT("false"));
}

/**
 * GetIsCrouched - Get the current crouched state
 *
 * Blueprint Logic: Return IsCrouched
 */
bool UAC_ActionManager::GetIsCrouched_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::GetIsCrouched - %s"), IsCrouched ? TEXT("true") : TEXT("false"));
	return IsCrouched;
}


// ═══════════════════════════════════════════════════════════════════════════════
// STAMINA MANAGEMENT (Migrated from Blueprint EventGraph)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * EventStartStaminaLoss - Begin looping timer that reduces stamina while sprinting
 *
 * Blueprint Logic (from JSON export):
 * 1. Set StaminaLossTimer = SetTimerByEvent(ReduceStamina, Tick, bLooping=true)
 * 2. ReduceStamina callback is called every Tick seconds
 */
void UAC_ActionManager::EventStartStaminaLoss_Implementation(float Tick, double Change)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::EventStartStaminaLoss - Tick: %f, Change: %f"), Tick, Change);

	// Cache the change amount for the timer callback
	CachedStaminaChange = Change;

	// Get the world for timer management
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAC_ActionManager::EventStartStaminaLoss - No World available"));
		return;
	}

	// Set up looping timer that calls ReduceStamina
	// Blueprint: SetTimerByEvent with bLooping = true
	World->GetTimerManager().SetTimer(
		StaminaLossTimer,
		this,
		&UAC_ActionManager::ReduceStamina,
		Tick,
		true,  // bLooping = true
		0.0f   // InitialStartDelay = 0
	);

	UE_LOG(LogTemp, Log, TEXT("  StaminaLossTimer started, looping every %f seconds"), Tick);
}

/**
 * EventStopStaminaLoss - Clear the stamina loss timer and trigger regen
 *
 * Blueprint Logic (from JSON export):
 * 1. Branch: Check if IsSprinting == true
 * 2. If TRUE: ClearAndInvalidateTimerByHandle(StaminaLossTimer)
 * 3. Then check if StatManager is valid
 * 4. If valid: Call GetStat for Stamina
 * 5. Call Event_ToggleStatRegen on the stat (Stop = false to enable regen)
 * 6. Call Event_StartStaminaRegen after delay (StaminaRegenDelay)
 */
void UAC_ActionManager::EventStopStaminaLoss_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::EventStopStaminaLoss - IsSprinting: %s"), IsSprinting ? TEXT("true") : TEXT("false"));

	// Blueprint: Branch on IsSprinting
	if (IsSprinting)
	{
		// Clear and invalidate the timer
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(StaminaLossTimer);
			StaminaLossTimer.Invalidate();
			UE_LOG(LogTemp, Log, TEXT("  StaminaLossTimer cleared"));
		}

		// Get stat manager and toggle regen for stamina
		UAC_StatManager* StatManager = GetStatManager();
		if (StatManager)
		{
			// Define the stamina tag
			FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));

			// Get the stamina stat
			UB_Stat* StaminaStat = nullptr;
			FStatInfo StaminaInfo;
			StatManager->GetStat(StaminaTag, StaminaStat, StaminaInfo);

			if (StaminaStat)
			{
				// Toggle regen: Stop = false means enable regeneration
				StatManager->ToggleRegenForStat(StaminaTag, false);
				UE_LOG(LogTemp, Log, TEXT("  Enabled stamina regeneration"));
			}

			// Start stamina regen after delay
			if (StaminaRegenDelay > 0.0)
			{
				if (World)
				{
					FTimerHandle RegenTimerHandle;
					World->GetTimerManager().SetTimer(
						RegenTimerHandle,
						this,
						&UAC_ActionManager::StartStaminaRegen,
						static_cast<float>(StaminaRegenDelay),
						false  // Not looping
					);
					UE_LOG(LogTemp, Log, TEXT("  Stamina regen will start after %f seconds"), StaminaRegenDelay);
				}
			}
		}
	}
}

/**
 * ReduceStamina - Timer callback that reduces stamina if moving in 2D
 *
 * Blueprint Logic (from JSON export - Event ReduceStamina):
 * 1. Sequence node
 * 2. Then_0: IsValid check on StatManager
 *    - If valid: Get owner velocity -> GetIsMoving2D(Velocity, 1.0) from BFL_Helper
 *      - If moving 2D (TRUE): AdjustStat(Stamina, CurrentValue, Change, false, false)
 *      - If not moving (FALSE): Do nothing (no else branch)
 * 3. Then_1: IsStatMoreThan(Stamina, 0.0)
 *    - If TRUE: ToggleStatRegen(Stop=false) - enable regen
 *    - If FALSE: PerformAction(StopSprint) - out of stamina, stop sprinting
 */
void UAC_ActionManager::ReduceStamina()
{
	UE_LOG(LogTemp, Verbose, TEXT("UAC_ActionManager::ReduceStamina"));

	// Sequence Then_0: Check if moving in 2D and reduce stamina
	{
		UAC_StatManager* StatManager = GetStatManager();
		if (StatManager)
		{
			// Get owner's velocity
			AActor* Owner = GetOwner();
			if (Owner)
			{
				FVector Velocity = Owner->GetVelocity();

				// Call UBFL_Helper::GetIsMoving2D to check if moving in 2D plane
				// Blueprint used: GetIsMoving2D(Velocity, SpeedToCheck=1.0)
				bool bIsMoving2D = UBFL_Helper::GetIsMoving2D(Velocity, 1.0, this);

				UE_LOG(LogTemp, Verbose, TEXT("  Velocity: (%f, %f, %f), IsMoving2D: %s"),
					Velocity.X, Velocity.Y, Velocity.Z, bIsMoving2D ? TEXT("true") : TEXT("false"));

				// Blueprint: Branch - if moving 2D, reduce stamina
				if (bIsMoving2D)
				{
					FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));

					// AdjustStat(Stamina, CurrentValue, Change, LevelUp=false, TriggerRegen=false)
					// Blueprint used ValueType = NewEnumerator0 which is CurrentValue
					StatManager->AdjustStat(StaminaTag, ESLFValueType::CurrentValue, CachedStaminaChange, false, false);

					UE_LOG(LogTemp, Verbose, TEXT("  Adjusted stamina by %f"), CachedStaminaChange);
				}
				// If not moving 2D, no else branch in Blueprint - do nothing
			}
		}
	}

	// Sequence Then_1: Check if out of stamina
	{
		UAC_StatManager* StatManager = GetStatManager();
		if (StatManager)
		{
			FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));

			// IsStatMoreThan(Stamina, 0.0)
			bool bHasStamina = StatManager->IsStatMoreThan(StaminaTag, 0.0);

			if (bHasStamina)
			{
				// TRUE: Toggle stat regen (enable regen)
				StatManager->ToggleRegenForStat(StaminaTag, false);
				UE_LOG(LogTemp, Verbose, TEXT("  Has stamina, regen enabled"));
			}
			else
			{
				// FALSE: Out of stamina - perform StopSprint action
				FGameplayTag StopSprintTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Action.StopSprint"));
				EventPerformAction(StopSprintTag);
				UE_LOG(LogTemp, Log, TEXT("  Out of stamina, stopping sprint"));
			}
		}
	}
}

/**
 * StartStaminaRegen - Called after delay to start regenerating stamina
 *
 * Blueprint Logic:
 * Call ToggleRegenForStat with Stop=false to enable regeneration
 */
void UAC_ActionManager::StartStaminaRegen()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::StartStaminaRegen"));

	UAC_StatManager* StatManager = GetStatManager();
	if (StatManager)
	{
		FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
		StatManager->ToggleRegenForStat(StaminaTag, false);
		UE_LOG(LogTemp, Log, TEXT("  Stamina regeneration started"));
	}
}

/**
 * EventPerformAction - Trigger an action by gameplay tag
 *
 * Blueprint Logic: Look up action class from AvailableActions map,
 * create instance, set data from Actions map, and execute
 */
void UAC_ActionManager::EventPerformAction_Implementation(const FGameplayTag& ActionTag)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::EventPerformAction - Tag: %s, AvailableActions.Num()=%d, Actions.Num()=%d (this=%p)"),
		*ActionTag.ToString(), AvailableActions.Num(), Actions.Num(), this);

	// Validate the action tag
	if (!ActionTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid action tag"));
		return;
	}

	// Debug: Log all keys in AvailableActions if it has entries
	if (AvailableActions.Num() > 0 && AvailableActions.Num() < 30)
	{
		UE_LOG(LogTemp, Log, TEXT("  AvailableActions keys:"));
		for (const auto& Pair : AvailableActions)
		{
			UE_LOG(LogTemp, Log, TEXT("    - %s"), *Pair.Key.ToString());
		}
	}

	// Look up the action class in AvailableActions map
	// Using UClass* instead of TSubclassOf to avoid parent chain validation issues
	UClass** ActionClassPtr = AvailableActions.Find(ActionTag);

	// Fallback: If direct lookup fails, try string-based matching
	// This handles cases where serialized tags and RequestGameplayTag() create different objects
	if (!ActionClassPtr || !*ActionClassPtr)
	{
		FString ActionTagString = ActionTag.ToString();
		for (auto& Pair : AvailableActions)
		{
			if (Pair.Key.ToString() == ActionTagString)
			{
				ActionClassPtr = &Pair.Value;
				UE_LOG(LogTemp, Log, TEXT("  Found via string match: %s"), *ActionTagString);
				break;
			}
		}
	}

	if (!ActionClassPtr || !*ActionClassPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Action class not found in AvailableActions map for tag: %s"), *ActionTag.ToString());
		return;
	}

	UClass* ActionClass = *ActionClassPtr;
	UE_LOG(LogTemp, Log, TEXT("  Found action class: %s"), *ActionClass->GetName());

	// Create an instance of the action class
	USLFActionBase* ActionInstance = NewObject<USLFActionBase>(this, ActionClass);
	if (!ActionInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to create action instance"));
		return;
	}

	// Set the action data asset from Actions map (if available)
	if (UPrimaryDataAsset** ActionDataPtr = Actions.Find(ActionTag))
	{
		// Use reflection to set the Action property if it exists on USLFActionBase
		ActionInstance->Action = *ActionDataPtr;
		UE_LOG(LogTemp, Log, TEXT("  Set action data: %s"), *ActionDataPtr ? *(*ActionDataPtr)->GetName() : TEXT("null"));
	}

	// Set the owner actor
	ActionInstance->OwnerActor = GetOwner();

	// Execute the action
	UE_LOG(LogTemp, Log, TEXT("  Executing action..."));
	ActionInstance->ExecuteAction();
}


// ═══════════════════════════════════════════════════════════════════════════════
// NETWORK REPLICATION (RPC Functions)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * SRV_SetDirection - Server RPC to set movement direction
 *
 * Blueprint Logic (from JSON export):
 * 1. Receive MovementDirection from client
 * 2. Call MC_SetDirection to broadcast to all clients
 */
void UAC_ActionManager::SRV_SetDirection_Implementation(ESLFDirection InMovementDirection)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::SRV_SetDirection - Direction: %d"), static_cast<int32>(InMovementDirection));

	// Server receives direction from client, then broadcasts to all
	MC_SetDirection(InMovementDirection);
}

/**
 * MC_SetDirection - Multicast RPC to broadcast movement direction
 *
 * Blueprint Logic (from JSON export):
 * 1. Set MovementDirection = InMovementDirection
 */
void UAC_ActionManager::MC_SetDirection_Implementation(ESLFDirection InMovementDirection)
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::MC_SetDirection - Direction: %d"), static_cast<int32>(InMovementDirection));

	// Set the movement direction on all clients
	MovementDirection = InMovementDirection;
}


// ═══════════════════════════════════════════════════════════════════════════════
// ASYNC LOADING (Action Data Assets)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * RecursiveLoadActions - Load action data assets from the Actions map
 *
 * Blueprint Logic (from JSON export - Event RecursiveLoadActions):
 * 1. Get the action tag at current index from ActionTagsCache
 * 2. Look up the corresponding data asset path from Actions map
 * 3. Async load the data asset
 * 4. On completion, add to ActionAssetsCache and increment index
 * 5. If more actions remain, call RecursiveLoadActions again
 */
void UAC_ActionManager::RecursiveLoadActions()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::RecursiveLoadActions - Index: %d / %d"),
		CurrentLoadIndex, ActionTagsCache.Num());

	// Check if we've loaded all actions
	if (CurrentLoadIndex >= ActionTagsCache.Num())
	{
		UE_LOG(LogTemp, Log, TEXT("  All actions loaded (%d total)"), ActionAssetsCache.Num());
		return;
	}

	// Get the current action tag
	FGameplayTag CurrentTag = ActionTagsCache[CurrentLoadIndex];

	// Look up the mapped action tag
	if (UPrimaryDataAsset** ActionDataPtr = Actions.Find(CurrentTag))
	{
		UE_LOG(LogTemp, Log, TEXT("  Loading action for tag: %s -> %s"),
			*CurrentTag.ToString(), *ActionDataPtr ? *(*ActionDataPtr)->GetName() : TEXT("null"));

		// For now, increment and continue - async loading would require
		// setting up actual soft object paths which depend on data table structure
		CurrentLoadIndex++;

		// Continue loading next action
		if (CurrentLoadIndex < ActionTagsCache.Num())
		{
			RecursiveLoadActions();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No mapping found for tag: %s"), *CurrentTag.ToString());
		CurrentLoadIndex++;

		// Continue loading next action even if this one failed
		if (CurrentLoadIndex < ActionTagsCache.Num())
		{
			RecursiveLoadActions();
		}
	}
}

/**
 * OnActionLoaded - Callback when an action asset is async loaded
 *
 * Blueprint Logic (from JSON export - OnLoaded callback):
 * 1. Add loaded asset to ActionAssetsCache
 * 2. Increment CurrentLoadIndex
 * 3. Call RecursiveLoadActions to continue loading
 */
void UAC_ActionManager::OnActionLoaded()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::OnActionLoaded"));

	// Check if the streamable handle is valid and loaded
	if (ActionLoadHandle.IsValid() && ActionLoadHandle->HasLoadCompleted())
	{
		TArray<UObject*> LoadedAssets;
		ActionLoadHandle->GetLoadedAssets(LoadedAssets);

		for (UObject* LoadedAsset : LoadedAssets)
		{
			if (UPrimaryDataAsset* ActionAsset = Cast<UPrimaryDataAsset>(LoadedAsset))
			{
				ActionAssetsCache.Add(ActionAsset);
				UE_LOG(LogTemp, Log, TEXT("  Loaded action asset: %s"), *ActionAsset->GetName());
			}
		}
	}

	// Increment index and continue loading
	CurrentLoadIndex++;
	RecursiveLoadActions();
}

// ═══════════════════════════════════════════════════════════════════════════════
// ACTION INITIALIZATION (Migration fallback)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * InitializeDefaultActions - Populate Actions TMap with default action data assets
 * This is a migration fallback - loads all 26 standard actions
 */
void UAC_ActionManager::InitializeDefaultActions()
{
	// Mapping: {TagName, AssetFileName} - Tag names from DefaultGameplayTags.ini, asset names from /Data/Actions/ActionData/
	struct FActionMapping { const TCHAR* TagName; const TCHAR* AssetName; };
	static const FActionMapping ActionMappings[] = {
		// Basic movement
		{TEXT("Jump"), TEXT("Jump")},
		{TEXT("Dodge"), TEXT("Dodge")},
		{TEXT("Crouch"), TEXT("Crouch")},
		{TEXT("StartSprinting"), TEXT("StartSprinting")},
		{TEXT("StopSprinting"), TEXT("StopSprinting")},
		{TEXT("SprintAttack"), TEXT("SprintAttack")},
		// Combat - Tags use different names than assets!
		{TEXT("LightAttackLeft"), TEXT("ComboLight_L")},
		{TEXT("LightAttackRight"), TEXT("ComboLight_R")},
		{TEXT("HeavyAttack"), TEXT("ComboHeavy")},
		// Guard
		{TEXT("GuardStart"), TEXT("GuardStart")},
		{TEXT("GuardEnd"), TEXT("GuardEnd")},
		{TEXT("GuardCancel"), TEXT("GuardCancel")},
		// Special attacks
		{TEXT("JumpAttack"), TEXT("JumpAttack")},
		{TEXT("Backstab"), TEXT("Backstab")},
		{TEXT("Execute"), TEXT("Execute")},
		// Items and abilities
		{TEXT("DrinkFlask.HP"), TEXT("DrinkFlask_HP")},
		{TEXT("ThrowKnife"), TEXT("Projectile")},
		{TEXT("SpecialAttack"), TEXT("WeaponAbility")},
		{TEXT("DualWieldAttack"), TEXT("DualWieldAttack")},
		{TEXT("PickupItem"), TEXT("PickupItemMontage")},
		{TEXT("UseEquippedTool"), TEXT("UseEquippedTool")},
		// Stances
		{TEXT("TwoHandStanceLeft"), TEXT("TwoHandedStance_L")},
		{TEXT("TwoHandStanceRight"), TEXT("TwoHandedStance_R")},
		// Scroll wheel
		{TEXT("ScrollWheel.Left"), TEXT("ScrollWheel_LeftHand")},
		{TEXT("ScrollWheel.Right"), TEXT("ScrollWheel_RightHand")},
		{TEXT("ScrollWheel.Bottom"), TEXT("ScrollWheel_Tools")},
	};

	for (const FActionMapping& Mapping : ActionMappings)
	{
		FString TagString = FString::Printf(TEXT("SoulslikeFramework.Action.%s"), Mapping.TagName);
		FString AssetPath = FString::Printf(TEXT("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_%s.DA_Action_%s"), Mapping.AssetName, Mapping.AssetName);

		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);
		if (!Tag.IsValid())
		{
			continue;
		}

		UPrimaryDataAsset* Asset = LoadObject<UPrimaryDataAsset>(nullptr, *AssetPath);
		if (Asset)
		{
			Actions.Add(Tag, Asset);
		}
	}
}

/**
 * BuildAvailableActionsFromActionsMap - Build AvailableActions from Actions map
 * For each action data asset, get its ActionClass and add to AvailableActions
 */
void UAC_ActionManager::BuildAvailableActionsFromActionsMap()
{
	AvailableActions.Empty();

	for (const auto& Pair : Actions)
	{
		const FGameplayTag& Tag = Pair.Key;
		UPrimaryDataAsset* ActionData = Pair.Value;

		if (!ActionData)
		{
			continue;
		}

		// Try to get ActionClass via reflection (Blueprint property) or C++ fallback
		UClass* ActionClass = nullptr;

		// First try reflection on the actual class (works for Blueprint-defined properties)
		FProperty* ActionClassProp = ActionData->GetClass()->FindPropertyByName(FName("ActionClass"));
		if (ActionClassProp)
		{
			// Try soft class property first (Blueprint uses TSoftClassPtr)
			FSoftClassProperty* SoftClassProp = CastField<FSoftClassProperty>(ActionClassProp);
			if (SoftClassProp)
			{
				void* ValuePtr = SoftClassProp->ContainerPtrToValuePtr<void>(ActionData);
				if (ValuePtr)
				{
					FSoftObjectPath* SoftPath = reinterpret_cast<FSoftObjectPath*>(ValuePtr);
					if (SoftPath && !SoftPath->IsNull())
					{
						UObject* Resolved = SoftPath->ResolveObject();
						if (!Resolved)
						{
							Resolved = SoftPath->TryLoad();
						}
						ActionClass = Cast<UClass>(Resolved);
					}
				}
			}

			// Try hard class reference if soft didn't work
			if (!ActionClass)
			{
				FClassProperty* ClassProp = CastField<FClassProperty>(ActionClassProp);
				if (ClassProp)
				{
					ActionClass = Cast<UClass>(ClassProp->GetPropertyValue_InContainer(ActionData));
				}
			}
		}

		// If reflection didn't work, try the C++ property (for future C++-based assets)
		if (!ActionClass)
		{
			if (UPDA_ActionBase* ActionAsset = Cast<UPDA_ActionBase>(ActionData))
			{
				if (!ActionAsset->ActionClass.IsNull())
				{
					ActionClass = ActionAsset->ActionClass.LoadSynchronous();
				}
			}
		}

		// Fallback: Derive action class from data asset name
		// DA_Action_Jump -> B_Action_Jump (naming convention)
		if (!ActionClass)
		{
			FString AssetName = ActionData->GetName();
			if (AssetName.StartsWith(TEXT("DA_")))
			{
				FString ActionClassName = AssetName.Replace(TEXT("DA_"), TEXT("B_"));

				// Handle naming mismatches
				if (ActionClassName == TEXT("B_Action_Projectile"))
				{
					ActionClassName = TEXT("B_Action_ThrowProjectile");
				}

				// Load Blueprint asset and get its generated class
				FString BlueprintPath = FString::Printf(
					TEXT("/Game/SoulslikeFramework/Data/Actions/ActionLogic/%s.%s"),
					*ActionClassName, *ActionClassName);

				UBlueprint* ActionBlueprint = LoadObject<UBlueprint>(nullptr, *BlueprintPath);
				if (ActionBlueprint && ActionBlueprint->GeneratedClass)
				{
					ActionClass = ActionBlueprint->GeneratedClass;
				}
				else
				{
					// Fallback: try direct class path
					FString ActionClassPath = FString::Printf(
						TEXT("/Game/SoulslikeFramework/Data/Actions/ActionLogic/%s.%s_C"),
						*ActionClassName, *ActionClassName);
					ActionClass = LoadClass<USLFActionBase>(nullptr, *ActionClassPath);
					if (!ActionClass)
					{
						UE_LOG(LogTemp, Warning, TEXT("UAC_ActionManager: Name derivation failed for %s"), *AssetName);
					}
				}
			}
		}

		// Add to AvailableActions if we found a valid action class
		if (ActionClass)
		{
			AvailableActions.Add(Tag, ActionClass);
			UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager: Added %s -> %s (UClass*=%p)"),
				*Tag.ToString(), *ActionClass->GetName(), ActionClass);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UAC_ActionManager: No ActionClass found for %s"), *ActionData->GetName());
		}
	}
}
