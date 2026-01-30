// W_ItemWheelSlot.h
// C++ Widget class for W_ItemWheelSlot
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_ItemWheelSlot.json
// Parent: UUserWidget
// Variables: 15 | Functions: 4 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_ItemWheelSlot.generated.h"

// Forward declarations for widget types
class UW_ItemWheel_NextSlot;
class UAC_EquipmentManager;
class UImage;
class UTextBlock;
class USizeBox;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_ItemWheelSlot_OnItemWheelSlotSelected, FGameplayTag, Slot);

UCLASS()
class SLFCONVERSION_API UW_ItemWheelSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_ItemWheelSlot(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (15)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag Identifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FVector2D SizeXY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer SlotsToTrack;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* ActiveItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FGameplayTag ActiveSlot;
	// Key = Slot tag, Value = Item at that slot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TMap<FGameplayTag, TObjectPtr<UPrimaryDataAsset>> TrackedItems;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag MainSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 Length;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 CurrentIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FGameplayTag PreviousSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<FInstancedStruct> ItemWheelData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FSLFItemWheelNextSlotInfo ExtraSlotInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool ShowEmptySlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool VisuallyAffectedByStanceChange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_ItemWheel_NextSlot*> ChildSlots;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_ItemWheelSlot_OnItemWheelSlotSelected OnItemWheelSlotSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void HandleNewItem(UPrimaryDataAsset* InItem);
	virtual void HandleNewItem_Implementation(UPrimaryDataAsset* InItem);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	FGameplayTag GetSlotWithIndex(int32 InIndex);
	virtual FGameplayTag GetSlotWithIndex_Implementation(int32 InIndex);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void RefreshChildSlots();
	virtual void RefreshChildSlots_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	bool AreAllSlotsEmpty();
	virtual bool AreAllSlotsEmpty_Implementation();


	// Event Handlers (10 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventInitializeFromLoad(FSLFItemWheelSaveInfo Info);
	virtual void EventInitializeFromLoad_Implementation(FSLFItemWheelSaveInfo Info);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventItemEquipped(UPDA_Item* InItem);
	virtual void EventItemEquipped_Implementation(UPDA_Item* InItem);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventItemRemoved();
	virtual void EventItemRemoved_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventOnInteractPressed(bool Pressed);
	virtual void EventOnInteractPressed_Implementation(bool Pressed);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventOnItemAmountUpdated(UPDA_Item* InItem, int32 NewCount);
	virtual void EventOnItemAmountUpdated_Implementation(UPDA_Item* InItem, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventOnItemEquippedToSlot(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot);
	virtual void EventOnItemEquippedToSlot_Implementation(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventOnItemUnequippedFromSlot(UPDA_Item* InItem, FGameplayTag TargetSlot);
	virtual void EventOnItemUnequippedFromSlot_Implementation(UPDA_Item* InItem, FGameplayTag TargetSlot);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventOnStanceChanged(bool RightHand, bool TwoHand);
	virtual void EventOnStanceChanged_Implementation(bool RightHand, bool TwoHand);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventScrollWheel();
	virtual void EventScrollWheel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheelSlot")
	void EventScrollWheelNoDelay();
	virtual void EventScrollWheelNoDelay_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Cached component reference
	UPROPERTY(Transient)
	TObjectPtr<UAC_EquipmentManager> EquipmentComponent;

	// Cached UI widget references (prefixed to avoid Blueprint name conflicts)
	UPROPERTY(Transient)
	TObjectPtr<UImage> CachedItemIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CachedDebugIndexText;

	UPROPERTY(Transient)
	TObjectPtr<USizeBox> CachedSlotSizeBox;

	// Internal handlers for delegate binding
	UFUNCTION()
	void HandleOnItemEquippedToSlot(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot);

	UFUNCTION()
	void HandleOnItemUnequippedFromSlot(UPrimaryDataAsset* Item, FGameplayTag TargetSlot);

	UFUNCTION()
	void HandleOnStanceChanged(bool RightHand, bool TwoHand);

	UFUNCTION()
	void HandleOnActiveToolSlotChanged(FGameplayTag NewSlot);
};
