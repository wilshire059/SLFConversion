// W_Settings_InputKeySelector.h
// C++ Widget class for W_Settings_InputKeySelector
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_InputKeySelector.json
// Parent: UW_Navigable
// Variables: 10 | Functions: 4 | Dispatchers: 1

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


#include "W_Settings_InputKeySelector.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Settings_InputKeySelector_OnKeySelectorSelected, UW_Settings_InputKeySelector*, KeySelector);

UCLASS()
class SLFCONVERSION_API UW_Settings_InputKeySelector : public UW_Navigable
{
	GENERATED_BODY()

public:
	UW_Settings_InputKeySelector(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FKey Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UDataTable* KeyCorrelationTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Outline")
	double OutlineWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Outline")
	FLinearColor OutlineColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Outline")
	FVector4 OutlineCornerRadii;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Border Color")
	FLinearColor Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Border Color")
	FLinearColor SelectedColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Border Color")
	FLinearColor SelectedOutlineColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Active;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_InputKeySelector_OnKeySelectorSelected OnKeySelectorSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_InputKeySelector")
	void TryGetKeyIcon(const FKey& InKey, TSoftObjectPtr<UTexture2D>& OutIcon, bool& OutSuccess);
	virtual void TryGetKeyIcon_Implementation(const FKey& InKey, TSoftObjectPtr<UTexture2D>& OutIcon, bool& OutSuccess);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_InputKeySelector")
	void SetKeySelectorSelected(bool InSelected);
	virtual void SetKeySelectorSelected_Implementation(bool InSelected);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_InputKeySelector")
	void SetActive(bool InActive, const FText& ActiveText);
	virtual void SetActive_Implementation(bool InActive, const FText& ActiveText);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_InputKeySelector")
	FKey GetCurrentKey();
	virtual FKey GetCurrentKey_Implementation();


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_InputKeySelector")
	void EventSetKeyVisual(FKey InKey);
	virtual void EventSetKeyVisual_Implementation(FKey InKey);

protected:
	// Cache references
	void CacheWidgetReferences();
};
