// W_ItemInfoEntry_RequiredStats.h
// C++ Widget class for W_ItemInfoEntry_RequiredStats
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_ItemInfoEntry_RequiredStats.json
// Parent: UUserWidget
// Variables: 3 | Functions: 0 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/TextBlock.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "W_ItemInfoEntry_RequiredStats.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_ItemInfoEntry_RequiredStats : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_ItemInfoEntry_RequiredStats(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// Data properties (set by parent widget before adding to UI)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FText EntryNameText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 RequiredValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double CurrentValue;

	// UI TextBlock widgets (bound from Blueprint)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "UI")
	UTextBlock* EntryText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "UI")
	UTextBlock* ValueText;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════


protected:
	// Cache references
	void CacheWidgetReferences();
};
