// SLFSoulslikeEnemy.h
// C++ base class for B_Soulslike_Enemy
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Soulslike_Enemy
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 migrated (EnemyId, PatrolPath) + inherited Cache_Rotation
// Functions:         1/1 migrated (CheckSense)
// Event Dispatchers: 1/1 migrated
// Interfaces:        BPI_Enemy (8) FULLY IMPLEMENTED, BPI_Executable (3) FULLY IMPLEMENTED
// Components:        4 (Healthbar, AC_AI_CombatManager, AC_AI_BehaviorManager, NS_Souls)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Enemy
//
// 20-PASS VALIDATION: 2026-01-03 - All interface functions fully implemented
// PURPOSE: Base enemy character - AI perception, attack events
// PARENT: B_BaseCharacter

#pragma once

#include "CoreMinimal.h"
#include "SLFBaseCharacter.h"
#include "Interfaces/BPI_Enemy.h"
#include "Interfaces/BPI_Executable.h"
#include "Interfaces/BPI_EnemyHealthbar.h"
#include "Components/WidgetComponent.h"
#include "Components/TimelineComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SLFGameTypes.h"
#include "SLFSoulslikeEnemy.generated.h"

// Forward declarations
class UAICombatManagerComponent;
class UAIBehaviorManagerComponent;
class ULootDropManagerComponent;
class USLFAIStateMachineComponent;
class ISLFExecutionIndicatorInterface;

// ═══════════════════════════════════════════════════════════════════════════════
// EVENT DISPATCHERS: 1/1 migrated
// ═══════════════════════════════════════════════════════════════════════════════

/** [1/1] Called when attack sequence ends */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyAttackEnd);

/**
 * Soulslike enemy character - AI, perception, combat
 * Implements BPI_Enemy (8 functions) and BPI_Executable (3 functions)
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSoulslikeEnemy : public ASLFBaseCharacter, public IBPI_Enemy, public IBPI_Executable
{
	GENERATED_BODY()

public:
	ASLFSoulslikeEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** Update animation variables on the AnimInstance directly (bypasses broken EventGraph) */
	void UpdateAnimationVariables();

public:
	// ═══════════════════════════════════════════════════════════════════
	// COMPONENTS (from JSON)
	// ═══════════════════════════════════════════════════════════════════

	/** Healthbar widget - named HealthbarWidget to avoid collision with Blueprint SCS "Healthbar" */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> HealthbarWidget;

	/** AI Combat Manager - named CombatManagerComponent to avoid collision with Blueprint SCS "AC_AI_CombatManager" */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAICombatManagerComponent> CombatManagerComponent;

	/** AI Behavior Manager - named BehaviorManagerComponent to avoid collision with Blueprint SCS "AC_AI_BehaviorManager" */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UAIBehaviorManagerComponent> BehaviorManagerComponent;

	/** Souls VFX Niagara component - named SoulsNiagaraComponent to avoid collision with Blueprint SCS "NS_Souls" */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UNiagaraComponent> SoulsNiagaraComponent;

	/** AI State Machine - replaces Behavior Tree for better performance */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USLFAIStateMachineComponent> AIStateMachine;

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Unique identifier for this enemy instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FGuid EnemyId;

	/** Patrol path reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|AI")
	TObjectPtr<AActor> PatrolPath;

	/** Loot drop manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<ULootDropManagerComponent> LootDropManagerComponent;

	// ═══════════════════════════════════════════════════════════════════
	// ROTATION TIMELINE SYSTEM
	// ═══════════════════════════════════════════════════════════════════
	// NOTE: Cache_Rotation is inherited from ASLFBaseCharacter

	/** Whether rotation towards target is active */
	UPROPERTY(BlueprintReadWrite, Category = "Enemy|Rotation")
	bool bRotatingTowardsTarget;

	/** Rotation alpha for lerp (0-1) */
	UPROPERTY(BlueprintReadWrite, Category = "Enemy|Rotation")
	float RotationAlpha;

	/** Duration for rotation */
	UPROPERTY(BlueprintReadWrite, Category = "Enemy|Rotation")
	float RotationDuration;

	/** Rotation timer handle */
	FTimerHandle RotationTimerHandle;

	/** Update rotation towards target each tick */
	void UpdateRotationTowardsTarget();

	// ═══════════════════════════════════════════════════════════════════
	// PERCEPTION DEBOUNCE
	// ═══════════════════════════════════════════════════════════════════

	/** Time when we last entered Combat state - used for debounce */
	UPROPERTY()
	float LastCombatEntryTime;

	/** Minimum time to stay in Combat before allowing perception-based state change */
	static constexpr float CombatDebounceTime = 2.0f;

	// ═══════════════════════════════════════════════════════════════════
	// PERCEPTION HANDLING
	// ═══════════════════════════════════════════════════════════════════

	/** Bind perception callbacks in BeginPlay */
	void SetupPerceptionCallbacks();

	/** Handle perception update from AI controller */
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// ═══════════════════════════════════════════════════════════════════
	// POISE BREAK / EXECUTION INDICATOR HANDLING
	// ═══════════════════════════════════════════════════════════════════

	/**
	 * Handler for OnPoiseBroken event from CombatManagerComponent
	 * bp_only flow (B_Soulslike_Enemy.json lines 10598-10899):
	 * 1. OnPoiseBroken(Broken?) event fires
	 * 2. SetVisibility(ExecutionWidget, bNewVisibility = Broken?)
	 * 3. GetWidget() from ExecutionWidget → ToggleExecutionIcon(Visible = Broken?)
	 */
	UFUNCTION()
	void HandleOnPoiseBroken(bool bBroken);

	/**
	 * Handler for OnDeath event from CombatManagerComponent
	 * bp_only flow: OnDeath -> PickAndSpawnLoot (spawns death currency)
	 */
	UFUNCTION()
	void HandleOnDeath(AActor* Killer);

	/**
	 * Handler for OnItemReadyForSpawn event from LootDropManagerComponent
	 * bp_only flow: Spawns B_DeathCurrency at enemy location with currency amount
	 */
	UFUNCTION()
	void HandleOnItemReadyForSpawn(FSLFLootItem Item);

	// ═══════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEnemyAttackEnd OnAttackEnd;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 1/1 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Check if target is sensed by AI perception
	 * @param Target - Actor to check for sensing
	 * @return True if target is perceived
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy|AI")
	bool CheckSense(AActor* Target);
	virtual bool CheckSense_Implementation(AActor* Target);

	/** Execute an AI ability - picks ability from CombatManager and executes it
	 * Fires OnAttackEnd when ability completes
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy|Combat")
	void PerformAbility();
	virtual void PerformAbility_Implementation();

	// ═══════════════════════════════════════════════════════════════════
	// BPI_ENEMY INTERFACE IMPLEMENTATIONS (8 functions)
	// ═══════════════════════════════════════════════════════════════════

	virtual void GetExecutionMoveToTransform_Implementation(FVector& MoveToLocation, FRotator& Rotation) override;
	virtual void StopRotateTowardsTarget_Implementation() override;
	virtual void RotateTowardsTarget_Implementation(double Duration) override;
	virtual void PickAndSpawnLoot_Implementation() override;
	virtual void GetPatrolPath_Implementation(AActor*& OutPatrolPath) override;
	virtual void DisplayDeathVfx_Implementation(FVector AttractorPosition) override;
	virtual void ToggleHealthbarVisual_Implementation(bool bVisible) override;
	virtual void UpdateEnemyHealth_Implementation(double CurrentValue, double MaxValue, double Change) override;

	// ═══════════════════════════════════════════════════════════════════
	// BPI_EXECUTABLE INTERFACE IMPLEMENTATIONS (3 functions)
	// ═══════════════════════════════════════════════════════════════════

	virtual void OnExecutionStarted_Implementation() override;
	virtual void OnExecuted_Implementation(FGameplayTag ExecutionTag) override;
	virtual void OnBackstabbed_Implementation(FGameplayTag ExecutionTag) override;
};
