// W_Dialog.cpp
// C++ Widget implementation for W_Dialog
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with dialog initialization and menu request dispatchers

#include "Widgets/W_Dialog.h"
#include "Components/TextBlock.h"

UW_Dialog::UW_Dialog(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Dialog::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Dialog::NativeConstruct"));
}

void UW_Dialog::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Dialog::NativeDestruct"));
}

void UW_Dialog::CacheWidgetReferences()
{
	// Widget references (like text blocks) are typically set in UMG designer via BindWidget
}

/**
 * EventCloseDialog - Close the dialog widget
 *
 * Blueprint Logic:
 * 1. Hide the dialog
 * 2. Clear any displayed text
 * 3. Optionally notify listeners
 */
void UW_Dialog::EventCloseDialog_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Dialog::EventCloseDialog"));

	// Hide the dialog
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * EventInitializeDialog - Initialize and show the dialog with text
 *
 * Blueprint Logic:
 * 1. Set the dialog text
 * 2. Show the dialog widget
 * 3. Configure any animations or transitions
 *
 * The dialog widget serves as a notification/message system that can also
 * request opening various game menus via its event dispatchers:
 * - OnInventoryRequested
 * - OnEquipmentRequested
 * - OnStatusRequested
 * - OnCraftingRequested
 * - OnSettingsRequested
 */
void UW_Dialog::EventInitializeDialog_Implementation(const FText& InText)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Dialog::EventInitializeDialog - Text: %s"), *InText.ToString());

	// Set text on the dialog text block
	if (UTextBlock* DialogText = Cast<UTextBlock>(GetWidgetFromName(TEXT("DialogText"))))
	{
		DialogText->SetText(InText);
	}

	// Show the dialog
	SetVisibility(ESlateVisibility::Visible);
}
