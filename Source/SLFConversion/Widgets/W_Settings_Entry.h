// W_Settings_Entry.h
// C++ Widget class for W_Settings_Entry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_Entry.json
// Parent: UUserWidget
// Variables: 13 | Functions: 11 | Dispatchers: 3

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


#include "W_Settings_Entry.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (13)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UObject* SettingTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText SettingName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FString CurrentValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<UObject*> SupportedResolutions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FName> Resolutions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TMap<FGameplayTag, FName> WindowModes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText SettingDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText ButtonText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UObject* UnhoveredColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UObject* HoveredColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime|Selection")
	bool Selected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* CustomSettingsAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	ESLFSettingEntry EntryType;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_Entry_OnWindowModeChanged OnWindowModeChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_Entry_OnEntrySelected OnEntrySelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_Entry_OnSingleButtonEntryPressed OnSingleButtonEntryPressed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (11)
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
	void SetCurrentValue_1(bool InCurrentValue);
	virtual void SetCurrentValue_1_Implementation(bool InCurrentValue);
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


	// Event Handlers (9 events)
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

protected:
	// Cache references
	void CacheWidgetReferences();
};
