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
class UWidgetSwitcher;
class USlider;
class UButton;
class UPrimaryDataAsset;

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

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FName> Resolutions;

	UPROPERTY(BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FName> WindowModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* CustomSettingsAsset;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UBorder* SelectedBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* SettingNameText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* ValueText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UWidgetSwitcher* EntrySwitcher;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	USlider* ValueSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UButton* LeftArrow;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UButton* RightArrow;

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
	void SetEntryType(ESLFSettingEntry InType);
	virtual void SetEntryType_Implementation(ESLFSettingEntry InType);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
	UWidget* OnGenerateItemWidget(const FName& InItem);
	virtual UWidget* OnGenerateItemWidget_Implementation(const FName& InItem);

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
	void SetEntrySelected(bool InSelected);
	virtual void SetEntrySelected_Implementation(bool InSelected);

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
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
