// W_NPC_Window_Vendor.cpp
// C++ Widget implementation for W_NPC_Window_Vendor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_NPC_Window_Vendor.h"
#include "Widgets/W_VendorSlot.h"
#include "Widgets/W_VendorAction.h"
#include "Interfaces/BPI_Controller.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"

UW_NPC_Window_Vendor::UW_NPC_Window_Vendor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SelectedSlot(nullptr)
	, ActiveFilterCategory(ESLFItemCategory::None)
	, NavigationIndex(0)
	, CurrentVendorAsset(nullptr)
	, VendorType(ESLFVendorType::Buy)
	, CachedUniformInventoryGrid(nullptr)
	, CachedItemInfoBoxSwitcher(nullptr)
	, CachedInventoryScrollBox(nullptr)
	, CachedVendorTitleText(nullptr)
	, CachedItemNameText_Details(nullptr)
	, CachedItemCategoryText(nullptr)
	, CachedItemDescription(nullptr)
	, CachedItemIcon(nullptr)
	, CachedVendorActionPopup(nullptr)
	, CachedCharacterStatsOverlay(nullptr)
	, bActionMenuOpen(false)
{
}

void UW_NPC_Window_Vendor::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Make widget focusable for keyboard/gamepad input
	SetIsFocusable(true);

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Hidden MainBlur widget"));
	}

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::NativeConstruct"));
}

void UW_NPC_Window_Vendor::NativeDestruct()
{
	// Unbind from all slot events
	for (UW_VendorSlot* VendorSlotWidget : VendorSlots)
	{
		if (VendorSlotWidget)
		{
			VendorSlotWidget->OnSelected.RemoveAll(this);
			VendorSlotWidget->OnPressed.RemoveAll(this);
			VendorSlotWidget->OnSlotCleared.RemoveAll(this);
			VendorSlotWidget->OnSlotAssigned.RemoveAll(this);
		}
	}

	// Unbind from vendor action popup
	if (CachedVendorActionPopup)
	{
		CachedVendorActionPopup->OnVendorActionClosed.RemoveAll(this);
		CachedVendorActionPopup->OnVendorItemPurchased.RemoveAll(this);
		CachedVendorActionPopup->OnVendorItemSold.RemoveAll(this);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::NativeDestruct"));
}

FReply UW_NPC_Window_Vendor::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] NativeOnKeyDown - Key: %s, ActionMenuOpen: %s"),
		*Key.ToString(), bActionMenuOpen ? TEXT("true") : TEXT("false"));

	// If action menu is open, route input to action popup
	if (bActionMenuOpen && CachedVendorActionPopup)
	{
		// Navigate Up - increase amount
		if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
		{
			CachedVendorActionPopup->EventNavigateVendorActionUp();
			return FReply::Handled();
		}

		// Navigate Down - decrease amount
		if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
		{
			CachedVendorActionPopup->EventNavigateVendorActionDown();
			return FReply::Handled();
		}

		// Confirm/OK - execute buy/sell
		if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
		{
			CachedVendorActionPopup->EventVendorActionBtnPressed();
			return FReply::Handled();
		}

		// Cancel/Back - close action menu
		if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Right)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Cancel pressed while action menu open - closing action menu"));
			EventCloseVendorAction();
			return FReply::Handled();
		}

		// Left/Right - toggle between confirm/cancel buttons (if applicable)
		if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left ||
			Key == EKeys::D || Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
		{
			CachedVendorActionPopup->EventNavigateConfirmButtonsHorizontal();
			return FReply::Handled();
		}

		return FReply::Handled();
	}

	// Action menu not open - normal slot navigation

	// Navigate Up
	if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		EventNavigateUp();
		return FReply::Handled();
	}

	// Navigate Down
	if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		EventNavigateDown();
		return FReply::Handled();
	}

	// Navigate Left
	if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
	{
		EventNavigateLeft();
		return FReply::Handled();
	}

	// Navigate Right
	if (Key == EKeys::D || Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
	{
		EventNavigateRight();
		return FReply::Handled();
	}

	// Confirm/OK
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	// Cancel/Back - CRITICAL: This closes vendor window and returns to NPC menu
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Gamepad_Special_Right)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Cancel/Back pressed - calling EventNavigateCancel"));
		EventNavigateCancel();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_NPC_Window_Vendor::CacheWidgetReferences()
{
	// Cache UniformGridPanel for vendor slots
	CachedUniformInventoryGrid = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("UniformInventoryGrid")));

	// Cache WidgetSwitcher for info panel
	CachedItemInfoBoxSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ItemInfoBoxSwitcher")));

	// Cache ScrollBox for scrolling
	CachedInventoryScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("InventoryScrollBox")));

	// Cache text blocks
	CachedVendorTitleText = Cast<UTextBlock>(GetWidgetFromName(TEXT("VendorTitleText")));
	CachedItemNameText_Details = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Details")));
	CachedItemCategoryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemCategoryText")));
	CachedItemDescription = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDescription")));

	// Cache item icon
	CachedItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon")));

	// Cache vendor action popup overlay container (includes darkening border)
	CachedVendorActionPopupOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("VendorActionPopup")));

	// Cache the W_VendorAction widget inside the overlay
	CachedVendorActionPopup = Cast<UW_VendorAction>(GetWidgetFromName(TEXT("W_VendorAction")));

	// Cache character stats overlay
	CachedCharacterStatsOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("CharacterStatsOverlay")));

	// Bind to vendor action popup events
	if (CachedVendorActionPopup)
	{
		CachedVendorActionPopup->OnVendorActionClosed.AddDynamic(this, &UW_NPC_Window_Vendor::EventCloseVendorAction);
		CachedVendorActionPopup->OnVendorItemPurchased.AddDynamic(this, &UW_NPC_Window_Vendor::HandleVendorItemPurchased);
		CachedVendorActionPopup->OnVendorItemSold.AddDynamic(this, &UW_NPC_Window_Vendor::HandleVendorItemSold);
	}

	// Hide action popup overlay initially (like Blueprint does)
	if (CachedVendorActionPopupOverlay)
	{
		CachedVendorActionPopupOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] CacheWidgetReferences - Grid: %s, Switcher: %s, ActionPopup: %s, PopupOverlay: %s"),
		CachedUniformInventoryGrid ? TEXT("Found") : TEXT("NULL"),
		CachedItemInfoBoxSwitcher ? TEXT("Found") : TEXT("NULL"),
		CachedVendorActionPopup ? TEXT("Found") : TEXT("NULL"),
		CachedVendorActionPopupOverlay ? TEXT("Found") : TEXT("NULL"));
}

void UW_NPC_Window_Vendor::BindSlotEvents(UW_VendorSlot* VendorSlotWidget)
{
	if (!VendorSlotWidget)
	{
		return;
	}

	// Bind to slot events
	VendorSlotWidget->OnSelected.AddDynamic(this, &UW_NPC_Window_Vendor::EventOnSlotSelected);
	VendorSlotWidget->OnPressed.AddDynamic(this, &UW_NPC_Window_Vendor::EventOnSlotPressed);
	VendorSlotWidget->OnSlotCleared.AddDynamic(this, &UW_NPC_Window_Vendor::EventOnSellSlotCleared);
	VendorSlotWidget->OnSlotAssigned.AddDynamic(this, &UW_NPC_Window_Vendor::EventOnVendorSlotAssigned);
}

void UW_NPC_Window_Vendor::UpdateSlotSelection(int32 NewIndex)
{
	// Deselect previous slot
	if (SelectedSlot)
	{
		SelectedSlot->EventOnSelected(false);
	}

	// Clamp index to valid range
	if (OccupiedVendorSlots.Num() > 0)
	{
		NavigationIndex = FMath::Clamp(NewIndex, 0, OccupiedVendorSlots.Num() - 1);
		SelectedSlot = OccupiedVendorSlots[NavigationIndex];

		if (SelectedSlot)
		{
			SelectedSlot->EventOnSelected(true);

			// Update item info panel
			EventSetupItemInfoPanel(SelectedSlot);

			// Scroll slot into view
			if (CachedInventoryScrollBox)
			{
				CachedInventoryScrollBox->ScrollWidgetIntoView(SelectedSlot, true, EDescendantScrollDestination::Center);
			}
		}
	}
	else
	{
		NavigationIndex = 0;
		SelectedSlot = nullptr;
	}
}

int32 UW_NPC_Window_Vendor::GetSlotsPerRow() const
{
	if (UPDA_Vendor* VendorAsset = Cast<UPDA_Vendor>(CurrentVendorAsset))
	{
		return FMath::Max(1, VendorAsset->DefaultSlotsPerRow);
	}
	return 5; // Default fallback
}

int32 UW_NPC_Window_Vendor::GetPlayerCurrency_Implementation()
{
	// Get owning player and return their currency via BPI_Controller interface
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (PC->Implements<UBPI_Controller>())
		{
			int32 Currency = 0;
			IBPI_Controller::Execute_GetCurrency(PC, Currency);
			UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::GetPlayerCurrency - Currency: %d"), Currency);
			return Currency;
		}
	}
	return 0;
}

UW_VendorSlot* UW_NPC_Window_Vendor::GetEmptySlot_Implementation()
{
	// Iterate VendorSlots and find one that doesn't have an item assigned
	for (UW_VendorSlot* VendorSlot : VendorSlots)
	{
		if (VendorSlot && !OccupiedVendorSlots.Contains(VendorSlot))
		{
			return VendorSlot;
		}
	}
	return nullptr;
}

void UW_NPC_Window_Vendor::AddNewSlots_Implementation()
{
	// Create W_VendorSlot widgets and add to VendorSlots array
	if (UWorld* World = GetWorld())
	{
		UClass* SlotClass = LoadClass<UW_VendorSlot>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot.W_VendorSlot_C"));
		if (SlotClass)
		{
			UW_VendorSlot* NewSlot = CreateWidget<UW_VendorSlot>(World, SlotClass);
			if (NewSlot)
			{
				VendorSlots.Add(NewSlot);
				BindSlotEvents(NewSlot);

				// Add to grid if available
				if (CachedUniformInventoryGrid)
				{
					int32 SlotIndex = VendorSlots.Num() - 1;
					int32 SlotsPerRow = GetSlotsPerRow();
					int32 Row = SlotIndex / SlotsPerRow;
					int32 Column = SlotIndex % SlotsPerRow;
					CachedUniformInventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);
				}

				UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::AddNewSlots - Added new vendor slot"));
			}
		}
	}
}

void UW_NPC_Window_Vendor::EventInitializeVendor_Implementation(const FText& NpcName, UDataAsset* InVendorAsset, uint8 InVendorType)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventInitializeVendor_Implementation - NPC: %s, Type: %d"),
		*NpcName.ToString(), InVendorType);

	// CRITICAL: Make this widget visible when initializing vendor
	SetVisibility(ESlateVisibility::Visible);

	// CRITICAL: Set keyboard focus so NativeOnKeyDown receives arrow/dpad input
	SetKeyboardFocus();
	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Set visibility to Visible, keyboard focus set"));

	// CRITICAL: Reset action menu state when reinitializing
	// Only reset if action menu was previously open (shouldn't happen but log if it does)
	if (bActionMenuOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window_Vendor] EventInitializeVendor called while action menu was open - resetting!"));
	}
	bActionMenuOpen = false;
	if (CachedVendorActionPopupOverlay)
	{
		CachedVendorActionPopupOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Store vendor data
	CurrentNpcName = NpcName;
	CurrentVendorAsset = InVendorAsset;
	VendorType = static_cast<ESLFVendorType>(InVendorType);

	// Update title text
	if (CachedVendorTitleText)
	{
		CachedVendorTitleText->SetText(NpcName);
	}

	// Clear existing slots
	VendorSlots.Empty();
	OccupiedVendorSlots.Empty();
	SelectedSlot = nullptr;
	NavigationIndex = 0;

	if (CachedUniformInventoryGrid)
	{
		CachedUniformInventoryGrid->ClearChildren();
	}

	// Cast to C++ UPDA_Vendor (Blueprint is now reparented to C++ class)
	UPDA_Vendor* VendorData = Cast<UPDA_Vendor>(InVendorAsset);
	if (!VendorData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window_Vendor] EventInitializeVendor - VendorAsset cast to UPDA_Vendor failed! Class: %s"),
			InVendorAsset ? *InVendorAsset->GetClass()->GetName() : TEXT("null"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] VendorData: %s, Items: %d, DefaultSlotCount: %d"),
		*VendorData->GetName(), VendorData->Items.Num(), VendorData->DefaultSlotCount);

	int32 SlotsPerRow = GetSlotsPerRow();

	// Load slot class
	UClass* SlotClass = LoadClass<UW_VendorSlot>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot.W_VendorSlot_C"));
	if (!SlotClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[W_NPC_Window_Vendor] Failed to load W_VendorSlot class"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Determine what items to show based on VendorType
	// Buy (0) = show vendor's items for sale
	// Sell (1) = show player's inventory items to sell
	// Trade (2) = show vendor's items for trade

	if (VendorType == ESLFVendorType::Sell)
	{
		// SELL MODE: Show player's inventory items
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] SELL MODE - Getting player inventory items"));

		// Get player's inventory component
		APlayerController* PC = GetOwningPlayer();
		if (PC && PC->Implements<UBPI_Controller>())
		{
			// Get inventory component from controller
			UInventoryManagerComponent* InventoryComp = nullptr;

			// Try to find inventory component
			TArray<UActorComponent*> Components;
			PC->GetComponents(UInventoryManagerComponent::StaticClass(), Components);
			if (Components.Num() > 0)
			{
				InventoryComp = Cast<UInventoryManagerComponent>(Components[0]);
			}

			if (InventoryComp)
			{
				// Get all inventory items
				TArray<FSLFInventoryItem> AllItems = InventoryComp->GetAllItems();
				UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Found %d inventory items"), AllItems.Num());

				int32 SlotCount = FMath::Max(VendorData->DefaultSlotCount, AllItems.Num());

				// Create slots
				for (int32 i = 0; i < SlotCount; i++)
				{
					UW_VendorSlot* NewSlot = CreateWidget<UW_VendorSlot>(World, SlotClass);
					if (NewSlot)
					{
						VendorSlots.Add(NewSlot);
						BindSlotEvents(NewSlot);

						if (CachedUniformInventoryGrid)
						{
							int32 Row = i / SlotsPerRow;
							int32 Column = i % SlotsPerRow;
							CachedUniformInventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);
						}
					}
				}

				// Populate slots with inventory items
				int32 SlotIndex = 0;
				for (const FSLFInventoryItem& InvItem : AllItems)
				{
					if (SlotIndex < VendorSlots.Num() && InvItem.ItemAsset)
					{
						UPDA_Item* ItemData = Cast<UPDA_Item>(InvItem.ItemAsset);
						if (ItemData)
						{
							UW_VendorSlot* CurrentSlot = VendorSlots[SlotIndex];

							// Use item's SellPrice, with fallback if not set
							// Default sell price = 10 if SellPrice is 0 (common in Soulslike games)
							int32 ItemSellPrice = ItemData->SellPrice;
							if (ItemSellPrice <= 0)
							{
								// Fallback: use a default sell value
								// Could be enhanced to use item category or rarity
								ItemSellPrice = 10;
								UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Item %s has no SellPrice, using default: %d"),
									*ItemData->GetName(), ItemSellPrice);
							}

							CurrentSlot->EventOccupySlot(
								ItemData,
								InvItem.Amount,
								ItemSellPrice,
								false,  // Not infinite - player has limited amount
								InVendorType
							);
							OccupiedVendorSlots.Add(CurrentSlot);
						}
					}
					SlotIndex++;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window_Vendor] Could not find InventoryManagerComponent on PlayerController"));
			}
		}
	}
	else
	{
		// BUY/TRADE MODE: Show vendor's items
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] BUY/TRADE MODE - Showing vendor items"));

		int32 SlotCount = FMath::Max(VendorData->DefaultSlotCount, VendorData->Items.Num());

		// Create slots
		for (int32 i = 0; i < SlotCount; i++)
		{
			UW_VendorSlot* NewSlot = CreateWidget<UW_VendorSlot>(World, SlotClass);
			if (NewSlot)
			{
				VendorSlots.Add(NewSlot);
				BindSlotEvents(NewSlot);

				if (CachedUniformInventoryGrid)
				{
					int32 Row = i / SlotsPerRow;
					int32 Column = i % SlotsPerRow;
					CachedUniformInventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);
				}
			}
		}

		// Populate slots with vendor items from TSet
		int32 SlotIndex = 0;
		for (const FSLFVendorItems& VendorItem : VendorData->Items)
		{
			if (SlotIndex >= VendorSlots.Num())
			{
				AddNewSlots();
			}

			if (SlotIndex < VendorSlots.Num() && VendorItem.Item)
			{
				UPDA_Item* ItemAsset = Cast<UPDA_Item>(VendorItem.Item);
				if (ItemAsset)
				{
					UW_VendorSlot* CurrentSlot = VendorSlots[SlotIndex];
					UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Adding vendor item: %s (Stock: %d, Price: %d, Infinite: %s)"),
						*ItemAsset->GetName(), VendorItem.StockAmount, VendorItem.Price,
						VendorItem.bInfiniteStock ? TEXT("Yes") : TEXT("No"));

					CurrentSlot->EventOccupySlot(
						ItemAsset,
						VendorItem.StockAmount,
						VendorItem.Price,
						VendorItem.bInfiniteStock,
						InVendorType
					);
					OccupiedVendorSlots.Add(CurrentSlot);
				}
			}
			SlotIndex++;
		}
	}

	// Select first slot if available
	if (OccupiedVendorSlots.Num() > 0)
	{
		UpdateSlotSelection(0);
	}

	// Show item info panel
	EventToggleItemInfo(OccupiedVendorSlots.Num() > 0);

	UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Initialized with %d items in %d slots (Mode: %s)"),
		OccupiedVendorSlots.Num(), VendorSlots.Num(),
		VendorType == ESLFVendorType::Buy ? TEXT("Buy") : (VendorType == ESLFVendorType::Sell ? TEXT("Sell") : TEXT("Trade")));
}

void UW_NPC_Window_Vendor::EventNavigateUp_Implementation()
{
	if (bActionMenuOpen)
	{
		// Forward to action popup - Up increases amount
		if (CachedVendorActionPopup)
		{
			CachedVendorActionPopup->EventNavigateVendorActionUp();
		}
		return;
	}

	// Move up one row
	int32 SlotsPerRow = GetSlotsPerRow();
	int32 NewIndex = NavigationIndex - SlotsPerRow;

	if (NewIndex >= 0)
	{
		UpdateSlotSelection(NewIndex);
	}

	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventNavigateUp - NewIndex: %d"), NavigationIndex);
}

void UW_NPC_Window_Vendor::EventNavigateDown_Implementation()
{
	if (bActionMenuOpen)
	{
		// Forward to action popup - Down decreases amount
		if (CachedVendorActionPopup)
		{
			CachedVendorActionPopup->EventNavigateVendorActionDown();
		}
		return;
	}

	// Move down one row
	int32 SlotsPerRow = GetSlotsPerRow();
	int32 NewIndex = NavigationIndex + SlotsPerRow;

	if (NewIndex < OccupiedVendorSlots.Num())
	{
		UpdateSlotSelection(NewIndex);
	}

	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventNavigateDown - NewIndex: %d"), NavigationIndex);
}

void UW_NPC_Window_Vendor::EventNavigateLeft_Implementation()
{
	if (bActionMenuOpen)
	{
		// Forward to action popup - horizontal navigation for confirm buttons
		if (CachedVendorActionPopup)
		{
			CachedVendorActionPopup->EventNavigateConfirmButtonsHorizontal();
		}
		return;
	}

	// Move left one slot
	int32 NewIndex = NavigationIndex - 1;

	if (NewIndex >= 0)
	{
		UpdateSlotSelection(NewIndex);
	}

	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventNavigateLeft - NewIndex: %d"), NavigationIndex);
}

void UW_NPC_Window_Vendor::EventNavigateRight_Implementation()
{
	if (bActionMenuOpen)
	{
		// Forward to action popup - horizontal navigation for confirm buttons
		if (CachedVendorActionPopup)
		{
			CachedVendorActionPopup->EventNavigateConfirmButtonsHorizontal();
		}
		return;
	}

	// Move right one slot
	int32 NewIndex = NavigationIndex + 1;

	if (NewIndex < OccupiedVendorSlots.Num())
	{
		UpdateSlotSelection(NewIndex);
	}

	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventNavigateRight - NewIndex: %d"), NavigationIndex);
}

void UW_NPC_Window_Vendor::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateOk_Implementation - bActionMenuOpen: %s"),
		bActionMenuOpen ? TEXT("true") : TEXT("false"));

	// If action menu is open, execute the action (buy/sell)
	if (bActionMenuOpen)
	{
		if (CachedVendorActionPopup)
		{
			CachedVendorActionPopup->EventVendorActionBtnPressed();
		}
		return;
	}

	if (!SelectedSlot || !SelectedSlot->IsOccupied)
	{
		return;
	}

	// Open vendor action popup
	if (CachedVendorActionPopup && CachedVendorActionPopupOverlay)
	{
		// Setup the popup with slot and vendor type
		CachedVendorActionPopup->EventSetupVendorAction(SelectedSlot, static_cast<uint8>(VendorType));

		// Show the popup OVERLAY (the container includes darkening background)
		// This matches how the Blueprint shows the VendorActionPopup overlay, not W_VendorAction directly
		CachedVendorActionPopupOverlay->SetVisibility(ESlateVisibility::Visible);
		bActionMenuOpen = true;

		// NOTE: Do NOT change keyboard focus - keep it on this widget (W_NPC_Window_Vendor)
		// so it continues to receive ALL input (keyboard AND gamepad) and can forward to popup.
		// UE5 routes gamepad input differently than keyboard, so changing focus breaks gamepad.

		// Notify visibility change
		EventOnActionMenuVisibilityChanged(static_cast<uint8>(ESlateVisibility::Visible));

		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Vendor action popup overlay shown for slot: %s"),
			SelectedSlot->AssignedItem ? *SelectedSlot->AssignedItem->GetName() : TEXT("Unknown"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_NPC_Window_Vendor] CachedVendorActionPopup: %s, CachedVendorActionPopupOverlay: %s"),
			CachedVendorActionPopup ? TEXT("Valid") : TEXT("NULL"),
			CachedVendorActionPopupOverlay ? TEXT("Valid") : TEXT("NULL"));
	}
}

void UW_NPC_Window_Vendor::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventNavigateCancel_Implementation"));

	if (bActionMenuOpen)
	{
		// Close action menu first
		EventCloseVendorAction();
	}
	else
	{
		// Hide vendor window and notify parent
		SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_NPC_Window_Vendor] Collapsed and broadcasting OnVendorWindowClosed"));

		// Broadcast to parent (W_NPC_Window) so it can restore navigation
		OnVendorWindowClosed.Broadcast();

		// Do NOT call RemoveFromParent() - let parent manage visibility
	}
}

void UW_NPC_Window_Vendor::EventCloseVendorAction_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventCloseVendorAction_Implementation"));

	// Hide the popup OVERLAY (the container includes darkening background)
	if (CachedVendorActionPopupOverlay)
	{
		CachedVendorActionPopupOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	bActionMenuOpen = false;

	// NOTE: No need to restore focus - we never changed it. This widget keeps focus at all times.

	// Notify visibility change
	EventOnActionMenuVisibilityChanged(static_cast<uint8>(ESlateVisibility::Collapsed));
}

void UW_NPC_Window_Vendor::EventOnActionMenuVisibilityChanged_Implementation(uint8 InVisibility)
{
	ESlateVisibility NewVisibility = static_cast<ESlateVisibility>(InVisibility);
	bActionMenuOpen = (NewVisibility == ESlateVisibility::Visible);

	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnActionMenuVisibilityChanged_Implementation - Visible: %s"),
		bActionMenuOpen ? TEXT("true") : TEXT("false"));
}

void UW_NPC_Window_Vendor::EventOnSlotSelected_Implementation(bool Selected, UW_VendorSlot* InSlot)
{
	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventOnSlotSelected_Implementation - Selected: %s"),
		Selected ? TEXT("true") : TEXT("false"));

	if (Selected && InSlot)
	{
		// Find the index of this slot
		int32 FoundIndex = OccupiedVendorSlots.Find(InSlot);
		if (FoundIndex != INDEX_NONE && FoundIndex != NavigationIndex)
		{
			// Deselect previous slot
			if (SelectedSlot && SelectedSlot != InSlot)
			{
				SelectedSlot->EventOnSelected(false);
			}

			NavigationIndex = FoundIndex;
			SelectedSlot = InSlot;

			// Update item info panel
			EventSetupItemInfoPanel(InSlot);
		}
	}
}

void UW_NPC_Window_Vendor::EventOnSlotPressed_Implementation(UW_VendorSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnSlotPressed_Implementation"));

	if (!InSlot || !InSlot->IsOccupied)
	{
		return;
	}

	// Select this slot and open action menu
	int32 FoundIndex = OccupiedVendorSlots.Find(InSlot);
	if (FoundIndex != INDEX_NONE)
	{
		UpdateSlotSelection(FoundIndex);
	}

	// Open vendor action popup
	EventNavigateOk();
}

void UW_NPC_Window_Vendor::EventOnVendorSlotAssigned_Implementation(UW_VendorSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnVendorSlotAssigned_Implementation"));

	if (InSlot && !OccupiedVendorSlots.Contains(InSlot))
	{
		OccupiedVendorSlots.Add(InSlot);
	}
}

void UW_NPC_Window_Vendor::EventOnSellSlotCleared_Implementation(UW_VendorSlot* InSlot, bool TriggerShift)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::EventOnSellSlotCleared_Implementation - TriggerShift: %s"),
		TriggerShift ? TEXT("true") : TEXT("false"));

	if (!InSlot)
	{
		return;
	}

	// Remove from occupied slots
	OccupiedVendorSlots.Remove(InSlot);

	// If this was the selected slot, select next available
	if (SelectedSlot == InSlot)
	{
		SelectedSlot = nullptr;
		if (OccupiedVendorSlots.Num() > 0)
		{
			NavigationIndex = FMath::Clamp(NavigationIndex, 0, OccupiedVendorSlots.Num() - 1);
			UpdateSlotSelection(NavigationIndex);
		}
		else
		{
			NavigationIndex = 0;
			EventToggleItemInfo(false);
		}
	}
}

void UW_NPC_Window_Vendor::EventSetupItemInfoPanel_Implementation(UW_VendorSlot* ForSlot)
{
	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventSetupItemInfoPanel_Implementation"));

	if (!ForSlot || !ForSlot->AssignedItem)
	{
		EventToggleItemInfo(false);
		return;
	}

	UPDA_Item* ItemAsset = Cast<UPDA_Item>(ForSlot->AssignedItem);
	if (!ItemAsset)
	{
		EventToggleItemInfo(false);
		return;
	}

	// Update item name
	if (CachedItemNameText_Details)
	{
		CachedItemNameText_Details->SetText(ItemAsset->DisplayName);
	}

	// Update item category
	if (CachedItemCategoryText)
	{
		FText CategoryText = FText::FromString(
			UEnum::GetDisplayValueAsText(ItemAsset->ItemInformation.Category.Category).ToString()
		);
		CachedItemCategoryText->SetText(CategoryText);
	}

	// Update item description
	if (CachedItemDescription)
	{
		CachedItemDescription->SetText(ItemAsset->Description);
	}

	// Update item icon
	if (CachedItemIcon)
	{
		UTexture2D* IconTexture = ItemAsset->ItemInformation.IconSmall.LoadSynchronous();
		if (IconTexture)
		{
			CachedItemIcon->SetBrushFromTexture(IconTexture);
		}
	}

	// Show the info panel
	EventToggleItemInfo(true);
}

void UW_NPC_Window_Vendor::EventToggleItemInfo_Implementation(bool Visible)
{
	UE_LOG(LogTemp, Verbose, TEXT("UW_NPC_Window_Vendor::EventToggleItemInfo_Implementation - Visible: %s"),
		Visible ? TEXT("true") : TEXT("false"));

	if (CachedItemInfoBoxSwitcher)
	{
		// Index 0 = empty/hidden state, Index 1 = item info visible
		CachedItemInfoBoxSwitcher->SetActiveWidgetIndex(Visible ? 1 : 0);
	}
}

// Helper functions for vendor action popup callbacks
void UW_NPC_Window_Vendor::HandleVendorItemPurchased(UW_VendorSlot* TargetSlot, int32 PurchasedAmount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::HandleVendorItemPurchased - Amount: %d"), PurchasedAmount);

	if (TargetSlot)
	{
		// Update slot count
		int32 NewCount = TargetSlot->Count - PurchasedAmount;

		if (NewCount <= 0 && !TargetSlot->Infinite)
		{
			// Clear the slot if out of stock
			TargetSlot->EventClearSlot(false);
		}
		else if (!TargetSlot->Infinite)
		{
			// Update the count
			TargetSlot->EventChangeAmount(NewCount);
		}

		// Update item info panel
		EventSetupItemInfoPanel(TargetSlot);
	}

	// NOTE: Do NOT call EventCloseVendorAction here!
	// The popup broadcasts OnVendorActionClosed which already triggers EventCloseVendorAction.
	// Calling it here would cause a double close.
}

void UW_NPC_Window_Vendor::HandleVendorItemSold(UPrimaryDataAsset* Item, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_NPC_Window_Vendor::HandleVendorItemSold - Item: %s, Amount: %d"),
		Item ? *Item->GetName() : TEXT("None"), Amount);

	// Find the slot with this item and update its count
	if (SelectedSlot && SelectedSlot->AssignedItem == Item)
	{
		int32 NewCount = SelectedSlot->Count - Amount;

		if (NewCount <= 0)
		{
			// Clear the slot if all items sold
			SelectedSlot->EventClearSlot(false);
			UE_LOG(LogTemp, Log, TEXT("  Slot cleared - all items sold"));
		}
		else
		{
			// Update the count
			SelectedSlot->EventChangeAmount(NewCount);
			UE_LOG(LogTemp, Log, TEXT("  Slot count updated to: %d"), NewCount);
		}

		// Update item info panel
		EventSetupItemInfoPanel(SelectedSlot);
	}

	// NOTE: Do NOT call EventCloseVendorAction here!
	// The popup broadcasts OnVendorActionClosed which already triggers EventCloseVendorAction.
	// Calling it here would cause a double close.
}
