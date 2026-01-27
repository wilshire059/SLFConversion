// W_Settings_KeyMapping.h
// C++ Widget class for W_Settings_KeyMapping
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/WidgetBlueprint/W_Settings_KeyMapping.json
// Parent: UW_Navigable_InputReader
// Variables: 5 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "Components/Image.h"


#include "W_Settings_KeyMapping.generated.h"

// Forward declarations for widget types
class UW_Settings_KeyMapping_Entry;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Settings_KeyMapping : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Settings_KeyMapping(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* TargetInputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInputMappingContext* NavigationMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_Settings_KeyMapping_Entry*> KeyMappingEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<FName> Categories;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS (Input Icons - bound in UMG)
	// ═══════════════════════════════════════════════════════════════════════

	/** Image widget for IA_NavigableMenu_Ok key icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UImage* SelectInputIcon;

	/** Image widget for IA_NavigableMenu_Back key icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UImage* BackInputIcon;

	/** Image widget for IA_NavigableMenu_Left key icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UImage* ScrollLeftInputIcon;

	/** Image widget for IA_NavigableMenu_Right key icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UImage* ScrollRightInputIcon;

	/** Image widget for IA_NavigableMenu_ResetToDefaults key icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, OptionalWidget = true), Category = "Widgets")
	UImage* DefaultInputIcon;

	// ═══════════════════════════════════════════════════════════════════════
	// INPUT ACTION REFERENCES (used for key icon updates)
	// ═══════════════════════════════════════════════════════════════════════

	/** Input Action for Ok/Select (IA_NavigableMenu_Ok) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* IA_NavigableMenu_Ok;

	/** Input Action for Back (IA_NavigableMenu_Back) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* IA_NavigableMenu_Back;

	/** Input Action for Left/Scroll Left (IA_NavigableMenu_Left) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* IA_NavigableMenu_Left;

	/** Input Action for Right/Scroll Right (IA_NavigableMenu_Right) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* IA_NavigableMenu_Right;

	/** Input Action for Reset to Defaults (IA_NavigableMenu_ResetToDefaults) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
	UInputAction* IA_NavigableMenu_ResetToDefaults;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * Called when input device changes (keyboard/gamepad).
	 * Updates all navigation input icons based on current device type.
	 *
	 * Blueprint Flow:
	 * 1. Sequence with 5 parallel branches for each input icon
	 * 2. Each branch: GetKeysForIA -> pick key based on device type -> GetInputIconForKey -> SetBrushFromSoftTexture
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping")
	void OnInputDeviceChanged();
	virtual void OnInputDeviceChanged_Implementation();

	// Event Handlers (3 events)
	// Note: Blueprint calls this as "Event Initialize Key Mapping" with spaces - DisplayName maps it
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping", meta = (DisplayName = "Event InitializeKeyMapping"))
	void EventInitializeKeyMapping();
	virtual void EventInitializeKeyMapping_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping", meta = (DisplayName = "Event ReinitializeAllMappings"))
	void EventReinitializeAllMappings();
	virtual void EventReinitializeAllMappings_Implementation();

	// Note: Blueprint calls this as "Event Reset All Entries to Default" with spaces
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_KeyMapping", meta = (DisplayName = "Event ResetAllEntriesToDefault"))
	void EventResetAllEntriesToDefault();
	virtual void EventResetAllEntriesToDefault_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();

	/**
	 * Helper function to update a single input icon based on the mapped keys for an input action.
	 *
	 * @param TargetIA The input action to get keys for
	 * @param TargetImage The image widget to update with the key icon
	 *
	 * Logic:
	 * 1. Call UBFL_Helper::GetKeysForIA(NavigationMapping, TargetIA) to get mapped keys
	 * 2. If keys array is not empty:
	 *    a. Select key index based on ActiveDeviceType (1 for Gamepad, 0 for keyboard/other)
	 *    b. Call GetInputIconForKey(SelectedKey) to get the icon texture
	 *    c. If icon is valid, call SetBrushFromSoftTexture on the target image
	 */
	void UpdateInputIconForAction(UInputAction* TargetIA, UImage* TargetImage);
};
