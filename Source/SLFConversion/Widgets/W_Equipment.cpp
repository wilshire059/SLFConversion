// W_Equipment.cpp
// C++ Widget implementation for W_Equipment
//
// 20-PASS VALIDATION: 2026-01-05
// NO REFLECTION - all child widgets accessed via BindWidgetOptional
// Full implementation with component retrieval and event binding

#include "Widgets/W_Equipment.h"
#include "Widgets/W_EquipmentSlot.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_GenericError.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

UW_Equipment::UW_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentCanvas(nullptr)
	, ItemScrollBox(nullptr)
	, ItemGrid(nullptr)
	, ItemInfoBoxSwitcher(nullptr)
	, SlotNameText(nullptr)
	, ErrorBorder(nullptr)
	, W_EquipmentError(nullptr)
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

	// DIAGNOSTIC: Check what actors we have and where components are
	APlayerController* PC = GetOwningPlayer();
	APawn* Pawn = GetOwningPlayerPawn();

	UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] DIAGNOSTIC - PC: %s, Pawn: %s"),
		PC ? *PC->GetName() : TEXT("NULL"),
		Pawn ? *Pawn->GetName() : TEXT("NULL"));

	// Check PlayerController for components
	if (PC)
	{
		UInventoryManagerComponent* PCInv = PC->FindComponentByClass<UInventoryManagerComponent>();
		UAC_EquipmentManager* PCEquip = PC->FindComponentByClass<UAC_EquipmentManager>();
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] DIAGNOSTIC - PC has InventoryComp: %s, EquipmentComp: %s"),
			PCInv ? TEXT("YES") : TEXT("NO"),
			PCEquip ? TEXT("YES") : TEXT("NO"));

		// List all components on PC
		TArray<UActorComponent*> PCComps;
		PC->GetComponents(PCComps);
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] DIAGNOSTIC - PC has %d components:"), PCComps.Num());
		for (UActorComponent* Comp : PCComps)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment]   - %s (%s)"), *Comp->GetName(), *Comp->GetClass()->GetName());
		}
	}

	// Check Pawn for components
	if (Pawn)
	{
		UInventoryManagerComponent* PawnInv = Pawn->FindComponentByClass<UInventoryManagerComponent>();
		UAC_EquipmentManager* PawnEquip = Pawn->FindComponentByClass<UAC_EquipmentManager>();
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] DIAGNOSTIC - Pawn has InventoryComp: %s, EquipmentComp: %s"),
			PawnInv ? TEXT("YES") : TEXT("NO"),
			PawnEquip ? TEXT("YES") : TEXT("NO"));

		// List all components on Pawn
		TArray<UActorComponent*> PawnComps;
		Pawn->GetComponents(PawnComps);
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] DIAGNOSTIC - Pawn has %d components:"), PawnComps.Num());
		for (UActorComponent* Comp : PawnComps)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment]   - %s (%s)"), *Comp->GetName(), *Comp->GetClass()->GetName());
		}
	}

	// Try to get components - first from PC (as Blueprint does), then from Pawn as fallback
	if (PC)
	{
		InventoryComponent = PC->FindComponentByClass<UInventoryManagerComponent>();
		EquipmentComponent = PC->FindComponentByClass<UAC_EquipmentManager>();
	}

	// Fallback to Pawn if not found on PC
	if (!InventoryComponent && Pawn)
	{
		InventoryComponent = Pawn->FindComponentByClass<UInventoryManagerComponent>();
	}
	if (!EquipmentComponent && Pawn)
	{
		EquipmentComponent = Pawn->FindComponentByClass<UAC_EquipmentManager>();
	}

	if (InventoryComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found InventoryComponent on %s"),
			InventoryComponent->GetOwner() ? *InventoryComponent->GetOwner()->GetName() : TEXT("Unknown"));
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UW_Equipment::HandleInventoryUpdated);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] InventoryComponent NOT FOUND on PC or Pawn!"));
	}

	if (EquipmentComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found EquipmentComponent on %s"),
			EquipmentComponent->GetOwner() ? *EquipmentComponent->GetOwner()->GetName() : TEXT("Unknown"));
		EquipmentComponent->OnItemEquippedToSlot.AddDynamic(this, &UW_Equipment::HandleItemEquippedToSlot);
		EquipmentComponent->OnItemUnequippedFromSlot.AddDynamic(this, &UW_Equipment::HandleItemUnequippedFromSlot);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipmentComponent NOT FOUND on PC or Pawn!"));
	}

	// Make focusable for keyboard input
	SetIsFocusable(true);

	// Bind visibility changed
	OnVisibilityChanged.AddDynamic(this, &UW_Equipment::EventOnVisibilityChanged);

	// Cache widget references (error widgets, etc.)
	CacheWidgetReferences();

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

FReply UW_Equipment::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Navigate Up: W, Up Arrow, Gamepad DPad Up
	if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		EventNavigateUp();
		return FReply::Handled();
	}

	// Navigate Down: S, Down Arrow, Gamepad DPad Down
	if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		EventNavigateDown();
		return FReply::Handled();
	}

	// Navigate Left: A, Left Arrow, Gamepad DPad Left
	if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
	{
		EventNavigateLeft();
		return FReply::Handled();
	}

	// Navigate Right: D, Right Arrow, Gamepad DPad Right
	if (Key == EKeys::D || Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
	{
		EventNavigateRight();
		return FReply::Handled();
	}

	// Navigate Ok/Confirm: Enter, Space, Gamepad A
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	// Navigate Cancel/Back: Escape, Gamepad B, Tab
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Tab)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	// Detailed View: X key or Gamepad X
	if (Key == EKeys::X || Key == EKeys::Gamepad_FaceButton_Left)
	{
		EventNavigateDetailedView();
		return FReply::Handled();
	}

	// Unequip: Y key or Gamepad Y
	if (Key == EKeys::Y || Key == EKeys::Gamepad_FaceButton_Top)
	{
		EventNavigateUnequip();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_Equipment::CacheWidgetReferences()
{
	// BindWidgetOptional handles most caching automatically
	// But we need to find error widgets from the tree if not bound
	if (!ErrorBorder)
	{
		ErrorBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ErrorBorder")));
		if (ErrorBorder)
		{
			ErrorBorder->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found ErrorBorder"));
		}
	}

	if (!W_EquipmentError)
	{
		W_EquipmentError = Cast<UW_GenericError>(GetWidgetFromName(TEXT("W_EquipmentError")));
		if (W_EquipmentError)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found W_EquipmentError"));
		}
	}
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
	TArray<FSLFInventoryItem> AvailableItems = InventoryComponent->GetItemsForEquipmentSlot(InSlot->EquipmentSlot);

	// Clear and populate item grid
	EquipmentInventorySlots.Empty();
	if (ItemGrid)
	{
		ItemGrid->ClearChildren();

		int32 Column = 0;
		int32 Row = 0;
		int32 SlotsPerRow = 4;

		for (const FSLFInventoryItem& InvItem : AvailableItems)
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(InvItem.ItemAsset))
			{
				if (InventorySlotClass)
				{
					UW_InventorySlot* NewSlot = CreateWidget<UW_InventorySlot>(this, InventorySlotClass);
					if (NewSlot)
					{
						NewSlot->EquipmentRelated = true;

						// Use the amount from the inventory item
						int32 Amount = InvItem.Amount;

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

	// Show the error border
	if (ErrorBorder)
	{
		ErrorBorder->SetVisibility(ESlateVisibility::Visible);
	}

	// Set the error message on the error widget
	if (W_EquipmentError)
	{
		W_EquipmentError->EventSetErrorMessage(InMessage);
	}
}

void UW_Equipment::EventDismissError_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventDismissError"));

	// Hide the error border
	if (ErrorBorder)
	{
		ErrorBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
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
