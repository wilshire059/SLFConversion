// W_StatusEffectBar.cpp
// C++ Widget implementation for W_StatusEffectBar
//
// 20-PASS VALIDATION: 2026-01-14
// Source: BlueprintDNA_v2/WidgetBlueprint/W_StatusEffectBar.json
//
// Blueprint Logic:
// - EventOnBuildupPercentChanged: Updates Bar progress to Effect->GetBuildupPercent()
// - EventOnStatusEffectRemoved: Removes this widget from parent

#include "Widgets/W_StatusEffectBar.h"
#include "Blueprints/B_StatusEffect.h"
#include "Components/ProgressBar.h"

UW_StatusEffectBar::UW_StatusEffectBar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Effect = nullptr;
	Bar = nullptr;
}

void UW_StatusEffectBar::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectBar::NativeConstruct"));
}

void UW_StatusEffectBar::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectBar::NativeDestruct"));
}

void UW_StatusEffectBar::CacheWidgetReferences()
{
	// Bar is bound via BindWidget meta, no manual caching needed
	// If not bound, try to find it
	if (!Bar)
	{
		Bar = Cast<UProgressBar>(GetWidgetFromName(TEXT("Bar")));
	}
}

/**
 * EventOnBuildupPercentChanged
 *
 * Blueprint Logic (from JSON graphs):
 * 1. Get Bar widget (ProgressBar)
 * 2. Get Effect->GetBuildupPercent()
 * 3. Set Bar->SetPercent(BuildupPercent)
 */
void UW_StatusEffectBar::EventOnBuildupPercentChanged_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectBar::EventOnBuildupPercentChanged"));

	if (!IsValid(Effect))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Effect is null"));
		return;
	}

	if (!IsValid(Bar))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Bar widget is null"));
		return;
	}

	// Get buildup percent from effect (0.0 to 1.0)
	double BuildupPercent = Effect->GetBuildupPercent();

	// Update progress bar
	Bar->SetPercent(static_cast<float>(BuildupPercent));

	UE_LOG(LogTemp, Verbose, TEXT("  Set Bar percent to: %.2f"), BuildupPercent);
}

/**
 * EventOnStatusEffectRemoved
 *
 * Blueprint Logic (from JSON graphs):
 * 1. Check if StatusEffectTag matches Effect's tag
 * 2. If match, remove this widget from parent
 */
void UW_StatusEffectBar::EventOnStatusEffectRemoved_Implementation(FGameplayTag StatusEffectTag)
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatusEffectBar::EventOnStatusEffectRemoved - Tag: %s"),
		*StatusEffectTag.ToString());

	// Check if this effect matches the removed tag
	if (IsValid(Effect))
	{
		// Get the effect's tag - GetEffectData returns UPrimaryDataAsset*, cast to UPDA_StatusEffect*
		UPDA_StatusEffect* EffectData = Cast<UPDA_StatusEffect>(Effect->GetEffectData());
		if (IsValid(EffectData) && EffectData->Tag.MatchesTagExact(StatusEffectTag))
		{
			UE_LOG(LogTemp, Log, TEXT("  Removing widget - effect tag matches"));
			RemoveFromParent();
		}
	}
}
