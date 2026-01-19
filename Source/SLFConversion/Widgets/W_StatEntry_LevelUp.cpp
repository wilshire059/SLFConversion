// W_StatEntry_LevelUp.cpp
// C++ Widget implementation for W_StatEntry_LevelUp
//
// 20-PASS VALIDATION: 2026-01-17

#include "Widgets/W_StatEntry_LevelUp.h"
#include "Widgets/W_StatEntry_StatName.h"
#include "Blueprints/SLFStatBase.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"

UW_StatEntry_LevelUp::UW_StatEntry_LevelUp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Stat(nullptr)
	, ShowAdjustButtons(true)
	, OldValue(0.0)
	, Selected(false)
	, CanLevelUp(true)
{
	// Default colors - EXACT bp_only values from W_StatEntry_LevelUp.json
	// Normal color (background): brownish beige (R=0.213542, G=0.185264, B=0.154062)
	// Selected color (background): light blue (R=0.240506, G=0.298422, B=0.744792)
	// Pending change TEXT color: GOLD (R=0.858824, G=0.745098, B=0.619608) - from bp_only JSON line 4874
	Color = FLinearColor(0.213542f, 0.185264f, 0.154062f, 1.0f);
	SelectedColor = FLinearColor(0.240506f, 0.298422f, 0.744792f, 1.0f);  // For background highlight
	PendingChangeTextColor = FLinearColor(0.858824f, 0.745098f, 0.619608f, 1.0f);  // GOLD for text with pending changes
}

void UW_StatEntry_LevelUp::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Bind button click events for increase/decrease stat
	if (UButton* IncreaseBtn = Cast<UButton>(GetWidgetFromName(TEXT("ButtonIncrease"))))
	{
		IncreaseBtn->OnClicked.AddDynamic(this, &UW_StatEntry_LevelUp::OnButtonIncreaseClicked);
		UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp: Bound ButtonIncrease"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_StatEntry_LevelUp: ButtonIncrease not found!"));
	}

	if (UButton* DecreaseBtn = Cast<UButton>(GetWidgetFromName(TEXT("ButtonDecrease"))))
	{
		DecreaseBtn->OnClicked.AddDynamic(this, &UW_StatEntry_LevelUp::OnButtonDecreaseClicked);
		UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp: Bound ButtonDecrease"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_StatEntry_LevelUp: ButtonDecrease not found!"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::NativeConstruct"));
}

void UW_StatEntry_LevelUp::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::NativeDestruct"));
}

void UW_StatEntry_LevelUp::CacheWidgetReferences()
{
	// Widget references will be cached by Blueprint UMG designer
	// No runtime caching needed
}

void UW_StatEntry_LevelUp::InitStatEntry_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp] InitStatEntry - Stat: %s"),
		Stat ? *Stat->GetName() : TEXT("None"));

	if (!IsValid(Stat))
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_LevelUp] InitStatEntry - Stat is NULL, cannot initialize!"));
		return;
	}

	// CRITICAL: Bind to Stat->OnStatUpdated delegate (matches bp_only Construct graph)
	// This allows the widget to receive updates when the stat value changes
	Stat->OnStatUpdated.RemoveAll(this);  // Prevent duplicate bindings
	Stat->OnStatUpdated.AddDynamic(this, &UW_StatEntry_LevelUp::OnStatValueUpdatedHandler);
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp] Bound to Stat->OnStatUpdated for: %s"), *Stat->GetName());

	// Get stat display info
	const FStatInfo& StatInfo = Stat->StatInfo;

	// CRITICAL FIX: Update OldValue to current stat value
	// This ensures after level-up confirm, OldValue reflects the new actual value
	// So PendingChange will be 0 and text color will reset to white
	OldValue = StatInfo.bShowMaxValue ? StatInfo.MaxValue : StatInfo.CurrentValue;
	UE_LOG(LogTemp, Log, TEXT("  Set OldValue to: %.0f"), OldValue);

	// Reset selection state - after confirm, entries should not be selected
	Selected = false;

	UE_LOG(LogTemp, Log, TEXT("  StatInfo.DisplayName: %s, CurrentValue: %.0f, MaxValue: %.0f"),
		*StatInfo.DisplayName.ToString(), StatInfo.CurrentValue, StatInfo.MaxValue);

	// Update stat name text - Blueprint uses a custom W_StatEntry_StatName widget
	// The widget is named "W_StatEntry_StatName" in the UMG hierarchy
	if (UW_StatEntry_StatName* StatNameWidget = Cast<UW_StatEntry_StatName>(GetWidgetFromName(TEXT("W_StatEntry_StatName"))))
	{
		// Use the SetStatText function which updates the internal TextBlock properly
		StatNameWidget->SetStatText(StatInfo.DisplayName);
		UE_LOG(LogTemp, Log, TEXT("  Set W_StatEntry_StatName to: %s"), *StatInfo.DisplayName.ToString());
	}
	else
	{
		// Fallback: try direct TextBlock named "StatName"
		if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatName"))))
		{
			NameText->SetText(StatInfo.DisplayName);
			UE_LOG(LogTemp, Log, TEXT("  Set StatName TextBlock to: %s"), *StatInfo.DisplayName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Neither W_StatEntry_StatName nor StatName widget found!"));
		}
	}

	// Update new value text - Blueprint widget name is "StatValueNew"
	// Display CurrentValue (or MaxValue depending on bShowMaxValue flag in StatInfo)
	if (UTextBlock* ValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValueNew"))))
	{
		double DisplayValue = StatInfo.bShowMaxValue ? StatInfo.MaxValue : StatInfo.CurrentValue;

		// Format based on bDisplayAsPercent flag
		FString ValueStr;
		if (StatInfo.bDisplayAsPercent)
		{
			ValueStr = FString::Printf(TEXT("%.0f%%"), DisplayValue);
		}
		else
		{
			ValueStr = FString::Printf(TEXT("%.0f"), DisplayValue);
		}
		ValueText->SetText(FText::FromString(ValueStr));

		// CRITICAL FIX: Reset text color to white (no pending change)
		// After level-up confirm, OldValue == CurrentValue so PendingChange is 0
		ValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		UE_LOG(LogTemp, Log, TEXT("  Set StatValueNew text to: %s (color: White)"), *ValueStr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatValueNew widget not found!"));
	}

	// CRITICAL FIX: Reset background border color to normal (not highlighted)
	if (UBorder* BgBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BgBorder"))))
	{
		BgBorder->SetBrushColor(Color);
		UE_LOG(LogTemp, Log, TEXT("  Reset BgBorder to normal Color"));
	}

	// Update old value text - Blueprint widget name is "StatValueOld"
	if (UTextBlock* OldValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValueOld"))))
	{
		FString OldValueStr = FString::Printf(TEXT("%.0f"), OldValue);
		OldValueText->SetText(FText::FromString(OldValueStr));
	}

	// Category is stored on the UW_StatEntry_LevelUp widget, set externally
	// (StatCategory is set by W_StatBlock_LevelUp when populating entries)

	// CRITICAL: Set button visibility based on ShowAdjustButtons
	// Only PrimaryAttributes block has ShowAdjustButtons=true
	// All other blocks (SecondaryStats, AttackStats, etc.) have ShowAdjustButtons=false
	ESlateVisibility ButtonVisibility = ShowAdjustButtons
		? ESlateVisibility::Visible
		: ESlateVisibility::Collapsed;

	if (UButton* IncreaseBtn = Cast<UButton>(GetWidgetFromName(TEXT("ButtonIncrease"))))
	{
		IncreaseBtn->SetVisibility(ButtonVisibility);
		UE_LOG(LogTemp, Log, TEXT("  ButtonIncrease visibility: %s (ShowAdjustButtons=%s)"),
			ShowAdjustButtons ? TEXT("Visible") : TEXT("Collapsed"),
			ShowAdjustButtons ? TEXT("true") : TEXT("false"));
	}
	if (UButton* DecreaseBtn = Cast<UButton>(GetWidgetFromName(TEXT("ButtonDecrease"))))
	{
		DecreaseBtn->SetVisibility(ButtonVisibility);
		UE_LOG(LogTemp, Log, TEXT("  ButtonDecrease visibility: %s"),
			ShowAdjustButtons ? TEXT("Visible") : TEXT("Collapsed"));
	}
}

void UW_StatEntry_LevelUp::SetLevelUpStatEntrySelected_Implementation(bool InSelected)
{
	Selected = InSelected;

	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp] SetLevelUpStatEntrySelected - Stat: %s, Selected: %s"),
		Stat ? *Stat->GetName() : TEXT("None"),
		Selected ? TEXT("true") : TEXT("false"));

	// bp_only: Set BgBorder brush color based on selection state
	if (UBorder* BgBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BgBorder"))))
	{
		BgBorder->SetBrushColor(Selected ? SelectedColor : Color);
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   BgBorder SetBrushColor: %s"),
			Selected ? TEXT("SelectedColor") : TEXT("Color"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_LevelUp]   BgBorder widget not found!"));
	}

	// Broadcast selection event with affected stats (matches bp_only pattern)
	// bp_only: Break Stat.StatInfo -> Extract StatModifiers.StatsToAffect -> Get Keys -> Broadcast
	if (IsValid(Stat))
	{
		TArray<FGameplayTag> AffectedStatTags;
		// Get affected stats from stat modifiers - keys are the affected stat tags
		Stat->StatInfo.StatModifiers.StatsToAffect.GetKeys(AffectedStatTags);

		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   Broadcasting OnStatEntrySelected with %d affected stats:"),
			AffectedStatTags.Num());
		for (const FGameplayTag& Tag : AffectedStatTags)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]     - %s"), *Tag.ToString());
		}

		OnStatEntrySelected.Broadcast(AffectedStatTags, Selected);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_LevelUp]   Stat is NULL, cannot get affected stats!"));
	}
}

void UW_StatEntry_LevelUp::EventDecreaseStat_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::EventDecreaseStat - Stat: %s"),
		Stat ? *Stat->GetName() : TEXT("None"));

	if (!IsValid(Stat) || !CanLevelUp)
	{
		return;
	}

	// Broadcast stat change request with Increase=false
	OnStatChangeRequest.Broadcast(Stat, false);
}

void UW_StatEntry_LevelUp::EventHighlight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::EventHighlight"));

	// bp_only: Set BgBorder brush color to SelectedColor
	if (UBorder* BgBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BgBorder"))))
	{
		BgBorder->SetBrushColor(SelectedColor);
		UE_LOG(LogTemp, Log, TEXT("  BgBorder SetBrushColor to SelectedColor"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  BgBorder widget not found!"));
	}
}

void UW_StatEntry_LevelUp::EventIncreaseStat_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::EventIncreaseStat - Stat: %s"),
		Stat ? *Stat->GetName() : TEXT("None"));

	if (!IsValid(Stat) || !CanLevelUp)
	{
		return;
	}

	// Broadcast stat change request with Increase=true
	OnStatChangeRequest.Broadcast(Stat, true);
}

void UW_StatEntry_LevelUp::EventOnStatUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, uint8 ValueType)
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp] EventOnStatUpdated - Stat: %s, Change: %.1f, UpdateAffectedStats: %s, ValueType: %d"),
		UpdatedStat ? *UpdatedStat->GetName() : TEXT("None"),
		Change,
		UpdateAffectedStats ? TEXT("true") : TEXT("false"),
		ValueType);

	// FIX: Use the stat's ACTUAL current value instead of OldValue + Change
	// The old formula broke because:
	// 1. Combat damage fires OnStatUpdated with negative Change
	// 2. The widget receives ALL stat changes, not just level-up changes
	// 3. bp_only pattern actually modifies the stat during preview, so we should show actual value
	double PreviewValue = 0.0;
	if (IsValid(UpdatedStat))
	{
		// Use MaxValue or CurrentValue based on bShowMaxValue flag
		PreviewValue = UpdatedStat->StatInfo.bShowMaxValue
			? UpdatedStat->StatInfo.MaxValue
			: UpdatedStat->StatInfo.CurrentValue;
	}
	else
	{
		// Fallback to old formula if stat is invalid
		PreviewValue = OldValue + Change;
	}

	// Determine if there's a pending change (compare to OldValue)
	double PendingChange = PreviewValue - OldValue;

	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   OldValue=%.0f, ActualValue=%.0f, PendingChange=%.0f"),
		OldValue, PreviewValue, PendingChange);

	// Update StatValueNew text to show the preview value
	if (UTextBlock* ValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValueNew"))))
	{
		FString ValueStr;
		if (IsValid(Stat) && Stat->StatInfo.bDisplayAsPercent)
		{
			ValueStr = FString::Printf(TEXT("%.0f%%"), PreviewValue);
		}
		else
		{
			ValueStr = FString::Printf(TEXT("%.0f"), PreviewValue);
		}
		ValueText->SetText(FText::FromString(ValueStr));

		// Change color to indicate pending change
		// FIX: Use PendingChange (difference from OldValue) not Change from delegate
		// FIX: Use GOLD color for pending changes (R=0.858824, G=0.745098, B=0.619608) - matches bp_only
		if (!FMath::IsNearlyZero(PendingChange))
		{
			// GOLD color to indicate pending change (matches bp_only IncreaseTxt color)
			ValueText->SetColorAndOpacity(FSlateColor(PendingChangeTextColor));
			UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   Set color to GOLD (pending change: %.0f)"), PendingChange);
		}
		else
		{
			// Default white color
			ValueText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   Set color to WHITE (no pending change)"));
		}

		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   Updated StatValueNew text to: %s"), *ValueStr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_LevelUp]   StatValueNew widget not found!"));
	}
}

void UW_StatEntry_LevelUp::EventRemoveHighlight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::EventRemoveHighlight"));

	// bp_only: Set BgBorder brush color back to normal Color (unless currently selected)
	if (!Selected)
	{
		if (UBorder* BgBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BgBorder"))))
		{
			BgBorder->SetBrushColor(Color);
			UE_LOG(LogTemp, Log, TEXT("  BgBorder SetBrushColor to Color (normal)"));
		}
	}
}

void UW_StatEntry_LevelUp::EventSetInitialValue_Implementation(double InOldValue)
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::EventSetInitialValue - OldValue: %.1f"), InOldValue);

	// Store the initial/old value
	OldValue = InOldValue;

	// Refresh display
	InitStatEntry();
}

void UW_StatEntry_LevelUp::EventToggleLevelUpEnabled_Implementation(bool Enabled)
{
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp] EventToggleLevelUpEnabled - Stat: %s, Enabled: %s, ShowAdjustButtons: %s"),
		Stat ? *Stat->GetName() : TEXT("None"),
		Enabled ? TEXT("true") : TEXT("false"),
		ShowAdjustButtons ? TEXT("true") : TEXT("false"));

	CanLevelUp = Enabled;

	// Update button enabled state (matches bp_only pattern)
	// bp_only: Event(Enabled?) -> Set CanLevelUp? variable -> Get ButtonIncrease -> SetIsEnabled(CanLevelUp?)
	if (ShowAdjustButtons)
	{
		if (UButton* IncreaseBtn = Cast<UButton>(GetWidgetFromName(TEXT("ButtonIncrease"))))
		{
			IncreaseBtn->SetIsEnabled(Enabled);
			UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   ButtonIncrease SetIsEnabled(%s)"),
				Enabled ? TEXT("true") : TEXT("false"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_LevelUp]   ButtonIncrease NOT FOUND!"));
		}
		if (UButton* DecreaseBtn = Cast<UButton>(GetWidgetFromName(TEXT("ButtonDecrease"))))
		{
			DecreaseBtn->SetIsEnabled(Enabled);
			UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   ButtonDecrease SetIsEnabled(%s)"),
				Enabled ? TEXT("true") : TEXT("false"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_LevelUp]   ButtonDecrease NOT FOUND!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp]   Skipped button enable/disable (ShowAdjustButtons=false)"));
	}
}

void UW_StatEntry_LevelUp::OnButtonIncreaseClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::OnButtonIncreaseClicked - Stat: %s"),
		Stat ? *Stat->GetName() : TEXT("None"));

	// Call the BlueprintNativeEvent which broadcasts OnStatChangeRequest
	EventIncreaseStat();
}

void UW_StatEntry_LevelUp::OnButtonDecreaseClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UW_StatEntry_LevelUp::OnButtonDecreaseClicked - Stat: %s"),
		Stat ? *Stat->GetName() : TEXT("None"));

	// Call the BlueprintNativeEvent which broadcasts OnStatChangeRequest
	EventDecreaseStat();
}

void UW_StatEntry_LevelUp::OnStatValueUpdatedHandler(USLFStatBase* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType)
{
	// Handler for Stat->OnStatUpdated delegate
	// Forwards to EventOnStatUpdated (converts ESLFValueType to uint8)
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_LevelUp] OnStatValueUpdatedHandler - Stat: %s, Change: %.1f, ValueType: %d"),
		UpdatedStat ? *UpdatedStat->GetName() : TEXT("None"), Change, static_cast<uint8>(ValueType));

	EventOnStatUpdated(UpdatedStat, Change, UpdateAffectedStats, static_cast<uint8>(ValueType));
}
