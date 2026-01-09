// W_Interaction.cpp
// C++ Widget implementation for W_Interaction
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Interaction.h"
#include "Components/TextBlock.h"

UW_Interaction::UW_Interaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedMessageText = nullptr;
}

void UW_Interaction::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::NativeConstruct"));
}

void UW_Interaction::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::NativeDestruct"));
}

void UW_Interaction::CacheWidgetReferences()
{
	// Cache the MessageText TextBlock from the Blueprint widget tree
	if (!CachedMessageText)
	{
		CachedMessageText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MessageText")));
		UE_LOG(LogTemp, Log, TEXT("UW_Interaction::CacheWidgetReferences - MessageText: %s"),
			CachedMessageText ? TEXT("Found") : TEXT("NOT FOUND"));
	}
}
void UW_Interaction::EventHide_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::EventHide_Implementation"));

	// Set widget visibility to Collapsed (per Blueprint logic)
	SetVisibility(ESlateVisibility::Collapsed);
}


void UW_Interaction::EventOnInteractableOverlap_Implementation(const FText& InteractionText)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::EventOnInteractableOverlap_Implementation - Text: %s"),
		*InteractionText.ToString());

	// Set the message text (per Blueprint logic)
	if (CachedMessageText)
	{
		CachedMessageText->SetText(InteractionText);
	}

	// Show the widget
	EventShow();
}


void UW_Interaction::EventOnItemOverlap_Implementation(FSLFItemInfo ItemInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::EventOnItemOverlap_Implementation - Item: %s"),
		*ItemInfo.DisplayName.ToString());

	// Set the message text to the item's DisplayName (per Blueprint logic)
	if (CachedMessageText)
	{
		CachedMessageText->SetText(ItemInfo.DisplayName);
		UE_LOG(LogTemp, Log, TEXT("  Set MessageText to: %s"), *ItemInfo.DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  MessageText is null - cannot set text!"));
	}

	// Show the widget
	EventShow();
}


void UW_Interaction::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::EventOnVisibilityChanged_Implementation"));
}


void UW_Interaction::EventShow_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Interaction::EventShow_Implementation"));

	// Set widget visibility to Visible (per Blueprint logic)
	SetVisibility(ESlateVisibility::Visible);
}
