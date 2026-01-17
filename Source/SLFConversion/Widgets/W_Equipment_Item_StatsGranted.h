// W_Equipment_Item_StatsGranted.h
// C++ Widget class for W_Equipment_Item_StatsGranted
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to use TMap for granted stat values
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment_Item_StatsGranted.json
// Parent: UUserWidget
// The widget displays stats granted by equipment (e.g., +5 Vigor)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/VerticalBox.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "W_Equipment_Item_StatsGranted.generated.h"

class UW_ItemInfoEntry;

UCLASS()
class SLFCONVERSION_API UW_Equipment_Item_StatsGranted : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Equipment_Item_StatsGranted(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// Vertical box for granted stat entries (Blueprint uses StatScalingEntriesBox name)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "W_Equipment_Item_StatsGranted")
	UVerticalBox* StatScalingEntriesBox;

	// Widget class for granted stat entries (reuses W_ItemInfoEntry)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_StatsGranted")
	TSubclassOf<UW_ItemInfoEntry> GrantedStatEntryWidgetClass;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Setup granted stats - receives TMap of GameplayTag->double (stat -> bonus value)
	// TargetGrantedStats = stats granted by the item being viewed
	// CurrentGrantedStats = stats granted by currently equipped item (for comparison)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment_Item_StatsGranted")
	void EventSetupGrantedStats(const TMap<FGameplayTag, double>& TargetGrantedStats, const TMap<FGameplayTag, double>& CurrentGrantedStats);
	virtual void EventSetupGrantedStats_Implementation(const TMap<FGameplayTag, double>& TargetGrantedStats, const TMap<FGameplayTag, double>& CurrentGrantedStats);

	// Helper to get display name from stat tag
	FText GetStatDisplayName(const FGameplayTag& StatTag) const;

protected:
	// Cache references
	void CacheWidgetReferences();
};
