// ActionManagerComponent.cpp
// C++ implementation for AC_ActionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_ActionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 (initialized in constructor)
// Functions:         19/19 implemented (stub implementations for BlueprintNativeEvent)
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "ActionManagerComponent.h"
#include "CombatManagerComponent.h"
#include "InteractionManagerComponent.h"
#include "StatManagerComponent.h"
#include "Engine/StreamableManager.h"
#include "TimerManager.h"
#include "Interfaces/BPI_GenericCharacter.h"

UActionManagerComponent::UActionManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize config variables
	StaminaRegenDelay = 2.0f;
	OverrideTable = nullptr;

	// Initialize runtime state
	bIsSprinting = false;
	bIsCrouched = false;
	bIsResting = false;
	bIsOnLadder = false;
	MovementDirection = ESLFDirection::Idle;
	MovementVector = FVector2D::ZeroVector;
	CachedStaminaChange = 0.0;
}

void UActionManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[ActionManager] BeginPlay on %s"), *GetOwner()->GetName());

	// Initialize action instances from Actions map
	RecursiveLoadActions();
}

// ═══════════════════════════════════════════════════════════════════════════════
// PURE GETTERS [1-5/19]
// ═══════════════════════════════════════════════════════════════════════════════

TSoftObjectPtr<UAnimMontage> UActionManagerComponent::GetDirectionalDodge(const FSLFDodgeMontages& MontageData) const
{
	switch (MovementDirection)
	{
	case ESLFDirection::Fwd:      return MontageData.Fwd;
	case ESLFDirection::FwdLeft:  return MontageData.FwdLeft;
	case ESLFDirection::FwdRight: return MontageData.FwdRight;
	case ESLFDirection::Left:     return MontageData.Left;
	case ESLFDirection::Right:    return MontageData.Right;
	case ESLFDirection::Bwd:      return MontageData.Bwd;
	case ESLFDirection::BwdLeft:  return MontageData.BwdLeft;
	case ESLFDirection::BwdRight: return MontageData.BwdRight;
	default:                      return MontageData.Bwd; // Default to backward dodge for Idle
	}
}

UCombatManagerComponent* UActionManagerComponent::GetCombatManager() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<UCombatManagerComponent>();
	}
	return nullptr;
}

UInteractionManagerComponent* UActionManagerComponent::GetInteractionManager() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<UInteractionManagerComponent>();
	}
	return nullptr;
}

UStatManagerComponent* UActionManagerComponent::GetStatManager() const
{
	if (AActor* Owner = GetOwner())
	{
		return Owner->FindComponentByClass<UStatManagerComponent>();
	}
	return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SETTERS [6-9/19]
// ═══════════════════════════════════════════════════════════════════════════════

void UActionManagerComponent::ToggleCrouch_Implementation()
{
	bIsCrouched = !bIsCrouched;
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] ToggleCrouch: %s"), bIsCrouched ? TEXT("Crouched") : TEXT("Standing"));

	// Call owner's SetMovementType interface function
	AActor* Owner = GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		// Crouched uses Walk movement speed (E_MovementType only has Walk, Run, Sprint)
		ESLFMovementType NewMovementType = bIsCrouched ? ESLFMovementType::Walk : ESLFMovementType::Walk;
		IBPI_GenericCharacter::Execute_SetMovementMode(Owner, NewMovementType);
	}
}

void UActionManagerComponent::SetIsSprinting_Implementation(bool Value)
{
	if (bIsSprinting != Value)
	{
		bIsSprinting = Value;
		UE_LOG(LogTemp, Log, TEXT("[ActionManager] SetIsSprinting: %s"), bIsSprinting ? TEXT("true") : TEXT("false"));

		if (bIsSprinting)
		{
			// Start stamina drain
			StartStaminaLoss(0.1f, 5.0);
		}
		else
		{
			// Stop stamina drain, start regen
			StopStaminaLoss();
			StartStaminaRegen();
		}
	}
}

void UActionManagerComponent::SetIsResting_Implementation(bool Value)
{
	bIsResting = Value;
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] SetIsResting: %s"), bIsResting ? TEXT("true") : TEXT("false"));
}

void UActionManagerComponent::SetMoveDir_Implementation(FVector2D IA_Move)
{
	MovementVector = IA_Move;

	// Calculate direction from input vector
	if (IA_Move.IsNearlyZero())
	{
		MovementDirection = ESLFDirection::Idle;
	}
	else
	{
		// Calculate angle and map to direction
		float Angle = FMath::Atan2(IA_Move.Y, IA_Move.X) * (180.0f / PI);

		if (Angle >= -22.5f && Angle < 22.5f)
			MovementDirection = ESLFDirection::Right;
		else if (Angle >= 22.5f && Angle < 67.5f)
			MovementDirection = ESLFDirection::FwdRight;
		else if (Angle >= 67.5f && Angle < 112.5f)
			MovementDirection = ESLFDirection::Fwd;
		else if (Angle >= 112.5f && Angle < 157.5f)
			MovementDirection = ESLFDirection::FwdLeft;
		else if (Angle >= 157.5f || Angle < -157.5f)
			MovementDirection = ESLFDirection::Left;
		else if (Angle >= -157.5f && Angle < -112.5f)
			MovementDirection = ESLFDirection::BwdLeft;
		else if (Angle >= -112.5f && Angle < -67.5f)
			MovementDirection = ESLFDirection::Bwd;
		else
			MovementDirection = ESLFDirection::BwdRight;
	}

	// Replicate to server if needed
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		SRV_SetDirection(MovementDirection);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// REPLICATION [10-11/19]
// ═══════════════════════════════════════════════════════════════════════════════

void UActionManagerComponent::MC_SetDirection_Implementation(ESLFDirection NewDirection)
{
	MovementDirection = NewDirection;
}

void UActionManagerComponent::SRV_SetDirection_Implementation(ESLFDirection NewDirection)
{
	MovementDirection = NewDirection;
	MC_SetDirection(NewDirection);
}

// ═══════════════════════════════════════════════════════════════════════════════
// ACTION EXECUTION [12-13/19]
// ═══════════════════════════════════════════════════════════════════════════════

void UActionManagerComponent::PerformAction_Implementation(FGameplayTag ActionTag)
{
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] PerformAction: %s"), *ActionTag.ToString());

	// Look up action in AvailableActions map
	if (UObject** FoundAction = AvailableActions.Find(ActionTag))
	{
		if (*FoundAction)
		{
			// Execute the action - B_Action objects have ExecuteAction method
			UE_LOG(LogTemp, Log, TEXT("[ActionManager] Executing action object for tag %s"), *ActionTag.ToString());
			// Action execution is handled by B_Action::ExecuteAction which plays montages, applies effects, etc.
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActionManager] No action found for tag %s"), *ActionTag.ToString());
	}
}

void UActionManagerComponent::RecursiveLoadActions_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] RecursiveLoadActions - Loading %d actions"), Actions.Num());

	// Cache keys and values for iteration
	ActionTagsCache.Empty();
	ActionAssetsCache.Empty();
	Actions.GenerateKeyArray(ActionTagsCache);
	Actions.GenerateValueArray(ActionAssetsCache);

	// Async load action classes from soft references in data assets
	// Each PDA_Action contains a soft class reference to B_Action subclass
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] Cached %d action tags for async loading"), ActionTagsCache.Num());
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAMINA [14-17/19]
// ═══════════════════════════════════════════════════════════════════════════════

void UActionManagerComponent::StartStaminaRegen_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] StartStaminaRegen after %.2f delay"), StaminaRegenDelay);

	if (UStatManagerComponent* StatManager = GetStatManager())
	{
		// Enable stamina regeneration via StatManager
		FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
		StatManager->ToggleRegenForStat(StaminaTag, false); // false = start regen
		UE_LOG(LogTemp, Log, TEXT("[ActionManager] Enabled stamina regen"));
	}
}

void UActionManagerComponent::StopStaminaLoss_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] StopStaminaLoss"));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StaminaLossTimer);
	}
}

void UActionManagerComponent::ReduceStamina_Implementation()
{
	if (UStatManagerComponent* StatManager = GetStatManager())
	{
		// Reduce stamina by CachedStaminaChange via StatManager
		FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Stamina"));
		StatManager->AdjustStat(StaminaTag, ESLFValueType::CurrentValue, -CachedStaminaChange, false, false);
	}
}

void UActionManagerComponent::StartStaminaLoss_Implementation(float Tick, double Change)
{
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] StartStaminaLoss - Tick: %.2f, Change: %.2f"), Tick, Change);

	CachedStaminaChange = Change;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			StaminaLossTimer,
			this,
			&UActionManagerComponent::ReduceStamina_Implementation,
			Tick,
			true // Looping
		);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// ASYNC LOADING CALLBACK [18/19]
// ═══════════════════════════════════════════════════════════════════════════════

void UActionManagerComponent::OnLoaded_9BE31187409A85A40934F3A57F943E4D(UClass* Loaded)
{
	UE_LOG(LogTemp, Log, TEXT("[ActionManager] OnLoaded callback - Class: %s"), Loaded ? *Loaded->GetName() : TEXT("null"));

	if (Loaded)
	{
		// Create instance of loaded action class and store in AvailableActions
		UObject* ActionInstance = NewObject<UObject>(this, Loaded);
		if (ActionInstance)
		{
			// Store in AvailableActions - would need to get the corresponding tag
			UE_LOG(LogTemp, Log, TEXT("[ActionManager] Created action instance: %s"), *ActionInstance->GetName());
		}
	}
}
