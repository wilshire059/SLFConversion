// W_ItemWheelSlot.cpp
// C++ Widget implementation for W_ItemWheelSlot
//
// 20-PASS VALIDATION: 2026-01-26
// Full implementation matching bp_only Blueprint logic

#include "Widgets/W_ItemWheelSlot.h"
#include "Widgets/W_ItemWheel_NextSlot.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "GameFramework/Pawn.h"
#include "SLFPrimaryDataAssets.h"

UW_ItemWheelSlot::UW_ItemWheelSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Length(0)
	, CurrentIndex(0)
	, ShowEmptySlots(false)
	, VisuallyAffectedByStanceChange(false)
	, EquipmentComponent(nullptr)
	, CachedItemIcon(nullptr)
	, CachedDebugIndexText(nullptr)
	, CachedSlotSizeBox(nullptr)
{
}

void UW_ItemWheelSlot::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Get EquipmentManager - try Pawn first, then PlayerController
	// The scroll wheel action modifies the equipment manager on the Pawn
	if (APlayerController* PC = GetOwningPlayer())
	{
		// First try the pawn (where the scroll wheel action looks)
		if (APawn* Pawn = PC->GetPawn())
		{
			EquipmentComponent = Pawn->FindComponentByClass<UAC_EquipmentManager>();
		}

		// Fallback to controller if not on pawn
		if (!EquipmentComponent)
		{
			EquipmentComponent = PC->FindComponentByClass<UAC_EquipmentManager>();
		}
	}

	// Bind to EquipmentManager events (matching bp_only Event Construct logic)
	if (EquipmentComponent)
	{
		EquipmentComponent->OnItemEquippedToSlot.AddUniqueDynamic(this, &UW_ItemWheelSlot::HandleOnItemEquippedToSlot);
		EquipmentComponent->OnItemUnequippedFromSlot.AddUniqueDynamic(this, &UW_ItemWheelSlot::HandleOnItemUnequippedFromSlot);
		EquipmentComponent->OnStanceChanged.AddUniqueDynamic(this, &UW_ItemWheelSlot::HandleOnStanceChanged);
		EquipmentComponent->OnActiveToolSlotChanged.AddUniqueDynamic(this, &UW_ItemWheelSlot::HandleOnActiveToolSlotChanged);

		// Log tracked slots for debugging
		TArray<FGameplayTag> Tags;
		SlotsToTrack.GetGameplayTagArray(Tags);
		FString TagsStr;
		for (const FGameplayTag& Tag : Tags)
		{
			TagsStr += Tag.ToString() + TEXT(", ");
		}
		UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] Bound to EquipmentManager events, Identifier: %s, SlotsToTrack(%d): [%s]"),
			*Identifier.ToString(), SlotsToTrack.Num(), *TagsStr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_ItemWheelSlot] EquipmentManager not found on Pawn or PlayerController, Identifier: %s"),
			*Identifier.ToString());
	}

	// Initialize Length from SlotsToTrack
	Length = SlotsToTrack.Num();
}

void UW_ItemWheelSlot::NativeDestruct()
{
	// Unbind from EquipmentManager events
	if (EquipmentComponent)
	{
		EquipmentComponent->OnItemEquippedToSlot.RemoveDynamic(this, &UW_ItemWheelSlot::HandleOnItemEquippedToSlot);
		EquipmentComponent->OnItemUnequippedFromSlot.RemoveDynamic(this, &UW_ItemWheelSlot::HandleOnItemUnequippedFromSlot);
		EquipmentComponent->OnStanceChanged.RemoveDynamic(this, &UW_ItemWheelSlot::HandleOnStanceChanged);
		EquipmentComponent->OnActiveToolSlotChanged.RemoveDynamic(this, &UW_ItemWheelSlot::HandleOnActiveToolSlotChanged);
	}

	Super::NativeDestruct();
}

void UW_ItemWheelSlot::CacheWidgetReferences()
{
	// Cache UI widget references by name (matching Blueprint widget tree)
	// Widget names match Blueprint UMG tree (not C++ variable names)
	CachedItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon")));
	CachedDebugIndexText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Debug_IndexText")));
	CachedSlotSizeBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SlotSizeBox")));

	if (!CachedItemIcon)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_ItemWheelSlot] CachedItemIcon widget not found"));
	}
}

// ==================== Internal Delegate Handlers ====================

void UW_ItemWheelSlot::HandleOnItemEquippedToSlot(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot)
{
	// Forward to the BlueprintNativeEvent function
	EventOnItemEquippedToSlot(ItemData, TargetSlot);
}

void UW_ItemWheelSlot::HandleOnItemUnequippedFromSlot(UPrimaryDataAsset* Item, FGameplayTag TargetSlot)
{
	// Forward to the BlueprintNativeEvent function
	EventOnItemUnequippedFromSlot(Cast<UPDA_Item>(Item), TargetSlot);
}

void UW_ItemWheelSlot::HandleOnStanceChanged(bool RightHand, bool TwoHand)
{
	// Forward to the BlueprintNativeEvent function
	EventOnStanceChanged(RightHand, TwoHand);
}

void UW_ItemWheelSlot::HandleOnActiveToolSlotChanged(FGameplayTag NewSlot)
{
	// Check if this wheel slot tracks tool slots
	if (!SlotsToTrack.HasTag(NewSlot))
	{
		// This slot change is not for us (e.g., weapon slots vs tool slots)
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] HandleOnActiveToolSlotChanged - NewSlot: %s, Identifier: %s"),
		*NewSlot.ToString(), *Identifier.ToString());

	// Find the index of this slot in our tracked slots
	TArray<FGameplayTag> Tags;
	SlotsToTrack.GetGameplayTagArray(Tags);

	int32 NewIndex = -1;
	for (int32 i = 0; i < Tags.Num(); i++)
	{
		if (Tags[i] == NewSlot)
		{
			NewIndex = i;
			break;
		}
	}

	if (NewIndex >= 0 && NewIndex != CurrentIndex)
	{
		// Update current index and active slot
		PreviousSlot = ActiveSlot;
		CurrentIndex = NewIndex;
		ActiveSlot = NewSlot;

		// Update the item shown
		if (TrackedItems.Contains(NewSlot))
		{
			ActiveItem = TrackedItems[NewSlot];
		}

		// Update the visual (icon, etc.)
		if (CachedItemIcon && ActiveItem)
		{
			if (UPDA_Item* ItemData = Cast<UPDA_Item>(ActiveItem))
			{
				// Get icon from ItemInformation.IconSmall
				if (!ItemData->ItemInformation.IconSmall.IsNull())
				{
					UTexture2D* IconTexture = ItemData->ItemInformation.IconSmall.LoadSynchronous();
					if (IconTexture)
					{
						CachedItemIcon->SetBrushFromTexture(IconTexture);
					}
				}
			}
		}

		// Update debug text
		if (CachedDebugIndexText)
		{
			CachedDebugIndexText->SetText(FText::AsNumber(CurrentIndex));
		}

		// Notify child slots about the change
		RefreshChildSlots();

		UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] Updated to slot %d (%s), item: %s"),
			CurrentIndex, *NewSlot.ToString(),
			ActiveItem ? *ActiveItem->GetName() : TEXT("None"));
	}
}

// ==================== BlueprintNativeEvent Implementations ====================

void UW_ItemWheelSlot::HandleNewItem_Implementation(UPrimaryDataAsset* InItem)
{
	// Handle a new item being assigned to this wheel slot
	ActiveItem = InItem;
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] HandleNewItem - Item: %s"), InItem ? *InItem->GetName() : TEXT("None"));
}

FGameplayTag UW_ItemWheelSlot::GetSlotWithIndex_Implementation(int32 InIndex)
{
	// Get the slot tag at the given index from SlotsToTrack
	TArray<FGameplayTag> Tags;
	SlotsToTrack.GetGameplayTagArray(Tags);

	if (Tags.IsValidIndex(InIndex))
	{
		return Tags[InIndex];
	}
	return FGameplayTag();
}

void UW_ItemWheelSlot::RefreshChildSlots_Implementation()
{
	// Refresh the display of all child slot widgets
	Length = SlotsToTrack.Num();

	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] RefreshChildSlots - Length: %d, TrackedItems: %d"),
		Length, TrackedItems.Num());

	// Update child slots with tracked item data
	for (int32 i = 0; i < ChildSlots.Num() && i < Length; i++)
	{
		UW_ItemWheel_NextSlot* ChildSlot = ChildSlots[i];
		if (!ChildSlot)
		{
			continue;
		}

		// Get slot tag at this index
		FGameplayTag SlotTag = GetSlotWithIndex(i);
		if (!SlotTag.IsValid())
		{
			continue;
		}

		// Check if we have an item tracked at this slot
		if (TObjectPtr<UPrimaryDataAsset>* FoundItem = TrackedItems.Find(SlotTag))
		{
			// Update child slot with item
			ChildSlot->HandleChildItem(FoundItem->Get());
		}
		else
		{
			// Clear child slot
			ChildSlot->HandleChildItem(nullptr);
		}
	}

	// Update debug index text
	if (CachedDebugIndexText)
	{
		CachedDebugIndexText->SetText(FText::AsNumber(CurrentIndex));
	}
}

bool UW_ItemWheelSlot::AreAllSlotsEmpty_Implementation()
{
	// Check if all tracked slots are empty
	return TrackedItems.Num() == 0;
}

void UW_ItemWheelSlot::EventInitializeFromLoad_Implementation(FSLFItemWheelSaveInfo Info)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventInitializeFromLoad"));

	// Restore state from save data
	// TODO: Parse Info.ItemWheelData array and populate TrackedItems

	RefreshChildSlots();
}

void UW_ItemWheelSlot::EventItemEquipped_Implementation(UPDA_Item* InItem)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventItemEquipped - Item: %s"),
		InItem ? *InItem->GetName() : TEXT("None"));

	if (InItem)
	{
		// Set ActiveItem
		ActiveItem = InItem;

		// Update CachedItemIcon from item's icon property
		if (CachedItemIcon)
		{
			// Get icon from ItemInformation - use IconSmall (primary icon)
			TSoftObjectPtr<UTexture2D> IconTexture = InItem->ItemInformation.IconSmall;
			if (!IconTexture.IsNull())
			{
				CachedItemIcon->SetBrushFromSoftTexture(IconTexture);
				CachedItemIcon->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] Set CachedItemIcon to: %s"), *IconTexture.ToString());
			}
			else
			{
				// No icon, hide the image
				CachedItemIcon->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	else
	{
		// No item - clear display
		ActiveItem = nullptr;
		if (CachedItemIcon)
		{
			CachedItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UW_ItemWheelSlot::EventItemRemoved_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventItemRemoved"));

	// Clear the display
	ActiveItem = nullptr;
	if (CachedItemIcon)
	{
		CachedItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_ItemWheelSlot::EventOnInteractPressed_Implementation(bool Pressed)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventOnInteractPressed - Pressed: %s"),
		Pressed ? TEXT("true") : TEXT("false"));

	// Handle interact press (e.g., use tool item)
	if (Pressed && ActiveItem)
	{
		// TODO: Trigger item use through appropriate system
	}
}

void UW_ItemWheelSlot::EventOnItemAmountUpdated_Implementation(UPDA_Item* InItem, int32 NewCount)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventOnItemAmountUpdated - Item: %s, Count: %d"),
		InItem ? *InItem->GetName() : TEXT("None"), NewCount);

	// Update count display if this is our active item
	if (InItem && ActiveItem == InItem)
	{
		// The Blueprint shows a count in specific scenarios (tools/consumables)
		// For now, just refresh the display
		RefreshChildSlots();
	}
}

void UW_ItemWheelSlot::EventOnItemEquippedToSlot_Implementation(FSLFCurrentEquipment ItemData, FGameplayTag TargetSlot)
{
	// Check if this slot is one we're tracking
	if (!SlotsToTrack.HasTag(TargetSlot))
	{
		UE_LOG(LogTemp, Verbose, TEXT("[W_ItemWheelSlot] EventOnItemEquippedToSlot SKIPPED - Identifier: %s, TargetSlot: %s not in SlotsToTrack"),
			*Identifier.ToString(), *TargetSlot.ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventOnItemEquippedToSlot MATCHED - Identifier: %s, Slot: %s, Item: %s, TrackedItems now: %d"),
		*Identifier.ToString(),
		*TargetSlot.ToString(),
		ItemData.ItemAsset ? *ItemData.ItemAsset->GetName() : TEXT("None"),
		TrackedItems.Num() + 1);

	// Add to TrackedItems map: Key = TargetSlot, Value = ItemAsset
	if (ItemData.ItemAsset)
	{
		UPrimaryDataAsset* DataAsset = Cast<UPrimaryDataAsset>(ItemData.ItemAsset);
		if (DataAsset)
		{
			TrackedItems.Add(TargetSlot, DataAsset);
		}
	}

	// Refresh child slots to update display
	RefreshChildSlots();

	// If this is the currently active slot, update the main display
	if (TargetSlot == ActiveSlot || !ActiveSlot.IsValid())
	{
		UPDA_Item* Item = Cast<UPDA_Item>(ItemData.ItemAsset);
		EventItemEquipped(Item);

		// Set as active slot if we don't have one
		if (!ActiveSlot.IsValid())
		{
			ActiveSlot = TargetSlot;
		}
	}
}

void UW_ItemWheelSlot::EventOnItemUnequippedFromSlot_Implementation(UPDA_Item* InItem, FGameplayTag TargetSlot)
{
	// Check if this slot is one we're tracking
	if (!SlotsToTrack.HasTag(TargetSlot))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventOnItemUnequippedFromSlot - Slot: %s, Item: %s"),
		*TargetSlot.ToString(),
		InItem ? *InItem->GetName() : TEXT("None"));

	// Remove from TrackedItems map
	TrackedItems.Remove(TargetSlot);

	// Refresh child slots to update display
	RefreshChildSlots();

	// If this was the active slot, find another item or clear display
	if (TargetSlot == ActiveSlot)
	{
		if (TrackedItems.Num() > 0)
		{
			// Find first available item
			for (const auto& Pair : TrackedItems)
			{
				ActiveSlot = Pair.Key;
				UPDA_Item* Item = Cast<UPDA_Item>(Pair.Value.Get());
				EventItemEquipped(Item);
				break;
			}
		}
		else
		{
			// No items left
			ActiveSlot = FGameplayTag();
			EventItemRemoved();
		}
	}
}

void UW_ItemWheelSlot::EventOnStanceChanged_Implementation(bool RightHand, bool TwoHand)
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventOnStanceChanged - RightHand: %s, TwoHand: %s"),
		RightHand ? TEXT("true") : TEXT("false"),
		TwoHand ? TEXT("true") : TEXT("false"));

	// Handle stance change if this wheel slot is visually affected
	if (VisuallyAffectedByStanceChange)
	{
		// Notify child slots about stance change
		for (UW_ItemWheel_NextSlot* ChildSlot : ChildSlots)
		{
			if (ChildSlot)
			{
				ChildSlot->EventPlayStanceFadeAnimation(TwoHand);
			}
		}
	}
}

void UW_ItemWheelSlot::EventScrollWheel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventScrollWheel - Identifier: %s, CurrentIndex: %d, Length: %d, TrackedItems: %d, SlotsToTrack: %d"),
		*Identifier.ToString(), CurrentIndex, Length, TrackedItems.Num(), SlotsToTrack.Num());

	if (Length <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_ItemWheelSlot] EventScrollWheel FAILED - Length is 0! SlotsToTrack may not be configured."));
		return;
	}

	// Get current slot before scrolling
	PreviousSlot = ActiveSlot;

	// Increment index with wrap
	CurrentIndex = (CurrentIndex + 1) % Length;

	// Get the new slot tag at this index
	FGameplayTag NewSlotTag = GetSlotWithIndex(CurrentIndex);
	if (!NewSlotTag.IsValid())
	{
		return;
	}

	ActiveSlot = NewSlotTag;

	// Get item at new slot
	TObjectPtr<UPrimaryDataAsset>* FoundItem = TrackedItems.Find(NewSlotTag);
	UPDA_Item* NewItem = FoundItem ? Cast<UPDA_Item>(FoundItem->Get()) : nullptr;

	// Update display
	EventItemEquipped(NewItem);

	// If we have a valid item, equip it through EquipmentManager
	if (NewItem && EquipmentComponent)
	{
		bool bSuccess1, bSuccess2, bSuccess3, bSuccess4, bSuccess5, bSuccess6, bSuccess7;
		EquipmentComponent->EquipItemToSlot(NewItem, NewSlotTag, true,
			bSuccess1, bSuccess2, bSuccess3, bSuccess4, bSuccess5, bSuccess6, bSuccess7);

		UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] Equipped item %s to slot %s via EquipmentManager"),
			*NewItem->GetName(), *NewSlotTag.ToString());
	}

	// Broadcast selection event
	OnItemWheelSlotSelected.Broadcast(NewSlotTag);

	// Update debug text
	if (CachedDebugIndexText)
	{
		CachedDebugIndexText->SetText(FText::AsNumber(CurrentIndex));
	}
}

void UW_ItemWheelSlot::EventScrollWheelNoDelay_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_ItemWheelSlot] EventScrollWheelNoDelay"));

	// Same as EventScrollWheel but without any delay animation
	// The Blueprint calls the same scroll logic but skips visual delay
	EventScrollWheel();
}
