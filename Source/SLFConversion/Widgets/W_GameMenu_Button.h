// W_GameMenu_Button.h
// C++ Widget class for W_GameMenu_Button
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_GameMenu_Button.json
// Parent: UUserWidget
// Variables: 9 | Functions: 1 | Dispatchers: 2

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


#include "W_GameMenu_Button.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GameMenu_Button_OnGameMenuButtonPressed, FGameplayTag, WidgetTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_GameMenu_Button_OnGameMenuButtonSelected, UW_GameMenu_Button*, Button);

UCLASS()
class SLFCONVERSION_API UW_GameMenu_Button : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_GameMenu_Button(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (9)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag TargetWidgetTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FText ButtonText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UTexture2D> ButtonImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor ImageColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor SelectedImageColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor ImageBorderColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor SelectedImageBorderColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Colors")
	FLinearColor HightlightColor;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_Button_OnGameMenuButtonPressed OnGameMenuButtonPressed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_GameMenu_Button_OnGameMenuButtonSelected OnGameMenuButtonSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu_Button")
	void SetGameMenuButtonSelected(bool InSelected);
	virtual void SetGameMenuButtonSelected_Implementation(bool InSelected);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_GameMenu_Button")
	void EventOnGameMenuButtonPressed();
	virtual void EventOnGameMenuButtonPressed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
