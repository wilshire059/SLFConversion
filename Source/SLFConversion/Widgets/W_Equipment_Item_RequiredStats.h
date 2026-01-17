// W_Equipment_Item_RequiredStats.h
// C++ Widget class for W_Equipment_Item_RequiredStats
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to use TMap for required stat values
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment_Item_RequiredStats.json
// Parent: UUserWidget
// The widget displays stat requirements (e.g., Dexterity: 10)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/UniformGridPanel.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "W_Equipment_Item_RequiredStats.generated.h"

class UW_ItemInfoEntry_RequiredStats;
class UAC_StatManager;
class UStatManagerComponent;
class UB_Stat;

UCLASS()
class SLFCONVERSION_API UW_Equipment_Item_RequiredStats : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Equipment_Item_RequiredStats(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// UniformGridPanel for required stat entries (2 columns layout)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "W_Equipment_Item_RequiredStats")
	UUniformGridPanel* RequiredStatEntriesBox;

	// Widget class for required stat entries
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_RequiredStats")
	TSubclassOf<UUserWidget> RequiredStatEntryWidgetClass;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Setup required stats - receives TMap of GameplayTag->int32 (stat -> required value)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment_Item_RequiredStats")
	void EventSetupRequiredStats(const TMap<FGameplayTag, int32>& RequiredStats);
	virtual void EventSetupRequiredStats_Implementation(const TMap<FGameplayTag, int32>& RequiredStats);

	// Helper to get display name from stat tag (abbreviated: Vig, Min, End, etc.)
	FText GetStatDisplayName(const FGameplayTag& StatTag) const;

	// Category tag for primary stats (SoulslikeFramework.Stat.Primary)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_RequiredStats")
	FGameplayTag PrimaryStatCategoryTag;

protected:
	// Cache references
	void CacheWidgetReferences();

	// Get all primary stat tags from StatManager
	FGameplayTagContainer GetPrimaryStatTags() const;

	// Get player's current value for a stat
	double GetPlayerCurrentStatValue(const FGameplayTag& StatTag) const;
};
