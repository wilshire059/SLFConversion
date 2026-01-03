// W_Crafting.h
// C++ Widget class for W_Crafting
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Crafting.json
// Parent: UW_Navigable_InputReader
// Variables: 7 | Functions: 5 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_Crafting.generated.h"

// Forward declarations for widget types
class UW_EquipmentSlot;
class UW_InventorySlot;

// Forward declarations for Blueprint types
class UAC_InventoryManager;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Crafting_OnCraftingClosed);

UCLASS()
class SLFCONVERSION_API UW_Crafting : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Crafting(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_InventoryManager* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_EquipmentSlot* SelectedSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_InventorySlot*> CraftingSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_InventorySlot*> UnlockedCraftableEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_InventorySlot* ActiveSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TMap<FGameplayTag, FGameplayTag> UnlockedCraftables;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Crafting_OnCraftingClosed OnCraftingClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void SetupItemInformationPanel(UPrimaryDataAsset* InItem);
	virtual void SetupItemInformationPanel_Implementation(UPrimaryDataAsset* InItem);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	int32 GetOwnedAmountFromTag(const FGameplayTag& Tag);
	virtual int32 GetOwnedAmountFromTag_Implementation(const FGameplayTag& Tag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	bool CheckIfCraftable(UPrimaryDataAsset* ItemAsset);
	virtual bool CheckIfCraftable_Implementation(UPrimaryDataAsset* ItemAsset);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void RefreshCraftables();
	virtual void RefreshCraftables_Implementation();


	// Event Handlers (14 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventAsyncLoadCraftables();
	virtual void EventAsyncLoadCraftables_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventOnCraftablesUpdated();
	virtual void EventOnCraftablesUpdated_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventOnCraftingSlotPressed(UW_InventorySlot* InSlot);
	virtual void EventOnCraftingSlotPressed_Implementation(UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventOnCraftingSlotSelected(bool Selected, UW_InventorySlot* InSlot);
	virtual void EventOnCraftingSlotSelected_Implementation(bool Selected, UW_InventorySlot* InSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventOnInventoryUpdated();
	virtual void EventOnInventoryUpdated_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventToggleCraftingAction(bool Show);
	virtual void EventToggleCraftingAction_Implementation(bool Show);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Crafting")
	void EventToggleItemInfo(bool Visible);
	virtual void EventToggleItemInfo_Implementation(bool Visible);

protected:
	// Cache references
	void CacheWidgetReferences();
};
