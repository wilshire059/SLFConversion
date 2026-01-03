// W_EquipmentSlot.h
// C++ Widget class for W_EquipmentSlot
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_EquipmentSlot.json
// Parent: UUserWidget
// Variables: 5 | Functions: 0 | Dispatchers: 2

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


#include "W_EquipmentSlot.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_EquipmentSlot_OnSelected, bool, Selected, UW_EquipmentSlot*, Slot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_EquipmentSlot_OnPressed, UW_EquipmentSlot*, Slot);

UCLASS()
class SLFCONVERSION_API UW_EquipmentSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_EquipmentSlot(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FGuid ID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* AssignedItem;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool IsOccupied;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UTexture2D> BackgroundIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag EquipmentSlot;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_EquipmentSlot_OnSelected OnSelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_EquipmentSlot_OnPressed OnPressed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EquipmentSlot")
	void EventClearEquipmentSlot();
	virtual void EventClearEquipmentSlot_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EquipmentSlot")
	void EventEquipmentPressed();
	virtual void EventEquipmentPressed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EquipmentSlot")
	void EventOccupyEquipmentSlot(UPDA_Item* InAssignedItem);
	virtual void EventOccupyEquipmentSlot_Implementation(UPDA_Item* InAssignedItem);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EquipmentSlot")
	void EventOnSelected(bool Selected);
	virtual void EventOnSelected_Implementation(bool Selected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_EquipmentSlot")
	void EventSetHighlighted(bool Highlighted);
	virtual void EventSetHighlighted_Implementation(bool Highlighted);

protected:
	// Cache references
	void CacheWidgetReferences();
};
