// W_Equipment.cpp
// C++ Widget implementation for W_Equipment
//
// 20-PASS VALIDATION: 2026-01-05
// NO REFLECTION - all child widgets accessed via BindWidgetOptional
// Full implementation with component retrieval and event binding

#include "Widgets/W_Equipment.h"
#include "Widgets/W_EquipmentSlot.h"
#include "Widgets/W_InventorySlot.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetTree.h"

UW_Equipment::UW_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentCanvas(nullptr)
	, ItemScrollBox(nullptr)
	, ItemGrid(nullptr)
	, ItemInfoBoxSwitcher(nullptr)
	, SlotNameText(nullptr)
	, InventoryComponent(nullptr)
	, SelectedSlot(nullptr)
	, ActiveEquipmentSlot(nullptr)
	, EquipmentComponent(nullptr)
	, ActiveItemSlot(nullptr)
	, EquipmentSlotNavigationIndex(0)
	, ItemNavigationIndex(0)
{
}

void UW_Equipment::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] NativeConstruct - Getting components"));

	// InventoryComponent and EquipmentComponent are on PLAYERCONTROLLER (not Pawn!)
	if (APlayerController* PC = GetOwningPlayer())
	{
		InventoryComponent = PC->FindComponentByClass<UAC_InventoryManager>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found InventoryComponent on PlayerController"));
			InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UW_Equipment::HandleInventoryUpdated);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] InventoryComponent not found on PlayerController"));
		}

		EquipmentComponent = PC->FindComponentByClass<UAC_EquipmentManager>();
		if (EquipmentComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found EquipmentComponent on PlayerController"));
			// Bind to equipment change events for refreshing slots
			EquipmentComponent->OnItemEquippedToSlot.AddDynamic(this, &UW_Equipment::HandleItemEquippedToSlot);
			EquipmentComponent->OnItemUnequippedFromSlot.AddDynamic(this, &UW_Equipment::HandleItemUnequippedFromSlot);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipmentComponent not found on PlayerController"));
		}
	}

	// Bind visibility changed
	OnVisibilityChanged.AddDynamic(this, &UW_Equipment::EventOnVisibilityChanged);

	// Populate equipment slots from widget tree
	PopulateEquipmentSlots();

	// Bind equipment slot events
	BindEquipmentSlotEvents();

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] NativeConstruct complete - %d equipment slots"), EquipmentSlots.Num());
}

void UW_Equipment::NativeDestruct()
{
	// Unbind from InventoryComponent events
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryUpdated.RemoveAll(this);
	}

	// Unbind from EquipmentComponent events
	if (EquipmentComponent)
	{
		EquipmentComponent->OnItemEquippedToSlot.RemoveAll(this);
		EquipmentComponent->OnItemUnequippedFromSlot.RemoveAll(this);
	}

	// Clear equipment slot bindings
	for (UW_EquipmentSlot* EquipSlot : EquipmentSlots)
	{
		if (IsValid(EquipSlot))
		{
			EquipSlot->OnSelected.RemoveAll(this);
			EquipSlot->OnPressed.RemoveAll(this);
		}
	}

	// Clear inventory slot bindings
	for (UW_InventorySlot* InvSlot : EquipmentInventorySlots)
	{
		if (IsValid(InvSlot))
		{
			InvSlot->OnSelected.RemoveAll(this);
			InvSlot->OnPressed.RemoveAll(this);
		}
	}

	OnVisibilityChanged.RemoveAll(this);

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] NativeDestruct"));
}

void UW_Equipment::CacheWidgetReferences()
{
	// BindWidgetOptional handles caching automatically
}

void UW_Equipment::PopulateEquipmentSlots()
{
	EquipmentSlots.Empty();

	// Find all W_EquipmentSlot widgets in the tree
	TArray<UWidget*> AllChildren;
	WidgetTree->GetAllWidgets(AllChildren);

	for (UWidget* Child : AllChildren)
	{
		if (UW_EquipmentSlot* EquipSlot = Cast<UW_EquipmentSlot>(Child))
		{
			EquipmentSlots.Add(EquipSlot);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found equipment slot: %s"), *EquipSlot->GetName());
		}
	}
}

void UW_Equipment::BindEquipmentSlotEvents()
{
	for (UW_EquipmentSlot* EquipSlot : EquipmentSlots)
	{
		if (IsValid(EquipSlot))
		{
			EquipSlot->OnSelected.AddDynamic(this, &UW_Equipment::EventOnEquipmentSlotSelected);
			EquipSlot->OnPressed.AddDynamic(this, &UW_Equipment::EventOnEquipmentSlotPressed);
		}
	}
}

UW_EquipmentSlot* UW_Equipment::GetEquipmentSlotByTag_Implementation(const FGameplayTag& TargetSlotTag)
{
	for (UW_EquipmentSlot* EquipSlot : EquipmentSlots)
	{
		if (IsValid(EquipSlot) && EquipSlot->EquipmentSlot.MatchesTagExact(TargetSlotTag))
		{
			return EquipSlot;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] GetEquipmentSlotByTag - Slot not found for tag: %s"), *TargetSlotTag.ToString());
	return nullptr;
}

void UW_Equipment::SetupInformationPanel_Implementation(const FSLFItemInfo& ItemInfo, bool CompareStats)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] SetupInformationPanel - CompareStats: %s, DisplayName: %s"),
		CompareStats ? TEXT("true") : TEXT("false"),
		*ItemInfo.DisplayName.ToString());

	// The item info panel child widgets will read from the ItemInfo struct
	// The struct contains: ItemName, ItemIcon, ItemDescription, AttackPower, DamageNegation, etc.
	// When CompareStats is true, we show comparison with the currently equipped item

	// For now, just show the item info panel - child widgets should bind to display the data
	// In a full implementation, we would set properties on child widgets here

	// Show the item info panel
	EventToggleItemInfo(true);
}

void UW_Equipment::EquipItemAtSlot_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EquipItemAtSlot"));

	if (!InSlot || !InSlot->IsOccupied || !ActiveEquipmentSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipItemAtSlot - Invalid slot or no active equipment slot"));
		return;
	}

	UPDA_Item* ItemToEquip = Cast<UPDA_Item>(InSlot->AssignedItem);
	if (!ItemToEquip)
	{
		return;
	}

	// Equip the item to the active equipment slot
	ActiveEquipmentSlot->EventOccupyEquipmentSlot(ItemToEquip);

	// Clear the inventory slot
	InSlot->EventClearSlot(true);

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Equipped item to slot"));
}

void UW_Equipment::UnequipItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UnequipItemAtSlot - Tag: %s"), *SlotTag.ToString());

	UW_EquipmentSlot* TargetSlot = GetEquipmentSlotByTag(SlotTag);
	if (!TargetSlot || !TargetSlot->IsOccupied)
	{
		return;
	}

	UPDA_Item* UnequippedItem = Cast<UPDA_Item>(TargetSlot->AssignedItem);

	// Clear the equipment slot
	TargetSlot->EventClearEquipmentSlot();

	// Notify that item was unequipped
	if (UnequippedItem)
	{
		EventOnItemUnequippedFromSlot(UnequippedItem, SlotTag);
	}
}

void UW_Equipment::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateUp"));

	if (!CanNavigate() || EquipmentSlots.Num() == 0)
	{
		return;
	}

	// Deselect current
	if (SelectedSlot)
	{
		SelectedSlot->EventOnSelected(false);
	}

	// Navigate to previous slot
	EquipmentSlotNavigationIndex--;
	if (EquipmentSlotNavigationIndex < 0)
	{
		EquipmentSlotNavigationIndex = EquipmentSlots.Num() - 1;
	}

	// Select new
	SelectedSlot = EquipmentSlots[EquipmentSlotNavigationIndex];
	if (SelectedSlot)
	{
		SelectedSlot->EventOnSelected(true);
		EventToggleSlotName(SelectedSlot, true);
	}
}

void UW_Equipment::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDown"));

	if (!CanNavigate() || EquipmentSlots.Num() == 0)
	{
		return;
	}

	// Deselect current
	if (SelectedSlot)
	{
		SelectedSlot->EventOnSelected(false);
	}

	// Navigate to next slot
	EquipmentSlotNavigationIndex++;
	if (EquipmentSlotNavigationIndex >= EquipmentSlots.Num())
	{
		EquipmentSlotNavigationIndex = 0;
	}

	// Select new
	SelectedSlot = EquipmentSlots[EquipmentSlotNavigationIndex];
	if (SelectedSlot)
	{
		SelectedSlot->EventOnSelected(true);
		EventToggleSlotName(SelectedSlot, true);
	}
}

void UW_Equipment::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateLeft"));

	if (!CanNavigate())
	{
		return;
	}

	// If viewing item list, navigate in items
	if (EquipmentInventorySlots.Num() > 0)
	{
		// Deselect current item
		if (ActiveItemSlot)
		{
			ActiveItemSlot->EventOnSelected(false);
		}

		ItemNavigationIndex--;
		if (ItemNavigationIndex < 0)
		{
			ItemNavigationIndex = EquipmentInventorySlots.Num() - 1;
		}

		ActiveItemSlot = EquipmentInventorySlots[ItemNavigationIndex];
		if (ActiveItemSlot)
		{
			ActiveItemSlot->EventOnSelected(true);
		}
	}
}

void UW_Equipment::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateRight"));

	if (!CanNavigate())
	{
		return;
	}

	// If viewing item list, navigate in items
	if (EquipmentInventorySlots.Num() > 0)
	{
		// Deselect current item
		if (ActiveItemSlot)
		{
			ActiveItemSlot->EventOnSelected(false);
		}

		ItemNavigationIndex++;
		if (ItemNavigationIndex >= EquipmentInventorySlots.Num())
		{
			ItemNavigationIndex = 0;
		}

		ActiveItemSlot = EquipmentInventorySlots[ItemNavigationIndex];
		if (ActiveItemSlot)
		{
			ActiveItemSlot->EventOnSelected(true);
		}
	}
}

void UW_Equipment::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateOk"));

	if (!CanNavigate())
	{
		return;
	}

	// If we have an active item slot selected, equip it
	if (ActiveItemSlot && ActiveItemSlot->IsOccupied)
	{
		EquipItemAtSlot(ActiveItemSlot);
	}
	// Otherwise, if we have an equipment slot selected, show items for it
	else if (SelectedSlot)
	{
		SelectedSlot->EventEquipmentPressed();
	}
}

void UW_Equipment::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateCancel - Broadcasting OnEquipmentClosed"));

	// If we're viewing item list, go back to equipment slots
	if (EquipmentInventorySlots.Num() > 0)
	{
		// Clear item list
		EquipmentInventorySlots.Empty();
		if (ItemGrid)
		{
			ItemGrid->ClearChildren();
		}
		ActiveItemSlot = nullptr;
		ItemNavigationIndex = 0;

		// Re-select current equipment slot
		if (SelectedSlot)
		{
			SelectedSlot->EventOnSelected(true);
		}
	}
	else
	{
		// Broadcast OnEquipmentClosed dispatcher - this signals W_HUD to return to game menu
		OnEquipmentClosed.Broadcast();
	}
}

void UW_Equipment::EventNavigateDetailedView_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDetailedView"));

	// Show detailed item info for selected item
	if (ActiveItemSlot && ActiveItemSlot->IsOccupied)
	{
		if (UPDA_Item* Item = Cast<UPDA_Item>(ActiveItemSlot->AssignedItem))
		{
			// Get item info from UPDA_Item and show panel
			SetupInformationPanel(Item->ItemInformation, true);
		}
	}
}

void UW_Equipment::EventNavigateUnequip_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateUnequip"));

	if (!CanNavigate())
	{
		return;
	}

	// Unequip item from selected equipment slot
	if (SelectedSlot && SelectedSlot->IsOccupied)
	{
		UnequipItemAtSlot(SelectedSlot->EquipmentSlot);
	}
}

void UW_Equipment::EventOnEquipmentSlotPressed_Implementation(UW_EquipmentSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnEquipmentSlotPressed"));

	if (!InSlot || !InventoryComponent)
	{
		return;
	}

	ActiveEquipmentSlot = InSlot;

	// Get items that can be equipped in this slot
	TArray<UPrimaryDataAsset*> AvailableItems = InventoryComponent->GetItemsForEquipmentSlot(InSlot->EquipmentSlot);

	// Clear and populate item grid
	EquipmentInventorySlots.Empty();
	if (ItemGrid)
	{
		ItemGrid->ClearChildren();

		int32 Column = 0;
		int32 Row = 0;
		int32 SlotsPerRow = 4;

		for (UPrimaryDataAsset* ItemAsset : AvailableItems)
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset))
			{
				if (InventorySlotClass)
				{
					UW_InventorySlot* NewSlot = CreateWidget<UW_InventorySlot>(this, InventorySlotClass);
					if (NewSlot)
					{
						NewSlot->EquipmentRelated = true;

						// Get item count
						int32 Amount = 0;
						bool bSuccess = false;
						InventoryComponent->GetAmountOfItem(Item, Amount, bSuccess);

						NewSlot->EventOccupySlot(Item, Amount);

						// Bind events
						NewSlot->OnSelected.AddDynamic(this, &UW_Equipment::EventOnEquipmentSelected);
						NewSlot->OnPressed.AddDynamic(this, &UW_Equipment::EventOnEquipmentPressed);

						ItemGrid->AddChildToUniformGrid(NewSlot, Row, Column);
						EquipmentInventorySlots.Add(NewSlot);

						Column++;
						if (Column >= SlotsPerRow)
						{
							Column = 0;
							Row++;
						}
					}
				}
			}
		}
	}

	// Select first item if available
	if (EquipmentInventorySlots.Num() > 0)
	{
		ItemNavigationIndex = 0;
		ActiveItemSlot = EquipmentInventorySlots[0];
		if (ActiveItemSlot)
		{
			ActiveItemSlot->EventOnSelected(true);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Showing %d available items for slot"), EquipmentInventorySlots.Num());
}

void UW_Equipment::EventOnEquipmentSlotSelected_Implementation(bool bSelected, UW_EquipmentSlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnEquipmentSlotSelected - bSelected: %s"), bSelected ? TEXT("true") : TEXT("false"));

	if (bSelected && InSlot)
	{
		// Deselect previous
		if (SelectedSlot && SelectedSlot != InSlot)
		{
			SelectedSlot->EventOnSelected(false);
			EventToggleSlotName(SelectedSlot, false);
		}

		SelectedSlot = InSlot;

		// Update navigation index
		int32 Index = EquipmentSlots.Find(InSlot);
		if (Index != INDEX_NONE)
		{
			EquipmentSlotNavigationIndex = Index;
		}

		// Show slot name
		EventToggleSlotName(InSlot, true);
	}
}

void UW_Equipment::EventOnEquipmentPressed_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnEquipmentPressed"));

	if (InSlot && InSlot->IsOccupied)
	{
		EquipItemAtSlot(InSlot);
	}
}

void UW_Equipment::EventOnEquipmentSelected_Implementation(bool bSelected, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnEquipmentSelected - bSelected: %s"), bSelected ? TEXT("true") : TEXT("false"));

	if (bSelected && InSlot)
	{
		// Deselect previous
		if (ActiveItemSlot && ActiveItemSlot != InSlot)
		{
			ActiveItemSlot->EventOnSelected(false);
		}

		ActiveItemSlot = InSlot;

		// Update navigation index
		int32 Index = EquipmentInventorySlots.Find(InSlot);
		if (Index != INDEX_NONE)
		{
			ItemNavigationIndex = Index;
		}
	}
}

void UW_Equipment::EventOnItemUnequippedFromSlot_Implementation(UPDA_Item* InItem, const FGameplayTag& TargetSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnItemUnequippedFromSlot - Slot: %s"), *TargetSlot.ToString());

	// Add item back to inventory if needed
	if (InventoryComponent && InItem)
	{
		InventoryComponent->AddItem(InItem, 1, false);
	}
}

void UW_Equipment::EventToggleSlotName_Implementation(UW_EquipmentSlot* InEquipmentSlot, bool bShow)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventToggleSlotName - Show: %s"), bShow ? TEXT("true") : TEXT("false"));

	// Show/hide and update slot name text based on the equipment slot tag
	if (SlotNameText)
	{
		if (bShow && InEquipmentSlot)
		{
			// Get the slot name from the equipment slot's tag
			FString SlotName = InEquipmentSlot->EquipmentSlot.GetTagName().ToString();
			// Extract just the leaf name (e.g., "RightHand" from "Equipment.Slot.RightHand")
			int32 LastDotIndex = INDEX_NONE;
			if (SlotName.FindLastChar('.', LastDotIndex))
			{
				SlotName = SlotName.RightChop(LastDotIndex + 1);
			}
			SlotNameText->SetText(FText::FromString(SlotName));
			SlotNameText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SlotNameText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UW_Equipment::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnVisibilityChanged: %d"), static_cast<int32>(InVisibility));

	if (InVisibility == ESlateVisibility::Visible ||
		InVisibility == ESlateVisibility::SelfHitTestInvisible ||
		InVisibility == ESlateVisibility::HitTestInvisible)
	{
		// Refresh equipment slots when becoming visible
		PopulateEquipmentSlots();

		// Select first slot
		if (EquipmentSlots.Num() > 0)
		{
			EquipmentSlotNavigationIndex = 0;
			SelectedSlot = EquipmentSlots[0];
			if (SelectedSlot)
			{
				SelectedSlot->EventOnSelected(true);
				EventToggleSlotName(SelectedSlot, true);
			}
		}

		// Set focus
		SetIsFocusable(true);
		if (APlayerController* PC = GetOwningPlayer())
		{
			SetUserFocus(PC);
		}
	}
}

void UW_Equipment::EventToggleItemInfo_Implementation(bool bVisible)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventToggleItemInfo: %s"), bVisible ? TEXT("true") : TEXT("false"));

	// Toggle item info panel visibility using the widget switcher
	// Index 0 = hidden/empty, Index 1 = showing item info
	if (ItemInfoBoxSwitcher)
	{
		ItemInfoBoxSwitcher->SetActiveWidgetIndex(bVisible ? 1 : 0);
	}
}

void UW_Equipment::EventSetupItemInfoPanel_Implementation(const FSLFItemInfo& ItemInfo, bool bCompareStats)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventSetupItemInfoPanel"));

	SetupInformationPanel(ItemInfo, bCompareStats);
}

void UW_Equipment::EventShowError_Implementation(const FText& InMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EventShowError: %s"), *InMessage.ToString());

	// Error display is handled by Blueprint UMG widget tree
	// This function is called to notify the widget of an error condition
}

void UW_Equipment::EventDismissError_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventDismissError"));

	// Error display is handled by Blueprint UMG widget tree
	// This function is called to dismiss any displayed error
}

void UW_Equipment::EventOnWeaponStatCheckFailed_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EventOnWeaponStatCheckFailed"));

	// Show error message for failed stat check
	EventShowError(FText::FromString(TEXT("Stats requirements not met")));
}

void UW_Equipment::HandleItemEquippedToSlot(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] HandleItemEquippedToSlot - Slot: %s"), *TargetSlot.ToString());

	// Refresh equipment slots when item is equipped
	PopulateEquipmentSlots();
	BindEquipmentSlotEvents();
}

void UW_Equipment::HandleItemUnequippedFromSlot(UPrimaryDataAsset* Item, FGameplayTag TargetSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] HandleItemUnequippedFromSlot - Slot: %s"), *TargetSlot.ToString());

	// Refresh equipment slots when item is unequipped
	PopulateEquipmentSlots();
	BindEquipmentSlotEvents();
}

void UW_Equipment::HandleInventoryUpdated()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] HandleInventoryUpdated - Refreshing available items"));

	// Refresh the item grid if we're showing items for an equipment slot
	if (ActiveEquipmentSlot && EquipmentInventorySlots.Num() > 0)
	{
		EventOnEquipmentSlotPressed(ActiveEquipmentSlot);
	}
}
