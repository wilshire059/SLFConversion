// W_Inventory.cpp
// C++ Widget implementation for W_Inventory
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with navigation and slot management

#include "Widgets/W_Inventory.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_Inventory_CategoryEntry.h"
#include "Components/ScrollBox.h"
#include "Widgets/W_InventoryAction.h"
#include "Components/TextBlock.h"
#include "Widgets/W_GenericError.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/InventoryManagerComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "SLFPrimaryDataAssets.h"

UW_Inventory::UW_Inventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, UniformInventoryGrid(nullptr)
	, UniformStorageGrid(nullptr)
	, InventoryComponent(nullptr)
	, SelectedSlot(nullptr)
	, ActiveFilterCategory(ESLFItemCategory::None)
	, StorageMode(false)
	, CategoryNavigationIndex(0)
	, ItemNavigationIndex(0)
{
}

void UW_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] NativeConstruct BEGIN"));

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Hidden MainBlur widget"));
	}

	// Cache widget references (gets InventoryComponent from PlayerController)
	CacheWidgetReferences();

	// Create category entries dynamically (Weapons, Shields, Armor, Tools, etc.)
	CreateCategoryEntries();

	// CRITICAL: Create inventory slots dynamically (per Blueprint Construct logic)
	// The Blueprint creates slots from 0 to SlotCount-1 and adds them to UniformInventoryGrid
	CreateInventorySlots();

	// Create storage slots if in storage mode
	if (StorageMode)
	{
		CreateStorageSlots();
	}

	// Bind visibility changed event (per Blueprint EventGraph)
	OnVisibilityChanged.AddDynamic(this, &UW_Inventory::EventOnVisibilityChanged);

	// Make focusable for keyboard input
	SetIsFocusable(true);

	// Select first slot if available
	if (InventorySlots.Num() > 0)
	{
		ItemNavigationIndex = 0;
		EventOnSlotSelected(true, InventorySlots[0]);
	}

	// Update input icons based on current input device
	UpdateInputIcons();

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] NativeConstruct COMPLETE - Created %d slots"), InventorySlots.Num());
}

void UW_Inventory::NativeDestruct()
{
	// Unbind visibility event
	OnVisibilityChanged.RemoveAll(this);

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::NativeDestruct"));
}

FReply UW_Inventory::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Navigate Up: W, Up Arrow, Gamepad DPad Up, Left Stick Up
	if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		EventNavigateUp();
		return FReply::Handled();
	}

	// Navigate Down: S, Down Arrow, Gamepad DPad Down, Left Stick Down
	if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		EventNavigateDown();
		return FReply::Handled();
	}

	// Navigate Left: A, Left Arrow, Gamepad DPad Left, Left Stick Left
	if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
	{
		EventNavigateLeft();
		return FReply::Handled();
	}

	// Navigate Right: D, Right Arrow, Gamepad DPad Right, Left Stick Right
	if (Key == EKeys::D || Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
	{
		EventNavigateRight();
		return FReply::Handled();
	}

	// Navigate Ok/Confirm: Enter, Space, Gamepad A (FaceButton_Bottom)
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	// Navigate Cancel/Back: Escape, Gamepad B (FaceButton_Right), Tab
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Tab)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	// Category navigation: Q/E or Shoulder buttons
	if (Key == EKeys::Q || Key == EKeys::Gamepad_LeftShoulder)
	{
		EventNavigateCategoryLeft();
		return FReply::Handled();
	}

	if (Key == EKeys::E || Key == EKeys::Gamepad_RightShoulder)
	{
		EventNavigateCategoryRight();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_Inventory::CacheWidgetReferences()
{
	// Get inventory component - Blueprint uses GetOwningPlayer() which returns PlayerController
	// Components are on PlayerController, NOT on Pawn
	APlayerController* PC = GetOwningPlayer();
	APawn* Pawn = GetOwningPlayerPawn();

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] CacheWidgetReferences - PC: %s, Pawn: %s"),
		PC ? *PC->GetName() : TEXT("NULL"),
		Pawn ? *Pawn->GetName() : TEXT("NULL"));

	// Check PlayerController first (where components actually are)
	if (PC)
	{
		InventoryComponent = PC->FindComponentByClass<UInventoryManagerComponent>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found InventoryComponent on PlayerController - SlotCount: %d, SlotsPerRow: %d"),
				InventoryComponent->SlotCount, InventoryComponent->SlotsPerRow);
		}
	}

	// Fallback to Pawn if not found on PC
	if (!InventoryComponent && Pawn)
	{
		InventoryComponent = Pawn->FindComponentByClass<UInventoryManagerComponent>();
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found InventoryComponent on Pawn (fallback) - SlotCount: %d"),
				InventoryComponent->SlotCount);
		}
	}

	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] InventoryComponent NOT FOUND on PC or Pawn!"));
	}

	// Log grid widget status from BindWidgetOptional
	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] BindWidget result - UniformInventoryGrid: %s, UniformStorageGrid: %s"),
		UniformInventoryGrid ? TEXT("BOUND") : TEXT("NULL"),
		UniformStorageGrid ? TEXT("BOUND") : TEXT("NULL"));

	// FALLBACK: If BindWidgetOptional didn't find the grids, search by name
	if (!UniformInventoryGrid)
	{
		UniformInventoryGrid = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("UniformInventoryGrid")));
		if (UniformInventoryGrid)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found UniformInventoryGrid via GetWidgetFromName"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] UniformInventoryGrid NOT FOUND via GetWidgetFromName either!"));
		}
	}

	if (!UniformStorageGrid)
	{
		UniformStorageGrid = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("UniformStorageGrid")));
		if (UniformStorageGrid)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found UniformStorageGrid via GetWidgetFromName"));
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// FIND AND CACHE CATEGORY ENTRIES
	// The AllItemsCategoryEntry widget is designed in UMG (WidgetTree)
	// ═══════════════════════════════════════════════════════════════════════════
	CategoryEntries.Empty();

	// Find AllItemsCategoryEntry - the "All Items" category entry
	if (UWidget* AllItemsWidget = GetWidgetFromName(TEXT("AllItemsCategoryEntry")))
	{
		if (UW_Inventory_CategoryEntry* CategoryEntry = Cast<UW_Inventory_CategoryEntry>(AllItemsWidget))
		{
			CategoryEntries.Add(CategoryEntry);

			// Bind OnSelected event
			CategoryEntry->OnSelected.AddDynamic(this, &UW_Inventory::EventOnCategorySelected);

			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found and bound AllItemsCategoryEntry"));
		}
	}

	// Find CategoriesBox (HorizontalBox containing all category entries)
	// Additional category entries may be children of this box
	if (UHorizontalBox* CategoriesBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("CategoriesBox"))))
	{
		for (int32 i = 0; i < CategoriesBox->GetChildrenCount(); ++i)
		{
			UWidget* Child = CategoriesBox->GetChildAt(i);
			if (UW_Inventory_CategoryEntry* CategoryEntry = Cast<UW_Inventory_CategoryEntry>(Child))
			{
				// Only add if not already in the array
				if (!CategoryEntries.Contains(CategoryEntry))
				{
					CategoryEntries.Add(CategoryEntry);
					CategoryEntry->OnSelected.AddDynamic(this, &UW_Inventory::EventOnCategorySelected);
					UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found and bound CategoryEntry: %s"), *Child->GetName());
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Cached %d category entries"), CategoryEntries.Num());

	// ═══════════════════════════════════════════════════════════════════════════
	// FIND ItemInfoBoxSwitcher - Controls which panel is shown on the right side
	// Index 0 = CharacterStatsOverlay, Index 1 = ItemInfo panel
	// ═══════════════════════════════════════════════════════════════════════════
	if (!ItemInfoBoxSwitcher)
	{
		ItemInfoBoxSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ItemInfoBoxSwitcher")));
		if (ItemInfoBoxSwitcher)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found ItemInfoBoxSwitcher, current index: %d"),
				ItemInfoBoxSwitcher->GetActiveWidgetIndex());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] ItemInfoBoxSwitcher NOT FOUND!"));
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// FIND ACTION MENU WIDGETS
	// W_InventoryAction - Action menu for inventory items (use, drop, etc.)
	// W_StorageAction - Action menu for storage items (store, retrieve, etc.)
	// ═══════════════════════════════════════════════════════════════════════════
	if (!W_InventoryAction)
	{
		W_InventoryAction = Cast<UW_InventoryAction>(GetWidgetFromName(TEXT("W_InventoryAction")));
		if (W_InventoryAction)
		{
			W_InventoryAction->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found W_InventoryAction"));
		}
	}

	if (!W_StorageAction)
	{
		W_StorageAction = Cast<UW_InventoryAction>(GetWidgetFromName(TEXT("W_StorageAction")));
		if (W_StorageAction)
		{
			W_StorageAction->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Found W_StorageAction"));
		}
	}

	// Find scroll boxes for action menu positioning
	if (!InventoryScrollBox)
	{
		InventoryScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("InventoryScrollBox")));
	}
	if (!StorageScrollBox)
	{
		StorageScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("StorageScrollBox")));
	}
}

/**
 * CreateInventorySlots - Create inventory slots dynamically
 * Per Blueprint Construct logic: For loop 0 to SlotCount-1, create W_InventorySlot, add to grid
 */
void UW_Inventory::CreateInventorySlots()
{
	// Clear existing slots
	InventorySlots.Empty();

	// Clear the grid if it exists
	if (UniformInventoryGrid)
	{
		UniformInventoryGrid->ClearChildren();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] UniformInventoryGrid is NULL - cannot create slots!"));
		return;
	}

	// Get slot count and slots per row from InventoryComponent
	int32 NumSlots = 20; // Default
	int32 SlotsPerRow = 10; // Default

	if (InventoryComponent)
	{
		NumSlots = InventoryComponent->SlotCount;
		SlotsPerRow = InventoryComponent->SlotsPerRow;

		if (NumSlots <= 0) NumSlots = 20;
		if (SlotsPerRow <= 0) SlotsPerRow = 10;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] CreateInventorySlots - Creating %d slots, %d per row"), NumSlots, SlotsPerRow);

	// Determine the slot widget class to use
	TSubclassOf<UW_InventorySlot> SlotClass = InventorySlotClass;
	if (!SlotClass)
	{
		// Try to load the Blueprint class
		SlotClass = LoadClass<UW_InventorySlot>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot.W_InventorySlot_C"));

		if (!SlotClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Could not load W_InventorySlot class - cannot create slots!"));
			return;
		}
	}

	// Create slots in a loop (matching Blueprint For Loop 0 to SlotCount-1)
	for (int32 Index = 0; Index < NumSlots; ++Index)
	{
		// Create the slot widget
		UW_InventorySlot* NewSlot = CreateWidget<UW_InventorySlot>(GetOwningPlayer(), SlotClass);
		if (!NewSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Failed to create slot at index %d"), Index);
			continue;
		}

		// Set slot properties (per Blueprint Create Widget node)
		NewSlot->StorageRelated = false;
		NewSlot->CraftingRelated = false;
		NewSlot->EquipmentRelated = false;
		// SlotColor is set via default value in Blueprint

		// Calculate grid position
		int32 Row = Index / SlotsPerRow;
		int32 Column = Index % SlotsPerRow;

		// Add to grid (per Blueprint AddChildToUniformGrid)
		UUniformGridSlot* GridSlot = UniformInventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);
		if (GridSlot)
		{
			// Grid slot created successfully
		}

		// Add to InventorySlots array
		InventorySlots.Add(NewSlot);

		// Bind slot events (per Blueprint Assign OnSelected, OnPressed, etc.)
		NewSlot->OnSelected.AddDynamic(this, &UW_Inventory::HandleSlotSelected);
		NewSlot->OnPressed.AddDynamic(this, &UW_Inventory::HandleSlotPressed);
		NewSlot->OnSlotCleared.AddDynamic(this, &UW_Inventory::HandleSlotCleared);
		NewSlot->OnSlotAssigned.AddDynamic(this, &UW_Inventory::HandleSlotAssigned);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Created %d inventory slots"), InventorySlots.Num());

	// Populate slots with items from inventory
	PopulateSlotsWithItems();
}

/**
 * CreateStorageSlots - Create storage slots dynamically
 */
void UW_Inventory::CreateStorageSlots()
{
	// Clear existing storage slots
	StorageSlots.Empty();

	// Clear the grid if it exists
	if (UniformStorageGrid)
	{
		UniformStorageGrid->ClearChildren();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] UniformStorageGrid is NULL - cannot create storage slots!"));
		return;
	}

	// Get slot count and slots per row from InventoryComponent
	int32 NumSlots = 20;
	int32 SlotsPerRow = 10;

	if (InventoryComponent)
	{
		NumSlots = InventoryComponent->SlotCount;
		SlotsPerRow = InventoryComponent->SlotsPerRow;

		if (NumSlots <= 0) NumSlots = 20;
		if (SlotsPerRow <= 0) SlotsPerRow = 10;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] CreateStorageSlots - Creating %d slots, %d per row"), NumSlots, SlotsPerRow);

	// Determine the slot widget class to use
	TSubclassOf<UW_InventorySlot> SlotClass = InventorySlotClass;
	if (!SlotClass)
	{
		SlotClass = LoadClass<UW_InventorySlot>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot.W_InventorySlot_C"));

		if (!SlotClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Could not load W_InventorySlot class for storage slots!"));
			return;
		}
	}

	// Create storage slots
	for (int32 Index = 0; Index < NumSlots; ++Index)
	{
		UW_InventorySlot* NewSlot = CreateWidget<UW_InventorySlot>(GetOwningPlayer(), SlotClass);
		if (!NewSlot)
		{
			continue;
		}

		// Storage slots have StorageRelated = true
		NewSlot->StorageRelated = true;
		NewSlot->CraftingRelated = false;
		NewSlot->EquipmentRelated = false;

		int32 Row = Index / SlotsPerRow;
		int32 Column = Index % SlotsPerRow;

		UniformStorageGrid->AddChildToUniformGrid(NewSlot, Row, Column);

		StorageSlots.Add(NewSlot);

		// Bind events
		NewSlot->OnSelected.AddDynamic(this, &UW_Inventory::HandleSlotSelected);
		NewSlot->OnPressed.AddDynamic(this, &UW_Inventory::HandleSlotPressed);
		NewSlot->OnSlotCleared.AddDynamic(this, &UW_Inventory::HandleSlotCleared);
		NewSlot->OnSlotAssigned.AddDynamic(this, &UW_Inventory::HandleSlotAssigned);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Created %d storage slots"), StorageSlots.Num());
}

/**
 * CreateCategoryEntries - Create category entry widgets dynamically
 * Creates entries for: Weapons, Shields, Armor, Tools, Bolstering, KeyItems, Abilities, Crafting
 */
void UW_Inventory::CreateCategoryEntries()
{
	// Find CategoriesBox (HorizontalBox containing all category entries)
	UHorizontalBox* CategoriesBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("CategoriesBox")));
	if (!CategoriesBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] CategoriesBox NOT FOUND - cannot create category entries!"));
		return;
	}

	// Determine the category entry widget class to use
	TSubclassOf<UW_Inventory_CategoryEntry> EntryClass = CategoryEntryClass;
	if (!EntryClass)
	{
		EntryClass = LoadClass<UW_Inventory_CategoryEntry>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory_CategoryEntry.W_Inventory_CategoryEntry_C"));

		if (!EntryClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Could not load W_Inventory_CategoryEntry class!"));
			return;
		}
	}

	// Define category data with icon paths
	struct FCategorySetup
	{
		ESLFItemCategory Category;
		FString IconPath;
	};

	TArray<FCategorySetup> Categories = {
		{ ESLFItemCategory::Weapons,    TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Category_Weapons.T_Category_Weapons") },
		{ ESLFItemCategory::Shields,    TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Category_Shields.T_Category_Shields") },
		{ ESLFItemCategory::Armor,      TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Category_Armor.T_Category_Armor") },
		{ ESLFItemCategory::Tools,      TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Category_Tools.T_Category_Tools") },
		{ ESLFItemCategory::Bolstering, TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Category_Bolstering.T_Category_Bolstering") },
		{ ESLFItemCategory::KeyItems,   TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Category_KeyItems.T_Category_KeyItems") },
		{ ESLFItemCategory::Abilities,  TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_AbilityGeneric.T_AbilityGeneric") },
		{ ESLFItemCategory::Crafting,   TEXT("/Game/SoulslikeFramework/Widgets/_Textures/T_Crafting.T_Crafting") },
	};

	// Create entry for each category
	for (const FCategorySetup& Setup : Categories)
	{
		UW_Inventory_CategoryEntry* Entry = CreateWidget<UW_Inventory_CategoryEntry>(GetOwningPlayer(), EntryClass);
		if (!Entry)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Failed to create category entry for category %d"), (int32)Setup.Category);
			continue;
		}

		// Set up category data
		Entry->InventoryCategoryData.Category = Setup.Category;
		Entry->InventoryCategoryData.CategoryIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(Setup.IconPath));
		Entry->AllCategoriesButton = false;

		// Bind OnSelected event
		Entry->OnSelected.AddDynamic(this, &UW_Inventory::EventOnCategorySelected);

		// Add to CategoriesBox
		CategoriesBox->AddChild(Entry);

		// Add to our array
		CategoryEntries.Add(Entry);

		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Created category entry for: %d"), (int32)Setup.Category);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Created %d category entries (total with AllItems: %d)"),
		Categories.Num(), CategoryEntries.Num());
}

/**
 * PopulateSlotsWithItems - Populate slots with items from InventoryComponent
 */
void UW_Inventory::PopulateSlotsWithItems()
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] PopulateSlotsWithItems - No InventoryComponent!"));
		return;
	}

	// Clear all slots first
	for (UW_InventorySlot* InvSlot : InventorySlots)
	{
		if (InvSlot && InvSlot->IsOccupied)
		{
			InvSlot->EventClearSlot(false);
		}
	}

	// Get all items from inventory using the GetAllItems function
	TArray<FSLFInventoryItem> AllItems = InventoryComponent->GetAllItems();

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] PopulateSlotsWithItems - %d items in inventory, %d slots available"),
		AllItems.Num(), InventorySlots.Num());

	int32 SlotIndex = 0;
	for (const FSLFInventoryItem& InvItem : AllItems)
	{
		if (SlotIndex >= InventorySlots.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Not enough slots for all items!"));
			break;
		}

		UObject* ItemAsset = InvItem.ItemAsset;
		if (!ItemAsset)
		{
			continue;
		}

		// Cast to UPDA_Item for EventOccupySlot
		UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset);
		if (!Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] Item is not UPDA_Item: %s"), *ItemAsset->GetName());
			continue;
		}

		// Get item count from the inventory item struct
		int32 ItemCount = InvItem.Amount;
		if (ItemCount <= 0)
		{
			ItemCount = 1;
		}

		// Find next available slot
		UW_InventorySlot* InvSlot = InventorySlots[SlotIndex];
		if (InvSlot)
		{
			InvSlot->EventOccupySlot(Item, ItemCount);
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Populated slot %d with %s x%d"),
				SlotIndex, *Item->GetName(), ItemCount);
		}

		SlotIndex++;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] PopulateSlotsWithItems complete - populated %d slots"), SlotIndex);
}

/**
 * AddNewSlots - Create new inventory/storage slots
 */
void UW_Inventory::AddNewSlots_Implementation(ESLFInventorySlotType SlotType)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::AddNewSlots - SlotType: %d"), (int32)SlotType);

	// Slots are typically created in Blueprint UMG designer
	// This function would dynamically add more slots if needed
}

/**
 * GetTranslationForActionMenu - Calculate position for action menu popup
 */
FVector2D UW_Inventory::GetTranslationForActionMenu_Implementation(UScrollBox* TargetScrollbox)
{
	if (TargetScrollbox)
	{
		FGeometry Geometry = TargetScrollbox->GetCachedGeometry();
		FVector2D LocalSize = Geometry.GetLocalSize();
		return FVector2D(LocalSize.X * 0.5f, LocalSize.Y * 0.5f);
	}
	return FVector2D::ZeroVector;
}

/**
 * SetStorageMode - Toggle between inventory and storage view
 */
void UW_Inventory::SetStorageMode_Implementation(bool InStorageMode)
{
	StorageMode = InStorageMode;

	// Reset navigation index when switching modes
	ItemNavigationIndex = 0;

	// Rebuild occupied slots for the new mode
	ReinitOccupiedInventorySlots();

	// Select first slot if available
	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
	if (ActiveSlots.Num() > 0)
	{
		EventOnSlotSelected(true, ActiveSlots[0]);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::SetStorageMode - StorageMode: %s"), StorageMode ? TEXT("true") : TEXT("false"));
}

/**
 * ReinitOccupiedInventorySlots - Rebuild list of slots with items
 */
void UW_Inventory::ReinitOccupiedInventorySlots_Implementation()
{
	OccupiedInventorySlots.Empty();

	TArray<UW_InventorySlot*>& SourceSlots = StorageMode ? StorageSlots : InventorySlots;

	for (UW_InventorySlot* InvSlot : SourceSlots)
	{
		if (InvSlot && InvSlot->IsOccupied && InvSlot->AssignedItem)
		{
			// Get item data for debug logging
			UPDA_Item* ItemData = Cast<UPDA_Item>(InvSlot->AssignedItem);
			ESLFItemCategory ItemCategory = ESLFItemCategory::None;

			if (ItemData)
			{
				ItemCategory = ItemData->ItemInformation.Category.Category;
				UE_LOG(LogTemp, Verbose, TEXT("  Item: %s, Category: %d"),
					*ItemData->GetName(), (int32)ItemCategory);
			}

			// Filter by category if active
			if (ActiveFilterCategory == ESLFItemCategory::None)
			{
				// No filter - show all occupied slots
				OccupiedInventorySlots.Add(InvSlot);
			}
			else if (ItemCategory == ActiveFilterCategory)
			{
				// Item matches filter
				OccupiedInventorySlots.Add(InvSlot);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::ReinitOccupiedInventorySlots - Filter: %d, Count: %d"),
		(int32)ActiveFilterCategory, OccupiedInventorySlots.Num());
}

/**
 * SetCategorization - Filter items by category
 */
void UW_Inventory::SetCategorization_Implementation(ESLFItemCategory ItemCategory)
{
	ActiveFilterCategory = ItemCategory;

	// Update category entry visuals - select the matching category
	for (int32 i = 0; i < CategoryEntries.Num(); ++i)
	{
		if (CategoryEntries[i])
		{
			// AllItems entry uses AllCategoriesButton=true and matches when ItemCategory is None
			// Other entries match when their category matches
			bool bShouldSelect = false;
			if (CategoryEntries[i]->AllCategoriesButton)
			{
				bShouldSelect = (ItemCategory == ESLFItemCategory::None);
			}
			else
			{
				bShouldSelect = (CategoryEntries[i]->InventoryCategoryData.Category == ItemCategory);
			}

			CategoryEntries[i]->SetInventoryCategorySelected(bShouldSelect);

			// Also update navigation index if this is the selected category
			if (bShouldSelect)
			{
				CategoryNavigationIndex = i;
			}
		}
	}

	// Rebuild occupied slots with new filter
	ReinitOccupiedInventorySlots();

	// Reset item navigation
	ItemNavigationIndex = 0;
	if (OccupiedInventorySlots.Num() > 0)
	{
		EventOnSlotSelected(true, OccupiedInventorySlots[0]);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::SetCategorization - Category: %d, NavIndex: %d"),
		(int32)ItemCategory, CategoryNavigationIndex);
}

/**
 * ResetCategorization - Show all item categories
 */
void UW_Inventory::ResetCategorization_Implementation()
{
	ActiveFilterCategory = ESLFItemCategory::None;
	CategoryNavigationIndex = 0;

	ReinitOccupiedInventorySlots();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::ResetCategorization"));
}

/**
 * EventNavigateUp - Move selection up in grid
 */
void UW_Inventory::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateUp"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	// Get slots per row from inventory component
	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Calculate new index (move up one row)
	int32 NewIndex = ItemNavigationIndex - SlotsPerRow;

	if (NewIndex >= 0)
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateDown - Move selection down in grid
 */
void UW_Inventory::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateDown"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Calculate new index (move down one row)
	int32 NewIndex = ItemNavigationIndex + SlotsPerRow;

	if (NewIndex < ActiveSlots.Num())
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateLeft - Move selection left in grid
 */
void UW_Inventory::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateLeft"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Don't wrap to previous row
	int32 CurrentRow = ItemNavigationIndex / SlotsPerRow;
	int32 NewIndex = ItemNavigationIndex - 1;
	int32 NewRow = NewIndex / SlotsPerRow;

	if (NewIndex >= 0 && NewRow == CurrentRow)
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateRight - Move selection right in grid
 */
void UW_Inventory::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateRight"));

	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;

	if (ActiveSlots.Num() == 0)
	{
		return;
	}

	int32 SlotsPerRow = InventoryComponent ? InventoryComponent->SlotsPerRow : 8;

	// Don't wrap to next row
	int32 CurrentRow = ItemNavigationIndex / SlotsPerRow;
	int32 NewIndex = ItemNavigationIndex + 1;
	int32 NewRow = NewIndex / SlotsPerRow;

	if (NewIndex < ActiveSlots.Num() && NewRow == CurrentRow)
	{
		// Deselect current
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(false);
		}

		ItemNavigationIndex = NewIndex;

		// Select new
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			ActiveSlots[ItemNavigationIndex]->EventOnSelected(true);
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			EventSetupItemInfoPanel(SelectedSlot);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  New index: %d"), ItemNavigationIndex);
}

/**
 * EventNavigateCategoryLeft - Switch to previous category
 */
void UW_Inventory::EventNavigateCategoryLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateCategoryLeft"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to previous category
	int32 NewIndex = CategoryNavigationIndex - 1;
	if (NewIndex < 0)
	{
		NewIndex = CategoryEntries.Num() - 1; // Wrap around
	}

	// Apply filter for the new category
	if (CategoryEntries.IsValidIndex(NewIndex) && CategoryEntries[NewIndex])
	{
		// AllItems entry has AllCategoriesButton=true, uses None category
		ESLFItemCategory NewCategory = CategoryEntries[NewIndex]->AllCategoriesButton
			? ESLFItemCategory::None
			: CategoryEntries[NewIndex]->InventoryCategoryData.Category;

		SetCategorization(NewCategory);
	}

	UE_LOG(LogTemp, Log, TEXT("  Category index: %d"), CategoryNavigationIndex);
}

/**
 * EventNavigateCategoryRight - Switch to next category
 */
void UW_Inventory::EventNavigateCategoryRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateCategoryRight"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to next category
	int32 NewIndex = CategoryNavigationIndex + 1;
	if (NewIndex >= CategoryEntries.Num())
	{
		NewIndex = 0; // Wrap around
	}

	// Apply filter for the new category
	if (CategoryEntries.IsValidIndex(NewIndex) && CategoryEntries[NewIndex])
	{
		// AllItems entry has AllCategoriesButton=true, uses None category
		ESLFItemCategory NewCategory = CategoryEntries[NewIndex]->AllCategoriesButton
			? ESLFItemCategory::None
			: CategoryEntries[NewIndex]->InventoryCategoryData.Category;

		SetCategorization(NewCategory);
	}

	UE_LOG(LogTemp, Log, TEXT("  Category index: %d"), CategoryNavigationIndex);
}

/**
 * EventNavigateOk - Confirm selection / open action menu
 */
void UW_Inventory::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateOk"));

	if (SelectedSlot && SelectedSlot->IsOccupied)
	{
		// Trigger slot pressed event which opens action menu
		SelectedSlot->EventSlotPressed();
	}
}

/**
 * EventNavigateCancel - Close inventory or go back
 */
void UW_Inventory::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateCancel"));

	// Broadcast that inventory is closing
	OnInventoryClosed.Broadcast();

	// Hide the inventory widget
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * EventNavigateDetailedView - Toggle between detailed view and summary view
 * 
 * Per Blueprint: Gets current ItemInfoBoxSwitcher index and switches to the other:
 *   If index 0 -> SetActiveWidgetIndex(1)
 *   If index 1 -> SetActiveWidgetIndex(0)
 * 
 * This toggles the widget switcher INDEX, not visibility.
 */
void UW_Inventory::EventNavigateDetailedView_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventNavigateDetailedView"));

	if (ItemInfoBoxSwitcher)
	{
		// Get current index and switch to the other
		int32 CurrentIndex = ItemInfoBoxSwitcher->GetActiveWidgetIndex();
		int32 NewIndex = (CurrentIndex == 0) ? 1 : 0;
		ItemInfoBoxSwitcher->SetActiveWidgetIndex(NewIndex);
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Toggled ItemInfoBoxSwitcher from index %d to %d"), 
			CurrentIndex, NewIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] EventNavigateDetailedView - ItemInfoBoxSwitcher is NULL!"));
	}
}

/**
 * EventOnSlotPressed - Handle slot being pressed/clicked
 *
 * Blueprint Logic:
 * 1. Set the slot as selected
 * 2. Update the item info panel
 * 3. If occupied, broadcast OnPressed for action menu
 */
void UW_Inventory::EventOnSlotPressed_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnSlotPressed - Slot: %s, IsOccupied: %s"),
		InSlot ? *InSlot->GetName() : TEXT("NULL"),
		InSlot && InSlot->IsOccupied ? TEXT("true") : TEXT("false"));

	if (!InSlot)
	{
		return;
	}

	// Deselect previous slot if different
	if (SelectedSlot && SelectedSlot != InSlot)
	{
		SelectedSlot->SetSlotSelected(false);
	}

	// Set as selected slot
	SelectedSlot = InSlot;
	InSlot->SetSlotSelected(true);

	// Find index of this slot
	TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
	ItemNavigationIndex = ActiveSlots.Find(InSlot);

	// CRITICAL: Update the item info panel to show the item details
	if (InSlot->IsOccupied)
	{
		EventSetupItemInfoPanel(InSlot);

		// Show action menu for the item
		UW_InventoryAction* ActionWidget = StorageMode ? W_StorageAction : W_InventoryAction;
		UScrollBox* TargetScrollBox = StorageMode ? StorageScrollBox : InventoryScrollBox;

		if (ActionWidget)
		{
			// Position the action menu relative to the scrollbox
			FVector2D Translation = GetTranslationForActionMenu(TargetScrollBox);
			ActionWidget->SetRenderTranslation(Translation);

			// Show the action menu
			ActionWidget->SetVisibility(ESlateVisibility::Visible);

			// Setup the action menu for the selected slot
			if (StorageMode)
			{
				ActionWidget->EventSetupForStorage(InSlot);
			}
			else
			{
				ActionWidget->EventSetupForInventory(InSlot);
			}

			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Showing action menu at translation (%.1f, %.1f)"),
				Translation.X, Translation.Y);
		}

		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Slot pressed - item info panel updated for %s"),
			InSlot->AssignedItem ? *InSlot->AssignedItem->GetName() : TEXT("Unknown"));
	}
	else
	{
		// Hide item info and action menu for empty slots
		EventToggleItemInfo(false);

		// Hide action menus
		if (W_InventoryAction)
		{
			W_InventoryAction->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (W_StorageAction)
		{
			W_StorageAction->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

/**
 * EventOnSlotSelected - Handle slot selection change
 * Called when we want to programmatically select a slot (e.g., from navigation)
 */
void UW_Inventory::EventOnSlotSelected_Implementation(bool bSelected, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnSlotSelected - bSelected: %s"), bSelected ? TEXT("true") : TEXT("false"));

	if (!InSlot)
	{
		return;
	}

	if (bSelected)
	{
		// Deselect previous slot (update visual only, don't recurse)
		if (SelectedSlot && SelectedSlot != InSlot)
		{
			SelectedSlot->SetSlotSelected(false);
		}

		SelectedSlot = InSlot;

		// Update the slot's visual state (don't call EventOnSelected which broadcasts)
		InSlot->SetSlotSelected(true);

		// Update item info panel
		EventSetupItemInfoPanel(InSlot);

		// Update navigation index
		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		ItemNavigationIndex = ActiveSlots.Find(InSlot);
	}
	else
	{
		// Deselecting
		if (SelectedSlot == InSlot)
		{
			SelectedSlot = nullptr;
		}
		InSlot->SetSlotSelected(false);
	}
}

/**
 * EventOnSlotCleared - Handle slot being cleared
 */
void UW_Inventory::EventOnSlotCleared_Implementation(UW_InventorySlot* InSlot, bool bTriggerShift)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnSlotCleared - TriggerShift: %s"), bTriggerShift ? TEXT("true") : TEXT("false"));

	// Remove from occupied slots
	OccupiedInventorySlots.Remove(InSlot);

	// If this was the selected slot, select next available
	if (SelectedSlot == InSlot)
	{
		SelectedSlot = nullptr;

		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		if (ActiveSlots.IsValidIndex(ItemNavigationIndex))
		{
			SelectedSlot = ActiveSlots[ItemNavigationIndex];
			if (SelectedSlot)
			{
				SelectedSlot->EventOnSelected(true);
			}
		}
	}

	if (bTriggerShift)
	{
		// Would shift items to fill gap
		ReinitOccupiedInventorySlots();
	}
}

/**
 * EventOnInventorySlotAssigned - Handle item assigned to slot
 */
void UW_Inventory::EventOnInventorySlotAssigned_Implementation(UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnInventorySlotAssigned"));

	if (!InSlot)
	{
		return;
	}

	// Add to occupied slots if not already present
	if (!OccupiedInventorySlots.Contains(InSlot))
	{
		OccupiedInventorySlots.Add(InSlot);
	}
}

/**
 * EventOnCategorySelected - Handle category tab selection
 */
void UW_Inventory::EventOnCategorySelected_Implementation(UW_Inventory_CategoryEntry* InCategoryEntry, ESLFItemCategory SelectedCategory)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnCategorySelected - Category: %d"), (int32)SelectedCategory);

	// Apply the new category filter
	SetCategorization(SelectedCategory);

	// Update category navigation index
	CategoryNavigationIndex = CategoryEntries.Find(InCategoryEntry);
}

/**
 * EventOnVisibilityChanged - Handle inventory visibility change
 */
void UW_Inventory::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnVisibilityChanged - Visibility: %d"), (int32)InVisibility);

	if (InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		// Inventory becoming visible - refresh data
		CacheWidgetReferences();

		// CRITICAL: Populate slots with items from InventoryComponent
		PopulateSlotsWithItems();

		// Rebuild occupied slots list
		ReinitOccupiedInventorySlots();

		// Select first slot
		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		if (ActiveSlots.Num() > 0)
		{
			ItemNavigationIndex = 0;
			EventOnSlotSelected(true, ActiveSlots[0]);
		}

		// Set focus to this widget for keyboard input (per Blueprint)
		if (APlayerController* PC = GetOwningPlayer())
		{
			SetUserFocus(PC);
			UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnVisibilityChanged - Set user focus"));
		}
	}
}

/**
 * EventOnActionMenuVisibilityChanged - Handle action menu visibility
 */
void UW_Inventory::EventOnActionMenuVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnActionMenuVisibilityChanged - Visibility: %d"), (int32)InVisibility);

	// When action menu closes, return focus to inventory
	if (InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden)
	{
		// Refresh slots in case item was used/discarded
		ReinitOccupiedInventorySlots();
	}
}

/**
 * EventToggleItemInfo - Show/hide item info panel
 * 
 * Per Blueprint: Sets VISIBILITY on ItemNameText_Inventory and ItemInfoBoxSwitcher
 * Visible? = false → Collapsed
 * Visible? = true  → Visible
 * 
 * NOTE: This does NOT change the widget switcher index.
 * Index switching is handled by EventNavigateDetailedView (Navigate DETAILED VIEW).
 */
void UW_Inventory::EventToggleItemInfo_Implementation(bool bVisible)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventToggleItemInfo - bVisible: %s"), bVisible ? TEXT("true") : TEXT("false"));

	// Determine target visibility
	ESlateVisibility TargetVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	// Set visibility on ItemNameText_Inventory
	if (UWidget* ItemNameInventory = GetWidgetFromName(TEXT("ItemNameText_Inventory")))
	{
		ItemNameInventory->SetVisibility(TargetVisibility);
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Set ItemNameText_Inventory visibility to %s"), 
			bVisible ? TEXT("Visible") : TEXT("Collapsed"));
	}

	// Set visibility on ItemInfoBoxSwitcher
	if (ItemInfoBoxSwitcher)
	{
		ItemInfoBoxSwitcher->SetVisibility(TargetVisibility);
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Set ItemInfoBoxSwitcher visibility to %s"), 
			bVisible ? TEXT("Visible") : TEXT("Collapsed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] EventToggleItemInfo - ItemInfoBoxSwitcher is NULL!"));
	}
}

/**
 * EventSetupItemInfoPanel - Configure item info panel for selected slot
 *
 * Blueprint Logic (from W_Inventory.json EventSetupItemInfoPanel graph):
 * 1. Get SelectedSlot->AssignedItem
 * 2. Get ItemInformation from AssignedItem
 * 3. Break FItemInfo struct to get DisplayName, ShortDescription, LongDescription,
 *    Category, IconSmall, IconLargeOptional, MaxAmount
 * 4. Set text blocks:
 *    - ItemNameText_Details, ItemNameText_Inventory, ItemNameText_LongDetails, ItemNameText_Storage <- DisplayName
 *    - ItemCategoryText <- Category.CategoryName (enum to string)
 *    - ItemDescription <- ShortDescription
 *    - ItemLongDescription <- LongDescription
 *    - ItemAmountText <- "X / MaxAmount" format (using slot's Count)
 * 5. Set image:
 *    - ItemIcon <- IconSmall (or IconLargeOptional if available)
 */
void UW_Inventory::EventSetupItemInfoPanel_Implementation(UW_InventorySlot* ForSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventSetupItemInfoPanel - ForSlot: %s"),
		ForSlot ? *ForSlot->GetName() : TEXT("NULL"));

	if (!ForSlot || !ForSlot->IsOccupied)
	{
		// Hide item info if no item
		EventToggleItemInfo(false);
		return;
	}

	// Get the item from the slot
	UPDA_Item* Item = Cast<UPDA_Item>(ForSlot->AssignedItem);
	if (!Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] EventSetupItemInfoPanel - AssignedItem is not UPDA_Item"));
		EventToggleItemInfo(false);
		return;
	}

	// Get ItemInformation from the item
	const FSLFItemInfo& ItemInfo = Item->ItemInformation;

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Setting up item info panel for: %s"), *ItemInfo.DisplayName.ToString());

	// Show the item info panel
	EventToggleItemInfo(true);

	// ═══════════════════════════════════════════════════════════════════════════
	// SET DISPLAY NAME on multiple text widgets
	// ═══════════════════════════════════════════════════════════════════════════

	// ItemNameText_Details
	if (UTextBlock* ItemNameDetails = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Details"))))
	{
		ItemNameDetails->SetText(ItemInfo.DisplayName);
	}

	// ItemNameText_Inventory
	if (UTextBlock* ItemNameInventory = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Inventory"))))
	{
		ItemNameInventory->SetText(ItemInfo.DisplayName);
	}

	// ItemNameText_LongDetails
	if (UTextBlock* ItemNameLongDetails = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_LongDetails"))))
	{
		ItemNameLongDetails->SetText(ItemInfo.DisplayName);
	}

	// ItemNameText_Storage
	if (UTextBlock* ItemNameStorage = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Storage"))))
	{
		ItemNameStorage->SetText(ItemInfo.DisplayName);
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// SET CATEGORY TEXT
	// ═══════════════════════════════════════════════════════════════════════════

	if (UTextBlock* CategoryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemCategoryText"))))
	{
		// Convert category enum to display string
		// ESLFItemCategory: None, Tools, Crafting, Bolstering, KeyItems, Abilities, Weapons, Shields, Armor
		FString CategoryString;
		switch (ItemInfo.Category.Category)
		{
		case ESLFItemCategory::Tools:
			CategoryString = TEXT("Tools");
			break;
		case ESLFItemCategory::Crafting:
			CategoryString = TEXT("Crafting");
			break;
		case ESLFItemCategory::Bolstering:
			CategoryString = TEXT("Bolstering");
			break;
		case ESLFItemCategory::KeyItems:
			CategoryString = TEXT("Key Items");
			break;
		case ESLFItemCategory::Abilities:
			CategoryString = TEXT("Abilities");
			break;
		case ESLFItemCategory::Weapons:
			CategoryString = TEXT("Weapons");
			break;
		case ESLFItemCategory::Shields:
			CategoryString = TEXT("Shields");
			break;
		case ESLFItemCategory::Armor:
			CategoryString = TEXT("Armor");
			break;
		case ESLFItemCategory::None:
		default:
			CategoryString = TEXT("-");
			break;
		}
		CategoryText->SetText(FText::FromString(CategoryString));
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// SET DESCRIPTION TEXT
	// ═══════════════════════════════════════════════════════════════════════════

	// ItemDescription (short)
	if (UTextBlock* DescriptionText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemDescription"))))
	{
		DescriptionText->SetText(ItemInfo.ShortDescription);
	}

	// ItemLongDescription
	if (UTextBlock* LongDescriptionText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemLongDescription"))))
	{
		LongDescriptionText->SetText(ItemInfo.LongDescription);
	}

	// ItemOnUseDescription - "On Use" effect text
	if (UTextBlock* OnUseDescText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemOnUseDescription"))))
	{
		OnUseDescText->SetText(ItemInfo.OnUseDescription);
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Set ItemOnUseDescription to: %s"), *ItemInfo.OnUseDescription.ToString());
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// SET AMOUNT TEXT (No. Held / Max Stack)
	// From WidgetTree: AmountText = No. Held value, MaxAmountText = Max Stack value
	// ═══════════════════════════════════════════════════════════════════════════

	// AmountText - "No. Held" value (just the count)
	if (UTextBlock* AmountTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText"))))
	{
		AmountTextWidget->SetText(FText::AsNumber(ForSlot->Count));
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Set AmountText (No. Held) to: %d"), ForSlot->Count);
	}

	// MaxAmountText - "Max Stack" value
	if (UTextBlock* MaxAmountTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaxAmountText"))))
	{
		MaxAmountTextWidget->SetText(FText::AsNumber(ItemInfo.MaxAmount));
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Set MaxAmountText (Max Stack) to: %d"), ItemInfo.MaxAmount);
	}

	// StoredAmountText - for storage mode
	if (UTextBlock* StoredAmountTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("StoredAmountText"))))
	{
		StoredAmountTextWidget->SetText(FText::AsNumber(ForSlot->Count));
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// SET ITEM ICON IMAGE
	// ═══════════════════════════════════════════════════════════════════════════

	if (UImage* IconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		// Prefer large icon if available, fall back to small icon
		UTexture2D* IconTexture = nullptr;

		if (!ItemInfo.IconLargeOptional.IsNull())
		{
			IconTexture = ItemInfo.IconLargeOptional.LoadSynchronous();
		}

		if (!IconTexture && !ItemInfo.IconSmall.IsNull())
		{
			IconTexture = ItemInfo.IconSmall.LoadSynchronous();
		}

		if (IconTexture)
		{
			IconImage->SetBrushFromTexture(IconTexture);
			IconImage->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Set ItemIcon to: %s"), *IconTexture->GetName());
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Warning, TEXT("[W_Inventory] No icon texture available for item"));
		}
	}

	// DetailsLargeImage - large icon in the item details panel (right side)
	if (UImage* LargeIcon = Cast<UImage>(GetWidgetFromName(TEXT("DetailsLargeImage"))))
	{
		UTexture2D* IconTexture = nullptr;

		if (!ItemInfo.IconLargeOptional.IsNull())
		{
			IconTexture = ItemInfo.IconLargeOptional.LoadSynchronous();
		}
		else if (!ItemInfo.IconSmall.IsNull())
		{
			IconTexture = ItemInfo.IconSmall.LoadSynchronous();
		}

		if (IconTexture)
		{
			LargeIcon->SetBrushFromTexture(IconTexture);
			LargeIcon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			LargeIcon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] EventSetupItemInfoPanel complete for: %s"), *ItemInfo.DisplayName.ToString());
}

/**
 * EventOnErrorReceived - Display error message
 *
 * Blueprint Logic:
 * 1. Set ErrorBorder visibility to Visible
 * 2. Call EventSetErrorMessage on W_Error widget
 */
void UW_Inventory::EventOnErrorReceived_Implementation(const FText& ErrorMessage)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventOnErrorReceived - %s"), *ErrorMessage.ToString());

	// Show ErrorBorder
	if (UWidget* ErrorBorder = GetWidgetFromName(TEXT("ErrorBorder")))
	{
		ErrorBorder->SetVisibility(ESlateVisibility::Visible);
	}

	// Setup error message on W_Error widget (W_GenericError)
	if (UW_GenericError* W_Error = Cast<UW_GenericError>(GetWidgetFromName(TEXT("W_Error"))))
	{
		W_Error->EventSetErrorMessage(ErrorMessage);
		UE_LOG(LogTemp, Log, TEXT("[W_Inventory] Displayed error via W_Error: %s"), *ErrorMessage.ToString());
	}
}

/**
 * EventDismissError - Hide error message
 */
void UW_Inventory::EventDismissError_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory::EventDismissError"));

	// Hide the error overlay widget
	if (UWidget* ErrorBorder = GetWidgetFromName(TEXT("ErrorBorder")))
	{
		ErrorBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// SLOT EVENT HANDLERS
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * HandleSlotSelected - Handler for slot OnSelected event
 * NOTE: Do NOT call EventOnSlotSelected here - it would create infinite recursion
 * This handler is called FROM the slot's OnSelected broadcast, so we just update internal state
 */
void UW_Inventory::HandleSlotSelected(bool bSelected, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Inventory] HandleSlotSelected - bSelected: %s, Slot: %s"),
		bSelected ? TEXT("true") : TEXT("false"),
		InSlot ? *InSlot->GetName() : TEXT("NULL"));

	if (!InSlot)
	{
		return;
	}

	if (bSelected)
	{
		// Update selected slot reference
		SelectedSlot = InSlot;

		// Update navigation index
		TArray<UW_InventorySlot*>& ActiveSlots = StorageMode ? StorageSlots : InventorySlots;
		ItemNavigationIndex = ActiveSlots.Find(InSlot);

		// Update item info panel
		EventSetupItemInfoPanel(InSlot);
	}
}

/**
 * HandleSlotPressed - Handler for slot OnPressed event
 */
void UW_Inventory::HandleSlotPressed(UW_InventorySlot* InSlot)
{
	EventOnSlotPressed(InSlot);
}

/**
 * HandleSlotCleared - Handler for slot OnSlotCleared event
 */
void UW_Inventory::HandleSlotCleared(UW_InventorySlot* InSlot, bool bTriggerShift)
{
	EventOnSlotCleared(InSlot, bTriggerShift);
}

/**
 * HandleSlotAssigned - Handler for slot OnSlotAssigned event
 */
void UW_Inventory::HandleSlotAssigned(UW_InventorySlot* InSlot)
{
	EventOnInventorySlotAssigned(InSlot);
}

/**
 * UpdateInputIcons - Update all input icon images based on current input bindings
 */
void UW_Inventory::UpdateInputIcons()
{
	// Define input keys for each icon widget name
	// These correspond to the typical inventory navigation keys
	struct FInputIconBinding {
		FName WidgetName;
		FKey Key;
	};

	// Note: These keys should match your Input Action mappings
	// Widget names must match the UMG designer names exactly
	TArray<FInputIconBinding> Bindings = {
		{TEXT("CategoryLeftInputIcon"), EKeys::Q},
		{TEXT("CategoryRightInputIcon"), EKeys::E},
		{TEXT("OkInputIcon"), EKeys::Enter},
		{TEXT("BackInputIcon"), EKeys::Escape},
		{TEXT("ScrollLeftInputIcon"), EKeys::A},
		{TEXT("ScrollRightInputIcon"), EKeys::D},
		{TEXT("DetailsInputIcon"), EKeys::Tab}
	};

	for (const FInputIconBinding& Binding : Bindings)
	{
		// Use GetWidgetFromName to find the widget dynamically
		// This works regardless of whether BindWidget/BindWidgetOptional found it
		UWidget* Widget = GetWidgetFromName(Binding.WidgetName);
		UImage* ImageWidget = Cast<UImage>(Widget);

		if (ImageWidget)
		{
			TSoftObjectPtr<UTexture2D> IconTexture;
			GetInputIconForKey(Binding.Key, IconTexture);

			if (!IconTexture.IsNull())
			{
				ImageWidget->SetBrushFromSoftTexture(IconTexture, false);
				ImageWidget->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Log, TEXT("UpdateInputIcons: Set %s to %s"), *Binding.WidgetName.ToString(), *IconTexture.ToString());
			}
			else
			{
				// Hide the icon if no texture is available
				ImageWidget->SetVisibility(ESlateVisibility::Collapsed);
				UE_LOG(LogTemp, Warning, TEXT("UpdateInputIcons: No texture for %s (Key: %s)"), *Binding.WidgetName.ToString(), *Binding.Key.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UpdateInputIcons: Widget '%s' not found"), *Binding.WidgetName.ToString());
		}
	}

	// Also update W_InventoryAction icons if available
	if (W_InventoryAction)
	{
		TSoftObjectPtr<UTexture2D> OkTexture;
		TSoftObjectPtr<UTexture2D> BackTexture;
		GetInputIconForKey(EKeys::Enter, OkTexture);
		GetInputIconForKey(EKeys::Escape, BackTexture);
		W_InventoryAction->EventUpdateOkInputIcons(OkTexture);
		W_InventoryAction->EventUpdateBackInputIcons(BackTexture);
	}
}

/**
 * EventOnHardwareDeviceDetected - Called when input device changes (keyboard/gamepad)
 */
void UW_Inventory::EventOnHardwareDeviceDetected_Implementation(FPlatformUserId UserId, FInputDeviceId DeviceId)
{
	Super::EventOnHardwareDeviceDetected_Implementation(UserId, DeviceId);

	// Update input icons when input device changes
	UpdateInputIcons();
}
