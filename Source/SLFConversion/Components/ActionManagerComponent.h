// ActionManagerComponent.h
// C++ base class for AC_ActionManager
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - AC_ActionManager
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         13/13 migrated
// Functions:         19/19 migrated (excluding ExecuteUbergraph)
// Event Dispatchers: 0/0
// Graphs:            10 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager
//
// PURPOSE: Manages combat actions, sprinting, crouching, movement state, stamina

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "ActionManagerComponent.generated.h"

// Forward declarations
class UDataAsset;
class UCombatManagerComponent;
class UInteractionManagerComponent;
class UStatManagerComponent;

// Types used from SLFEnums.h/SLFGameTypes.h:
// - ESLFDirection
// - FSLFDodgeMontages

UCLASS(ClassGroup = (Soulslike), meta = (BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class SLFCONVERSION_API UActionManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UActionManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 13/13 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Config Variables (3) ---

	/** [1/13] Delay before stamina regeneration starts after action */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float StaminaRegenDelay;

	/** [2/13] Data table for action overrides */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataTable* OverrideTable;

	/** [3/13] Map of action tags to action data assets (PDA_Action) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, UDataAsset*> Actions;

	// --- Runtime State Variables (10) ---

	/** [4/13] Whether character is currently sprinting */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsSprinting;

	/** [5/13] Whether character is currently crouched */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsCrouched;

	/** [6/13] Whether character is at a resting point */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsResting;

	/** [7/13] Whether character is on a ladder */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool bIsOnLadder;

	/** [8/13] Map of available action instances (B_Action runtime objects) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UObject*> AvailableActions;

	/** [9/13] Current movement direction */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	ESLFDirection MovementDirection;

	/** [10/13] Current movement input vector */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FVector2D MovementVector;

	/** [11/13] Timer handle for stamina loss during sprint */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FTimerHandle StaminaLossTimer;

	/** [12/13] Cached action data assets for quick lookup */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<UDataAsset*> ActionAssetsCache;

	/** [13/13] Cached action tags for iteration */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FGameplayTag> ActionTagsCache;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 19/19 migrated (excluding ExecuteUbergraph)
	// ═══════════════════════════════════════════════════════════════════

	// --- Pure Getters (4 functions) ---

	/** [1/19] Get whether character is crouched */
	UFUNCTION(BlueprintPure, Category = "Action Manager|Getters")
	bool GetIsCrouched() const { return bIsCrouched; }

	/** [2/19] Get directional dodge montage based on current MovementDirection
	 * @param MontageData - Struct containing all directional dodge montages
	 * @return The appropriate dodge montage for current direction
	 */
	UFUNCTION(BlueprintPure, Category = "Action Manager|Getters")
	TSoftObjectPtr<UAnimMontage> GetDirectionalDodge(const FSLFDodgeMontages& MontageData) const;

	/** [3/19] Get the CombatManager component from owner */
	UFUNCTION(BlueprintPure, Category = "Action Manager|Getters")
	UCombatManagerComponent* GetCombatManager() const;

	/** [4/19] Get the InteractionManager component from owner */
	UFUNCTION(BlueprintPure, Category = "Action Manager|Getters")
	UInteractionManagerComponent* GetInteractionManager() const;

	/** [5/19] Get the StatManager component from owner */
	UFUNCTION(BlueprintPure, Category = "Action Manager|Getters")
	UStatManagerComponent* GetStatManager() const;

	// --- Setters (4 functions) ---

	/** [6/19] Toggle crouch state - flips bIsCrouched and calls owner's SetMovementType */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Setters")
	void ToggleCrouch();
	virtual void ToggleCrouch_Implementation();

	/** [7/19] Set sprinting state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Setters")
	void SetIsSprinting(bool Value);
	virtual void SetIsSprinting_Implementation(bool Value);

	/** [8/19] Set resting state */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Setters")
	void SetIsResting(bool Value);
	virtual void SetIsResting_Implementation(bool Value);

	/** [9/19] Set movement direction from input vector
	 * Converts FVector2D to ESLFDirection using angle calculations
	 * @param IA_Move - Raw input vector from input action
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Setters")
	void SetMoveDir(FVector2D IA_Move);
	virtual void SetMoveDir_Implementation(FVector2D IA_Move);

	// --- Replication Functions (2 functions) ---

	/** [10/19] Multicast RPC - Set direction on all clients */
	UFUNCTION(NetMulticast, Reliable, Category = "Action Manager|Replication")
	void MC_SetDirection(ESLFDirection NewDirection);
	void MC_SetDirection_Implementation(ESLFDirection NewDirection);

	/** [11/19] Server RPC - Set direction on server */
	UFUNCTION(Server, Reliable, Category = "Action Manager|Replication")
	void SRV_SetDirection(ESLFDirection NewDirection);
	void SRV_SetDirection_Implementation(ESLFDirection NewDirection);

	// --- Action Execution (2 functions) ---

	/** [12/19] Perform an action by tag - main action execution entry point
	 * Looks up action in AvailableActions map and calls ExecuteAction
	 * @param ActionTag - Tag identifying the action to perform
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Actions")
	void PerformAction(FGameplayTag ActionTag);
	virtual void PerformAction_Implementation(FGameplayTag ActionTag);

	/** [13/19] Recursively load action classes from soft references
	 * Called during initialization to async load action blueprints
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Actions")
	void RecursiveLoadActions();
	virtual void RecursiveLoadActions_Implementation();

	// --- Stamina Functions (4 functions) ---

	/** [14/19] Start stamina regeneration after delay */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Stamina")
	void StartStaminaRegen();
	virtual void StartStaminaRegen_Implementation();

	/** [15/19] Stop stamina loss timer */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Stamina")
	void StopStaminaLoss();
	virtual void StopStaminaLoss_Implementation();

	/** [16/19] Reduce stamina (called by timer) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Stamina")
	void ReduceStamina();
	virtual void ReduceStamina_Implementation();

	/** [17/19] Start stamina loss with tick interval
	 * @param Tick - Timer tick interval
	 * @param Change - Amount to reduce per tick
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action Manager|Stamina")
	void StartStaminaLoss(float Tick, double Change);
	virtual void StartStaminaLoss_Implementation(float Tick, double Change);

	// --- Async Loading Callback (1 function) ---

	/** [18/19] Callback when async class load completes
	 * Internal function bound to async load delegate
	 * @param Loaded - The loaded class
	 */
	UFUNCTION()
	void OnLoaded_9BE31187409A85A40934F3A57F943E4D(UClass* Loaded);

	// --- Lifecycle (1 function) ---
	// [19/19] ReceiveBeginPlay - handled by BeginPlay() override above

protected:
	/** Internal: Current stamina change amount for timer callback */
	double CachedStaminaChange;
};
