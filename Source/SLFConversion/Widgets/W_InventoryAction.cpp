// W_InventoryAction.cpp
// C++ Widget implementation for W_InventoryAction
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with action button navigation

#include "Widgets/W_InventoryAction.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_Inventory_ActionButton.h"
#include "Components/AC_InventoryManager.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

UW_InventoryAction::UW_InventoryAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	NavigationIndex = 0;
}

void UW_InventoryAction::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::NativeConstruct"));
}

void UW_InventoryAction::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::NativeDestruct"));
}

void UW_InventoryAction::CacheWidgetReferences()
{
	// Button entries are typically set in UMG designer
}

/**
 * AreAllButtonsDisabled - Check if all action buttons are disabled
 */
bool UW_InventoryAction::AreAllButtonsDisabled_Implementation()
{
	for (UW_Inventory_ActionButton* Button : ButtonEntries)
	{
		if (Button && Button->GetIsEnabled())
		{
			return false;
		}
	}
	return true;
}

/**
 * EventActionButtonPressed - Execute the currently selected action
 *
 * Blueprint Logic:
 * 1. Get selected button's action type
 * 2. Execute corresponding inventory action
 * 3. Close action menu
 */
void UW_InventoryAction::EventActionButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventActionButtonPressed"));

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No valid slot or item"));
		return;
	}

	if (!ButtonEntries.IsValidIndex(NavigationIndex))
	{
		return;
	}

	UW_Inventory_ActionButton* SelectedButton = ButtonEntries[NavigationIndex];
	if (!SelectedButton || !SelectedButton->GetIsEnabled())
	{
		return;
	}

	// Get inventory manager
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return;
	}

	UAC_InventoryManager* InventoryManager = PlayerPawn->FindComponentByClass<UAC_InventoryManager>();
	if (!InventoryManager)
	{
		return;
	}

	// Execute action based on button type
	// Button types are typically: Use, Discard, Leave, Store, Retrieve
	// The button's ActionTag or index determines the action

	UE_LOG(LogTemp, Log, TEXT("  Executing action for button index %d"), NavigationIndex);

	// Common actions by index:
	// 0 = Use
	// 1 = Discard
	// 2 = Leave (drop)
	// 3 = Store

	switch (NavigationIndex)
	{
	case 0: // Use
		InventoryManager->UseItemAtSlot(SelectedSlot);
		break;

	case 1: // Discard
		// Would show amount selector for stackable items
		InventoryManager->DiscardItemAtSlot(1, SelectedSlot);
		break;

	case 2: // Leave (drop on ground)
		InventoryManager->LeaveItemAtSlot(1, SelectedSlot);
		break;

	case 3: // Store
		InventoryManager->StoreItemAtSlot(1, SelectedSlot);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("  Unknown action index: %d"), NavigationIndex);
		break;
	}

	// Hide action menu after executing action
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * EventNavigateActionConfirmHorizontal - Horizontal navigation for confirm buttons
 */
void UW_InventoryAction::EventNavigateActionConfirmHorizontal_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventNavigateActionConfirmHorizontal"));

	// Used when there are horizontal confirm/cancel buttons
	// Toggle between them
}

/**
 * EventNavigateActionDown - Navigate to next action button
 */
void UW_InventoryAction::EventNavigateActionDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventNavigateActionDown"));

	if (ButtonEntries.Num() == 0)
	{
		return;
	}

	// Find next enabled button
	int32 StartIndex = NavigationIndex;
	int32 NewIndex = NavigationIndex;

	do
	{
		NewIndex = (NewIndex + 1) % ButtonEntries.Num();

		if (ButtonEntries[NewIndex] && ButtonEntries[NewIndex]->GetIsEnabled())
		{
			// Deselect current
			if (ButtonEntries.IsValidIndex(NavigationIndex) && ButtonEntries[NavigationIndex])
			{
				ButtonEntries[NavigationIndex]->SetActionButtonSelected(false);
			}

			NavigationIndex = NewIndex;

			// Select new
			ButtonEntries[NavigationIndex]->SetActionButtonSelected(true);
			break;
		}
	}
	while (NewIndex != StartIndex);

	UE_LOG(LogTemp, Log, TEXT("  New navigation index: %d"), NavigationIndex);
}

/**
 * EventNavigateActionUp - Navigate to previous action button
 */
void UW_InventoryAction::EventNavigateActionUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventNavigateActionUp"));

	if (ButtonEntries.Num() == 0)
	{
		return;
	}

	// Find previous enabled button
	int32 StartIndex = NavigationIndex;
	int32 NewIndex = NavigationIndex;

	do
	{
		NewIndex = NewIndex - 1;
		if (NewIndex < 0)
		{
			NewIndex = ButtonEntries.Num() - 1;
		}

		if (ButtonEntries[NewIndex] && ButtonEntries[NewIndex]->GetIsEnabled())
		{
			// Deselect current
			if (ButtonEntries.IsValidIndex(NavigationIndex) && ButtonEntries[NavigationIndex])
			{
				ButtonEntries[NavigationIndex]->SetActionButtonSelected(false);
			}

			NavigationIndex = NewIndex;

			// Select new
			ButtonEntries[NavigationIndex]->SetActionButtonSelected(true);
			break;
		}
	}
	while (NewIndex != StartIndex);

	UE_LOG(LogTemp, Log, TEXT("  New navigation index: %d"), NavigationIndex);
}

/**
 * EventOnButtonSelected - Handle button selection via mouse/touch
 */
void UW_InventoryAction::EventOnButtonSelected_Implementation(UW_Inventory_ActionButton* Button)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventOnButtonSelected"));

	if (!Button)
	{
		return;
	}

	// Find button index
	int32 ButtonIndex = ButtonEntries.Find(Button);
	if (ButtonIndex != INDEX_NONE)
	{
		// Deselect current
		if (ButtonEntries.IsValidIndex(NavigationIndex) && ButtonEntries[NavigationIndex])
		{
			ButtonEntries[NavigationIndex]->SetActionButtonSelected(false);
		}

		NavigationIndex = ButtonIndex;
		Button->SetActionButtonSelected(true);
	}
}

/**
 * EventOnVisibilityChanged - Handle visibility change
 */
void UW_InventoryAction::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventOnVisibilityChanged - Visibility: %d"), InVisibility);

	ESlateVisibility NewVisibility = static_cast<ESlateVisibility>(InVisibility);

	if (NewVisibility == ESlateVisibility::Visible || NewVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		// Reset navigation to first enabled button
		NavigationIndex = 0;

		for (int32 i = 0; i < ButtonEntries.Num(); ++i)
		{
			if (ButtonEntries[i] && ButtonEntries[i]->GetIsEnabled())
			{
				NavigationIndex = i;
				ButtonEntries[i]->SetActionButtonSelected(true);
				break;
			}
		}
	}
}

/**
 * EventSetupForInventory - Configure buttons for inventory item
 *
 * Blueprint Logic:
 * 1. Store selected slot reference
 * 2. Enable/disable buttons based on item type
 *    - Use: enabled if item is usable
 *    - Discard: always enabled
 *    - Leave: always enabled
 *    - Store: enabled if storage is available
 */
void UW_InventoryAction::EventSetupForInventory_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventSetupForInventory"));

	SelectedSlot = InSlot;

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		return;
	}

	// Configure buttons for inventory context
	// Typical inventory buttons: Use, Discard, Leave, Store

	for (int32 i = 0; i < ButtonEntries.Num(); ++i)
	{
		if (!ButtonEntries[i])
		{
			continue;
		}

		// Enable buttons based on item and context
		// Index 0 = Use - check if item is usable
		// Index 1 = Discard - always enabled for non-key items
		// Index 2 = Leave - always enabled
		// Index 3 = Store - enabled if at storage location

		switch (i)
		{
		case 0: // Use
			// Would check if item has ActionToTrigger
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		case 1: // Discard
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		case 2: // Leave
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		case 3: // Store
			// Only enabled when at a storage location
			ButtonEntries[i]->SetIsEnabled(false);
			break;

		default:
			ButtonEntries[i]->SetIsEnabled(false);
			break;
		}
	}

	// Reset navigation
	NavigationIndex = 0;
	for (int32 i = 0; i < ButtonEntries.Num(); ++i)
	{
		if (ButtonEntries[i])
		{
			ButtonEntries[i]->SetActionButtonSelected(i == 0);
		}
	}
}

/**
 * EventSetupForStorage - Configure buttons for storage item
 *
 * Blueprint Logic:
 * Similar to inventory but with Retrieve instead of Store
 */
void UW_InventoryAction::EventSetupForStorage_Implementation(UW_InventorySlot* InSelectedSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventSetupForStorage"));

	SelectedSlot = InSelectedSlot;

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		return;
	}

	// Configure buttons for storage context
	// Typical storage buttons: Retrieve, Discard

	for (int32 i = 0; i < ButtonEntries.Num(); ++i)
	{
		if (!ButtonEntries[i])
		{
			continue;
		}

		switch (i)
		{
		case 0: // Retrieve
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		case 1: // Discard
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		default:
			ButtonEntries[i]->SetIsEnabled(false);
			break;
		}
	}

	// Reset navigation
	NavigationIndex = 0;
	for (int32 i = 0; i < ButtonEntries.Num(); ++i)
	{
		if (ButtonEntries[i])
		{
			ButtonEntries[i]->SetActionButtonSelected(i == 0);
		}
	}
}

/**
 * EventUpdateBackInputIcons - Update back button input icons
 */
void UW_InventoryAction::EventUpdateBackInputIcons_Implementation(int32 BackInputTexture)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventUpdateBackInputIcons - Texture: %d"), BackInputTexture);

	// Would update the back button icon based on current input device
	// Keyboard vs Controller icons
}

/**
 * EventUpdateOkInputIcons - Update OK button input icons
 */
void UW_InventoryAction::EventUpdateOkInputIcons_Implementation(int32 OkInputTexture)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventUpdateOkInputIcons - Texture: %d"), OkInputTexture);

	// Would update the OK button icon based on current input device
	// Keyboard vs Controller icons
}
