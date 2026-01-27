// W_InventoryAction.cpp
// C++ Widget implementation for W_InventoryAction
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with action button navigation

#include "Widgets/W_InventoryAction.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_Inventory_ActionButton.h"
#include "Widgets/W_InventoryActionAmount.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/WidgetSwitcher.h"
#include "SLFPrimaryDataAssets.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"

UW_InventoryAction::UW_InventoryAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	NavigationIndex = 0;
	SlotIndex = 0;
	bIsStorageSlot = false;
	bInStorageMode = false;

	// Initialize cached pointers
	CachedSwitcher = nullptr;
	CachedAmountPanel = nullptr;
	bInAmountSelectionMode = false;
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
	// Cache Switcher (root widget that switches between main panel and amount panel)
	// bp_only: RootWidgetName = "Switcher"
	CachedSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("Switcher")));

	// Cache AmountActionPanel (W_InventoryActionAmount child widget)
	// bp_only: WidgetTree shows "AmountActionPanel" as W_InventoryActionAmount_C
	CachedAmountPanel = Cast<UW_InventoryActionAmount>(GetWidgetFromName(TEXT("AmountActionPanel")));

	// Bind to amount panel events
	if (CachedAmountPanel)
	{
		CachedAmountPanel->OnRequestConfirmed.AddUniqueDynamic(this, &UW_InventoryAction::OnAmountConfirmedHandler);
		CachedAmountPanel->OnRequestCanceled.AddUniqueDynamic(this, &UW_InventoryAction::OnAmountCanceledHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::CacheWidgetReferences - Bound to AmountActionPanel events"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::CacheWidgetReferences - Switcher: %s, AmountPanel: %s"),
		CachedSwitcher ? TEXT("found") : TEXT("null"),
		CachedAmountPanel ? TEXT("found") : TEXT("null"));

	// Populate ButtonEntries from UMG designer widgets
	ButtonEntries.Empty();

	// Find action buttons by name - order matters for navigation
	// Use, Leave, Discard are the main buttons
	// LeaveAmount and DiscardAmount are for quantity selection
	if (UW_Inventory_ActionButton* UseBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_Use"))))
	{
		ButtonEntries.Add(UseBtn);
		// Bind to button's OnActionButtonPressed event
		UseBtn->OnActionButtonPressed.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonPressedHandler);
		UseBtn->OnActionButtonSelected.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonSelectedHandler);
	}
	if (UW_Inventory_ActionButton* LeaveBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_Leave"))))
	{
		ButtonEntries.Add(LeaveBtn);
		LeaveBtn->OnActionButtonPressed.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonPressedHandler);
		LeaveBtn->OnActionButtonSelected.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonSelectedHandler);
	}
	if (UW_Inventory_ActionButton* DiscardBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_Discard"))))
	{
		ButtonEntries.Add(DiscardBtn);
		DiscardBtn->OnActionButtonPressed.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonPressedHandler);
		DiscardBtn->OnActionButtonSelected.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonSelectedHandler);
	}
	if (UW_Inventory_ActionButton* LeaveAmountBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_LeaveAmount"))))
	{
		ButtonEntries.Add(LeaveAmountBtn);
		LeaveAmountBtn->OnActionButtonPressed.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonPressedHandler);
		LeaveAmountBtn->OnActionButtonSelected.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonSelectedHandler);
	}
	if (UW_Inventory_ActionButton* DiscardAmountBtn = Cast<UW_Inventory_ActionButton>(GetWidgetFromName(TEXT("W_IAB_DiscardAmount"))))
	{
		ButtonEntries.Add(DiscardAmountBtn);
		DiscardAmountBtn->OnActionButtonPressed.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonPressedHandler);
		DiscardAmountBtn->OnActionButtonSelected.AddUniqueDynamic(this, &UW_InventoryAction::OnActionButtonSelectedHandler);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::CacheWidgetReferences - Found %d button entries, bound to OnActionButtonPressed"), ButtonEntries.Num());
}

void UW_InventoryAction::OnActionButtonPressedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::OnActionButtonPressedHandler - Calling EventActionButtonPressed"));
	EventActionButtonPressed();
}

void UW_InventoryAction::OnActionButtonSelectedHandler(UW_Inventory_ActionButton* Button)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::OnActionButtonSelectedHandler"));
	EventOnButtonSelected(Button);
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
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventActionButtonPressed - NavigationIndex: %d, ButtonEntries: %d, bInAmountSelectionMode: %s"),
		NavigationIndex, ButtonEntries.Num(),
		bInAmountSelectionMode ? TEXT("true") : TEXT("false"));

	// bp_only: If in amount selection mode, forward to amount panel (OK pressed = confirm)
	if (bInAmountSelectionMode && CachedAmountPanel)
	{
		UE_LOG(LogTemp, Log, TEXT("  In amount selection mode - forwarding to EventAmountConfirmButtonPressed"));
		CachedAmountPanel->EventAmountConfirmButtonPressed();
		return;
	}

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No valid slot or item - SelectedSlot: %s, IsOccupied: %s"),
			SelectedSlot ? TEXT("valid") : TEXT("null"),
			(SelectedSlot && SelectedSlot->IsOccupied) ? TEXT("true") : TEXT("false"));
		return;
	}

	if (!ButtonEntries.IsValidIndex(NavigationIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("  NavigationIndex %d is invalid (ButtonEntries.Num: %d)"), NavigationIndex, ButtonEntries.Num());
		return;
	}

	UW_Inventory_ActionButton* SelectedButton = ButtonEntries[NavigationIndex];
	if (!SelectedButton || !SelectedButton->GetIsEnabled())
	{
		UE_LOG(LogTemp, Warning, TEXT("  SelectedButton is null or disabled - Button: %s, Enabled: %s"),
			SelectedButton ? TEXT("valid") : TEXT("null"),
			(SelectedButton && SelectedButton->GetIsEnabled()) ? TEXT("true") : TEXT("false"));
		return;
	}

	// Get inventory manager exactly like bp_only does:
	// bp_only: GetOwningPlayer() -> BFL_Helper::GetInventoryComponent() -> call functions
	// BFL_Helper uses: Target->FindComponentByClass<UAC_InventoryManager>()
	// BUT: PC_SoulslikeFramework has UInventoryManagerComponent*, not UAC_InventoryManager*
	// So we use FindComponentByClass<UInventoryManagerComponent>
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("  PlayerController is null"));
		return;
	}

	// Direct FindComponentByClass - exactly like BFL_Helper but with correct class
	UInventoryManagerComponent* InventoryManager = PC->FindComponentByClass<UInventoryManagerComponent>();

	UE_LOG(LogTemp, Log, TEXT("  PC: %s, FindComponentByClass result: %s"),
		*PC->GetName(),
		InventoryManager ? *InventoryManager->GetName() : TEXT("NULL"));

	if (!InventoryManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("  InventoryManager not found on PlayerController"));
		return;
	}

	// Execute action based on button type and context
	// Button indices differ based on context:
	//
	// Inventory context (EventSetupForInventory):
	//   0 = Use, 1 = Leave, 2 = Discard, 3 = LeaveAmount, 4 = DiscardAmount
	//
	// Storage context (EventSetupForStorage):
	//   0 = Retrieve, 1 = Discard
	//
	// When bInStorageMode is true and we have an inventory slot, "Leave" becomes "Store"

	// UInventoryManagerComponent uses integer SlotIndex, not slot pointers
	// SlotIndex is set by W_Inventory before showing the action menu
	UE_LOG(LogTemp, Log, TEXT("  Executing action - ButtonIndex: %d, SlotIndex: %d, Slot: %s, IsStorageSlot: %s, InStorageMode: %s"),
		NavigationIndex,
		SlotIndex,
		SelectedSlot ? *SelectedSlot->GetName() : TEXT("null"),
		bIsStorageSlot ? TEXT("true") : TEXT("false"),
		bInStorageMode ? TEXT("true") : TEXT("false"));

	if (bIsStorageSlot)
	{
		// Storage slot context - actions are Retrieve/Discard
		// bp_only: Works with slot widgets, extracts item from slot->AssignedItem
		// We do the same - get item directly from SelectedSlot
		UDataAsset* StorageItem = SelectedSlot->AssignedItem;

		switch (NavigationIndex)
		{
		case 0: // Retrieve - move item from storage to inventory
			if (StorageItem)
			{
				UE_LOG(LogTemp, Log, TEXT("  Action: Retrieve from storage - Item: %s"), *StorageItem->GetName());
				// Add to inventory, remove from storage
				InventoryManager->AddItem(StorageItem, 1, false);
				InventoryManager->RemoveStoredItem(StorageItem, 1);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  Action: Retrieve failed - SelectedSlot->AssignedItem is null"));
			}
			break;

		case 1: // Discard from storage
			if (StorageItem)
			{
				UE_LOG(LogTemp, Log, TEXT("  Action: Discard from storage - Item: %s"), *StorageItem->GetName());
				// Just remove from storage
				InventoryManager->RemoveStoredItem(StorageItem, 1);
			}
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("  Unknown storage action index: %d"), NavigationIndex);
			break;
		}
	}
	else
	{
		// Inventory slot context - actions are Use/Leave/Discard
		// bp_only: Works with slot widgets, extracts item from slot->AssignedItem
		UDataAsset* InventoryItem = SelectedSlot->AssignedItem;

		switch (NavigationIndex)
		{
		case 0: // Use
			if (InventoryItem)
			{
				UE_LOG(LogTemp, Log, TEXT("  Action: Use item - Item: %s"), *InventoryItem->GetName());
				InventoryManager->UseItemFromInventory(InventoryItem);
			}
			break;

		case 1: // Leave - but if in storage mode, this becomes Store
			if (bInStorageMode)
			{
				// bp_only: AddItemAtSlotToStorage gets item from slot->AssignedItem, then:
				// 1. Adds item to StoredItems map
				// 2. Removes item from Items map
				// We replicate this by using AddItemToStorage + RemoveItem directly
				UDataAsset* ItemToStore = SelectedSlot->AssignedItem;
				if (ItemToStore)
				{
					UE_LOG(LogTemp, Log, TEXT("  Action: Store to storage - Item: %s"), *ItemToStore->GetName());
					InventoryManager->AddItemToStorage(ItemToStore, 1);
					InventoryManager->RemoveItem(ItemToStore, 1);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("  Action: Store failed - SelectedSlot->AssignedItem is null"));
				}
			}
			else
			{
				// For Leave (drop to ground), we also use the item directly
				UDataAsset* ItemToLeave = SelectedSlot->AssignedItem;
				if (ItemToLeave)
				{
					UE_LOG(LogTemp, Log, TEXT("  Action: Leave/drop on ground - Item: %s"), *ItemToLeave->GetName());
					// LeaveItemAtSlot spawns a pickup actor - we need to use the slot index for this
					// because we need to remove from the correct TMap entry
					// Find the slot key by matching the item
					for (const auto& Pair : InventoryManager->Items)
					{
						if (Pair.Value.ItemAsset == ItemToLeave)
						{
							InventoryManager->LeaveItemAtSlot(Pair.Key, 1);
							break;
						}
					}
				}
			}
			break;

		case 2: // Discard
			if (InventoryItem)
			{
				UE_LOG(LogTemp, Log, TEXT("  Action: Discard - Item: %s"), *InventoryItem->GetName());
				// Just remove from inventory (destroy the item)
				InventoryManager->RemoveItem(InventoryItem, 1);
			}
			break;

		case 3: // LeaveAmount (for stackable items) - show amount selector
			// bp_only: When LeaveAmount pressed, switch to amount selector panel
			// In storage mode, this becomes StoreAmount
			if (bInStorageMode)
			{
				UE_LOG(LogTemp, Log, TEXT("  Action: StoreAmount - showing amount selector"));
				ShowAmountSelector(ESLFInventoryAmountedActionType::StoreAmount);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("  Action: LeaveAmount - showing amount selector"));
				ShowAmountSelector(ESLFInventoryAmountedActionType::LeaveAmount);
			}
			return;  // Don't hide menu - amount selector is now visible

		case 4: // DiscardAmount (for stackable items) - show amount selector
			UE_LOG(LogTemp, Log, TEXT("  Action: DiscardAmount - showing amount selector"));
			ShowAmountSelector(ESLFInventoryAmountedActionType::DiscardAmount);
			return;  // Don't hide menu - amount selector is now visible

		default:
			UE_LOG(LogTemp, Warning, TEXT("  Unknown inventory action index: %d"), NavigationIndex);
			break;
		}
	}

	// Hide action menu after executing action
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * EventNavigateActionConfirmHorizontal - Horizontal navigation for confirm buttons
 *
 * bp_only: When in amount selection mode, forward to AmountActionPanel
 */
void UW_InventoryAction::EventNavigateActionConfirmHorizontal_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventNavigateActionConfirmHorizontal - bInAmountSelectionMode: %s"),
		bInAmountSelectionMode ? TEXT("true") : TEXT("false"));

	// If in amount selection mode, forward to amount panel
	if (bInAmountSelectionMode && CachedAmountPanel)
	{
		CachedAmountPanel->EventNavigateConfirmButtonsHorizontal();
		return;
	}

	// Button menu mode - no horizontal navigation needed for vertical button list
}

/**
 * EventNavigateActionDown - Navigate to next action button
 *
 * bp_only: When in amount selection mode, forward to AmountActionPanel (decrease amount)
 */
void UW_InventoryAction::EventNavigateActionDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventNavigateActionDown - bInAmountSelectionMode: %s"),
		bInAmountSelectionMode ? TEXT("true") : TEXT("false"));

	// If in amount selection mode, forward to amount panel (down = decrease amount)
	if (bInAmountSelectionMode && CachedAmountPanel)
	{
		CachedAmountPanel->EventNavigateAmountDown();
		return;
	}

	// Button menu mode - navigate buttons
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
 *
 * bp_only: When in amount selection mode, forward to AmountActionPanel (increase amount)
 */
void UW_InventoryAction::EventNavigateActionUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventNavigateActionUp - bInAmountSelectionMode: %s"),
		bInAmountSelectionMode ? TEXT("true") : TEXT("false"));

	// If in amount selection mode, forward to amount panel (up = increase amount)
	if (bInAmountSelectionMode && CachedAmountPanel)
	{
		CachedAmountPanel->EventNavigateAmountUp();
		return;
	}

	// Button menu mode - navigate buttons
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
 * EventSetupForInventory - Configure for inventory item
 *
 * bp_only Logic:
 * - If bInStorageMode is TRUE: IMMEDIATELY show amount selector with StoreAmount
 *   (User wants to move item FROM inventory TO storage)
 * - If bInStorageMode is FALSE: Show button menu (Use/Leave/Discard/etc.)
 *
 * Button menu buttons (non-storage mode):
 * - Use: enabled if (bUsable AND ActionToTrigger.IsValid) OR (bUsable AND ItemClass.IsValid)
 * - Leave: always enabled
 * - Discard: always enabled
 * - LeaveAmount/DiscardAmount: enabled if item count > 1
 */
void UW_InventoryAction::EventSetupForInventory_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventSetupForInventory - bInStorageMode: %s"),
		bInStorageMode ? TEXT("true") : TEXT("false"));

	SelectedSlot = InSlot;
	bIsStorageSlot = false;  // This is an inventory slot, not a storage slot

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid slot or not occupied"));
		return;
	}

	// bp_only: If in storage mode, immediately show amount selector for StoreAmount
	// This happens when clicking an inventory item while the storage UI is open
	if (bInStorageMode)
	{
		ESLFInventoryAmountedActionType Operation = ESLFInventoryAmountedActionType::StoreAmount;

		UE_LOG(LogTemp, Log, TEXT("  Storage mode - showing IMMEDIATE amount selector for StoreAmount"));
		UE_LOG(LogTemp, Log, TEXT("  Slot: %s, Item: %s, Count: %d"),
			*SelectedSlot->GetName(),
			SelectedSlot->AssignedItem ? *SelectedSlot->AssignedItem->GetName() : TEXT("null"),
			SelectedSlot->Count);

		bInAmountSelectionMode = true;

		// Initialize the amount panel with StoreAmount operation
		if (CachedAmountPanel)
		{
			CachedAmountPanel->EventInitializeForSlot(static_cast<uint8>(Operation), SelectedSlot);
			UE_LOG(LogTemp, Log, TEXT("  Initialized AmountActionPanel for StoreAmount"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  AmountActionPanel not found!"));
		}

		// Immediately switch to amount selection panel (index 1) - NO button menu!
		if (CachedSwitcher)
		{
			CachedSwitcher->SetActiveWidgetIndex(1);
			UE_LOG(LogTemp, Log, TEXT("  Switched to AmountActionPanel (index 1) - skipping button menu"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Switcher not found!"));
		}

		return;  // Don't show button menu
	}

	// Non-storage mode: Show button menu with Use/Leave/Discard options
	UE_LOG(LogTemp, Log, TEXT("  Normal inventory mode - showing button menu"));

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

	// Ensure we're on the main panel (index 0) with button menu visible
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(0);
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
 * EventSetupForStorage - Configure for storage item (IMMEDIATE amount selector)
 *
 * bp_only Logic (from W_InventoryAction.json EventSetupForStorage graph):
 * 1. Set SelectedSlot
 * 2. Call AmountActionPanel.EventInitializeForSlot(OperationType, Slot)
 * 3. Immediately switch Switcher to index 1 (AmountActionPanel)
 *
 * This does NOT show the button menu - it goes directly to amount selection!
 * OperationType for storage slots is RetrieveAmount (moving FROM storage TO inventory)
 */
void UW_InventoryAction::EventSetupForStorage_Implementation(UW_InventorySlot* InSelectedSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::EventSetupForStorage - IMMEDIATE amount selector"));

	SelectedSlot = InSelectedSlot;
	bIsStorageSlot = true;  // This is a storage slot

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid slot or not occupied"));
		return;
	}

	// bp_only: Storage slots use RetrieveAmount operation (moving FROM storage TO inventory)
	// This is called when clicking an item that's already IN the storage container
	ESLFInventoryAmountedActionType Operation = ESLFInventoryAmountedActionType::RetrieveAmount;

	UE_LOG(LogTemp, Log, TEXT("  Slot: %s, Item: %s, Count: %d, Operation: RetrieveAmount"),
		*SelectedSlot->GetName(),
		SelectedSlot->AssignedItem ? *SelectedSlot->AssignedItem->GetName() : TEXT("null"),
		SelectedSlot->Count);

	bInAmountSelectionMode = true;

	// Initialize the amount panel with RetrieveAmount operation
	if (CachedAmountPanel)
	{
		CachedAmountPanel->EventInitializeForSlot(static_cast<uint8>(Operation), SelectedSlot);
		UE_LOG(LogTemp, Log, TEXT("  Initialized AmountActionPanel for RetrieveAmount"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  AmountActionPanel not found!"));
	}

	// Immediately switch to amount selection panel (index 1) - NO button menu!
	// bp_only: Switcher has MainPanel at index 0, AmountActionPanel at index 1
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(1);
		UE_LOG(LogTemp, Log, TEXT("  Switched to AmountActionPanel (index 1) - skipping button menu"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Switcher not found!"));
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

/**
 * ShowAmountSelector - Switch to amount selection panel
 *
 * bp_only logic:
 * 1. Set bInAmountSelectionMode = true
 * 2. Initialize AmountActionPanel with OperationType and SelectedSlot
 * 3. Switch Switcher to index 1 (AmountActionPanel)
 */
void UW_InventoryAction::ShowAmountSelector(ESLFInventoryAmountedActionType Operation)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::ShowAmountSelector - Operation: %d, Slot: %s"),
		static_cast<int32>(Operation),
		SelectedSlot ? *SelectedSlot->GetName() : TEXT("null"));

	bInAmountSelectionMode = true;

	// Initialize the amount panel with the operation type and selected slot
	if (CachedAmountPanel && SelectedSlot)
	{
		CachedAmountPanel->EventInitializeForSlot(static_cast<uint8>(Operation), SelectedSlot);
		UE_LOG(LogTemp, Log, TEXT("  Initialized AmountActionPanel"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  AmountActionPanel or SelectedSlot is null - AmountPanel: %s, Slot: %s"),
			CachedAmountPanel ? TEXT("valid") : TEXT("null"),
			SelectedSlot ? TEXT("valid") : TEXT("null"));
	}

	// Switch to amount selection panel (index 1)
	// bp_only: Switcher has MainPanel at index 0, AmountActionPanel at index 1
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(1);
		UE_LOG(LogTemp, Log, TEXT("  Switched to AmountActionPanel (index 1)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Switcher not found"));
	}
}

/**
 * OnAmountConfirmedHandler - Handle amount selection confirmation
 *
 * bp_only logic:
 * 1. Perform the actual inventory action with the selected Amount
 * 2. Switch back to main panel (index 0)
 * 3. Hide the action menu
 */
void UW_InventoryAction::OnAmountConfirmedHandler(int32 Amount, ESLFInventoryAmountedActionType Operation)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::OnAmountConfirmedHandler - Amount: %d, Operation: %d"),
		Amount, static_cast<int32>(Operation));

	bInAmountSelectionMode = false;

	// Get inventory manager
	APlayerController* PC = GetOwningPlayer();
	UInventoryManagerComponent* InventoryManager = PC ? PC->FindComponentByClass<UInventoryManagerComponent>() : nullptr;

	if (!InventoryManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("  InventoryManager not found"));
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	if (!SelectedSlot || !SelectedSlot->AssignedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("  No valid slot or item"));
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	UDataAsset* Item = SelectedSlot->AssignedItem;

	// Perform action based on operation type
	switch (Operation)
	{
	case ESLFInventoryAmountedActionType::LeaveAmount:
		// bp_only: Leave X amount of items on the ground
		UE_LOG(LogTemp, Log, TEXT("  Performing LeaveAmount: %d of %s"), Amount, *Item->GetName());
		// Find the slot key by matching the item
		for (const auto& Pair : InventoryManager->Items)
		{
			if (Pair.Value.ItemAsset == Item)
			{
				InventoryManager->LeaveItemAtSlot(Pair.Key, Amount);
				break;
			}
		}
		break;

	case ESLFInventoryAmountedActionType::DiscardAmount:
		// bp_only: Discard X amount of items (destroy them)
		UE_LOG(LogTemp, Log, TEXT("  Performing DiscardAmount: %d of %s"), Amount, *Item->GetName());
		InventoryManager->RemoveItem(Item, Amount);
		break;

	case ESLFInventoryAmountedActionType::StoreAmount:
		// bp_only: Store X amount of items to storage
		UE_LOG(LogTemp, Log, TEXT("  Performing StoreAmount: %d of %s"), Amount, *Item->GetName());
		InventoryManager->AddItemToStorage(Item, Amount);
		InventoryManager->RemoveItem(Item, Amount);
		break;

	case ESLFInventoryAmountedActionType::RetrieveAmount:
		// bp_only: Retrieve X amount of items from storage
		UE_LOG(LogTemp, Log, TEXT("  Performing RetrieveAmount: %d of %s"), Amount, *Item->GetName());
		InventoryManager->AddItem(Item, Amount, false);
		InventoryManager->RemoveStoredItem(Item, Amount);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("  Unknown operation type: %d"), static_cast<int32>(Operation));
		break;
	}

	// Switch back to main panel
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(0);
	}

	// Hide action menu
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * OnAmountCanceledHandler - Handle amount selection cancellation
 *
 * bp_only logic:
 * 1. Switch back to main panel (index 0)
 * 2. Keep action menu visible (user can pick a different action)
 */
void UW_InventoryAction::OnAmountCanceledHandler(ESLFInventoryAmountedActionType Operation)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryAction::OnAmountCanceledHandler - Operation: %d"),
		static_cast<int32>(Operation));

	bInAmountSelectionMode = false;

	// Switch back to main panel (index 0)
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(0);
		UE_LOG(LogTemp, Log, TEXT("  Switched back to MainPanel (index 0)"));
	}

	// Don't hide - user can select a different action
}
