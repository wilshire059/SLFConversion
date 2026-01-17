// W_Equipment_Item_AttackPower.h
// C++ Widget class for W_Equipment_Item_AttackPower
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to use TMap for stat values
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment_Item_AttackPower.json
// Parent: UUserWidget
// The widget iterates over attack power stats and creates W_ItemInfoEntry for each

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/VerticalBox.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "W_Equipment_Item_AttackPower.generated.h"

class UW_ItemInfoEntry;
class UAC_StatManager;
class UStatManagerComponent;

UCLASS()
class SLFCONVERSION_API UW_Equipment_Item_AttackPower : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Equipment_Item_AttackPower(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// Vertical box that holds entry widgets - matches Blueprint's AttackPowerEntriesBox
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "W_Equipment_Item_AttackPower")
	UVerticalBox* AttackPowerEntriesBox;

	// Widget class for entry items - Blueprint uses W_ItemInfoEntry
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_AttackPower")
	TSubclassOf<UW_ItemInfoEntry> EntryWidgetClass;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Setup attack power stats - receives TMap of GameplayTag->int32 for damage values
	// TargetAttackPowerStats = stats on the item being viewed
	// CurrentAttackPowerStats = stats on currently equipped item (for comparison)
	// CanCompare = whether to show comparison arrows
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment_Item_AttackPower")
	void EventSetupAttackPowerStats(const TMap<FGameplayTag, int32>& TargetAttackPowerStats, const TMap<FGameplayTag, int32>& CurrentAttackPowerStats, bool CanCompare);
	virtual void EventSetupAttackPowerStats_Implementation(const TMap<FGameplayTag, int32>& TargetAttackPowerStats, const TMap<FGameplayTag, int32>& CurrentAttackPowerStats, bool CanCompare);

	// Helper to get display name from stat tag
	FText GetStatDisplayName(const FGameplayTag& StatTag) const;

	// Category tag for attack power stats (SoulslikeFramework.Stat.Secondary.AttackPower)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_AttackPower")
	FGameplayTag AttackPowerCategoryTag;

protected:
	// Cache references
	void CacheWidgetReferences();

	// Get all attack power stat tags from StatManager
	FGameplayTagContainer GetAttackPowerStatTags() const;
};
