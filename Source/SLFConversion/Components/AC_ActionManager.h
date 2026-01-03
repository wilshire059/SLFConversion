// AC_ActionManager.h
// C++ component for AC_ActionManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_ActionManager.json
// Variables: 13 | Functions: 9 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "Engine/StreamableManager.h"

#include "AC_ActionManager.generated.h"

// Forward declarations
class UAC_CombatManager;
class UAC_InteractionManager;
class UAC_StatManager;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UB_Stat;
class UPDA_Action;
class UB_Action;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_ActionManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_ActionManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (13)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double StaminaRegenDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UDataTable* OverrideTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, UPrimaryDataAsset*> Actions;  // UPrimaryDataAsset for Blueprint compatibility
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsSprinting;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsCrouched;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsResting;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsOnLadder;
	// Using UClass* instead of TSubclassOf<UB_Action> to avoid implicit parent class validation
	// that fails for Blueprint classes after migration (parent chain not fully resolved)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, UClass*> AvailableActions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	ESLFDirection MovementDirection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FVector2D MovementVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle StaminaLossTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<UPrimaryDataAsset*> ActionAssetsCache;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<FGameplayTag> ActionTagsCache;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (9)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	UAC_StatManager* GetStatManager();
	virtual UAC_StatManager* GetStatManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	UAC_InteractionManager* GetInteractionManager();
	virtual UAC_InteractionManager* GetInteractionManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	UAC_CombatManager* GetCombatManager();
	virtual UAC_CombatManager* GetCombatManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	void SetMoveDir(const FVector2D& IA_Move);
	virtual void SetMoveDir_Implementation(const FVector2D& IA_Move);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	void GetDirectionalDodge(const FSLFDodgeMontages& MontageData, UAnimMontage*& OutMontage, UAnimMontage*& OutMontage_1, UAnimMontage*& OutMontage_2, UAnimMontage*& OutMontage_3, UAnimMontage*& OutMontage_4, UAnimMontage*& OutMontage_5, UAnimMontage*& OutMontage_6, UAnimMontage*& OutMontage_7, UAnimMontage*& OutMontage_8);
	virtual void GetDirectionalDodge_Implementation(const FSLFDodgeMontages& MontageData, UAnimMontage*& OutMontage, UAnimMontage*& OutMontage_1, UAnimMontage*& OutMontage_2, UAnimMontage*& OutMontage_3, UAnimMontage*& OutMontage_4, UAnimMontage*& OutMontage_5, UAnimMontage*& OutMontage_6, UAnimMontage*& OutMontage_7, UAnimMontage*& OutMontage_8);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	void SetIsResting(bool Value);
	virtual void SetIsResting_Implementation(bool Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	void SetIsSprinting(bool Value);
	virtual void SetIsSprinting_Implementation(bool Value);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	void ToggleCrouch();
	virtual void ToggleCrouch_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager")
	bool GetIsCrouched();
	virtual bool GetIsCrouched_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// STAMINA MANAGEMENT (Migrated from Blueprint EventGraph)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * StartStaminaLoss - Begin looping timer that reduces stamina while sprinting
	 * @param Tick - Timer interval in seconds
	 * @param Change - Amount to adjust stamina each tick (negative for loss)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager|Stamina")
	void EventStartStaminaLoss(float Tick, double Change);
	virtual void EventStartStaminaLoss_Implementation(float Tick, double Change);

	/**
	 * StopStaminaLoss - Clear the stamina loss timer and optionally enable regen
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager|Stamina")
	void EventStopStaminaLoss();
	virtual void EventStopStaminaLoss_Implementation();

	/**
	 * PerformAction - Trigger an action by gameplay tag
	 * @param ActionTag - The gameplay tag identifying the action to perform
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_ActionManager|Actions")
	void EventPerformAction(const FGameplayTag& ActionTag);
	virtual void EventPerformAction_Implementation(const FGameplayTag& ActionTag);

protected:
	/**
	 * ReduceStamina - Timer callback that reduces stamina if owner is moving in 2D
	 * Called on a loop while sprinting - checks velocity and adjusts stamina
	 */
	UFUNCTION()
	void ReduceStamina();

	/**
	 * StartStaminaRegen - Begin regenerating stamina after delay
	 */
	UFUNCTION()
	void StartStaminaRegen();

	/** Cached stamina change amount for timer callback */
	double CachedStaminaChange;

	/** Current index for async loading actions */
	int32 CurrentLoadIndex;

	/** Handle for async asset loading */
	TSharedPtr<FStreamableHandle> ActionLoadHandle;

	// ═══════════════════════════════════════════════════════════════════════
	// NETWORK REPLICATION (RPC Functions)
	// ═══════════════════════════════════════════════════════════════════════

	/** Server RPC to set movement direction */
	UFUNCTION(Server, Reliable, Category = "AC_ActionManager|Network")
	void SRV_SetDirection(ESLFDirection InMovementDirection);
	virtual void SRV_SetDirection_Implementation(ESLFDirection InMovementDirection);

	/** Multicast RPC to broadcast movement direction to all clients */
	UFUNCTION(NetMulticast, Reliable, Category = "AC_ActionManager|Network")
	void MC_SetDirection(ESLFDirection InMovementDirection);
	virtual void MC_SetDirection_Implementation(ESLFDirection InMovementDirection);

	// ═══════════════════════════════════════════════════════════════════════
	// ACTION INITIALIZATION (Migration fallback)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * InitializeDefaultActions - Populate Actions TMap with default action data assets
	 * Called from BeginPlay if Actions is empty (migration fallback)
	 */
	void InitializeDefaultActions();

	/**
	 * BuildAvailableActionsFromActionsMap - Build AvailableActions TMap from Actions
	 * Creates action class instances from action data assets
	 */
	void BuildAvailableActionsFromActionsMap();

	// ═══════════════════════════════════════════════════════════════════════
	// ASYNC LOADING
	// ═══════════════════════════════════════════════════════════════════════

	/** Recursively load action data assets */
	UFUNCTION()
	void RecursiveLoadActions();

	/** Callback when an action asset finishes loading */
	UFUNCTION()
	void OnActionLoaded();
};
