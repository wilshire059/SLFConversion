// W_Settings_Entry.h
// C++ Widget class for W_Settings_Entry
//
// 20-PASS VALIDATION: 2026-01-03
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_Entry.json

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "GenericPlatform/GenericWindow.h"
#include "W_Settings_Entry.generated.h"

// Forward declarations
class UBorder;
class UTextBlock;
class USlider;
class UButton;
class UHorizontalBox;
class UPrimaryDataAsset;
class UComboBoxKey;
class UW_Settings_CenteredText;
class UPDA_CustomSettings;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_Entry_OnWindowModeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Settings_Entry_OnEntrySelected, UW_Settings_Entry*, Entry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Settings_Entry_OnSingleButtonEntryPressed);

UCLASS()
class SLFCONVERSION_API UW_Settings_Entry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Settings_Entry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// CONFIGURATION VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag SettingTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText SettingName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText SettingDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText ButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	ESLFSettingEntry EntryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor UnhoveredColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor HoveredColor;

	// ═══════════════════════════════════════════════════════════════════════
	// RUNTIME VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	FString CurrentValue;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	bool Selected;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TArray<FIntPoint> SupportedResolutions;

	// Resolution map: FName key (e.g., "1920x1080") → FIntPoint value
	// Matches Blueprint: TMap<FName, FIntPoint>
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FName, FIntPoint> Resolutions;

	// Window mode map: FName key (e.g., "Fullscreen") → EWindowMode::Type value
	// Matches Blueprint: TMap<FName, EWindowMode::Type>
	// Note: Use TEnumAsByte because EWindowMode is not a UENUM
	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FName, TEnumAsByte<EWindowMode::Type>> WindowModes;

	// Custom game settings asset - matches Blueprint PDA_CustomSettings_C type
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPDA_CustomSettings* CustomSettingsAsset;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS (names must match Blueprint exactly)
	// ═══════════════════════════════════════════════════════════════════════

	// Background border - used for selection highlighting
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UBorder* BackgroundBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* SettingNameText;

	// Value text for slider view
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* SliderValue;

	// Value text for button/dropdown view
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* ButtonsValue;

	// Slider control
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	USlider* Slider;

	// Decrease/Increase buttons (for double button view)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UButton* DecreaseButton;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UButton* IncreaseButton;

	// View panels - shown/hidden based on EntryType (NO WidgetSwitcher!)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UHorizontalBox* ButtonView;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UHorizontalBox* DoubleButtonView;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UHorizontalBox* SliderView;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UHorizontalBox* DropdownView;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UHorizontalBox* KeySelectorView;

	// ComboBox for dropdown entry type
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UComboBoxKey* DropDown;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_Entry_OnWindowModeChanged OnWindowModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_Entry_OnEntrySelected OnEntrySelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_Entry_OnSingleButtonEntryPressed OnSingleButtonEntryPressed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetEntryType(ESLFSettingEntry Type);
	virtual void SetEntryType_Implementation(ESLFSettingEntry Type);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	FText GetQualityLevelText(int32 QualityLevel);
	virtual FText GetQualityLevelText_Implementation(int32 QualityLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetCurrentValue(const FString& InCurrentValue);
	virtual void SetCurrentValue_Implementation(const FString& InCurrentValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	int32 GetIncrementedValue(int32 MaxClamp);
	virtual int32 GetIncrementedValue_Implementation(int32 MaxClamp);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	int32 GetDecrementedValue(int32 MaxClamp);
	virtual int32 GetDecrementedValue_Implementation(int32 MaxClamp);

	// NOTE: OnGenerateItemWidget is implemented in Blueprint, not C++
	// The Blueprint creates W_Settings_CenteredText widgets for ComboBox items

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetCurrentBoolValue(bool InCurrentValue);
	virtual void SetCurrentBoolValue_Implementation(bool InCurrentValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetCurrentScreenModeValue(EWindowMode::Type FullscreenMode);
	virtual void SetCurrentScreenModeValue_Implementation(EWindowMode::Type FullscreenMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetCurrentResolutionValue(const FIntPoint& Resolution);
	virtual void SetCurrentResolutionValue_Implementation(const FIntPoint& Resolution);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetEntrySelected(UPARAM(DisplayName = "Selected") bool bSelected);
	virtual void SetEntrySelected_Implementation(bool bSelected);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void SetCameraInvertValue(bool TargetBool);
	virtual void SetCameraInvertValue_Implementation(bool TargetBool);

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT HANDLERS
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventDecreaseSetting();
	virtual void EventDecreaseSetting_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventIncreaseSetting();
	virtual void EventIncreaseSetting_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventOnButtonSettingDecrease();
	virtual void EventOnButtonSettingDecrease_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventOnButtonSettingIncrease();
	virtual void EventOnButtonSettingIncrease_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventOnSliderSettingDecrease();
	virtual void EventOnSliderSettingDecrease_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventOnSliderSettingIncrease();
	virtual void EventOnSliderSettingIncrease_Implementation();

	// Note: Blueprint calls this as "Event Refresh Resolutions" with spaces
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry", meta = (DisplayName = "Event RefreshResolutions"))
	void EventRefreshResolutions();
	virtual void EventRefreshResolutions_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventScrollDropdownLeft();
	virtual void EventScrollDropdownLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventScrollDropdownRight();
	virtual void EventScrollDropdownRight_Implementation();

	// Called when OK/Enter is pressed on this entry
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	void EventActivateEntry();
	virtual void EventActivateEntry_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// COMBOBOX WIDGET GENERATION (for dropdown items)
	// ═══════════════════════════════════════════════════════════════════════

	// Called by ComboBox to generate widgets for dropdown items
	UFUNCTION()
	UWidget* OnGenerateItemWidget(FName Item);

	// Called by ComboBox to generate the content widget (selected item display)
	UFUNCTION()
	UWidget* OnGenerateContentWidget(FName Item);

protected:
	// Update visuals based on configuration
	void ApplyVisualConfig();

	// Button event handlers
	UFUNCTION()
	void OnLeftArrowClicked();

	UFUNCTION()
	void OnRightArrowClicked();

	UFUNCTION()
	void OnEntryHovered();

	UFUNCTION()
	void OnSliderValueChanged(float Value);
};
