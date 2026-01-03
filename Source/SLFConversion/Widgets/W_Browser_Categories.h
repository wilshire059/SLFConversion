// W_Browser_Categories.h
// C++ Widget class for W_Browser_Categories
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Browser_Categories.json
// Parent: UUserWidget
// Variables: 2 | Functions: 0 | Dispatchers: 1

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


#include "W_Browser_Categories.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Browser_Categories_OnCategorySelected, ESLFItemCategory, Category);

UCLASS()
class SLFCONVERSION_API UW_Browser_Categories : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Browser_Categories(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* Asset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<ESLFItemCategory> Categories;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Browser_Categories_OnCategorySelected OnCategorySelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_Categories")
	void EventOnCategoryClicked(uint8 InCategory);
	virtual void EventOnCategoryClicked_Implementation(uint8 InCategory);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_Categories")
	void EventSetup();
	virtual void EventSetup_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
