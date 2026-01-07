// W_Settings_KeyMapping_Entry.h
// C++ Widget class for W_Settings_KeyMapping_Entry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_KeyMapping_Entry.json
// Parent: UW_Navigable
// Variables: 10 | Functions: 3 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include "W_Settings_KeyMapping_Entry.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Settings_KeyMapping_Entry_OnKeymapEntrySelected, UW_Settings_KeyMapping_Entry*, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_KeyMapping_Entry_OnKeymapUpdated);

UCLASS()
class SLFCONVERSION_API UW_Settings_KeyMapping_Entry : public UW_Navigable
{
	GENERATED_BODY()

public:
	UW_Settings_KeyMapping_Entry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* InputMapping_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputAction* InputAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText KeymapName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor UnhoveredColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor HoveredColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 SelectorNavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool SelectingKeyboardKey;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool SelectingGamepadKey;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TSet<FPlayerKeyMapping> Mappings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FKey UpdatedKey;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_KeyMapping_Entry_OnKeymapEntrySelected OnKeymapEntrySelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_KeyMapping_Entry_OnKeymapUpdated OnKeymapUpdated;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping_Entry")
	void SelectMappingForSlot(EPlayerMappableKeySlot InSlot, FName& OutMappingName, EPlayerMappableKeySlot& OutOutSlot, FKey& OutDefaultKey, FKey& OutCurrentKey);
	virtual void SelectMappingForSlot_Implementation(EPlayerMappableKeySlot InSlot, FName& OutMappingName, EPlayerMappableKeySlot& OutOutSlot, FKey& OutDefaultKey, FKey& OutCurrentKey);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping_Entry")
	void UpdateMapping(UInputKeySelector* IKS, const FName& MappingName, EPlayerMappableKeySlot InSlot, const FKey& NewKey, bool RefreshAll);
	virtual void UpdateMapping_Implementation(UInputKeySelector* IKS, const FName& MappingName, EPlayerMappableKeySlot InSlot, const FKey& NewKey, bool RefreshAll);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping_Entry")
	void IsKeyUsed(EPlayerMappableKeySlot InSlot, const FKey& IncomingKey, UInputKeySelector* KeySelector, bool& OutNeverUsed, bool& OutNeverUsed1);
	virtual void IsKeyUsed_Implementation(EPlayerMappableKeySlot InSlot, const FKey& IncomingKey, UInputKeySelector* KeySelector, bool& OutNeverUsed, bool& OutNeverUsed1);


	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping_Entry")
	void EventFocusZero();
	virtual void EventFocusZero_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping_Entry")
	void EventInitKeymappingEntry();
	virtual void EventInitKeymappingEntry_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
