// W_VendorAction.cpp
// C++ Widget implementation for W_VendorAction
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with buy/sell logic, amount selection, and currency handling

#include "Widgets/W_VendorAction.h"
#include "Widgets/W_VendorSlot.h"
#include "Widgets/W_GenericButton.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/InventoryManagerComponent.h"
#include "Interfaces/BPI_Controller.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"

UW_VendorAction::UW_VendorAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	AssignedItemAmount = 0;
	MaxPossibleAmount = 0;
	DesiredAmount = 1;
	AssignedItem = nullptr;
	AssignedItemPrice = 0;
	ActionType = ESLFVendorType::Buy;
	CachedItemNameText = nullptr;
	CachedCurrentAmountText = nullptr;
	CachedMaxAmountText = nullptr;
	CachedPriceText = nullptr;
	CachedTotalPriceText = nullptr;
	CachedReqCurrencyText = nullptr;
	CachedIncreaseButton = nullptr;
	CachedDecreaseButton = nullptr;
	CachedOkButtonInner = nullptr;
	CachedCancelButtonInner = nullptr;
}

void UW_VendorAction::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Bind button click events
	BindButtonEvents();

	// CRITICAL: Make this widget focusable so it can receive keyboard/gamepad input
	SetIsFocusable(true);

	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::NativeConstruct"));
}

void UW_VendorAction::NativeDestruct()
{
	// Unbind button events
	// IncreaseButton and DecreaseButton are UButton (OnClicked)
	if (CachedIncreaseButton)
	{
		CachedIncreaseButton->OnClicked.RemoveAll(this);
	}
	if (CachedDecreaseButton)
	{
		CachedDecreaseButton->OnClicked.RemoveAll(this);
	}
	// OkButton and CancelButton inner buttons (OnClicked)
	if (CachedOkButtonInner)
	{
		CachedOkButtonInner->OnClicked.RemoveAll(this);
	}
	if (CachedCancelButtonInner)
	{
		CachedCancelButtonInner->OnClicked.RemoveAll(this);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::NativeDestruct"));
}

FReply UW_VendorAction::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Up navigation - increase amount
	if (Key == EKeys::Up || Key == EKeys::W || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Up pressed - increasing amount"));
		EventNavigateVendorActionUp();
		return FReply::Handled();
	}

	// Down navigation - decrease amount
	if (Key == EKeys::Down || Key == EKeys::S || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Down pressed - decreasing amount"));
		EventNavigateVendorActionDown();
		return FReply::Handled();
	}

	// Confirm/OK - execute action (buy or sell)
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] OK pressed - executing action"));
		EventVendorActionBtnPressed();
		return FReply::Handled();
	}

	// Cancel - close popup
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Right)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Cancel pressed - closing popup"));
		OnVendorActionClosed.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_VendorAction::CacheWidgetReferences()
{
	// Cache text widget references from Blueprint UMG
	CachedItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText")));
	CachedCurrentAmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CurrentAmountText")));
	CachedMaxAmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaxAmountText")));
	CachedPriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PriceText")));
	CachedTotalPriceText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TotalPriceText")));
	CachedReqCurrencyText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ReqCurrencyText")));

	// Cache button references from Blueprint UMG
	// IncreaseButton and DecreaseButton are native UButton widgets
	CachedIncreaseButton = Cast<UButton>(GetWidgetFromName(TEXT("IncreaseButton")));
	CachedDecreaseButton = Cast<UButton>(GetWidgetFromName(TEXT("DecreaseButton")));

	// W_GB_OK and W_GB_Cancel are W_GenericButton Blueprint widgets
	// The Blueprint was NOT reparented to UW_GenericButton C++ class, so Cast fails.
	// Workaround: Find the W_GenericButton widgets and get their inner UButton
	CachedOkButtonInner = nullptr;
	CachedCancelButtonInner = nullptr;

	{
		TArray<UWidget*> AllWidgets;
		WidgetTree->GetAllWidgets(AllWidgets);

		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Searching %d widgets for OK/Cancel buttons"), AllWidgets.Num());

		for (UWidget* Widget : AllWidgets)
		{
			if (!Widget) continue;

			FString WidgetName = Widget->GetName();

			// Check for OK button widget
			if (!CachedOkButtonInner && WidgetName.Contains(TEXT("W_GB_OK")))
			{
				UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Found OK widget: %s"), *WidgetName);
				// This is a W_GenericButton_C (UUserWidget subclass)
				// Find the UButton inside it
				if (UUserWidget* GenericButtonWidget = Cast<UUserWidget>(Widget))
				{
					// Search for Button inside W_GenericButton
					if (UWidgetTree* InnerTree = GenericButtonWidget->WidgetTree)
					{
						TArray<UWidget*> InnerWidgets;
						InnerTree->GetAllWidgets(InnerWidgets);
						for (UWidget* InnerWidget : InnerWidgets)
						{
							if (UButton* InnerButton = Cast<UButton>(InnerWidget))
							{
								CachedOkButtonInner = InnerButton;
								UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Found OK inner button: %s"), *InnerButton->GetName());
							}
							// Make ButtonBorder not block clicks (it sits on top of Button in Overlay)
							if (InnerWidget->GetName().Contains(TEXT("ButtonBorder")))
							{
								InnerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
								UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Set OK ButtonBorder to SelfHitTestInvisible"));
							}
						}
					}
				}
			}

			// Check for Cancel button widget
			if (!CachedCancelButtonInner && WidgetName.Contains(TEXT("W_GB_Cancel")))
			{
				UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Found Cancel widget: %s"), *WidgetName);
				if (UUserWidget* GenericButtonWidget = Cast<UUserWidget>(Widget))
				{
					if (UWidgetTree* InnerTree = GenericButtonWidget->WidgetTree)
					{
						TArray<UWidget*> InnerWidgets;
						InnerTree->GetAllWidgets(InnerWidgets);
						for (UWidget* InnerWidget : InnerWidgets)
						{
							if (UButton* InnerButton = Cast<UButton>(InnerWidget))
							{
								CachedCancelButtonInner = InnerButton;
								UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Found Cancel inner button: %s"), *InnerButton->GetName());
							}
							// Make ButtonBorder not block clicks (it sits on top of Button in Overlay)
							if (InnerWidget->GetName().Contains(TEXT("ButtonBorder")))
							{
								InnerWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
								UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Set Cancel ButtonBorder to SelfHitTestInvisible"));
							}
						}
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] CacheWidgetReferences - ItemName: %s, CurrentAmount: %s, MaxAmount: %s, ReqCurrency: %s"),
		CachedItemNameText ? TEXT("Found") : TEXT("NULL"),
		CachedCurrentAmountText ? TEXT("Found") : TEXT("NULL"),
		CachedMaxAmountText ? TEXT("Found") : TEXT("NULL"),
		CachedReqCurrencyText ? TEXT("Found") : TEXT("NULL"));

	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] CacheWidgetReferences - IncreaseBtn: %s, DecreaseBtn: %s, OkBtn: %s, CancelBtn: %s"),
		CachedIncreaseButton ? TEXT("Found") : TEXT("NULL"),
		CachedDecreaseButton ? TEXT("Found") : TEXT("NULL"),
		CachedOkButtonInner ? TEXT("Found") : TEXT("NULL"),
		CachedCancelButtonInner ? TEXT("Found") : TEXT("NULL"));
}

void UW_VendorAction::BindButtonEvents()
{
	// Bind Increase button (native UButton - uses OnClicked)
	if (CachedIncreaseButton)
	{
		CachedIncreaseButton->OnClicked.AddDynamic(this, &UW_VendorAction::HandleIncreaseButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Bound IncreaseButton OnClicked"));
	}

	// Bind Decrease button (native UButton - uses OnClicked)
	if (CachedDecreaseButton)
	{
		CachedDecreaseButton->OnClicked.AddDynamic(this, &UW_VendorAction::HandleDecreaseButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Bound DecreaseButton OnClicked"));
	}

	// Bind OK button (inner UButton from W_GenericButton - uses OnClicked)
	if (CachedOkButtonInner)
	{
		CachedOkButtonInner->OnClicked.AddDynamic(this, &UW_VendorAction::HandleOkButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Bound OkButton OnClicked"));
	}

	// Bind Cancel button (inner UButton from W_GenericButton - uses OnClicked)
	if (CachedCancelButtonInner)
	{
		CachedCancelButtonInner->OnClicked.AddDynamic(this, &UW_VendorAction::HandleCancelButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] Bound CancelButton OnClicked"));
	}
}

void UW_VendorAction::HandleIncreaseButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] HandleIncreaseButtonPressed - calling EventNavigateVendorActionUp"));
	EventNavigateVendorActionUp();
}

void UW_VendorAction::HandleDecreaseButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] HandleDecreaseButtonPressed - calling EventNavigateVendorActionDown"));
	EventNavigateVendorActionDown();
}

void UW_VendorAction::HandleOkButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] HandleOkButtonPressed - calling EventVendorActionBtnPressed"));
	EventVendorActionBtnPressed();
}

void UW_VendorAction::HandleCancelButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] HandleCancelButtonPressed - broadcasting OnVendorActionClosed"));
	OnVendorActionClosed.Broadcast();
}

void UW_VendorAction::UpdateDisplayedInfo()
{
	// Update item name
	if (CachedItemNameText && AssignedItem)
	{
		// Try to get display name from UPDA_Item
		UPDA_Item* ItemData = Cast<UPDA_Item>(AssignedItem);
		if (ItemData)
		{
			CachedItemNameText->SetText(ItemData->DisplayName);
		}
		else
		{
			CachedItemNameText->SetText(FText::FromString(AssignedItem->GetName()));
		}
	}

	// Update current amount (desired amount)
	if (CachedCurrentAmountText)
	{
		CachedCurrentAmountText->SetText(FText::AsNumber(DesiredAmount));
	}

	// Update max amount
	if (CachedMaxAmountText)
	{
		CachedMaxAmountText->SetText(FText::AsNumber(MaxPossibleAmount));
	}

	// Update price per item
	if (CachedPriceText)
	{
		CachedPriceText->SetText(FText::AsNumber(AssignedItemPrice));
	}

	// Update total price
	int32 TotalPrice = DesiredAmount * AssignedItemPrice;
	if (CachedTotalPriceText)
	{
		CachedTotalPriceText->SetText(FText::AsNumber(TotalPrice));
	}

	// Update required currency text (same as total price)
	if (CachedReqCurrencyText)
	{
		CachedReqCurrencyText->SetText(FText::AsNumber(TotalPrice));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] UpdateDisplayedInfo - Item: %s, Desired: %d, Max: %d, Price: %d, Total: %d"),
		AssignedItem ? *AssignedItem->GetName() : TEXT("None"),
		DesiredAmount, MaxPossibleAmount, AssignedItemPrice, TotalPrice);
}

void UW_VendorAction::UpdateButtonStates()
{
	// Determine if player can afford at least one item
	bool bCanAfford = (MaxPossibleAmount > 0);

	// Disable OK button if player can't afford anything
	if (CachedOkButtonInner)
	{
		CachedOkButtonInner->SetIsEnabled(bCanAfford);
		UE_LOG(LogTemp, Log, TEXT("[W_VendorAction] OK button %s (MaxPossible: %d)"),
			bCanAfford ? TEXT("ENABLED") : TEXT("DISABLED"), MaxPossibleAmount);
	}

	// Disable increase button if at max or can't afford any
	if (CachedIncreaseButton)
	{
		CachedIncreaseButton->SetIsEnabled(bCanAfford && DesiredAmount < MaxPossibleAmount);
	}

	// Disable decrease button if at minimum (1) or can't afford any
	if (CachedDecreaseButton)
	{
		CachedDecreaseButton->SetIsEnabled(bCanAfford && DesiredAmount > 1);
	}

	// Update the required currency text color to indicate insufficient funds
	if (CachedReqCurrencyText)
	{
		if (bCanAfford)
		{
			// Normal color - white/default
			CachedReqCurrencyText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}
		else
		{
			// Red color to indicate insufficient funds
			CachedReqCurrencyText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
	}
}

/**
 * GetMaxPossibleAmount - Calculate maximum buy/sell amount
 *
 * Blueprint Logic:
 * For Buy:
 * 1. Get player's current currency
 * 2. Calculate max affordable = currency / item price
 * 3. Clamp to vendor's available stock (if not infinite)
 *
 * For Sell:
 * 1. Get owned amount of item from inventory
 * 2. Return that amount
 */
void UW_VendorAction::GetMaxPossibleAmount_Implementation(int32& OutMaxPossible, int32& OutMaxPossible1)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::GetMaxPossibleAmount - ActionType: %d, Price: %d"),
		static_cast<int32>(ActionType), AssignedItemPrice);

	OutMaxPossible = 0;
	OutMaxPossible1 = 0;

	// Use GetOwningPlayer() - same pattern as W_Inventory and W_NPC_Window_Vendor
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("  GetMaxPossibleAmount - No OwningPlayer!"));
		return;
	}

	// Get currency via BPI_Controller interface - same pattern as W_NPC_Window_Vendor::GetPlayerCurrency
	int32 PlayerCurrency = 0;
	if (PC->Implements<UBPI_Controller>())
	{
		IBPI_Controller::Execute_GetCurrency(PC, PlayerCurrency);
		UE_LOG(LogTemp, Log, TEXT("  GetMaxPossibleAmount - Got currency via BPI_Controller: %d"), PlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  GetMaxPossibleAmount - PC does not implement BPI_Controller!"));
	}

	if (ActionType == ESLFVendorType::Buy)
	{
		// For buying: max is min of (what player can afford, vendor stock)
		if (AssignedItemPrice > 0)
		{
			int32 MaxAffordable = PlayerCurrency / AssignedItemPrice;
			UE_LOG(LogTemp, Log, TEXT("  Buy - Price: %d, MaxAffordable: %d"), AssignedItemPrice, MaxAffordable);

			// Check vendor stock
			if (IsValid(SelectedSlot))
			{
				UE_LOG(LogTemp, Log, TEXT("  Buy - Slot Count: %d, Infinite: %s"),
					SelectedSlot->Count, SelectedSlot->Infinite ? TEXT("true") : TEXT("false"));

				if (SelectedSlot->Infinite)
				{
					OutMaxPossible = MaxAffordable;
				}
				else
				{
					OutMaxPossible = FMath::Min(MaxAffordable, SelectedSlot->Count);
				}
			}
			else
			{
				OutMaxPossible = MaxAffordable;
			}
		}
		else
		{
			// Free item - limit by stock only
			if (IsValid(SelectedSlot))
			{
				OutMaxPossible = SelectedSlot->Infinite ? 99 : SelectedSlot->Count;
			}
			else
			{
				OutMaxPossible = 99;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("  Buy - Final MaxPossible: %d"), OutMaxPossible);
	}
	else // Sell
	{
		// For selling: max is how many the player owns
		if (IsValid(AssignedItem))
		{
			// Get inventory manager - same pattern as W_Inventory::CacheWidgetReferences
			UInventoryManagerComponent* InventoryManager = PC->FindComponentByClass<UInventoryManagerComponent>();

			// Fallback to pawn
			if (!InventoryManager)
			{
				if (APawn* Pawn = GetOwningPlayerPawn())
				{
					InventoryManager = Pawn->FindComponentByClass<UInventoryManagerComponent>();
				}
			}

			if (InventoryManager)
			{
				int32 OwnedAmount = 0;
				bool bSuccess = false;
				InventoryManager->GetAmountOfItem(AssignedItem, OwnedAmount, bSuccess);
				if (bSuccess)
				{
					OutMaxPossible = OwnedAmount;
				}
				else
				{
					OutMaxPossible = AssignedItemAmount;
				}
			}
			else
			{
				OutMaxPossible = AssignedItemAmount;
			}
		}

		UE_LOG(LogTemp, Log, TEXT("  Sell - MaxOwned: %d"), OutMaxPossible);
	}

	OutMaxPossible1 = OutMaxPossible;
}

/**
 * BuyItem - Execute item purchase from vendor
 *
 * Blueprint Logic:
 * 1. Calculate total cost = price * desired amount
 * 2. Check player has enough currency
 * 3. Deduct currency from player
 * 4. Add item to player inventory
 * 5. Reduce vendor stock (if not infinite)
 * 6. Broadcast purchase event
 */
void UW_VendorAction::BuyItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::BuyItem - Item: %s, Amount: %d, Price: %d"),
		AssignedItem ? *AssignedItem->GetName() : TEXT("None"),
		DesiredAmount, AssignedItemPrice);

	if (!IsValid(AssignedItem) || DesiredAmount <= 0 || AssignedItemPrice < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid purchase parameters"));
		return;
	}

	// Use GetOwningPlayer() - same pattern as W_Inventory
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("  BuyItem - No OwningPlayer!"));
		return;
	}

	// Get currency via BPI_Controller interface
	int32 PlayerCurrency = 0;
	if (PC->Implements<UBPI_Controller>())
	{
		IBPI_Controller::Execute_GetCurrency(PC, PlayerCurrency);
	}

	// Calculate total cost
	int32 TotalCost = AssignedItemPrice * DesiredAmount;

	// Check if player can afford
	if (PlayerCurrency < TotalCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Not enough currency: have %d, need %d"), PlayerCurrency, TotalCost);
		return;
	}

	// Deduct currency via BPI_Controller interface
	if (PC->Implements<UBPI_Controller>())
	{
		IBPI_Controller::Execute_AdjustCurrency(PC, -TotalCost);
	}

	// Get inventory manager - same pattern as W_Inventory::CacheWidgetReferences
	UInventoryManagerComponent* InventoryManager = PC->FindComponentByClass<UInventoryManagerComponent>();
	if (!InventoryManager)
	{
		if (APawn* Pawn = GetOwningPlayerPawn())
		{
			InventoryManager = Pawn->FindComponentByClass<UInventoryManagerComponent>();
		}
	}

	// Add item to inventory
	if (InventoryManager)
	{
		InventoryManager->AddItem(AssignedItem, DesiredAmount, true);
		UE_LOG(LogTemp, Log, TEXT("  Purchased %d x %s for %d"), DesiredAmount, *AssignedItem->GetName(), TotalCost);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  BuyItem - No InventoryManagerComponent found!"));
	}

	// Broadcast event - HandleVendorItemPurchased in W_NPC_Window_Vendor will update the slot count
	if (IsValid(SelectedSlot))
	{
		OnVendorItemPurchased.Broadcast(SelectedSlot, DesiredAmount);
		// NOTE: Slot count update is done in HandleVendorItemPurchased, not here
	}
}

/**
 * SellItem - Execute item sale to vendor
 *
 * Blueprint Logic:
 * 1. Calculate total value = price * desired amount
 * 2. Check player has item in inventory
 * 3. Remove item from player inventory
 * 4. Add currency to player
 * 5. Broadcast sale event
 */
void UW_VendorAction::SellItem_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::SellItem - Item: %s, Amount: %d, Price: %d"),
		AssignedItem ? *AssignedItem->GetName() : TEXT("None"),
		DesiredAmount, AssignedItemPrice);

	if (!IsValid(AssignedItem) || DesiredAmount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid sale parameters"));
		return;
	}

	// Use GetOwningPlayer() - same pattern as W_Inventory
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("  SellItem - No OwningPlayer!"));
		return;
	}

	// Get inventory manager - same pattern as W_Inventory::CacheWidgetReferences
	UInventoryManagerComponent* InventoryManager = PC->FindComponentByClass<UInventoryManagerComponent>();
	if (!InventoryManager)
	{
		if (APawn* Pawn = GetOwningPlayerPawn())
		{
			InventoryManager = Pawn->FindComponentByClass<UInventoryManagerComponent>();
		}
	}

	if (!InventoryManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("  SellItem - No InventoryManagerComponent found!"));
		return;
	}

	// Calculate total value
	int32 TotalValue = AssignedItemPrice * DesiredAmount;

	UE_LOG(LogTemp, Log, TEXT("  Selling: %d x %s @ %d each = %d total"),
		DesiredAmount, *AssignedItem->GetName(), AssignedItemPrice, TotalValue);

	// Remove item from inventory
	UE_LOG(LogTemp, Log, TEXT("  Calling InventoryManager->RemoveItem(%s, %d)"), *AssignedItem->GetName(), DesiredAmount);
	InventoryManager->RemoveItem(AssignedItem, DesiredAmount);

	// Add currency via BPI_Controller interface
	if (PC->Implements<UBPI_Controller>())
	{
		UE_LOG(LogTemp, Log, TEXT("  Calling AdjustCurrency(+%d) via BPI_Controller"), TotalValue);
		IBPI_Controller::Execute_AdjustCurrency(PC, TotalValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  PC does NOT implement BPI_Controller - currency NOT added!"));
	}

	UE_LOG(LogTemp, Log, TEXT("  SOLD: %d x %s for %d currency"), DesiredAmount, *AssignedItem->GetName(), TotalValue);

	// Broadcast event
	OnVendorItemSold.Broadcast(AssignedItem, DesiredAmount);
}

/**
 * EventNavigateConfirmButtonsHorizontal - Toggle between confirm/cancel buttons
 */
void UW_VendorAction::EventNavigateConfirmButtonsHorizontal_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::EventNavigateConfirmButtonsHorizontal"));

	// Would toggle selection between confirm and cancel buttons
	// This is for horizontal navigation when confirm/cancel are side by side
}

/**
 * EventNavigateVendorActionDown - Decrease desired amount
 */
void UW_VendorAction::EventNavigateVendorActionDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::EventNavigateVendorActionDown"));

	// Decrease amount (with wrapping or clamping)
	if (DesiredAmount > 1)
	{
		DesiredAmount--;
		UpdateDisplayedInfo();
		UpdateButtonStates();
		UE_LOG(LogTemp, Log, TEXT("  New desired amount: %d"), DesiredAmount);
	}
}

/**
 * EventNavigateVendorActionUp - Increase desired amount
 */
void UW_VendorAction::EventNavigateVendorActionUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::EventNavigateVendorActionUp"));

	// Increase amount up to max
	if (DesiredAmount < MaxPossibleAmount)
	{
		DesiredAmount++;
		UpdateDisplayedInfo();
		UpdateButtonStates();
		UE_LOG(LogTemp, Log, TEXT("  New desired amount: %d"), DesiredAmount);
	}
}

/**
 * EventSetupVendorAction - Initialize vendor action dialog
 *
 * Blueprint Logic:
 * 1. Store selected slot reference
 * 2. Extract item data from slot
 * 3. Set action type (buy/sell)
 * 4. Calculate max possible amount
 * 5. Reset desired amount to 1
 * 6. Update UI display
 */
void UW_VendorAction::EventSetupVendorAction_Implementation(UW_VendorSlot* InSelectedSlot, uint8 InType)
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::EventSetupVendorAction - Type: %d"), InType);

	SelectedSlot = InSelectedSlot;
	ActionType = static_cast<ESLFVendorType>(InType);

	if (!IsValid(SelectedSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid slot"));
		return;
	}

	// Extract item data from slot
	AssignedItem = SelectedSlot->AssignedItem;
	AssignedItemPrice = SelectedSlot->AssignedItemPrice;
	AssignedItemAmount = SelectedSlot->Count;

	UE_LOG(LogTemp, Log, TEXT("  Item: %s, Price: %d, Amount: %d"),
		AssignedItem ? *AssignedItem->GetName() : TEXT("None"),
		AssignedItemPrice, AssignedItemAmount);

	// Calculate max possible
	int32 MaxPossible1 = 0;
	GetMaxPossibleAmount(MaxPossibleAmount, MaxPossible1);

	// Reset desired amount - start at 1 if player can afford at least 1, otherwise 0
	if (MaxPossibleAmount > 0)
	{
		DesiredAmount = 1;
	}
	else
	{
		DesiredAmount = 0;
	}

	UE_LOG(LogTemp, Log, TEXT("  MaxPossible: %d, DesiredAmount: %d"), MaxPossibleAmount, DesiredAmount);

	// Update button states based on whether player can afford any
	UpdateButtonStates();

	// CRITICAL: Update the UI display with the new item info
	UpdateDisplayedInfo();
}

/**
 * EventVendorActionBtnPressed - Execute the vendor action (buy or sell)
 *
 * Blueprint Logic:
 * 1. Check action type
 * 2. Call BuyItem or SellItem accordingly
 * 3. Close the dialog
 */
void UW_VendorAction::EventVendorActionBtnPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_VendorAction::EventVendorActionBtnPressed - ActionType: %d, DesiredAmount: %d, MaxPossible: %d"),
		static_cast<int32>(ActionType), DesiredAmount, MaxPossibleAmount);

	// Safety check - don't allow action if player can't afford anything
	if (DesiredAmount <= 0 || MaxPossibleAmount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  Cannot complete action - insufficient funds or no items selected"));
		// Don't close the popup - let player cancel or fix the issue
		return;
	}

	if (ActionType == ESLFVendorType::Buy)
	{
		BuyItem();
	}
	else // Sell
	{
		SellItem();
	}

	// Close the dialog - W_NPC_Window_Vendor will hide the overlay via EventCloseVendorAction
	// Do NOT call SetVisibility here - the overlay container manages visibility
	OnVendorActionClosed.Broadcast();
}
