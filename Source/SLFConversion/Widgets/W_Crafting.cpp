// W_Crafting.cpp
// C++ Widget implementation for W_Crafting
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/WidgetBlueprint/W_Crafting.json
//
// Full implementation with crafting logic

#include "Widgets/W_Crafting.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_CraftingAction.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/WidgetSwitcher.h"
#include "Components/UniformGridPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "GameplayTagsManager.h"

UW_Crafting::UW_Crafting(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryComponent(nullptr)
	, SelectedSlot(nullptr)
	, ActiveSlot(nullptr)
	, NavigationIndex(0)
	, CraftingActionPopup(nullptr)
	, ItemInfoBoxSwitcher(nullptr)
	, UniformCraftingItemsGrid(nullptr)
	, W_CraftingAction(nullptr)
{
}

void UW_Crafting::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Hidden MainBlur widget"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::NativeConstruct"));

	// Cache widget references
	CacheWidgetReferences();

	// Components are on PlayerController based on debug logs
	APlayerController* PC = GetOwningPlayer();
	APawn* Pawn = GetOwningPlayerPawn();

	UE_LOG(LogTemp, Log, TEXT("[W_Crafting] NativeConstruct - PC: %s, Pawn: %s"),
		PC ? *PC->GetName() : TEXT("NULL"),
		Pawn ? *Pawn->GetName() : TEXT("NULL"));

	// Check PlayerController first (where components actually are)
	if (PC)
	{
		InventoryComponent = PC->FindComponentByClass<UInventoryManagerComponent>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found InventoryComponent on PlayerController"));
		}
	}

	// Fallback to Pawn if not found on PC
	if (!InventoryComponent && Pawn)
	{
		InventoryComponent = Pawn->FindComponentByClass<UInventoryManagerComponent>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found InventoryComponent on Pawn (fallback)"));
		}
	}

	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Crafting] InventoryComponent NOT FOUND on PC or Pawn!"));
	}

	// Make focusable for keyboard input
	SetIsFocusable(true);

	// Bind visibility changed
	OnVisibilityChanged.AddDynamic(this, &UW_Crafting::EventOnVisibilityChanged);

	// Load and refresh craftables list
	EventAsyncLoadCraftables();
}

void UW_Crafting::NativeDestruct()
{
	OnVisibilityChanged.RemoveAll(this);

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::NativeDestruct"));
}

FReply UW_Crafting::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_Crafting] NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Check if CraftingActionPopup is visible - if so, handle input for popup
	bool bPopupVisible = CraftingActionPopup && CraftingActionPopup->GetVisibility() == ESlateVisibility::Visible;

	if (bPopupVisible && W_CraftingAction)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Popup visible - delegating input to W_CraftingAction"));

		// Navigate Up: Increase craft amount
		if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
		{
			W_CraftingAction->EventNavigateCraftingActionUp();
			return FReply::Handled();
		}

		// Navigate Down: Decrease craft amount
		if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
		{
			W_CraftingAction->EventNavigateCraftingActionDown();
			return FReply::Handled();
		}

		// Navigate Left/Right: Switch between Craft and Cancel buttons
		if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left ||
			Key == EKeys::D || Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
		{
			W_CraftingAction->EventNavigateConfirmButtonsHorizontal();
			return FReply::Handled();
		}

		// Navigate Ok/Confirm: Execute craft
		if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
		{
			W_CraftingAction->EventCraftButtonPressed();
			return FReply::Handled();
		}

		// Navigate Cancel/Back: Close popup (not entire menu)
		if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Tab)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Closing CraftingAction popup"));
			EventToggleCraftingAction(false);
			return FReply::Handled();
		}

		return FReply::Handled();
	}

	// Normal navigation when popup is not visible
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

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_Crafting::CacheWidgetReferences()
{
	// Cache CraftingActionPopup (Overlay)
	if (!CraftingActionPopup)
	{
		CraftingActionPopup = Cast<UOverlay>(GetWidgetFromName(TEXT("CraftingActionPopup")));
		if (CraftingActionPopup)
		{
			CraftingActionPopup->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found CraftingActionPopup"));
		}
	}

	// Cache ItemInfoBoxSwitcher (WidgetSwitcher)
	if (!ItemInfoBoxSwitcher)
	{
		ItemInfoBoxSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ItemInfoBoxSwitcher")));
		if (ItemInfoBoxSwitcher)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found ItemInfoBoxSwitcher"));
		}
	}

	// Cache UniformCraftingItemsGrid (UniformGridPanel)
	if (!UniformCraftingItemsGrid)
	{
		UniformCraftingItemsGrid = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("UniformCraftingItemsGrid")));
		if (UniformCraftingItemsGrid)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found UniformCraftingItemsGrid"));
		}
	}

	// Cache W_CraftingAction
	if (!W_CraftingAction)
	{
		W_CraftingAction = Cast<UW_CraftingAction>(GetWidgetFromName(TEXT("W_CraftingAction")));
		if (W_CraftingAction)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found W_CraftingAction"));

			// Bind to OnCraftingActionClosed to close popup when craft completes or cancel is pressed
			W_CraftingAction->OnCraftingActionClosed.AddDynamic(this, &UW_Crafting::HandleCraftingActionClosed);
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Bound OnCraftingActionClosed delegate"));
		}
	}
}

/**
 * SetupItemInformationPanel - Display item details in the info panel
 */
void UW_Crafting::SetupItemInformationPanel_Implementation(UPrimaryDataAsset* InItem)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::SetupItemInformationPanel - Item: %s"),
		InItem ? *InItem->GetName() : TEXT("None"));

	if (!IsValid(InItem))
	{
		return;
	}

	// Cast to UPDA_Item to access item information
	if (UPDA_Item* Item = Cast<UPDA_Item>(InItem))
	{
		// Update item name
		if (UTextBlock* NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
		{
			NameText->SetText(Item->ItemInformation.DisplayName);
		}

		// Update item description
		if (UTextBlock* DescText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDescriptionText"))))
		{
			DescText->SetText(Item->ItemInformation.ShortDescription);
		}

		// Update item icon
		if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
		{
			if (!Item->ItemInformation.IconSmall.IsNull())
			{
				UTexture2D* ItemIcon = Item->ItemInformation.IconSmall.LoadSynchronous();
				if (ItemIcon)
				{
					IconImage->SetBrushFromTexture(ItemIcon);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("  Updated info panel for: %s"), *Item->ItemInformation.DisplayName.ToString());
	}
}

/**
 * GetOwnedAmountFromTag - Get how many of an item the player owns by tag
 */
int32 UW_Crafting::GetOwnedAmountFromTag_Implementation(const FGameplayTag& Tag)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::GetOwnedAmountFromTag - Tag: %s"), *Tag.ToString());

	if (!IsValid(InventoryComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No inventory component"));
		return 0;
	}

	int32 Amount = 0;
	bool bSuccess = false;
	InventoryComponent->GetAmountOfItemWithTag(Tag, Amount, bSuccess);

	UE_LOG(LogTemp, Log, TEXT("  Amount: %d, Success: %s"), Amount, bSuccess ? TEXT("true") : TEXT("false"));
	return Amount;
}

/**
 * CheckIfCraftable - Check if player has all required materials to craft an item
 *
 * Blueprint Logic:
 * 1. Get ItemAsset->ItemInformation->CraftingDetails->RequiredItems map
 * 2. For each required item (amount -> item asset):
 *    a. Get the item's tag
 *    b. Check owned amount via GetOwnedAmountFromTag
 *    c. If owned < required, return false (with break)
 * 3. If all requirements met, return true
 */
bool UW_Crafting::CheckIfCraftable_Implementation(UPrimaryDataAsset* ItemAsset)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::CheckIfCraftable - Item: %s"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("None"));

	if (!IsValid(ItemAsset))
	{
		return false;
	}

	// Cast to UPDA_Item to access crafting info
	UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset);
	if (!IsValid(Item))
	{
		return false;
	}

	// Check if crafting is unlocked for this item
	if (!Item->bCrafingUnlocked)
	{
		UE_LOG(LogTemp, Log, TEXT("  Crafting not unlocked"));
		return false;
	}

	// Get required items from crafting details
	// RequiredItems is TMap<FGameplayTag, int32> where key is item tag, value is required amount
	const TMap<FGameplayTag, int32>& RequiredItems = Item->ItemInformation.CraftingDetails.RequiredItems;

	if (RequiredItems.Num() == 0)
	{
		// No requirements = craftable
		UE_LOG(LogTemp, Log, TEXT("  No requirements - craftable"));
		return true;
	}

	// Check each required item
	for (const auto& Requirement : RequiredItems)
	{
		FGameplayTag ItemTag = Requirement.Key;
		int32 RequiredAmount = Requirement.Value;

		if (!ItemTag.IsValid())
		{
			continue;
		}

		// Check owned amount
		int32 OwnedAmount = GetOwnedAmountFromTag(ItemTag);

		UE_LOG(LogTemp, Log, TEXT("  Requirement: %s x%d, Owned: %d"),
			*ItemTag.ToString(), RequiredAmount, OwnedAmount);

		if (OwnedAmount < RequiredAmount)
		{
			UE_LOG(LogTemp, Log, TEXT("  Not enough - NOT craftable"));
			return false;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  All requirements met - CRAFTABLE"));
	return true;
}

/**
 * RefreshCraftables - Rebuild the list of craftable items
 *
 * Blueprint Logic:
 * 1. Clear CraftingSlots array
 * 2. Clear UniformCraftingItemsGrid children
 * 3. Get Values from UnlockedCraftables map
 * 4. For each item:
 *    - Create W_InventorySlot widget
 *    - Bind OnSelected and OnPressed dispatchers
 *    - Call EventOccupySlot with item
 *    - Add to grid at calculated row/column (4 columns)
 *    - Add to CraftingSlots and UnlockedCraftableEntries arrays
 * 5. After loop, if slots exist, select first slot
 */
void UW_Crafting::RefreshCraftables_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::RefreshCraftables"));

	// Clear existing slots
	CraftingSlots.Empty();
	UnlockedCraftableEntries.Empty();

	// Clear grid children
	if (UniformCraftingItemsGrid)
	{
		UniformCraftingItemsGrid->ClearChildren();
	}

	// Get all values from UnlockedCraftables map
	TArray<UPrimaryDataAsset*> CraftableItems;
	UnlockedCraftables.GenerateValueArray(CraftableItems);

	const int32 ColumnsPerRow = 4;
	int32 Index = 0;

	// Process each unlocked craftable
	for (UPrimaryDataAsset* ItemAsset : CraftableItems)
	{
		if (!IsValid(ItemAsset))
		{
			continue;
		}

		// Check if this item can be crafted
		bool bCanCraft = CheckIfCraftable(ItemAsset);

		UE_LOG(LogTemp, Log, TEXT("  Item: %s, CanCraft: %s"),
			*ItemAsset->GetName(), bCanCraft ? TEXT("true") : TEXT("false"));

		// Create slot widget
		UW_InventorySlot* SlotWidget = nullptr;
		if (SlotWidgetClass)
		{
			SlotWidget = CreateWidget<UW_InventorySlot>(GetOwningPlayer(), SlotWidgetClass);
		}
		else
		{
			// Fallback to loading class from path
			UClass* SlotClass = LoadClass<UW_InventorySlot>(nullptr, TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot.W_InventorySlot_C"));
			if (SlotClass)
			{
				SlotWidget = CreateWidget<UW_InventorySlot>(GetOwningPlayer(), SlotClass);
			}
		}

		if (!SlotWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Crafting] Failed to create slot widget for %s"), *ItemAsset->GetName());
			continue;
		}

		// Mark as crafting related
		SlotWidget->CraftingRelated = true;

		// Bind dispatchers
		SlotWidget->OnSelected.AddDynamic(this, &UW_Crafting::EventOnCraftingSlotSelected);
		SlotWidget->OnPressed.AddDynamic(this, &UW_Crafting::EventOnCraftingSlotPressed);

		// Occupy slot with item
		if (UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset))
		{
			SlotWidget->EventOccupySlot(Item, 1);
		}

		// Enable/disable based on craftability
		SlotWidget->CraftingSlotEnabled = bCanCraft;
		SlotWidget->EventToggleSlot(bCanCraft);

		// Add to grid at calculated position
		if (UniformCraftingItemsGrid)
		{
			int32 Row = Index / ColumnsPerRow;
			int32 Column = Index % ColumnsPerRow;
			UniformCraftingItemsGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
		}

		// Add to tracking arrays
		CraftingSlots.Add(SlotWidget);
		UnlockedCraftableEntries.Add(SlotWidget);

		Index++;
	}

	// Broadcast update event
	EventOnCraftablesUpdated();

	// Select first slot if any exist
	if (CraftingSlots.Num() > 0)
	{
		NavigationIndex = 0;
		ActiveSlot = CraftingSlots[0];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}

	UE_LOG(LogTemp, Log, TEXT("  Refreshed %d craftables, %d slots created"),
		UnlockedCraftables.Num(), CraftingSlots.Num());
}

void UW_Crafting::EventAsyncLoadCraftables_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventAsyncLoadCraftables"));

	// Clear existing unlocked craftables
	UnlockedCraftables.Empty();

	// Get the asset manager
	UAssetManager& AssetManager = UAssetManager::Get();

	// Get all PDA_Item assets using the primary asset type
	// The Blueprint uses "PDA_Item_C" as the PrimaryAssetType
	FPrimaryAssetType ItemType = FPrimaryAssetType(TEXT("PDA_Item"));

	TArray<FPrimaryAssetId> AssetIdList;
	AssetManager.GetPrimaryAssetIdList(ItemType, AssetIdList);

	UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Found %d PDA_Item assets"), AssetIdList.Num());

	if (AssetIdList.Num() == 0)
	{
		// Fallback: Scan content folders for PDA_Item data assets
		// This handles cases where assets aren't registered with AssetManager
		UE_LOG(LogTemp, Log, TEXT("[W_Crafting] AssetManager found no items, using fallback scan"));

		IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

		// Get all assets in the Items folder and its subfolders
		TArray<FAssetData> AssetDataList;
		AssetRegistry.GetAssetsByPath(TEXT("/Game/SoulslikeFramework/Data/Items"), AssetDataList, true);

		UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Fallback: Found %d assets in Items folder"), AssetDataList.Num());

		// Load each asset and check if it's a craftable UPDA_Item
		for (const FAssetData& AssetData : AssetDataList)
		{
			// Only process DataAsset types (PDA_Item inherits from UPrimaryDataAsset)
			if (!AssetData.AssetClassPath.GetAssetName().ToString().Contains(TEXT("PDA_Item")))
			{
				// Quick check - skip if obviously not an item
				// But still try to load to be sure
			}

			UObject* LoadedAsset = AssetData.GetAsset();
			if (UPDA_Item* ItemAsset = Cast<UPDA_Item>(LoadedAsset))
			{
				if (ItemAsset->bCrafingUnlocked)
				{
					FGameplayTag ItemTag = ItemAsset->ItemInformation.ItemTag;

					// If ItemTag is missing, generate one from the asset name
					if (!ItemTag.IsValid())
					{
						// Generate tag like "SoulslikeFramework.Item.DA_Sword01"
						FString GeneratedTagName = FString::Printf(TEXT("SoulslikeFramework.Item.%s"), *ItemAsset->GetName());
						ItemTag = FGameplayTag::RequestGameplayTag(FName(*GeneratedTagName), false);

						// If tag doesn't exist in the tag table, we can still use it as a unique key
						if (!ItemTag.IsValid())
						{
							// Create a native tag - this works without needing a tag table entry
							ItemTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*GeneratedTagName), false);
						}

						UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Generated temp tag for %s: %s"),
							*ItemAsset->GetName(), *GeneratedTagName);
					}

					// Add to craftables map - use asset name as fallback key if tag still invalid
					if (ItemTag.IsValid())
					{
						UnlockedCraftables.Add(ItemTag, ItemAsset);
						UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Added craftable: %s (Tag: %s)"),
							*ItemAsset->GetName(), *ItemTag.ToString());
					}
					else
					{
						// Last resort - create a unique tag just for this session
						FString UniqueTagName = FString::Printf(TEXT("SoulslikeFramework.TempItem.%s"), *ItemAsset->GetName());
						FGameplayTag TempTag = FGameplayTag::RequestGameplayTag(FName(*UniqueTagName), false);
						if (!TempTag.IsValid())
						{
							// Force add to gameplay tags manager as a dynamic tag
							TempTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(*UniqueTagName));
						}
						UnlockedCraftables.Add(TempTag, ItemAsset);
						UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Added craftable with temp tag: %s"), *ItemAsset->GetName());
					}
				}
			}
		}

		// Extended fallback is no longer needed since we now handle items without ItemTags
		// The Items folder scan above should find all craftable items
	}
	else
	{
		// Use the asset ID list from AssetManager
		TArray<FSoftObjectPath> ItemsToLoad;
		for (const FPrimaryAssetId& AssetId : AssetIdList)
		{
			FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
			if (!AssetPath.IsNull())
			{
				ItemsToLoad.Add(AssetPath);
			}
		}

		// Synchronously load all items (async would be better but simpler for now)
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		StreamableManager.RequestSyncLoad(ItemsToLoad);

		// Now get the loaded assets and filter for craftables
		for (const FPrimaryAssetId& AssetId : AssetIdList)
		{
			UObject* LoadedAsset = AssetManager.GetPrimaryAssetObject(AssetId);
			if (UPDA_Item* ItemAsset = Cast<UPDA_Item>(LoadedAsset))
			{
				if (ItemAsset->bCrafingUnlocked)
				{
					FGameplayTag ItemTag = ItemAsset->ItemInformation.ItemTag;
					if (ItemTag.IsValid())
					{
						UnlockedCraftables.Add(ItemTag, ItemAsset);
						UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Added craftable: %s (Tag: %s)"),
							*ItemAsset->GetName(), *ItemTag.ToString());
					}
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Total craftables loaded: %d"), UnlockedCraftables.Num());

	// Now refresh the UI
	RefreshCraftables();
}

void UW_Crafting::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateCancel"));

	// Close crafting menu
	OnCraftingClosed.Broadcast();
}

void UW_Crafting::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateDown"));

	if (UnlockedCraftableEntries.Num() == 0)
	{
		return;
	}

	// Move to next row (based on slots per row)
	int32 SlotsPerRow = 4; // Would be configured
	NavigationIndex = FMath::Min(NavigationIndex + SlotsPerRow, UnlockedCraftableEntries.Num() - 1);

	// Update selection
	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateLeft"));

	if (UnlockedCraftableEntries.Num() == 0 || NavigationIndex <= 0)
	{
		return;
	}

	NavigationIndex--;

	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateOk"));

	// Show crafting action dialog for selected slot
	if (IsValid(ActiveSlot))
	{
		EventOnCraftingSlotPressed(ActiveSlot);
	}
}

void UW_Crafting::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateRight"));

	if (UnlockedCraftableEntries.Num() == 0)
	{
		return;
	}

	NavigationIndex = FMath::Min(NavigationIndex + 1, UnlockedCraftableEntries.Num() - 1);

	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventNavigateUp"));

	if (UnlockedCraftableEntries.Num() == 0)
	{
		return;
	}

	// Move to previous row
	int32 SlotsPerRow = 4;
	NavigationIndex = FMath::Max(0, NavigationIndex - SlotsPerRow);

	if (UnlockedCraftableEntries.IsValidIndex(NavigationIndex))
	{
		ActiveSlot = UnlockedCraftableEntries[NavigationIndex];
		EventOnCraftingSlotSelected(true, ActiveSlot);
	}
}

void UW_Crafting::EventOnCraftablesUpdated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnCraftablesUpdated"));

	// Would update UI to reflect new craftable states
}

void UW_Crafting::EventOnCraftingSlotPressed_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnCraftingSlotPressed"));

	if (!IsValid(InSlot))
	{
		return;
	}

	// Show crafting action dialog
	EventToggleCraftingAction(true);
}

void UW_Crafting::EventOnCraftingSlotSelected_Implementation(bool Selected, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnCraftingSlotSelected - Selected: %s, Slot: %s"),
		Selected ? TEXT("true") : TEXT("false"),
		InSlot ? *InSlot->GetName() : TEXT("None"));

	// Blueprint logic:
	// 1. For Each Loop through CraftingSlots → EventSetHighlighted(false) on each
	// 2. After loop complete, Branch on Selected
	// 3. If Selected==true: EventSetHighlighted(true) on InSlot, SetupItemInformationPanel(InSlot->AssignedItem)

	// Clear highlighting on all slots first
	for (UW_InventorySlot* CraftSlot : CraftingSlots)
	{
		if (IsValid(CraftSlot))
		{
			CraftSlot->EventSetHighlighted(false);
		}
	}

	// If selecting a slot
	if (Selected && IsValid(InSlot))
	{
		ActiveSlot = InSlot;

		// Highlight the selected slot
		InSlot->EventSetHighlighted(true);

		// Update item info panel with slot's assigned item
		if (IsValid(InSlot->AssignedItem))
		{
			SetupItemInformationPanel(InSlot->AssignedItem);
			EventToggleItemInfo(true);
		}
	}
}

void UW_Crafting::EventOnInventoryUpdated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnInventoryUpdated"));

	// Refresh craftable states when inventory changes
	RefreshCraftables();
}

void UW_Crafting::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventOnVisibilityChanged - %d"), static_cast<int32>(InVisibility));

	if (InVisibility == ESlateVisibility::Visible ||
		InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		// Load and refresh when becoming visible
		EventAsyncLoadCraftables();

		// Set focus to this widget for keyboard input
		if (APlayerController* PC = GetOwningPlayer())
		{
			SetUserFocus(PC);
			UE_LOG(LogTemp, Log, TEXT("[W_Crafting] Set user focus"));
		}
	}
}

void UW_Crafting::EventToggleCraftingAction_Implementation(bool Show)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventToggleCraftingAction - Show: %s"),
		Show ? TEXT("true") : TEXT("false"));

	// Set visibility on CraftingActionPopup overlay
	// Blueprint logic: Select(Show? → false=Collapsed, true=Visible)
	if (CraftingActionPopup)
	{
		ESlateVisibility NewVisibility = Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
		CraftingActionPopup->SetVisibility(NewVisibility);
		UE_LOG(LogTemp, Log, TEXT("  Set CraftingActionPopup visibility to %s"),
			Show ? TEXT("Visible") : TEXT("Collapsed"));
	}

	// Setup the crafting action widget with current slot
	if (Show && W_CraftingAction && IsValid(ActiveSlot))
	{
		UE_LOG(LogTemp, Log, TEXT("  Setting up W_CraftingAction for slot: %s"), *ActiveSlot->GetName());

		// Set the assigned item on W_CraftingAction
		W_CraftingAction->AssignedItem = ActiveSlot->AssignedItem;

		// Call EventSetupCraftingAction to initialize the popup
		W_CraftingAction->EventSetupCraftingAction(ActiveSlot);
	}
}

void UW_Crafting::EventToggleItemInfo_Implementation(bool Visible)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Crafting::EventToggleItemInfo - Visible: %s"),
		Visible ? TEXT("true") : TEXT("false"));

	// Set visibility on ItemInfoBoxSwitcher
	// Blueprint logic: Select(Visible? → false=Collapsed, true=SelfHitTestInvisible)
	if (ItemInfoBoxSwitcher)
	{
		ESlateVisibility NewVisibility = Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
		ItemInfoBoxSwitcher->SetVisibility(NewVisibility);
		UE_LOG(LogTemp, Log, TEXT("  Set ItemInfoBoxSwitcher visibility to %s"),
			Visible ? TEXT("SelfHitTestInvisible") : TEXT("Collapsed"));
	}
}

void UW_Crafting::HandleCraftingActionClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Crafting] HandleCraftingActionClosed - Closing popup"));

	// Close the crafting action popup
	EventToggleCraftingAction(false);
}
