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
#include "Widgets/W_Equipment_Item_AttackPower.h"
#include "Widgets/W_Equipment_Item_StatScaling.h"
#include "Widgets/W_Equipment_Item_RequiredStats.h"
#include "Widgets/W_Equipment_Item_StatsGranted.h"
#include "Widgets/W_Equipment_Item_DamageNegation.h"
#include "Widgets/W_Equipment_Item_Resistance.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_EquipmentManager.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/DataTable.h"
#include "SLFPrimaryDataAssets.h"

UW_Equipment::UW_Equipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentCanvas(nullptr)
	, ItemScrollBox(nullptr)
	, UniformEquipmentItemsGrid(nullptr)
	, EquipmentSlotsUniformGrid(nullptr)
	, ItemInfoBoxSwitcher(nullptr)
	, InfoSwitcher(nullptr)
	, Info_Weapon(nullptr)
	, Info_Armor(nullptr)
	, Info_Talisman(nullptr)
	, SlotNameText(nullptr)
	, EquipmentSwitcher(nullptr)
	, ErrorBorder(nullptr)
	, W_EquipmentError(nullptr)
	, InventoryComponent(nullptr)
	, AC_InventoryComponent(nullptr)
	, SelectedSlot(nullptr)
	, ActiveEquipmentSlot(nullptr)
	, EquipmentComponent(nullptr)
	, ActiveItemSlot(nullptr)
	, EquipmentSlotNavigationIndex(0)
	, ItemNavigationIndex(0)
	, CurrentGridRow(0)
	, CurrentGridColumn(0)
	, EquipmentSlotClass(nullptr)
{
}

void UW_Equipment::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Hidden MainBlur widget"));
	}

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
		AC_InventoryComponent = PC->FindComponentByClass<UAC_InventoryManager>();
		EquipmentComponent = PC->FindComponentByClass<UAC_EquipmentManager>();
	}

	// Fallback to Pawn if not found on PC
	if (!InventoryComponent && Pawn)
	{
		InventoryComponent = Pawn->FindComponentByClass<UInventoryManagerComponent>();
	}
	if (!AC_InventoryComponent && Pawn)
	{
		AC_InventoryComponent = Pawn->FindComponentByClass<UAC_InventoryManager>();
	}
	if (!EquipmentComponent && Pawn)
	{
		EquipmentComponent = Pawn->FindComponentByClass<UAC_EquipmentManager>();
	}

	// Log which inventory component we found
	if (InventoryComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found InventoryComponent (UInventoryManagerComponent) on %s"),
			InventoryComponent->GetOwner() ? *InventoryComponent->GetOwner()->GetName() : TEXT("Unknown"));
		InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UW_Equipment::HandleInventoryUpdated);
	}
	else if (AC_InventoryComponent)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found AC_InventoryComponent (UAC_InventoryManager) on %s"),
			AC_InventoryComponent->GetOwner() ? *AC_InventoryComponent->GetOwner()->GetName() : TEXT("Unknown"));
		AC_InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UW_Equipment::HandleInventoryUpdated);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] NO InventoryComponent found on PC or Pawn!"));
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

	// Update input icons (hide if no texture to prevent white squares)
	UpdateInputIcons();

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

void UW_Equipment::UpdateInputIcons()
{
	// Define input keys for each icon widget name
	// These correspond to the typical equipment navigation keys
	struct FInputIconBinding {
		FName WidgetName;
		FKey Key;
	};

	// Note: These keys should match your Input Action mappings
	// Widget names must match the UMG designer names exactly
	TArray<FInputIconBinding> Bindings = {
		{TEXT("OkInputIcon"), EKeys::Enter},
		{TEXT("BackInputIcon"), EKeys::Escape},
		{TEXT("UnequipInputIcon"), EKeys::Y},
		{TEXT("ScrollLeftInputIcon"), EKeys::A},
		{TEXT("ScrollRightInputIcon"), EKeys::D},
		{TEXT("DetailsInputIcon"), EKeys::X}
	};

	for (const FInputIconBinding& Binding : Bindings)
	{
		// Use GetWidgetFromName to find the widget dynamically
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
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UpdateInputIcons: Set %s to %s"), *Binding.WidgetName.ToString(), *IconTexture.ToString());
			}
			else
			{
				// Hide the icon if no texture is available (prevents white squares)
				ImageWidget->SetVisibility(ESlateVisibility::Collapsed);
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UpdateInputIcons: Hiding %s (no texture for key %s)"), *Binding.WidgetName.ToString(), *Binding.Key.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UpdateInputIcons: Widget '%s' not found"), *Binding.WidgetName.ToString());
		}
	}
}

void UW_Equipment::PopulateEquipmentSlots()
{
	// Clear existing slots and positions
	EquipmentSlots.Empty();
	EquipmentSlotsPositions.Empty();

	if (!EquipmentSlotsUniformGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] PopulateEquipmentSlots - EquipmentSlotsUniformGrid not found!"));
		return;
	}

	// Clear the grid
	EquipmentSlotsUniformGrid->ClearChildren();

	// Try to find EquipmentSlotClass if not set
	if (!EquipmentSlotClass)
	{
		// Try to load the default W_EquipmentSlot Blueprint class
		static const TCHAR* DefaultSlotPath = TEXT("/Game/SoulslikeFramework/Widgets/Equipment/W_EquipmentSlot.W_EquipmentSlot_C");
		EquipmentSlotClass = LoadClass<UW_EquipmentSlot>(nullptr, DefaultSlotPath);

		if (EquipmentSlotClass)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] PopulateEquipmentSlots - Loaded default EquipmentSlotClass"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] PopulateEquipmentSlots - EquipmentSlotClass not set and default not found!"));
			return;
		}
	}

	// Define equipment slots directly in C++ (matching original DT_ExampleEquipmentSlotInfo)
	// This avoids the Blueprint struct row type mismatch issue with DataTables
	struct FEquipmentSlotDef
	{
		FGameplayTag SlotTag;
		int32 Row;
		int32 Column;
	};

	TArray<FEquipmentSlotDef> SlotDefinitions;

	// Right Hand Weapons (Column 0)
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1")), 0, 0});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 2")), 1, 0});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 3")), 2, 0});

	// Left Hand Weapons (Column 1)
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1")), 0, 1});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 2")), 1, 1});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 3")), 2, 1});

	// Armor (Column 2)
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Head")), 0, 2});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Armor")), 1, 2});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Gloves")), 2, 2});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Greaves")), 3, 2});

	// Accessories (Column 3)
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Trinket 1")), 0, 3});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Trinket 2")), 1, 3});

	// Ammo (Column 4)
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Arrow")), 0, 4});
	SlotDefinitions.Add({FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Bullet")), 1, 4});

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] PopulateEquipmentSlots - Creating %d slots from C++ definitions"), SlotDefinitions.Num());

	for (const FEquipmentSlotDef& SlotDef : SlotDefinitions)
	{
		if (!SlotDef.SlotTag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] PopulateEquipmentSlots - Invalid slot tag, skipping"));
			continue;
		}

		// Create the equipment slot widget
		UW_EquipmentSlot* NewSlot = CreateWidget<UW_EquipmentSlot>(this, EquipmentSlotClass);
		if (!NewSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] PopulateEquipmentSlots - Failed to create slot widget"));
			continue;
		}

		// Configure the slot
		NewSlot->EquipmentSlot = SlotDef.SlotTag;
		// BackgroundIcon can be set from default in W_EquipmentSlot Blueprint

		// Add to grid at specified position
		EquipmentSlotsUniformGrid->AddChildToUniformGrid(NewSlot, SlotDef.Row, SlotDef.Column);

		// Track the slot and its position
		EquipmentSlots.Add(NewSlot);
		EquipmentSlotsPositions.Add(FVector2D(SlotDef.Row, SlotDef.Column));

		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Created equipment slot: %s at Row=%d, Col=%d"),
			*SlotDef.SlotTag.ToString(), SlotDef.Row, SlotDef.Column);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] PopulateEquipmentSlots - Created %d slots"), EquipmentSlots.Num());
}

UW_EquipmentSlot* UW_Equipment::GetSlotAtGridPosition(int32 Row, int32 Column) const
{
	for (int32 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (EquipmentSlotsPositions.IsValidIndex(i))
		{
			const FVector2D& Pos = EquipmentSlotsPositions[i];
			if (FMath::RoundToInt(Pos.X) == Row && FMath::RoundToInt(Pos.Y) == Column)
			{
				return EquipmentSlots[i];
			}
		}
	}
	return nullptr;
}

void UW_Equipment::SelectSlotAtGridPosition(int32 Row, int32 Column)
{
	UW_EquipmentSlot* NewSlot = GetSlotAtGridPosition(Row, Column);
	if (NewSlot)
	{
		// Deselect old slot
		if (SelectedSlot && SelectedSlot != NewSlot)
		{
			SelectedSlot->EventOnSelected(false);
		}

		// Select new slot
		SelectedSlot = NewSlot;
		CurrentGridRow = Row;
		CurrentGridColumn = Column;

		// Update index for backwards compatibility
		int32 NewIndex = EquipmentSlots.Find(NewSlot);
		if (NewIndex != INDEX_NONE)
		{
			EquipmentSlotNavigationIndex = NewIndex;
		}

		SelectedSlot->EventOnSelected(true);
		EventToggleSlotName(SelectedSlot, true);

		// Update unequip icon visibility based on whether slot has an item
		UpdateUnequipIconVisibility();

		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Selected slot at Row=%d, Col=%d"), Row, Column);
	}
}

int32 UW_Equipment::GetMaxRowsInColumn(int32 Column) const
{
	// Column layout: Right Hand (0), Left Hand (1), Armor (2), Trinkets (3), Ammo (4)
	switch (Column)
	{
		case 0: return 3;  // Right Hand Weapons 1-3
		case 1: return 3;  // Left Hand Weapons 1-3
		case 2: return 4;  // Head, Armor, Gloves, Greaves
		case 3: return 2;  // Trinket 1-2
		case 4: return 2;  // Arrow, Bullet
		default: return 0;
	}
}

void UW_Equipment::UpdateUnequipIconVisibility()
{
	// Show UnequipInputIcon only when selected slot has an item equipped
	UImage* UnequipIcon = Cast<UImage>(GetWidgetFromName(TEXT("UnequipInputIcon")));
	if (UnequipIcon)
	{
		bool bShowUnequip = SelectedSlot && SelectedSlot->IsOccupied;
		UnequipIcon->SetVisibility(bShowUnequip ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UpdateUnequipIconVisibility: %s (SelectedSlot=%s, IsOccupied=%s)"),
			bShowUnequip ? TEXT("Visible") : TEXT("Hidden"),
			SelectedSlot ? TEXT("valid") : TEXT("null"),
			(SelectedSlot && SelectedSlot->IsOccupied) ? TEXT("true") : TEXT("false"));
	}
}

void UW_Equipment::BindEquipmentSlotEvents()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] BindEquipmentSlotEvents - Binding %d slots"), EquipmentSlots.Num());

	for (int32 i = 0; i < EquipmentSlots.Num(); i++)
	{
		UW_EquipmentSlot* EquipSlot = EquipmentSlots[i];
		if (IsValid(EquipSlot))
		{
			// Remove any existing bindings first to prevent duplicates
			EquipSlot->OnSelected.RemoveAll(this);
			EquipSlot->OnPressed.RemoveAll(this);

			// Bind fresh
			EquipSlot->OnSelected.AddDynamic(this, &UW_Equipment::EventOnEquipmentSlotSelected);
			EquipSlot->OnPressed.AddDynamic(this, &UW_Equipment::EventOnEquipmentSlotPressed);

			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Bound events for slot %d: %s"), i, *EquipSlot->EquipmentSlot.ToString());
		}
	}
}

void UW_Equipment::SyncEquipmentSlotsFromManager()
{
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] SyncEquipmentSlotsFromManager - No EquipmentComponent"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] SyncEquipmentSlotsFromManager - Syncing %d slots"), EquipmentSlots.Num());

	for (UW_EquipmentSlot* EquipSlot : EquipmentSlots)
	{
		if (!IsValid(EquipSlot))
		{
			continue;
		}

		FSLFItemInfo ItemData;
		UPrimaryDataAsset* ItemAsset = nullptr;
		FGuid ItemId;
		FSLFItemInfo ItemData2;
		UPrimaryDataAsset* ItemAsset2 = nullptr;
		FGuid ItemId2;

		EquipmentComponent->GetItemAtSlot(
			EquipSlot->EquipmentSlot,
			ItemData, ItemAsset, ItemId,
			ItemData2, ItemAsset2, ItemId2
		);

		if (ItemAsset)
		{
			if (UPDA_Item* Item = Cast<UPDA_Item>(ItemAsset))
			{
				EquipSlot->EventOccupyEquipmentSlot(Item);
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] SyncEquipmentSlotsFromManager - Slot %s has item: %s"),
					*EquipSlot->EquipmentSlot.ToString(), *Item->GetName());
			}
		}
		else
		{
			EquipSlot->EventClearEquipmentSlot();
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

	// ════════════════════════════════════════════════════════════════════════
	// IMAGES - Set both compact (ItemIcon) and detailed (DetailsLargeImage) views
	// bp_only: Uses SetBrushFromSoftTexture which accepts TSoftObjectPtr directly
	// ════════════════════════════════════════════════════════════════════════

	// Set ItemIcon (compact view - small icon)
	if (UImage* ItemIcon = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		if (!ItemInfo.IconSmall.IsNull())
		{
			ItemIcon->SetBrushFromSoftTexture(ItemInfo.IconSmall, false);
			ItemIcon->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set ItemIcon from IconSmall: %s"), *ItemInfo.IconSmall.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] ItemInfo.IconSmall is null!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] ItemIcon widget not found!"));
	}

	// Set DetailsLargeImage (detailed view - use large icon if available, else small)
	if (UImage* LargeImage = Cast<UImage>(GetWidgetFromName(TEXT("DetailsLargeImage"))))
	{
		TSoftObjectPtr<UTexture2D> IconToUse = ItemInfo.IconLargeOptional.IsNull() ? ItemInfo.IconSmall : ItemInfo.IconLargeOptional;
		if (!IconToUse.IsNull())
		{
			LargeImage->SetBrushFromSoftTexture(IconToUse, false);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set DetailsLargeImage"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] DetailsLargeImage widget not found!"));
	}

	// ════════════════════════════════════════════════════════════════════════
	// TEXT WIDGETS - Populate both compact and detailed view text
	// ════════════════════════════════════════════════════════════════════════

	// Detailed view widgets (ItemLongInfo)
	if (UTextBlock* ItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_LongDetails"))))
	{
		ItemNameText->SetText(ItemInfo.DisplayName);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set ItemNameText_LongDetails to: %s"), *ItemInfo.DisplayName.ToString());
	}

	if (UTextBlock* DescText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemLongDescription"))))
	{
		DescText->SetText(ItemInfo.LongDescription);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set ItemLongDescription"));
	}

	// Compact view widgets (ItemInfoOverlay)
	if (UTextBlock* ItemNameDetails = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Details"))))
	{
		ItemNameDetails->SetText(ItemInfo.DisplayName);
	}

	// bp_only: Blueprint sets ALL FOUR text widgets with DisplayName at once:
	// ItemNameText_Details, ItemNameText_LongDetails, ItemNameText, ItemNameText_Grid
	// The regular view ItemNameText shows on the LEFT panel
	if (UTextBlock* ItemNameRegular = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
	{
		ItemNameRegular->SetText(ItemInfo.DisplayName);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set ItemNameText (regular view) to: %s"), *ItemInfo.DisplayName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] ItemNameText widget not found!"));
	}

	// Grid view text
	if (UTextBlock* ItemNameGrid = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Grid"))))
	{
		ItemNameGrid->SetText(ItemInfo.DisplayName);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set ItemNameText_Grid to: %s"), *ItemInfo.DisplayName.ToString());
	}

	if (UTextBlock* CategoryText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemCategoryText"))))
	{
		FString ItemCategoryName = UEnum::GetDisplayValueAsText(ItemInfo.Category.Category).ToString();
		CategoryText->SetText(FText::FromString(ItemCategoryName));
	}

	// ════════════════════════════════════════════════════════════════════════
	// STAT SUB-WIDGETS - Call setup functions on child stat widgets
	// bp_only: W_Equipment has child widgets for attack power, required stats, etc.
	// bp_only: Uses "Switch on E_ItemCategory" to show different stats per category
	// ════════════════════════════════════════════════════════════════════════

	// Get equipment stat data
	const FSLFEquipmentInfo& EquipInfo = ItemInfo.EquipmentDetails;
	const FSLFEquipmentWeaponStatInfo& WeaponStats = EquipInfo.WeaponStatInfo;
	const ESLFItemCategory ItemCategory = ItemInfo.Category.Category;

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Item category: %d"), static_cast<int32>(ItemCategory));

	// Check if this is a weapon-type item (Weapons, Shields)
	bool bIsWeaponType = (ItemCategory == ESLFItemCategory::Weapons || ItemCategory == ESLFItemCategory::Shields);
	// Check if this is armor
	bool bIsArmor = (ItemCategory == ESLFItemCategory::Armor);
	// Check if this is a talisman/bolstering item
	bool bIsTalisman = (ItemCategory == ESLFItemCategory::Bolstering);

	// ════════════════════════════════════════════════════════════════════════
	// INFO SWITCHER - Switch to correct panel based on item category
	// bp_only: Uses InfoSwitcher->SetActiveWidget() to show Info_Weapon, Info_Armor, or Info_Talisman
	// This is CRITICAL - without this, the stats panels won't be visible
	// ════════════════════════════════════════════════════════════════════════
	if (InfoSwitcher)
	{
		UWidget* TargetPanel = nullptr;
		FString PanelName;

		if (bIsWeaponType)
		{
			TargetPanel = Info_Weapon;
			PanelName = TEXT("Info_Weapon");
		}
		else if (bIsArmor)
		{
			TargetPanel = Info_Armor;
			PanelName = TEXT("Info_Armor");
		}
		else if (bIsTalisman)
		{
			TargetPanel = Info_Talisman;
			PanelName = TEXT("Info_Talisman");
		}
		else
		{
			// Default to weapon panel for other item types
			TargetPanel = Info_Weapon;
			PanelName = TEXT("Info_Weapon (default)");
		}

		if (TargetPanel)
		{
			InfoSwitcher->SetActiveWidget(TargetPanel);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] InfoSwitcher->SetActiveWidget(%s)"), *PanelName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] %s panel is NULL, cannot set active widget"), *PanelName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] InfoSwitcher is NULL - stats panels won't switch correctly!"));
	}

	// Get widget references for visibility control
	UW_Equipment_Item_AttackPower* AttackPowerWidget = Cast<UW_Equipment_Item_AttackPower>(GetWidgetFromName(TEXT("W_Equipment_Item_AttackPower")));
	UW_Equipment_Item_RequiredStats* RequiredStatsWidget = Cast<UW_Equipment_Item_RequiredStats>(GetWidgetFromName(TEXT("W_Equipment_Item_RequiredStats")));
	UW_Equipment_Item_StatScaling* StatScalingWidget = Cast<UW_Equipment_Item_StatScaling>(GetWidgetFromName(TEXT("W_Equipment_Item_StatScaling")));
	UW_Equipment_Item_StatsGranted* StatsGrantedWidget = Cast<UW_Equipment_Item_StatsGranted>(GetWidgetFromName(TEXT("W_Equipment_Item_StatsGranted")));

	// ════════════════════════════════════════════════════════════════════════
	// WEAPON-SPECIFIC STATS (AttackPower, RequiredStats, StatScaling)
	// ════════════════════════════════════════════════════════════════════════
	if (bIsWeaponType)
	{
		// Build attack power map from StatChanges (which contains damage values like Physical: 34)
		TMap<FGameplayTag, int32> TargetAttackPowerMap;
		for (const auto& StatPair : EquipInfo.StatChanges)
		{
			// StatChanges contains damage types - convert Delta to int32
			TargetAttackPowerMap.Add(StatPair.Key, static_cast<int32>(StatPair.Value.Delta));
		}

		// TODO: Get currently equipped item's stats for comparison
		TMap<FGameplayTag, int32> CurrentAttackPowerMap;

		// W_Equipment_Item_AttackPower - Setup attack power stats
		if (AttackPowerWidget)
		{
			AttackPowerWidget->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found W_Equipment_Item_AttackPower widget - StatChanges: %d"),
				TargetAttackPowerMap.Num());

			if (TargetAttackPowerMap.Num() > 0)
			{
				AttackPowerWidget->EventSetupAttackPowerStats(TargetAttackPowerMap, CurrentAttackPowerMap, CompareStats);
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Called EventSetupAttackPowerStats with %d stats"), TargetAttackPowerMap.Num());
			}
		}

		// W_Equipment_Item_RequiredStats - Always setup for weapons (shows all primary stats, with 0 for no requirement)
		if (RequiredStatsWidget)
		{
			RequiredStatsWidget->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found W_Equipment_Item_RequiredStats widget - has requirement: %s, count: %d"),
				WeaponStats.bHasStatRequirement ? TEXT("true") : TEXT("false"), WeaponStats.StatRequirementInfo.Num());

			// Always call - widget shows all primary stats; pass empty map if no requirements
			RequiredStatsWidget->EventSetupRequiredStats(WeaponStats.StatRequirementInfo);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Called EventSetupRequiredStats with %d requirements"), WeaponStats.StatRequirementInfo.Num());
		}

		// W_Equipment_Item_StatScaling - Always setup for weapons (shows all primary stats, with "-" for no scaling)
		if (StatScalingWidget)
		{
			StatScalingWidget->SetVisibility(ESlateVisibility::Visible);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Found W_Equipment_Item_StatScaling widget - has scaling: %s, count: %d"),
				WeaponStats.bHasStatScaling ? TEXT("true") : TEXT("false"), WeaponStats.ScalingInfo.Num());

			// Always call - widget shows all primary stats; pass empty map if no scaling
			StatScalingWidget->EventSetupStatScaling(WeaponStats.ScalingInfo);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Called EventSetupStatScaling with %d scaling stats"), WeaponStats.ScalingInfo.Num());
		}

		// Hide armor-specific widgets for weapons
		if (StatsGrantedWidget)
		{
			StatsGrantedWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		// Also hide DamageNegation, Resistance, and Weight widgets for weapons
		if (UW_Equipment_Item_DamageNegation* DamageNegationWidget = Cast<UW_Equipment_Item_DamageNegation>(GetWidgetFromName(TEXT("W_Equipment_Item_DamageNegation"))))
		{
			DamageNegationWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UW_Equipment_Item_Resistance* ResistanceWidget = Cast<UW_Equipment_Item_Resistance>(GetWidgetFromName(TEXT("W_Equipment_Item_Resistance"))))
		{
			ResistanceWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UWidget* ArmorWeightBox = GetWidgetFromName(TEXT("ArmorWeightBox")))
		{
			ArmorWeightBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	// ════════════════════════════════════════════════════════════════════════
	// ARMOR-SPECIFIC STATS (DamageNegation, Resistances, Weight)
	// bp_only shows: Weight, Damage Negation (Physical/Magic/Fire/Frost/Lightning/Holy),
	//                Resistances (Immunity/Focus/Robustness/Vitality)
	// ════════════════════════════════════════════════════════════════════════
	else if (bIsArmor)
	{
		// Hide weapon-specific widgets for armor
		if (AttackPowerWidget)
		{
			AttackPowerWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (RequiredStatsWidget)
		{
			RequiredStatsWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (StatScalingWidget)
		{
			StatScalingWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		// StatsGranted is not shown for armor in bp_only (armor shows DamageNegation/Resistances instead)
		if (StatsGrantedWidget)
		{
			StatsGrantedWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		// Get DamageNegation and Resistance widget references
		UW_Equipment_Item_DamageNegation* DamageNegationWidget = Cast<UW_Equipment_Item_DamageNegation>(GetWidgetFromName(TEXT("W_Equipment_Item_DamageNegation")));
		UW_Equipment_Item_Resistance* ResistanceWidget = Cast<UW_Equipment_Item_Resistance>(GetWidgetFromName(TEXT("W_Equipment_Item_Resistance")));

		// ════════════════════════════════════════════════════════════════════════════════
		// AAA PATTERN: Build maps from item's StatChanges
		// The child widgets (DamageNegation, Resistance) define their own stat type arrays
		// and iterate over them, looking up values from these maps (default 0 if not found)
		// ════════════════════════════════════════════════════════════════════════════════
		TMap<FGameplayTag, int32> TargetDamageNegationStats;
		TMap<FGameplayTag, int32> TargetResistanceStats;
		int32 ItemWeight = 0;

		// Populate maps from item's StatChanges
		for (const auto& StatPair : EquipInfo.StatChanges)
		{
			FString TagString = StatPair.Key.ToString();
			int32 Value = static_cast<int32>(StatPair.Value.Delta);

			if (TagString.Contains(TEXT("Defense.Negation")))
			{
				TargetDamageNegationStats.Add(StatPair.Key, Value);
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Armor DamageNegation stat: %s = %d"), *TagString, Value);
			}
			else if (TagString.Contains(TEXT("Stat.Resistance")))
			{
				TargetResistanceStats.Add(StatPair.Key, Value);
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Armor Resistance stat: %s = %d"), *TagString, Value);
			}
			else if (TagString.Contains(TEXT("Misc.Weight")))
			{
				ItemWeight = Value;
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Armor Weight: %d"), Value);
			}
		}

		// TODO: Get currently equipped armor stats for comparison
		TMap<FGameplayTag, int32> CurrentDamageNegationStats;
		TMap<FGameplayTag, int32> CurrentResistanceStats;

		// W_Equipment_Item_DamageNegation - Setup damage negation stats
		if (DamageNegationWidget)
		{
			DamageNegationWidget->SetVisibility(ESlateVisibility::Visible);
			DamageNegationWidget->EventSetupDamageNegationStats(TargetDamageNegationStats, CurrentDamageNegationStats, CompareStats);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Called EventSetupDamageNegationStats with %d stats"), TargetDamageNegationStats.Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] W_Equipment_Item_DamageNegation widget not found!"));
		}

		// W_Equipment_Item_Resistance - Setup resistance stats
		if (ResistanceWidget)
		{
			ResistanceWidget->SetVisibility(ESlateVisibility::Visible);
			ResistanceWidget->EventSetupResistanceEntries(TargetResistanceStats, CurrentResistanceStats, CompareStats);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Called EventSetupResistanceEntries with %d stats"), TargetResistanceStats.Num());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] W_Equipment_Item_Resistance widget not found!"));
		}

		// Weight display - ArmorWeightBox with ArmorWeightText
		if (UWidget* ArmorWeightBox = GetWidgetFromName(TEXT("ArmorWeightBox")))
		{
			ArmorWeightBox->SetVisibility(ESlateVisibility::Visible);

			// Set the weight value text
			if (UTextBlock* WeightValueText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ArmorWeightText"))))
			{
				WeightValueText->SetText(FText::AsNumber(ItemWeight));
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set ArmorWeightText to: %d"), ItemWeight);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] ArmorWeightText widget not found!"));
			}
		}
	}
	// ════════════════════════════════════════════════════════════════════════
	// OTHER ITEM TYPES - Hide all stat widgets
	// ════════════════════════════════════════════════════════════════════════
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Other item category - hiding all stat widgets"));
		if (AttackPowerWidget) AttackPowerWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (RequiredStatsWidget) RequiredStatsWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (StatScalingWidget) StatScalingWidget->SetVisibility(ESlateVisibility::Collapsed);
		if (StatsGrantedWidget) StatsGrantedWidget->SetVisibility(ESlateVisibility::Collapsed);

		// Also hide armor widgets for non-armor items
		if (UW_Equipment_Item_DamageNegation* DamageNegationWidget = Cast<UW_Equipment_Item_DamageNegation>(GetWidgetFromName(TEXT("W_Equipment_Item_DamageNegation"))))
		{
			DamageNegationWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UW_Equipment_Item_Resistance* ResistanceWidget = Cast<UW_Equipment_Item_Resistance>(GetWidgetFromName(TEXT("W_Equipment_Item_Resistance"))))
		{
			ResistanceWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (UWidget* ArmorWeightBox = GetWidgetFromName(TEXT("ArmorWeightBox")))
		{
			ArmorWeightBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// ════════════════════════════════════════════════════════════════════════
	// ABILITY SECTION - Set ability icon, name, and cost from WeaponAbility
	// bp_only: If weapon has a WeaponAbility, display its icon and info
	// IMPORTANT: Only show for weapon types (not armor!)
	// ════════════════════════════════════════════════════════════════════════

	// Get ability overlay widget to control visibility
	UWidget* AbilityOverlay = GetWidgetFromName(TEXT("AbilityImageOverlay"));

	// Only show ability section for weapon types that have an ability
	if (bIsWeaponType && EquipInfo.WeaponAbility)
	{
		// Cast to UPDA_WeaponAbility to access Icon, AbilityName, Cost
		UPDA_WeaponAbility* WeaponAbility = Cast<UPDA_WeaponAbility>(EquipInfo.WeaponAbility);
		if (WeaponAbility)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Setting up ability section - Name: %s"), *WeaponAbility->AbilityName.ToString());

			// Set AbilityIcon
			if (UImage* AbilityIcon = Cast<UImage>(GetWidgetFromName(TEXT("AbilityIcon"))))
			{
				if (!WeaponAbility->Icon.IsNull())
				{
					AbilityIcon->SetBrushFromSoftTexture(WeaponAbility->Icon, false);
					AbilityIcon->SetVisibility(ESlateVisibility::Visible);
					UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Set AbilityIcon from: %s"), *WeaponAbility->Icon.ToString());
				}
			}

			// Set AbilityNameText
			if (UTextBlock* AbilityNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AbilityNameText"))))
			{
				AbilityNameText->SetText(WeaponAbility->AbilityName);
			}

			// Set AbilityCostText
			if (UTextBlock* AbilityCostText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AbilityCostText"))))
			{
				AbilityCostText->SetText(FText::AsNumber(WeaponAbility->Cost));
			}

			// Show the ability overlay
			if (AbilityOverlay)
			{
				AbilityOverlay->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] WeaponAbility failed to cast to UPDA_WeaponAbility"));
			if (AbilityOverlay)
			{
				AbilityOverlay->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	else
	{
		// Not a weapon or no ability - hide the ability overlay
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Not a weapon or no WeaponAbility, hiding ability section"));
		if (AbilityOverlay)
		{
			AbilityOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// NOTE: Don't call EventToggleItemInfo here - that controls VISIBILITY of the info panel,
	// which is managed separately. This function just populates the content.
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
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipItemAtSlot - Item is not UPDA_Item"));
		return;
	}

	// Call EquipmentManager to actually equip the item
	if (EquipmentComponent)
	{
		// Check stat requirements first
		bool bCanEquip = true;
		bool bCanEquip2 = true;
		EquipmentComponent->CheckRequiredStats(ItemToEquip->ItemInformation, bCanEquip, bCanEquip2);

		if (!bCanEquip)
		{
			EventShowError(FText::FromString(TEXT("Stats requirements not met")));
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipItemAtSlot - Stats requirements not met"));
			return;
		}

		// Call EquipmentManager to equip
		bool bSuccess = false;
		bool bSuccess2 = false;
		bool bSuccess3 = false;
		bool bSuccess4 = false;
		bool bSuccess5 = false;
		bool bSuccess6 = false;
		bool bSuccess7 = false;

		EquipmentComponent->EquipItemToSlot(
			ItemToEquip,
			ActiveEquipmentSlot->EquipmentSlot,
			true,  // ChangeStats
			bSuccess, bSuccess2, bSuccess3, bSuccess4, bSuccess5, bSuccess6, bSuccess7
		);

		if (!bSuccess)
		{
			EventShowError(FText::FromString(TEXT("Failed to equip item")));
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipItemAtSlot - EquipItemToSlot failed"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EquipItemAtSlot - EquipmentManager::EquipItemToSlot succeeded"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EquipItemAtSlot - No EquipmentComponent!"));
	}

	// Update the visual slot
	ActiveEquipmentSlot->EventOccupyEquipmentSlot(ItemToEquip);

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Equipped item to slot"));
}

void UW_Equipment::UnequipItemAtSlot_Implementation(const FGameplayTag& SlotTag)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UnequipItemAtSlot - Tag: %s"), *SlotTag.ToString());

	UW_EquipmentSlot* TargetSlot = GetEquipmentSlotByTag(SlotTag);
	if (!TargetSlot || !TargetSlot->IsOccupied)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] UnequipItemAtSlot - Slot not found or not occupied"));
		return;
	}

	UPDA_Item* UnequippedItem = Cast<UPDA_Item>(TargetSlot->AssignedItem);

	// Call EquipmentManager to actually unequip the item
	if (EquipmentComponent)
	{
		EquipmentComponent->UnequipItemAtSlot(SlotTag);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] UnequipItemAtSlot - Called EquipmentManager::UnequipItemAtSlot"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] UnequipItemAtSlot - No EquipmentComponent!"));
	}

	// Clear the equipment slot visual
	TargetSlot->EventClearEquipmentSlot();

	// Notify that item was unequipped (for adding back to inventory display)
	if (UnequippedItem)
	{
		EventOnItemUnequippedFromSlot(UnequippedItem, SlotTag);
	}
}

void UW_Equipment::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateUp"));

	if (!CanNavigate())
	{
		return;
	}

	// Check if we're in item selection view (has inventory slots to navigate)
	if (EquipmentInventorySlots.Num() > 0)
	{
		// Navigate through inventory items
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
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateUp - Selected item %d of %d"), ItemNavigationIndex + 1, EquipmentInventorySlots.Num());
		}
		return;
	}

	// Otherwise navigate through equipment slots using grid-based navigation
	if (EquipmentSlots.Num() == 0)
	{
		return;
	}

	// Grid-based navigation: Up decreases row in current column
	int32 NewRow = CurrentGridRow - 1;
	int32 MaxRows = GetMaxRowsInColumn(CurrentGridColumn);

	if (NewRow < 0)
	{
		NewRow = MaxRows - 1;  // Wrap to bottom of column
	}

	SelectSlotAtGridPosition(NewRow, CurrentGridColumn);
}

void UW_Equipment::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDown"));

	if (!CanNavigate())
	{
		return;
	}

	// Check if we're in item selection view (has inventory slots to navigate)
	if (EquipmentInventorySlots.Num() > 0)
	{
		// Navigate through inventory items
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
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDown - Selected item %d of %d"), ItemNavigationIndex + 1, EquipmentInventorySlots.Num());
		}
		return;
	}

	// Otherwise navigate through equipment slots using grid-based navigation
	if (EquipmentSlots.Num() == 0)
	{
		return;
	}

	// Grid-based navigation: Down increases row in current column
	int32 NewRow = CurrentGridRow + 1;
	int32 MaxRows = GetMaxRowsInColumn(CurrentGridColumn);

	if (NewRow >= MaxRows)
	{
		NewRow = 0;  // Wrap to top of column
	}

	SelectSlotAtGridPosition(NewRow, CurrentGridColumn);
}

void UW_Equipment::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateLeft"));

	if (!CanNavigate())
	{
		return;
	}

	// If viewing item list, navigate in items (grid: move left by 1 slot)
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
		return;
	}

	// Grid-based navigation: Left decreases column, clamping row to valid range
	if (EquipmentSlots.Num() == 0)
	{
		return;
	}

	int32 NewColumn = CurrentGridColumn - 1;
	if (NewColumn < 0)
	{
		NewColumn = NumColumns - 1;  // Wrap to rightmost column
	}

	// Clamp row to valid range in new column
	int32 MaxRows = GetMaxRowsInColumn(NewColumn);
	int32 NewRow = FMath::Min(CurrentGridRow, MaxRows - 1);

	SelectSlotAtGridPosition(NewRow, NewColumn);
}

void UW_Equipment::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateRight"));

	if (!CanNavigate())
	{
		return;
	}

	// If viewing item list, navigate in items (grid: move right by 1 slot)
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
		return;
	}

	// Grid-based navigation: Right increases column, clamping row to valid range
	if (EquipmentSlots.Num() == 0)
	{
		return;
	}

	int32 NewColumn = CurrentGridColumn + 1;
	if (NewColumn >= NumColumns)
	{
		NewColumn = 0;  // Wrap to leftmost column
	}

	// Clamp row to valid range in new column
	int32 MaxRows = GetMaxRowsInColumn(NewColumn);
	int32 NewRow = FMath::Min(CurrentGridRow, MaxRows - 1);

	SelectSlotAtGridPosition(NewRow, NewColumn);
}

void UW_Equipment::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateOk"));

	if (!CanNavigate())
	{
		return;
	}

	// If we have an active item slot selected (in item selection view), equip it
	if (ActiveItemSlot && ActiveItemSlot->IsOccupied)
	{
		EquipItemAtSlot(ActiveItemSlot);

		// After equipping, switch back to equipment slots view
		EquipmentInventorySlots.Empty();
		if (UniformEquipmentItemsGrid)
		{
			UniformEquipmentItemsGrid->ClearChildren();
		}
		ActiveItemSlot = nullptr;
		ItemNavigationIndex = 0;

		if (EquipmentSwitcher)
		{
			EquipmentSwitcher->SetActiveWidgetIndex(0);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Switched back to equipment slots view after equipping"));
		}
	}
	// Otherwise, if we have an equipment slot selected (in equipment slots view)
	else if (SelectedSlot)
	{
		// bp_only: Pressing OK always shows item selection (to equip or SWAP items)
		// Whether slot is occupied or empty, we show the item list
		// Unequip is a SEPARATE action via Y button (EventNavigateUnequip)
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Showing item selection for slot (IsOccupied=%s)"),
			SelectedSlot->IsOccupied ? TEXT("true") : TEXT("false"));
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
		if (UniformEquipmentItemsGrid)
		{
			UniformEquipmentItemsGrid->ClearChildren();
		}

		// Switch back to equipment slots view (index 0)
		if (EquipmentSwitcher)
		{
			EquipmentSwitcher->SetActiveWidgetIndex(0);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Switched back to equipment slots view"));
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

	if (!CanNavigate())
	{
		return;
	}

	// bp_only: Check if ErrorBorder is visible - don't toggle if showing an error
	if (ErrorBorder && ErrorBorder->IsVisible())
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDetailedView - ErrorBorder is visible, not toggling"));
		return;
	}

	// Try to find ItemInfoBoxSwitcher if not bound
	if (!ItemInfoBoxSwitcher)
	{
		ItemInfoBoxSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ItemInfoBoxSwitcher")));
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDetailedView - Manual find ItemInfoBoxSwitcher: %s"),
			ItemInfoBoxSwitcher ? TEXT("FOUND") : TEXT("NULL"));
	}

	// bp_only: Toggle ItemInfoBoxSwitcher between index 0 (compact view) and index 1 (detailed view)
	if (ItemInfoBoxSwitcher)
	{
		int32 CurrentIndex = ItemInfoBoxSwitcher->GetActiveWidgetIndex();
		int32 NewIndex = (CurrentIndex == 0) ? 1 : 0;
		int32 NumChildren = ItemInfoBoxSwitcher->GetNumWidgets();

		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDetailedView - ItemInfoBoxSwitcher has %d children, CurrentIndex=%d, NewIndex=%d, IsVisible=%s"),
			NumChildren, CurrentIndex, NewIndex,
			ItemInfoBoxSwitcher->IsVisible() ? TEXT("yes") : TEXT("no"));

		// Log children info
		for (int32 i = 0; i < NumChildren; i++)
		{
			UWidget* Child = ItemInfoBoxSwitcher->GetWidgetAtIndex(i);
			if (Child)
			{
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment]   Child[%d]: %s, Visible: %s"),
					i, *Child->GetName(),
					Child->IsVisible() ? TEXT("yes") : TEXT("no"));
			}
		}

		// Toggle the active widget index
		ItemInfoBoxSwitcher->SetActiveWidgetIndex(NewIndex);

		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventNavigateDetailedView - Toggled ItemInfoBoxSwitcher from %d to %d"), CurrentIndex, NewIndex);

		// If showing detailed view (index 1), populate with current selected item info
		if (NewIndex == 1)
		{
			// Get item from active item slot (in item selection view) or selected equipment slot
			UPDA_Item* ItemToShow = nullptr;

			if (ActiveItemSlot && ActiveItemSlot->IsOccupied)
			{
				ItemToShow = Cast<UPDA_Item>(ActiveItemSlot->AssignedItem);
			}
			else if (SelectedSlot && SelectedSlot->IsOccupied)
			{
				ItemToShow = Cast<UPDA_Item>(SelectedSlot->AssignedItem);
			}

			if (ItemToShow)
			{
				// Populate the detailed view widgets directly (don't call SetupInformationPanel
				// as that also calls EventToggleItemInfo which affects visibility, not index)
				if (UTextBlock* ItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_LongDetails"))))
				{
					ItemNameText->SetText(ItemToShow->ItemInformation.DisplayName);
				}
				if (UTextBlock* DescText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemLongDescription"))))
				{
					DescText->SetText(ItemToShow->ItemInformation.LongDescription);
				}
				if (UImage* LargeImage = Cast<UImage>(GetWidgetFromName(TEXT("DetailsLargeImage"))))
				{
					// Use SetBrushFromSoftTexture (matches Blueprint pattern)
					TSoftObjectPtr<UTexture2D> IconToUse = ItemToShow->ItemInformation.IconLargeOptional.IsNull()
						? ItemToShow->ItemInformation.IconSmall
						: ItemToShow->ItemInformation.IconLargeOptional;
					if (!IconToUse.IsNull())
					{
						LargeImage->SetBrushFromSoftTexture(IconToUse, false);
					}
				}
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Populated detailed view for item: %s"), *ItemToShow->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] No item to show in detailed view (ActiveItemSlot=%s, SelectedSlot=%s)"),
					ActiveItemSlot ? TEXT("valid") : TEXT("null"),
					SelectedSlot ? TEXT("valid") : TEXT("null"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] EventNavigateDetailedView - ItemInfoBoxSwitcher not found!"));
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

	// Load InventorySlotClass if not set
	if (!InventorySlotClass)
	{
		static const TCHAR* DefaultInventorySlotPath = TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot.W_InventorySlot_C");
		InventorySlotClass = LoadClass<UW_InventorySlot>(nullptr, DefaultInventorySlotPath);
		if (InventorySlotClass)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Loaded default InventorySlotClass"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] InventorySlotClass not set and default not found at: %s"), DefaultInventorySlotPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] InventorySlotClass already set: %s"), *InventorySlotClass->GetName());
	}

	ActiveEquipmentSlot = InSlot;

	// Get items that can be equipped in this slot
	TArray<FSLFInventoryItem> AvailableItems;

	if (InventoryComponent)
	{
		// Use UInventoryManagerComponent (returns FSLFInventoryItem directly)
		AvailableItems = InventoryComponent->GetItemsForEquipmentSlot(InSlot->EquipmentSlot);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Using InventoryComponent, got %d items"), AvailableItems.Num());
	}
	else if (AC_InventoryComponent)
	{
		// Use UAC_InventoryManager (returns UPrimaryDataAsset*, need to convert)
		TArray<UPrimaryDataAsset*> RawItems = AC_InventoryComponent->GetItemsForEquipmentSlot(InSlot->EquipmentSlot);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Using AC_InventoryComponent, got %d raw items"), RawItems.Num());

		// Convert to FSLFInventoryItem format
		for (UPrimaryDataAsset* RawItem : RawItems)
		{
			if (UPDA_Item* ItemData = Cast<UPDA_Item>(RawItem))
			{
				FSLFInventoryItem InvItem;
				InvItem.ItemAsset = RawItem;
				InvItem.Amount = 1;  // Default amount (AC_InventoryManager doesn't track amounts in the same way)
				AvailableItems.Add(InvItem);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Equipment] No inventory component available to get items!"));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Got %d available items, UniformEquipmentItemsGrid=%s, InventorySlotClass=%s"),
		AvailableItems.Num(),
		UniformEquipmentItemsGrid ? TEXT("valid") : TEXT("NULL"),
		InventorySlotClass ? TEXT("valid") : TEXT("NULL"));

	// Clear existing item slots
	EquipmentInventorySlots.Empty();
	if (UniformEquipmentItemsGrid)
	{
		UniformEquipmentItemsGrid->ClearChildren();
	}

	// Switch to item selection view (index 1)
	if (EquipmentSwitcher)
	{
		EquipmentSwitcher->SetActiveWidgetIndex(1);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Switched to item selection view"));
	}

	// Populate item grid
	if (UniformEquipmentItemsGrid)
	{
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

						// bp_only: Check if this item is already equipped to the active slot
						// If so, show the equipped indicator (X overlay)
						if (EquipmentComponent && ActiveEquipmentSlot)
						{
							bool bIsEquipped = EquipmentComponent->IsItemEquippedToSlot(Item->ItemInformation, ActiveEquipmentSlot->EquipmentSlot);
							NewSlot->EventToggleEquippedVisual(bIsEquipped);
						}

						// Bind events
						NewSlot->OnSelected.AddDynamic(this, &UW_Equipment::EventOnEquipmentSelected);
						NewSlot->OnPressed.AddDynamic(this, &UW_Equipment::EventOnEquipmentPressed);

						UniformEquipmentItemsGrid->AddChildToUniformGrid(NewSlot, Row, Column);
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

	// Select first item if available and show item info panel
	if (EquipmentInventorySlots.Num() > 0)
	{
		ItemNavigationIndex = 0;
		ActiveItemSlot = EquipmentInventorySlots[0];
		if (ActiveItemSlot)
		{
			ActiveItemSlot->EventOnSelected(true);

			// Show the item info panel (compact view by default)
			EventToggleItemInfo(true);

			// Make sure switcher is at index 0 (compact view) when entering item selection
			if (ItemInfoBoxSwitcher)
			{
				ItemInfoBoxSwitcher->SetActiveWidgetIndex(0);
			}

			// Populate compact view with first item's info
			if (UPDA_Item* FirstItem = Cast<UPDA_Item>(ActiveItemSlot->AssignedItem))
			{
				SetupInformationPanel(FirstItem->ItemInformation, false);
			}
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

		// Update grid position tracking
		if (EquipmentSlotsPositions.IsValidIndex(Index))
		{
			CurrentGridRow = FMath::RoundToInt(EquipmentSlotsPositions[Index].X);
			CurrentGridColumn = FMath::RoundToInt(EquipmentSlotsPositions[Index].Y);
		}

		// Show slot name and update unequip icon
		EventToggleSlotName(InSlot, true);
		UpdateUnequipIconVisibility();
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

		// Update item info panel with newly selected item
		if (InSlot->IsOccupied)
		{
			if (UPDA_Item* SelectedItem = Cast<UPDA_Item>(InSlot->AssignedItem))
			{
				SetupInformationPanel(SelectedItem->ItemInformation, false);
				UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Updated info panel for item: %s"), *SelectedItem->GetName());
			}
		}
	}
}

void UW_Equipment::EventOnItemUnequippedFromSlot_Implementation(UPDA_Item* InItem, const FGameplayTag& TargetSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventOnItemUnequippedFromSlot - Slot: %s"), *TargetSlot.ToString());

	// NOTE: In Soulslike systems, items remain in inventory even when equipped.
	// We do NOT add the item back to inventory here - it was never removed.
	// The unequip just clears the equipment slot, the item is still in inventory.
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
		// CRITICAL: Reset to equipment slots view BEFORE populating
		// This ensures we're showing the correct panel when creating slots
		if (EquipmentSwitcher)
		{
			EquipmentSwitcher->SetActiveWidgetIndex(0);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] Reset to equipment slots view (index 0)"));
		}

		// Clear any leftover item slots from previous session
		EquipmentInventorySlots.Empty();
		ActiveItemSlot = nullptr;
		ItemNavigationIndex = 0;

		// Refresh equipment slots when becoming visible
		PopulateEquipmentSlots();

		// CRITICAL: Bind events to newly created slots (slots were recreated above)
		BindEquipmentSlotEvents();

		// Sync equipment slot visuals with EquipmentManager state
		SyncEquipmentSlotsFromManager();

		// Select first slot
		if (EquipmentSlots.Num() > 0)
		{
			EquipmentSlotNavigationIndex = 0;
			CurrentGridRow = 0;
			CurrentGridColumn = 0;
			SelectedSlot = EquipmentSlots[0];
			if (SelectedSlot)
			{
				SelectedSlot->EventOnSelected(true);
				EventToggleSlotName(SelectedSlot, true);
				UpdateUnequipIconVisibility();
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

	// bp_only: EventToggleItemInfo controls VISIBILITY of the info panel widgets
	// NOT the active index (that's controlled by EventNavigateDetailedView)
	ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

	// Set visibility on ItemInfoBoxSwitcher
	if (ItemInfoBoxSwitcher)
	{
		ItemInfoBoxSwitcher->SetVisibility(NewVisibility);
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] EventToggleItemInfo - Set ItemInfoBoxSwitcher visibility to %s"),
			bVisible ? TEXT("Visible") : TEXT("Collapsed"));
	}

	// Also set visibility on ItemNameText widgets (compact info labels)
	if (UTextBlock* ItemNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
	{
		ItemNameText->SetVisibility(NewVisibility);
	}
	if (UTextBlock* ItemNameTextGrid = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText_Grid"))))
	{
		ItemNameTextGrid->SetVisibility(NewVisibility);
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

	// Update just the specific slot that was equipped, don't recreate all slots
	// (Recreating slots during equip operation would invalidate ActiveEquipmentSlot pointer)
	UW_EquipmentSlot* TargetEquipSlot = GetEquipmentSlotByTag(TargetSlot);
	if (TargetEquipSlot && ItemData.ItemAsset)
	{
		if (UPDA_Item* Item = Cast<UPDA_Item>(ItemData.ItemAsset))
		{
			TargetEquipSlot->EventOccupyEquipmentSlot(Item);
			UE_LOG(LogTemp, Log, TEXT("[W_Equipment] HandleItemEquippedToSlot - Updated slot visual for %s"), *Item->GetName());
		}
	}

	// Update unequip icon visibility if this is the currently selected slot
	if (SelectedSlot && SelectedSlot == TargetEquipSlot)
	{
		UpdateUnequipIconVisibility();
	}
}

void UW_Equipment::HandleItemUnequippedFromSlot(UPrimaryDataAsset* Item, FGameplayTag TargetSlot)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Equipment] HandleItemUnequippedFromSlot - Slot: %s"), *TargetSlot.ToString());

	// Update just the specific slot that was unequipped, don't recreate all slots
	UW_EquipmentSlot* TargetEquipSlot = GetEquipmentSlotByTag(TargetSlot);
	if (TargetEquipSlot)
	{
		TargetEquipSlot->EventClearEquipmentSlot();
		UE_LOG(LogTemp, Log, TEXT("[W_Equipment] HandleItemUnequippedFromSlot - Cleared slot visual"));
	}

	// Update unequip icon visibility if this is the currently selected slot
	if (SelectedSlot && SelectedSlot == TargetEquipSlot)
	{
		UpdateUnequipIconVisibility();
	}
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
