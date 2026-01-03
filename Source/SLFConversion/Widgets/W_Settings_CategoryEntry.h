// W_Settings_CategoryEntry.h
// C++ Widget class for W_Settings_CategoryEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_CategoryEntry.json
// Parent: UUserWidget
// Variables: 7 | Functions: 1 | Dispatchers: 1

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


#include "W_Settings_CategoryEntry.generated.h"

// Forward declarations for widget types
class UW_Settings;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_Settings_CategoryEntry_OnSelected, UW_Settings_CategoryEntry*, CategoryEntry, int32, Index);

UCLASS()
class SLFCONVERSION_API UW_Settings_CategoryEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Settings_CategoryEntry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (7)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTag SettingCategory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 SwitcherIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor SelectedColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FLinearColor DeselectedColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_Settings* W_Settings;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Settings_CategoryEntry_OnSelected OnSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_CategoryEntry")
	void SetCategorySelected(bool InSelected);
	virtual void SetCategorySelected_Implementation(bool InSelected);

protected:
	// Cache references
	void CacheWidgetReferences();
};
