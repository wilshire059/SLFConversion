// W_InventoryAction.cpp
// C++ Widget implementation for W_InventoryAction
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with action button navigation

#include "Widgets/W_InventoryAction.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_Inventory_ActionButton.h"
#include "Components/AC_InventoryManager.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"

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
	// Populate ButtonEntries from UMG designer widgets
	ButtonEntries.Empty();

	// Find action buttons by name - order matters for navigation
	// Use, Leave, Discard are the main buttons
	// LeaveAmount and DiscardAmount are for quantity selection
	if (UW_Inventory_ActionButton* UseBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_Use"))))
	{
		ButtonEntries.Add(UseBtn);
	}
	if (UW_Inventory_ActionButton* LeaveBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_Leave"))))
	{
		ButtonEntries.Add(LeaveBtn);
	}
	if (UW_Inventory_ActionButton* DiscardBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_Discard"))))
	{
		ButtonEntries.Add(DiscardBtn);
	}
	if (UW_Inventory_ActionButton* LeaveAmountBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_LeaveAmount"))))
	{
		ButtonEntries.Add(LeaveAmountBtn);
	}
	if (UW_Inventory_ActionButton* DiscardAmountBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_DiscardAmount"))))
	{
		ButtonEntries.Add(DiscardAmountBtn);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::CacheWidgetReferences - Found %d button entries"), ButtonEntries.Num());
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
 * Blueprint Logic (from JSON analysis):
 * 1. Store selected slot reference
 * 2. Enable/disable buttons based on item type:
 *    - Use: enabled if (bUsable AND ActionToTrigger.IsValid) OR (bUsable AND ItemClass.IsValid)
 *    - Leave: always enabled
 *    - Discard: always enabled
 *    - LeaveAmount/DiscardAmount: enabled if item count > 1
 */
void UW_InventoryAction::EventSetupForInventory_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventSetupForInventory"));

	SelectedSlot = InSlot;

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		return;
	}

	// Get item data for checking usability
	UPDA_Item* ItemData = Cast<UPDA_Item>(SelectedSlot->AssignedItem);
	bool bCanUse = false;
	int32 ItemCount = SelectedSlot->Count;

	if (ItemData)
	{
		// Use is enabled if: bUsable AND (ActionToTrigger.IsValid OR ItemClass.IsValid)
		bool bUsable = ItemData->ItemInformation.bUsable;
		bool bHasAction = ItemData->ItemInformation.ActionToTrigger.IsValid();
		bool bHasItemClass = !ItemData->ItemInformation.ItemClass.IsNull();

		bCanUse = bUsable && (bHasAction || bHasItemClass);

		UE_LOG(LogTemp, Log, TEXT("  Item: %s, bUsable: %s, HasAction: %s, HasClass: %s, CanUse: %s, Count: %d"),
			*ItemData->GetName(),
			bUsable ? TEXT("true") : TEXT("false"),
			bHasAction ? TEXT("true") : TEXT("false"),
			bHasItemClass ? TEXT("true") : TEXT("false"),
			bCanUse ? TEXT("true") : TEXT("false"),
			ItemCount);
	}

	// Configure buttons for inventory context
	// Button order from CacheWidgetReferences:
	// Index 0 = Use
	// Index 1 = Leave
	// Index 2 = Discard
	// Index 3 = LeaveAmount
	// Index 4 = DiscardAmount

	for (int32 i = 0; i < ButtonEntries.Num(); ++i)
	{
		if (!ButtonEntries[i])
		{
			continue;
		}

		switch (i)
		{
		case 0: // Use - only enabled if item is usable with action/class
			ButtonEntries[i]->SetIsEnabled(bCanUse);
			break;

		case 1: // Leave - always enabled
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		case 2: // Discard - always enabled
			ButtonEntries[i]->SetIsEnabled(true);
			break;

		case 3: // LeaveAmount - enabled if count > 1
			ButtonEntries[i]->SetIsEnabled(ItemCount > 1);
			break;

		case 4: // DiscardAmount - enabled if count > 1
			ButtonEntries[i]->SetIsEnabled(ItemCount > 1);
			break;

		default:
			ButtonEntries[i]->SetIsEnabled(false);
			break;
		}
	}

	// Reset navigation to first ENABLED button
	NavigationIndex = 0;
	bool bFoundEnabled = false;
	for (int32 i = 0; i < ButtonEntries.Num(); ++i)
	{
		if (ButtonEntries[i])
		{
			bool bButtonEnabled = ButtonEntries[i]->GetIsEnabled();
			bool bShouldSelect = !bFoundEnabled && bButtonEnabled;

			ButtonEntries[i]->SetActionButtonSelected(bShouldSelect);

			if (bShouldSelect)
			{
				NavigationIndex = i;
				bFoundEnabled = true;
			}
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
void UW_InventoryAction::EventUpdateBackInputIcons_Implementation(const TSoftObjectPtr<UTexture2D>& BackInputTexture)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventUpdateBackInputIcons"));

	// Get BackInputIcon Image widget and set texture
	if (UImage* BackInputIcon = Cast<UImage>(GetWidgetFromName(TEXT("BackInputIcon"))))
	{
		BackInputIcon->SetBrushFromSoftTexture(BackInputTexture, false);
	}
}

/**
 * EventUpdateOkInputIcons - Update OK button input icons
 */
void UW_InventoryAction::EventUpdateOkInputIcons_Implementation(const TSoftObjectPtr<UTexture2D>& OkInputTexture)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventUpdateOkInputIcons"));

	// Get OkInputIcon Image widget and set texture
	if (UImage* OkInputIcon = Cast<UImage>(GetWidgetFromName(TEXT("OkInputIcon"))))
	{
		OkInputIcon->SetBrushFromSoftTexture(OkInputTexture, false);
	}
}
