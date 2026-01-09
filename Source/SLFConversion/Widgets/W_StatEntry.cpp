// W_StatEntry.cpp
// C++ Widget implementation for W_StatEntry
//
// 20-PASS VALIDATION: 2026-01-08 - Full implementation with stat display

#include "Widgets/W_StatEntry.h"
#include "Blueprints/B_Stat.h"
#include "Components/TextBlock.h"
#include "SLFStatTypes.h"

UW_StatEntry::UW_StatEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Stat(nullptr)
	, Tooltip(nullptr)
	, ShowAdjustButtons(false)
	, OldValue(0.0)
{
}

void UW_StatEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry::NativeConstruct"));
}

void UW_StatEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry::NativeDestruct"));
}

void UW_StatEntry::CacheWidgetReferences()
{
	// Widget bindings are typically in the UMG designer
	// Additional lookup via GetWidgetFromName if needed
}

void UW_StatEntry::InitStatEntry_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] InitStatEntry called, Stat: %s"), Stat ? TEXT("VALID") : TEXT("NULL"));

	if (!Stat)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry] InitStatEntry - No Stat assigned!"));
		return;
	}

	// Get StatInfo from the Stat object
	const FStatInfo& StatInfo = Stat->StatInfo;

	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] Stat Tag: %s, DisplayName: %s, CurrentValue: %.1f, MaxValue: %.1f"),
		*StatInfo.Tag.ToString(),
		*StatInfo.DisplayName.ToString(),
		StatInfo.CurrentValue,
		StatInfo.MaxValue);

	// Set the stat name text
	// The Blueprint uses a nested widget W_StatEntry_StatName which has a TextBlock called StatName
	// We'll try to find it by searching for StatName directly first
	if (UTextBlock* StatNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatName"))))
	{
		StatNameText->SetText(StatInfo.DisplayName);
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] Set StatName text to: %s"), *StatInfo.DisplayName.ToString());
	}
	else
	{
		// Try the nested widget approach - find W_StatEntry_StatName and get its StatName child
		if (UWidget* StatNameWidget = GetWidgetFromName(TEXT("W_StatEntry_StatName")))
		{
			// Cast to UserWidget and try to get StatName from it
			if (UUserWidget* StatNameUserWidget = Cast<UUserWidget>(StatNameWidget))
			{
				if (UTextBlock* NestedStatName = Cast<UTextBlock>(StatNameUserWidget->GetWidgetFromName(TEXT("StatName"))))
				{
					NestedStatName->SetText(StatInfo.DisplayName);
					UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] Set nested StatName text to: %s"), *StatInfo.DisplayName.ToString());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry] Could not find StatName or W_StatEntry_StatName widget"));
		}
	}

	// Format the stat value text
	// Blueprint logic:
	// - If DisplayAsPercent: Show value as percentage
	// - If ShowMaxValue: Show "CurrentValue / MaxValue"
	// - Otherwise: Just show CurrentValue
	FString ValueString;
	if (StatInfo.bDisplayAsPercent)
	{
		// Display as percentage (CurrentValue is already 0-100 range)
		ValueString = FString::Printf(TEXT("%.0f%%"), StatInfo.CurrentValue);
	}
	else if (StatInfo.bShowMaxValue)
	{
		// Show current / max format
		ValueString = FString::Printf(TEXT("%.0f / %.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
	}
	else
	{
		// Just show the current value
		ValueString = FString::Printf(TEXT("%.0f"), StatInfo.CurrentValue);
	}

	// Set the stat value text
	if (UTextBlock* StatValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValue"))))
	{
		StatValueText->SetText(FText::FromString(ValueString));
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] Set StatValue text to: %s"), *ValueString);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry] Could not find StatValue widget"));
	}

	// Store the current value as OldValue for tracking changes
	OldValue = StatInfo.CurrentValue;

	// Toggle adjust buttons visibility
	if (UWidget* AdjustButtonsWidget = GetWidgetFromName(TEXT("AdjustButtons")))
	{
		AdjustButtonsWidget->SetVisibility(ShowAdjustButtons ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	// Bind to stat updates if the Stat has an OnStatUpdated delegate
	if (Stat->OnStatUpdated.IsBound() || true) // Always try to bind
	{
		// Remove any existing binding first to avoid duplicates
		Stat->OnStatUpdated.RemoveAll(this);
		Stat->OnStatUpdated.AddDynamic(this, &UW_StatEntry::EventOnStatUpdated);
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] Bound to OnStatUpdated for stat: %s"), *StatInfo.Tag.ToString());
	}
}

void UW_StatEntry::EventOnStatUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType)
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry] EventOnStatUpdated - Change: %.1f, ValueType: %d"), Change, ValueType);

	if (!UpdatedStat)
	{
		return;
	}

	// Re-display the stat with updated values
	const FStatInfo& StatInfo = UpdatedStat->StatInfo;

	// Format the value string based on display settings
	FString ValueString;
	if (StatInfo.bDisplayAsPercent)
	{
		ValueString = FString::Printf(TEXT("%.0f%%"), StatInfo.CurrentValue);
	}
	else if (StatInfo.bShowMaxValue)
	{
		ValueString = FString::Printf(TEXT("%.0f / %.0f"), StatInfo.CurrentValue, StatInfo.MaxValue);
	}
	else
	{
		ValueString = FString::Printf(TEXT("%.0f"), StatInfo.CurrentValue);
	}

	// Update the value text
	if (UTextBlock* StatValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValue"))))
	{
		StatValueText->SetText(FText::FromString(ValueString));
	}

	// Update OldValue tracking
	OldValue = StatInfo.CurrentValue;
}
