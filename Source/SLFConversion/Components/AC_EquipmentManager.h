// AC_EquipmentManager.h
// C++ component for AC_EquipmentManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_EquipmentManager.json
// Variables: 19 | Functions: 41 | Dispatchers: 9

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_EquipmentManager.generated.h"

// Forward declarations
class UAC_BuffManager;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;

// Event Dispatcher Declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_EquipmentManager_OnItemEquippedToSlot, FSLFCurrentEquipment, ItemData, FGameplayTag, TargetSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_EquipmentManager_OnItemUnequippedFromSlot, UPrimaryDataAsset*, Item, FGameplayTag, TargetSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAC_EquipmentManager_OnWeaponAbilityUpdated, UPrimaryDataAsset*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_EquipmentManager_OnWeaponAbilityRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_EquipmentManager_OnSaveRequested, FGameplayTag, SaveDataTag, TArray<FInstancedStruct>, EquipmentData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_EquipmentManager_OnWeaponStatCheckFailed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_EquipmentManager_OnItemEffectRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAC_EquipmentManager_OnItemEffectAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAC_EquipmentManager_OnStanceChanged, bool, RightHand, bool, TwoHand);

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_EquipmentManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_EquipmentManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (19)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UDataTable* SlotTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FGameplayTag> AllEquippedItems;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FGameplayTag> HiddenItems;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FGameplayTag> SpawnedItemsAtSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FGameplayTag> SpawnedBuffAtSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime", Replicated)
	ESLFOverlayState ActiveOverlayState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	ESLFWeaponAbilityHandle WeaponAbilitySlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer LeftHandSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer RightHandSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FGameplayTagContainer GrantedTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer ToolSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FGameplayTagContainer RightHandOverlayTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FGameplayTagContainer LeftHandOverlayTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FGameplayTag ActiveToolSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	ESLFOverlayState LeftHandOverlayState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	ESLFOverlayState RightHandOverlayState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAnimSequenceBase* ActiveBlockSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool IsAsyncWeaponBusy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<FSLFEquipmentItemsSaveInfo> Cached_LoadedEquipment;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool bLeftHandTwoHandStance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool bRightHandTwoHandStance;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (9)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnItemEquippedToSlot OnItemEquippedToSlot;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnItemUnequippedFromSlot OnItemUnequippedFromSlot;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnWeaponAbilityUpdated OnWeaponAbilityUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnWeaponAbilityRemoved OnWeaponAbilityRemoved;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnSaveRequested OnSaveRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnWeaponStatCheckFailed OnWeaponStatCheckFailed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnItemEffectRemoved OnItemEffectRemoved;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnItemEffectAdded OnItemEffectAdded;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FAC_EquipmentManager_OnStanceChanged OnStanceChanged;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (41)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	bool IsSlotOccupied(const FGameplayTag& SlotTag);
	virtual bool IsSlotOccupied_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void EquipWeaponToSlot(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3);
	virtual void EquipWeaponToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void EquipArmorToSlot(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3);
	virtual void EquipArmorToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnequipWeaponAtSlot(const FGameplayTag& SlotTag);
	virtual void UnequipWeaponAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnequipArmorAtSlot(const FGameplayTag& SlotTag);
	virtual void UnequipArmorAtSlot_Implementation(const FGameplayTag& SlotTag);

	/**
	 * Apply armor skeletal mesh to character body slot
	 * Uses existing ItemInformation.EquipmentDetails.SkeletalMeshInfo data
	 * Calls ISLFPlayerInterface mesh-swap functions (ChangeArmor, ChangeHeadpiece, etc.)
	 */
	void ApplyArmorMeshToCharacter(UPDA_Item* ArmorItem, const FGameplayTag& SlotTag);

	/**
	 * Remove armor mesh from character body slot (revert to default)
	 * Called from UnequipArmorAtSlot
	 */
	void RemoveArmorMeshFromCharacter(const FGameplayTag& SlotTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void HideItemAtSlot(const FGameplayTag& SlotTag);
	virtual void HideItemAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void IsItemEquipped(const FSLFItemInfo& TargetItem, bool& OutEquipped, FGameplayTag& OutOnSlot);
	virtual void IsItemEquipped_Implementation(const FSLFItemInfo& TargetItem, bool& OutEquipped, FGameplayTag& OutOnSlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	bool IsItemEquippedToSlot(const FSLFItemInfo& TargetItem, const FGameplayTag& ActiveEquipmentSlot);
	virtual bool IsItemEquippedToSlot_Implementation(const FSLFItemInfo& TargetItem, const FGameplayTag& ActiveEquipmentSlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void GetBuffManager(UAC_BuffManager*& OutReturnValue, UAC_BuffManager*& OutReturnValue_1);
	virtual void GetBuffManager_Implementation(UAC_BuffManager*& OutReturnValue, UAC_BuffManager*& OutReturnValue_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void GetSpawnedItemAtSlot(const FGameplayTag& SlotTag, AB_Item*& OutItem, AB_Item*& OutItem_1);
	virtual void GetSpawnedItemAtSlot_Implementation(const FGameplayTag& SlotTag, AB_Item*& OutItem, AB_Item*& OutItem_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	FGameplayTag GetActiveWeaponSlot(bool RightHand);
	virtual FGameplayTag GetActiveWeaponSlot_Implementation(bool RightHand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	FGameplayTag GetActiveToolSlot();
	virtual FGameplayTag GetActiveToolSlot_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void SetActiveToolSlot(const FGameplayTag& InActiveToolSlot);
	virtual void SetActiveToolSlot_Implementation(const FGameplayTag& InActiveToolSlot);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	FGameplayTag GetActiveWheelSlotForTool();
	virtual FGameplayTag GetActiveWheelSlotForTool_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	FGameplayTag GetActiveWheelSlotForWeapon(bool RightHand);
	virtual FGameplayTag GetActiveWheelSlotForWeapon_Implementation(bool RightHand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void ApplyStatChanges(const FSLFItemInfo& Item, bool Equipped);
	virtual void ApplyStatChanges_Implementation(const FSLFItemInfo& Item, bool Equipped);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void GetItemAtSlot(const FGameplayTag& SlotTag, FSLFItemInfo& OutItemData, UPrimaryDataAsset*& OutItemAsset, FGuid& OutId, FSLFItemInfo& OutItemData_1, UPrimaryDataAsset*& OutItemAsset_1, FGuid& OutId_1);
	virtual void GetItemAtSlot_Implementation(const FGameplayTag& SlotTag, FSLFItemInfo& OutItemData, UPrimaryDataAsset*& OutItemAsset, FGuid& OutId, FSLFItemInfo& OutItemData_1, UPrimaryDataAsset*& OutItemAsset_1, FGuid& OutId_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void CheckRequiredStats(const FSLFItemInfo& Item, bool& OutCanEquip, bool& OutCanEquip_1);
	virtual void CheckRequiredStats_Implementation(const FSLFItemInfo& Item, bool& OutCanEquip, bool& OutCanEquip_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void SerializeEquipmentData();
	virtual void SerializeEquipmentData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void EquipItemToSlot(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3, bool& OutSuccess_4, bool& OutSuccess_5, bool& OutSuccess_6);
	virtual void EquipItemToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2, bool& OutSuccess_3, bool& OutSuccess_4, bool& OutSuccess_5, bool& OutSuccess_6);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnequipItemAtSlot(const FGameplayTag& SlotTag);
	virtual void UnequipItemAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	bool EquipTalismanToSlot(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats);
	virtual bool EquipTalismanToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnequipTalismanAtSlot(const FGameplayTag& SlotTag);
	virtual void UnequipTalismanAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	int32 GetAmountOfEquippedItem(UPrimaryDataAsset* Item);
	virtual int32 GetAmountOfEquippedItem_Implementation(UPrimaryDataAsset* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void EquipToolToSlot(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2);
	virtual void EquipToolToSlot_Implementation(UPrimaryDataAsset* TargetItem, const FGameplayTag& TargetEquipmentSlot, bool ChangeStats, bool& OutSuccess, bool& OutSuccess_1, bool& OutSuccess_2);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnequipToolAtSlot(const FGameplayTag& SlotTag);
	virtual void UnequipToolAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UpdateOverlayStates();
	virtual void UpdateOverlayStates_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	bool AreBothWeaponSlotsActive();
	virtual bool AreBothWeaponSlotsActive_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void RefreshActiveGuardSequence();
	virtual void RefreshActiveGuardSequence_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void DoesEquipmentSupportGuard(bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2);
	virtual void DoesEquipmentSupportGuard_Implementation(bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void OverrideOverlayStates(ESLFOverlayState InLeftHandOverlayState, ESLFOverlayState InRightHandOverlayState);
	virtual void OverrideOverlayStates_Implementation(ESLFOverlayState InLeftHandOverlayState, ESLFOverlayState InRightHandOverlayState);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void IsDualWieldPossible(bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2);
	virtual void IsDualWieldPossible_Implementation(bool& OutReturnValue, bool& OutReturnValue_1, bool& OutReturnValue_2);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void WieldItemAtSlot(const FGameplayTag& SlotTag);
	virtual void WieldItemAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnwieldItemAtSlot(const FGameplayTag& SlotTag);
	virtual void UnwieldItemAtSlot_Implementation(const FGameplayTag& SlotTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void AdjustForTwoHandStance(bool RightHand);
	virtual void AdjustForTwoHandStance_Implementation(bool RightHand);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void IsTwoHandStanceActive(bool& OutLeftHand, bool& OutRightHand, bool& OutLeftHand_1, bool& OutRightHand_1, bool& OutLeftHand_2, bool& OutRightHand_2);
	virtual void IsTwoHandStanceActive_Implementation(bool& OutLeftHand, bool& OutRightHand, bool& OutLeftHand_1, bool& OutRightHand_1, bool& OutLeftHand_2, bool& OutRightHand_2);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void ReinitializeMovementWithWeight();
	virtual void ReinitializeMovementWithWeight_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void UnequipToolFromOtherSlots(UPrimaryDataAsset* Item);
	virtual void UnequipToolFromOtherSlots_Implementation(UPrimaryDataAsset* Item);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void TryGrantBuffs(const TMap<FGameplayTag, UPrimaryDataAsset*>& Buffs_Ranks, bool IsLoading);
	virtual void TryGrantBuffs_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& Buffs_Ranks, bool IsLoading);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void TryRemoveBuffs(const TMap<FGameplayTag, UPrimaryDataAsset*>& Buffs_Ranks, double DelayDuration);
	virtual void TryRemoveBuffs_Implementation(const TMap<FGameplayTag, UPrimaryDataAsset*>& Buffs_Ranks, double DelayDuration);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_EquipmentManager")
	void GetGuardHitMontage(UAnimMontage*& OutGuardHitMontage, UAnimMontage*& OutGuardHitMontage_1, UAnimMontage*& OutGuardHitMontage_2, UAnimMontage*& OutGuardHitMontage_3, UAnimMontage*& OutGuardHitMontage_4, UAnimMontage*& OutGuardHitMontage_5);
	virtual void GetGuardHitMontage_Implementation(UAnimMontage*& OutGuardHitMontage, UAnimMontage*& OutGuardHitMontage_1, UAnimMontage*& OutGuardHitMontage_2, UAnimMontage*& OutGuardHitMontage_3, UAnimMontage*& OutGuardHitMontage_4, UAnimMontage*& OutGuardHitMontage_5);

	// ═══════════════════════════════════════════════════════════════════════
	// WEAPON DAMAGE GETTERS (for AnimNotify weapon trace)
	// ═══════════════════════════════════════════════════════════════════════

	/** Get the total damage from currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "AC_EquipmentManager|Combat")
	double GetWeaponDamage() const;

	/** Get the poise damage from currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "AC_EquipmentManager|Combat")
	double GetWeaponPoiseDamage() const;

	/** Get status effects from currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "AC_EquipmentManager|Combat")
	TMap<FGameplayTag, UPrimaryDataAsset*> GetWeaponStatusEffects() const;
};
