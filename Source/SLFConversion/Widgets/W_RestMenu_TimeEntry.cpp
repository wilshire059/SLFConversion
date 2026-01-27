// W_RestMenu_TimeEntry.cpp
// C++ Widget implementation for W_RestMenu_TimeEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_RestMenu_TimeEntry.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"

UW_RestMenu_TimeEntry::UW_RestMenu_TimeEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_RestMenu_TimeEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Update the time text from TimeInfo (set before AddToViewport)
	UpdateTimeText();

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::NativeConstruct - TimeInfo.Name: %s"), *TimeInfo.Name.ToString());
}

void UW_RestMenu_TimeEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::NativeDestruct"));
}

void UW_RestMenu_TimeEntry::CacheWidgetReferences()
{
	// Find widgets by name from the UMG designer
	if (WidgetTree)
	{
		TimeTextWidget = Cast<UTextBlock>(WidgetTree->FindWidget(FName(TEXT("TimeText"))));
		BtnBorderWidget = Cast<UBorder>(WidgetTree->FindWidget(FName(TEXT("BtnBorder"))));
		BtnWidget = Cast<UButton>(WidgetTree->FindWidget(FName(TEXT("Btn"))));

		// Bind button events
		if (BtnWidget)
		{
			BtnWidget->OnClicked.AddDynamic(this, &UW_RestMenu_TimeEntry::HandleButtonClicked);
			BtnWidget->OnHovered.AddDynamic(this, &UW_RestMenu_TimeEntry::HandleButtonHovered);
			BtnWidget->OnUnhovered.AddDynamic(this, &UW_RestMenu_TimeEntry::HandleButtonUnhovered);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::CacheWidgetReferences - TimeText: %s, BtnBorder: %s, Btn: %s"),
		TimeTextWidget ? TEXT("found") : TEXT("null"),
		BtnBorderWidget ? TEXT("found") : TEXT("null"),
		BtnWidget ? TEXT("found") : TEXT("null"));
}

void UW_RestMenu_TimeEntry::SetTimeEntrySelected_Implementation(bool InSelected)
{
	Selected = InSelected;

	// Update visual state to reflect selection
	if (BtnBorderWidget)
	{
		if (InSelected)
		{
			// Use the configured SelectedColor for highlighting
			BtnBorderWidget->SetBrushColor(SelectedColor);
		}
		else
		{
			// Default/unselected color (transparent or default)
			BtnBorderWidget->SetBrushColor(FLinearColor::Transparent);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::SetTimeEntrySelected - Selected: %s"), InSelected ? TEXT("true") : TEXT("false"));
}
void UW_RestMenu_TimeEntry::EventTimeEntryPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::EventTimeEntryPressed - Broadcasting with TimeInfo.Name: %s"), *TimeInfo.Name.ToString());

	// Broadcast the dispatcher with our TimeInfo
	OnTimeEntryPressed.Broadcast(TimeInfo);
}

void UW_RestMenu_TimeEntry::UpdateTimeText()
{
	if (TimeTextWidget)
	{
		// Format as "Until {TimeName}" matching Blueprint logic
		FString FormattedText = FString::Printf(TEXT("Until %s"), *TimeInfo.Name.ToString());
		TimeTextWidget->SetText(FText::FromString(FormattedText));

		UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::UpdateTimeText - Set text to: %s"), *FormattedText);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_RestMenu_TimeEntry::UpdateTimeText - TimeTextWidget is null!"));
	}
}

void UW_RestMenu_TimeEntry::HandleButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::HandleButtonClicked"));
	EventTimeEntryPressed();
}

void UW_RestMenu_TimeEntry::HandleButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::HandleButtonHovered"));
	SetTimeEntrySelected(true);
	OnTimeEntrySelected.Broadcast(this);
}

void UW_RestMenu_TimeEntry::HandleButtonUnhovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu_TimeEntry::HandleButtonUnhovered"));
	SetTimeEntrySelected(false);
}
