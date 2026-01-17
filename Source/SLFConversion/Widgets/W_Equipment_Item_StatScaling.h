// W_Equipment_Item_StatScaling.h
// C++ Widget class for W_Equipment_Item_StatScaling
//
// 20-PASS VALIDATION: 2026-01-16 - Fixed to use TMap for scaling grades
// Source: BlueprintDNA/WidgetBlueprint/W_Equipment_Item_StatScaling.json
// Parent: UUserWidget
// The widget displays stat scaling grades (S, A, B, C, D, E)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Components/UniformGridPanel.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "W_Equipment_Item_StatScaling.generated.h"

class UW_ItemInfoEntry_StatScaling;
class UAC_StatManager;
class UStatManagerComponent;

UCLASS()
class SLFCONVERSION_API UW_Equipment_Item_StatScaling : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Equipment_Item_StatScaling(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// UniformGridPanel for scaling entries (2 columns layout)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional), Category = "W_Equipment_Item_StatScaling")
	UUniformGridPanel* StatScalingEntriesBox;

	// Widget class for scaling entries
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_StatScaling")
	TSubclassOf<UUserWidget> ScalingEntryWidgetClass;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Setup stat scaling - receives TMap of GameplayTag->ESLFStatScaling
	// Maps stats like Dexterity, Strength to scaling grades like A, S, B, etc.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Equipment_Item_StatScaling")
	void EventSetupStatScaling(const TMap<FGameplayTag, ESLFStatScaling>& ScalingInfo);
	virtual void EventSetupStatScaling_Implementation(const TMap<FGameplayTag, ESLFStatScaling>& ScalingInfo);

	// Helper to get display name from stat tag
	FText GetStatDisplayName(const FGameplayTag& StatTag) const;

	// Helper to convert scaling enum to letter grade text
	FText GetScalingGradeText(ESLFStatScaling ScalingGrade) const;

	// Category tag for primary stats (SoulslikeFramework.Stat.Primary)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "W_Equipment_Item_StatScaling")
	FGameplayTag PrimaryStatCategoryTag;

protected:
	// Cache references
	void CacheWidgetReferences();

	// Get all primary stat tags from StatManager
	FGameplayTagContainer GetPrimaryStatTags() const;
};
