// B_Action.h
// C++ class for Blueprint B_Action
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Action.json
// Parent: Object -> UObject
// Variables: 2 | Functions: 12 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "B_Action.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UB_Action : public UObject
{
	GENERATED_BODY()

public:
	UB_Action();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* Action;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	AActor* OwnerActor;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (12)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_EquipmentManager* GetEquipmentManager();
	virtual UAC_EquipmentManager* GetEquipmentManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_InteractionManager* GetInteractionManager();
	virtual UAC_InteractionManager* GetInteractionManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_InventoryManager* GetInventoryManager();
	virtual UAC_InventoryManager* GetInventoryManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_StatManager* GetStatManager();
	virtual UAC_StatManager* GetStatManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_CombatManager* GetCombatManager();
	virtual UAC_CombatManager* GetCombatManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_ActionManager* GetActionManager();
	virtual UAC_ActionManager* GetActionManager_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	UAC_InputBuffer* GetInputBuffer();
	virtual UAC_InputBuffer* GetInputBuffer_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	void GetWeaponStaminaMultiplier(ESLFActionWeaponSlot Type, double& OutMultiplier, double& OutMultiplier1, double& OutMultiplier2, double& OutMultiplier3, double& OutMultiplier4, double& OutMultiplier5);
	virtual void GetWeaponStaminaMultiplier_Implementation(ESLFActionWeaponSlot Type, double& OutMultiplier, double& OutMultiplier1, double& OutMultiplier2, double& OutMultiplier3, double& OutMultiplier4, double& OutMultiplier5);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	void GetOwnerAnimInstance(UAnimInstance*& OutAnimInstance, UAnimInstance*& OutAnimInstance1);
	virtual void GetOwnerAnimInstance_Implementation(UAnimInstance*& OutAnimInstance, UAnimInstance*& OutAnimInstance1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	void CheckStatRequirement(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot, bool& OutSuccess, bool& OutSuccess1);
	virtual void CheckStatRequirement_Implementation(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot, bool& OutSuccess, bool& OutSuccess1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	void AdjustStatByRequirement(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot);
	virtual void AdjustStatByRequirement_Implementation(ESLFActionWeaponSlot StaminaMultiplierWeaponSlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	void GetWeaponAnimset(ESLFActionWeaponSlot WeaponSlot, UPrimaryDataAsset*& OutAnimset, UPrimaryDataAsset*& OutAnimset1);
	virtual void GetWeaponAnimset_Implementation(ESLFActionWeaponSlot WeaponSlot, UPrimaryDataAsset*& OutAnimset, UPrimaryDataAsset*& OutAnimset1);

	/** Execute the action - override in subclasses to implement action logic */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Action")
	void ExecuteAction();
	virtual void ExecuteAction_Implementation();
};
