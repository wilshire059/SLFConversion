// W_Dialog.h
// C++ Widget class for W_Dialog
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Dialog.json
// Parent: UUserWidget
// Variables: 0 | Functions: 0 | Dispatchers: 5

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


#include "W_Dialog.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Dialog_OnInventoryRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Dialog_OnEquipmentRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Dialog_OnStatusRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Dialog_OnCraftingRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Dialog_OnSettingsRequested);

UCLASS()
class SLFCONVERSION_API UW_Dialog : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Dialog(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Dialog_OnInventoryRequested OnInventoryRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Dialog_OnEquipmentRequested OnEquipmentRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Dialog_OnStatusRequested OnStatusRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Dialog_OnCraftingRequested OnCraftingRequested;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Dialog_OnSettingsRequested OnSettingsRequested;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Dialog")
	void EventCloseDialog();
	virtual void EventCloseDialog_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Dialog")
	void EventInitializeDialog(const FText& InText);
	virtual void EventInitializeDialog_Implementation(const FText& InText);

protected:
	// Cache references
	void CacheWidgetReferences();
};
