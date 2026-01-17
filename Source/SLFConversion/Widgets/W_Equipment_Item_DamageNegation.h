// W_Equipment_Item_DamageNegation.h
// C++ Widget class for W_Equipment_Item_DamageNegation
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment_Item_DamageNegation.json
// Parent: UUserWidget
// Variables: 0 | Functions: 0 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/VerticalBox.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "W_Equipment_Item_DamageNegation.generated.h"

// Forward declarations
class UW_ItemInfoEntry;

UCLASS()
class SLFCONVERSION_API UW_Equipment_Item_DamageNegation : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Equipment_Item_DamageNegation(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// Container for stat entry widgets (Blueprint name: DamageNegationEntriesBox)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "UI")
	UVerticalBox* DamageNegationEntriesBox;

	// Alias for code clarity
	UVerticalBox* StatsContainer;

	// Widget class for creating stat entries
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UW_ItemInfoEntry> ItemInfoEntryClass;

	// Event Handlers (1 events)
	// Signature: TMap<FGameplayTag, int32> for damage negation values (Physical, Magic, Fire, Frost, Lightning, Holy)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment_Item_DamageNegation")
	void EventSetupDamageNegationStats(const TMap<FGameplayTag, int32>& TargetDamageNegationStats, const TMap<FGameplayTag, int32>& CurrentDamageNegationStats, bool CanCompare);
	virtual void EventSetupDamageNegationStats_Implementation(const TMap<FGameplayTag, int32>& TargetDamageNegationStats, const TMap<FGameplayTag, int32>& CurrentDamageNegationStats, bool CanCompare);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Helper to extract display name from gameplay tag
	FText GetDisplayNameFromTag(const FGameplayTag& Tag) const;

	// Helper to create entries from map directly (fallback when no StatManager)
	void CreateEntriesFromMap(const TMap<FGameplayTag, int32>& TargetStats, const TMap<FGameplayTag, int32>& CurrentStats, bool CanCompare);
};
