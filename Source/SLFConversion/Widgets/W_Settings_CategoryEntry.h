// W_Settings_CategoryEntry.h
// C++ Widget class for W_Settings_CategoryEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// UPDATED: 2026-01-22 - Full implementation with icon display and button events
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

#include "W_Settings_CategoryEntry.generated.h"

// Forward declarations for widget types
class UW_Settings;
class UButton;
class UImage;
class UBorder;

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
	virtual void NativePreConstruct() override;

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
	void BindButtonEvents();
	void ApplyIconToWidget();

	// Button event handlers
	UFUNCTION()
	void HandleButtonClicked();

	UFUNCTION()
	void HandleButtonHovered();

	UFUNCTION()
	void HandleButtonUnhovered();

	// Cached widget references
	UPROPERTY(Transient)
	UButton* CachedButton;

	UPROPERTY(Transient)
	UImage* CachedCategoryIcon;

	UPROPERTY(Transient)
	UBorder* CachedSelectedGlow;
};
