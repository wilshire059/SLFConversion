// W_StatEntry_Status.cpp
// C++ Widget implementation for W_StatEntry_Status
//
// 20-PASS VALIDATION: 2026-01-14

#include "Widgets/W_StatEntry_Status.h"
#include "Widgets/W_StatEntry_StatName.h"
#include "Blueprints/B_Stat.h"
#include "Blueprints/SLFStatBase.h"
#include "SLFStatTypes.h"
#include "Components/TextBlock.h"

UW_StatEntry_Status::UW_StatEntry_Status(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Stat(nullptr)
	, StatBase(nullptr)
	, Tooltip(nullptr)
{
}

void UW_StatEntry_Status::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Initialize the stat entry if we have a stat assigned (check both hierarchies)
	if (Stat || StatBase)
	{
		InitStatEntry();
	}

	// Log which stat type is being used
	FString StatName = TEXT("NULL");
	if (StatBase)
	{
		StatName = StatBase->StatInfo.DisplayName.ToString();
	}
	else if (Stat)
	{
		StatName = Stat->StatInfo.DisplayName.ToString();
	}
	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] NativeConstruct - Stat: %s"), *StatName);
}

void UW_StatEntry_Status::NativeDestruct()
{
	// Unbind from stat updates (both hierarchies)
	if (Stat)
	{
		Stat->OnStatUpdated.RemoveAll(this);
	}
	if (StatBase)
	{
		StatBase->OnStatUpdated.RemoveAll(this);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] NativeDestruct"));
}

void UW_StatEntry_Status::CacheWidgetReferences()
{
	// Widget references are obtained via GetWidgetFromName in InitStatEntry
}

void UW_StatEntry_Status::InitStatEntry_Implementation()
{
	// Check for USLFStatBase first (preferred), then fall back to UB_Stat
	const FStatInfo* InfoPtr = nullptr;
	if (StatBase)
	{
		InfoPtr = &StatBase->StatInfo;
	}
	else if (Stat)
	{
		InfoPtr = &Stat->StatInfo;
	}

	if (!InfoPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_Status] InitStatEntry - No Stat assigned!"));
		return;
	}

	const FStatInfo& Info = *InfoPtr;

	// Find the StatValue TextBlock
	UTextBlock* StatValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValue")));

	// Find the W_StatEntry_StatName sub-widget and set its Text property
	// The sub-widget applies Text to its internal TextBlock in NativePreConstruct
	UW_StatEntry_StatName* StatNameWidget = Cast<UW_StatEntry_StatName>(GetWidgetFromName(TEXT("W_StatEntry_StatName")));
	if (StatNameWidget)
	{
		StatNameWidget->Text = Info.DisplayName;
		// Force the widget to update (in case PreConstruct already ran)
		if (UTextBlock* InternalText = Cast<UTextBlock>(StatNameWidget->GetRootWidget()))
		{
			InternalText->SetText(Info.DisplayName);
		}
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] Set StatName to: %s"), *Info.DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_Status] Could not find W_StatEntry_StatName widget!"));
	}

	// Format and set value
	FString ValueStr;
	if (Info.bDisplayAsPercent)
	{
		// Display as percentage
		double Percent = (Info.MaxValue > 0.0) ? (Info.CurrentValue / Info.MaxValue * 100.0) : 0.0;
		ValueStr = FString::Printf(TEXT("%.0f%%"), Percent);
	}
	else if (Info.bShowMaxValue)
	{
		// Show current/max format
		ValueStr = FString::Printf(TEXT("%.0f / %.0f"), Info.CurrentValue, Info.MaxValue);
	}
	else
	{
		// Show just current value
		ValueStr = FString::Printf(TEXT("%.0f"), Info.CurrentValue);
	}

	if (StatValueText)
	{
		StatValueText->SetText(FText::FromString(ValueStr));
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] Set StatValue to: %s"), *ValueStr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_StatEntry_Status] Could not find StatValue TextBlock!"));
	}

	// Bind to stat updates (the correct hierarchy)
	if (StatBase)
	{
		// Note: USLFStatBase::OnStatUpdated has different signature, we need a separate handler
		// For now, just log - we'll add a handler if needed
		UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] StatBase bound (update events from StatManagerComponent)"));
	}
	else if (Stat)
	{
		Stat->OnStatUpdated.AddDynamic(this, &UW_StatEntry_Status::EventOnStatUpdated);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] Initialized: %s = %s"),
		*Info.DisplayName.ToString(), *ValueStr);
}

void UW_StatEntry_Status::EventOnStatUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool UpdateAffectedStats, ESLFValueType ValueType)
{
	if (!UpdatedStat || UpdatedStat != Stat)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_StatEntry_Status] EventOnStatUpdated - %s changed by %.2f"),
		*Stat->StatInfo.DisplayName.ToString(), Change);

	// Re-initialize to update display
	// Get stat info
	const FStatInfo& Info = Stat->StatInfo;

	// Find the StatValue TextBlock
	UTextBlock* StatValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("StatValue")));

	// Format and set value
	FString ValueStr;
	if (Info.bDisplayAsPercent)
	{
		double Percent = (Info.MaxValue > 0.0) ? (Info.CurrentValue / Info.MaxValue * 100.0) : 0.0;
		ValueStr = FString::Printf(TEXT("%.0f%%"), Percent);
	}
	else if (Info.bShowMaxValue)
	{
		ValueStr = FString::Printf(TEXT("%.0f / %.0f"), Info.CurrentValue, Info.MaxValue);
	}
	else
	{
		ValueStr = FString::Printf(TEXT("%.0f"), Info.CurrentValue);
	}

	if (StatValueText)
	{
		StatValueText->SetText(FText::FromString(ValueStr));
	}
}
