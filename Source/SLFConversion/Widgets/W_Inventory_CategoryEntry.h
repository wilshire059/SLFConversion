// W_Inventory_CategoryEntry.h
// C++ Widget class for W_Inventory_CategoryEntry
//
// 20-PASS VALIDATION: 2026-01-08
// Source: BlueprintDNA/WidgetBlueprint/W_Inventory_CategoryEntry.json
// Parent: UUserWidget
// Variables: 3 | Functions: 1 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"

#include "W_Inventory_CategoryEntry.generated.h"

// Forward declarations
class UImage;
class UButton;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FW_Inventory_CategoryEntry_OnSelected, UW_Inventory_CategoryEntry*, CategoryEntry, ESLFItemCategory, SelectedCategory);

UCLASS()
class SLFCONVERSION_API UW_Inventory_CategoryEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Inventory_CategoryEntry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config Override")
	bool AllCategoriesButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config Override")
	FSLFInventoryCategory InventoryCategoryData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Inventory_CategoryEntry_OnSelected OnSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Inventory_CategoryEntry")
	void SetInventoryCategorySelected(bool InSelected);
	virtual void SetInventoryCategorySelected_Implementation(bool InSelected);

	// Set up the category icon from InventoryCategoryData (public for external setup)
	void SetupCategoryIcon();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Bind button events
	void BindButtonEvents();

	// Button event handlers
	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonUnhovered();

	// Cached widget references
	UPROPERTY()
	UImage* CategoryIconWidget;

	UPROPERTY()
	UButton* ButtonWidget;
};
