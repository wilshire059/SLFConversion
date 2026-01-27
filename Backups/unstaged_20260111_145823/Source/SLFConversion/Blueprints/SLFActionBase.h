// SLFActionBase.h
// C++ base class for B_Action
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_Action
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         2/2 migrated
// Functions:         12/12 migrated (excluding EventGraph)
// Event Dispatchers: 0/0
// Graphs:            13 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/Actions/B_Action
//
// PURPOSE: Base action class - executable player actions (attack, dodge, etc.)
// CHILDREN: B_Action_Dodge, B_Action_Jump, B_Action_ComboLight_L/R, etc.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SLFActionBase.generated.h"

// Forward declarations
class UDataAsset;
class UAnimInstance;
// Use AC_* component classes (the actual components on characters)
class UAC_StatManager;
class UAC_ActionManager;
class UAC_CombatManager;
class UAC_EquipmentManager;
class UAC_InventoryManager;
class UAC_InteractionManager;
class UAC_InputBuffer;

/**
 * Action data struct - matches FActionData in PDA_Action
 */
USTRUCT(BlueprintType)
struct FSLFActionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FGameplayTag ActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	float StaminaCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	bool bRequiresWeapon = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	FGameplayTag RequiredStatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action")
	float RequiredStatValue = 0.0f;
};

/**
 * Base action object - executable player action
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionBase : public UObject
{
	GENERATED_BODY()

public:
	USLFActionBase();

	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 2/2 migrated
	// ═══════════════════════════════════════════════════════════════════

	/** [1/2] Action data asset (PDA_Action) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataAsset* Action;

	/** [2/2] Owner actor (the character performing the action) */
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	AActor* OwnerActor;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 12/12 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Component Getters (7) ---

	/** [1/12] Get equipment manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_EquipmentManager* GetEquipmentManager();
	virtual UAC_EquipmentManager* GetEquipmentManager_Implementation();

	/** [2/12] Get interaction manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_InteractionManager* GetInteractionManager();
	virtual UAC_InteractionManager* GetInteractionManager_Implementation();

	/** [3/12] Get inventory manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_InventoryManager* GetInventoryManager();
	virtual UAC_InventoryManager* GetInventoryManager_Implementation();

	/** [4/12] Get stat manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_StatManager* GetStatManager();
	virtual UAC_StatManager* GetStatManager_Implementation();

	/** [5/12] Get combat manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_CombatManager* GetCombatManager();
	virtual UAC_CombatManager* GetCombatManager_Implementation();

	/** [6/12] Get action manager from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_ActionManager* GetActionManager();
	virtual UAC_ActionManager* GetActionManager_Implementation();

	/** [7/12] Get input buffer from owner */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Components")
	UAC_InputBuffer* GetInputBuffer();
	virtual UAC_InputBuffer* GetInputBuffer_Implementation();

	// --- Animation/Weapon Helpers (3) ---

	/** [8/12] Get stamina multiplier from currently equipped weapon */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Weapon")
	float GetWeaponStaminaMultiplier();
	virtual float GetWeaponStaminaMultiplier_Implementation();

	/** [9/12] Get owner's anim instance */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Animation")
	UAnimInstance* GetOwnerAnimInstance();
	virtual UAnimInstance* GetOwnerAnimInstance_Implementation();

	/** [10/12] Get weapon animset data asset */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Weapon")
	UDataAsset* GetWeaponAnimset();
	virtual UDataAsset* GetWeaponAnimset_Implementation();

	// --- Stat Requirements (2) ---

	/** [11/12] Check if stat requirement is met
	 * @param StatTag - Stat to check
	 * @param RequiredValue - Minimum required value
	 * @return True if requirement is met
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Stats")
	bool CheckStatRequirement(FGameplayTag StatTag, float RequiredValue);
	virtual bool CheckStatRequirement_Implementation(FGameplayTag StatTag, float RequiredValue);

	/** [12/12] Adjust stat by action requirement (consume stamina, etc.)
	 * @param StatTag - Stat to adjust
	 * @param Amount - Amount to subtract
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action|Stats")
	void AdjustStatByRequirement(FGameplayTag StatTag, float Amount);
	virtual void AdjustStatByRequirement_Implementation(FGameplayTag StatTag, float Amount);

	// --- Execution (called by ActionManager) ---

	/** Execute this action - override in child classes */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void ExecuteAction();
	virtual void ExecuteAction_Implementation();

	/** Can this action be executed right now? */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool CanExecuteAction();
	virtual bool CanExecuteAction_Implementation();
};
