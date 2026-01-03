// W_CraftingEntrySimple.h
// C++ Widget class for W_CraftingEntrySimple
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_CraftingEntrySimple.json
// Parent: UUserWidget
// Variables: 3 | Functions: 1 | Dispatchers: 0

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


#include "W_CraftingEntrySimple.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_CraftingEntrySimple : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_CraftingEntrySimple(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* RequiredItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 RequiredAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 OwnedAmount;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingEntrySimple")
	int32 GetOwnedAmount();
	virtual int32 GetOwnedAmount_Implementation();


	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingEntrySimple")
	void EventRefreshAmounts();
	virtual void EventRefreshAmounts_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CraftingEntrySimple")
	void EventSetupEntry(const FText& ItemName, int32 CreatedAmount);
	virtual void EventSetupEntry_Implementation(const FText& ItemName, int32 CreatedAmount);

protected:
	// Cache references
	void CacheWidgetReferences();
};
