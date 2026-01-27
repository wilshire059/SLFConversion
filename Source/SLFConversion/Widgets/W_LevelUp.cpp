// W_LevelUp.cpp
// C++ Widget implementation for W_LevelUp
//
// 20-PASS VALIDATION: 2026-01-07
// Full implementation with level-up flow

#include "Widgets/W_LevelUp.h"
#include "Widgets/W_StatEntry_LevelUp.h"
#include "Widgets/W_StatBlock_LevelUp.h"
#include "Widgets/W_LevelCurrencyBlock_LevelUp.h"
#include "Blueprints/SLFStatBase.h"
#include "Components/StatManagerComponent.h"  // For UStatManagerComponent
#include "Components/InventoryManagerComponent.h"  // On PlayerController
#include "Components/AC_InventoryManager.h"  // On Pawn (fallback)
#include "Components/AC_ProgressManager.h"
#include "Components/ProgressManagerComponent.h"  // For UProgressManagerComponent on PlayerController
#include "Components/Button.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/BPI_Controller.h"  // For GetCurrency via interface

UW_LevelUp::UW_LevelUp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryComponent(nullptr)
	, ControllerInventoryComponent(nullptr)
	, StatManagerComp(nullptr)
	, CachedPrimaryAttributes(nullptr)
	, CachedSecondaryStats(nullptr)
	, CachedMiscStats(nullptr)
	, CachedAttackStats(nullptr)
	, CachedNegationStats(nullptr)
	, CachedResistanceStats(nullptr)
{
	SetIsFocusable(true);
	NavigationIndex = 0;
	CurrentPlayerCurrency = 0;
	CurrentPlayerLevel = 0;  // Level starts at 0 (matches bp_only)
	bCanAffordLevelUp = false;
}

void UW_LevelUp::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Hidden MainBlur widget"));
	}

	// Cache widget references
	CacheWidgetReferences();

	// CRITICAL: Bind to InventoryComponent's OnCurrencyUpdated to receive real-time currency changes
	// This ensures level-up menu updates when currency changes (e.g., from death currency pickup)
	if (IsValid(ControllerInventoryComponent))
	{
		ControllerInventoryComponent->OnCurrencyUpdated.RemoveAll(this);  // Prevent duplicate bindings
		ControllerInventoryComponent->OnCurrencyUpdated.AddDynamic(this, &UW_LevelUp::OnCurrencyUpdatedFromInventory);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Bound to ControllerInventoryComponent->OnCurrencyUpdated"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] ControllerInventoryComponent is NULL - cannot bind to OnCurrencyUpdated!"));
	}

	// Get current level and currency from components BEFORE initializing entries
	if (IsValid(StatManagerComp))
	{
		CurrentPlayerLevel = StatManagerComp->Level;
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Got level from StatManager: %d"), CurrentPlayerLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] StatManagerComp is NULL - cannot get current level!"));
	}

	// Get currency via BPI_Controller interface (matches bp_only pattern)
	// bp_only: Get Player Controller -> Cast to BPI_Controller -> GetCurrency()
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->Implements<UBPI_Controller>())
	{
		int32 Currency = 0;
		IBPI_Controller::Execute_GetCurrency(PC, Currency);
		CurrentPlayerCurrency = Currency;
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Got currency via BPI_Controller: %d"), CurrentPlayerCurrency);
	}
	else if (IsValid(ControllerInventoryComponent))
	{
		// Fallback: Get currency from controller's inventory component
		CurrentPlayerCurrency = ControllerInventoryComponent->GetCurrentCurrency();
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Fallback: Got currency from ControllerInventoryComponent: %d"), CurrentPlayerCurrency);
	}
	else if (IsValid(InventoryComponent))
	{
		// Fallback to pawn's inventory component
		CurrentPlayerCurrency = InventoryComponent->GetCurrentCurency();
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Fallback: Got currency from Pawn InventoryComponent: %d"), CurrentPlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] No BPI_Controller or InventoryComponent found - cannot get current currency!"));
	}

	// Bind to StatManager's OnStatsInitialized delegate
	// This is needed because child W_StatBlock_LevelUp widgets haven't constructed yet
	// when this NativeConstruct runs (parent constructs before children)
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		if (UStatManagerComponent* StatManager = PlayerPawn->FindComponentByClass<UStatManagerComponent>())
		{
			// Check if stats are already initialized
			if (StatManager->ActiveStats.Num() > 0)
			{
				// Stats ready, but child widgets still need to construct
				// Defer SetAllStatEntries to next tick so children can process first
				UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Stats already initialized (%d), deferring SetAllStatEntries..."),
					StatManager->ActiveStats.Num());

				// Use timer to defer by one frame
				FTimerHandle DeferredHandle;
				GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
				{
					UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Deferred SetAllStatEntries executing..."));
					SetAllStatEntries();
				});
			}
			else
			{
				// Stats not ready yet, bind to OnStatsInitialized
				StatManager->OnStatsInitialized.AddDynamic(this, &UW_LevelUp::OnStatsInitialized);
				UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Bound to OnStatsInitialized, waiting..."));
			}
		}
	}

	// Find the LevelAndCurrencyBlock child widget and initialize it
	if (UW_LevelCurrencyBlock_LevelUp* LevelCurrencyBlock = Cast<UW_LevelCurrencyBlock_LevelUp>(GetWidgetFromName(TEXT("LevelAndCurrencyBlock"))))
	{
		// Bind to OnLevelUpCostChanged to enable/disable buttons based on affordability
		LevelCurrencyBlock->OnLevelUpCostChanged.AddDynamic(this, &UW_LevelUp::OnLevelUpCostChanged);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Bound to OnLevelUpCostChanged"));

		LevelCurrencyBlock->EventInitializeLevelUpCost(CurrentPlayerLevel, CurrentPlayerCurrency);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Initialized LevelAndCurrencyBlock with Level: %d, Currency: %d"),
			CurrentPlayerLevel, CurrentPlayerCurrency);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] LevelAndCurrencyBlock widget not found!"));
	}

	// CRITICAL: Bind Confirm button OnClicked to trigger level-up confirmation
	// This enables mouse click on the Confirm button
	if (UButton* ConfirmBtn = Cast<UButton>(GetWidgetFromName(TEXT("ConfirmButton"))))
	{
		ConfirmBtn->OnClicked.AddDynamic(this, &UW_LevelUp::OnConfirmButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Bound ConfirmButton OnClicked"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] ConfirmButton widget not found!"));
	}

	// Set keyboard/mouse focus to this widget for input
	SetKeyboardFocus();

	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::NativeConstruct - Level: %d, Currency: %d"), CurrentPlayerLevel, CurrentPlayerCurrency);
}

void UW_LevelUp::NativeDestruct()
{
	// Unbind from StatManager events
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		if (UStatManagerComponent* StatManager = PlayerPawn->FindComponentByClass<UStatManagerComponent>())
		{
			StatManager->OnStatsInitialized.RemoveAll(this);
		}
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::NativeDestruct"));
}

FReply UW_LevelUp::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] NativeOnKeyDown - Key: %s"), *Key.ToString());

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

	// Navigate Left (Decrease Stat): A, Left Arrow, Gamepad DPad Left
	if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
	{
		EventNavigateLeft();
		return FReply::Handled();
	}

	// Navigate Right (Increase Stat): D, Right Arrow, Gamepad DPad Right
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

void UW_LevelUp::OnStatsInitialized()
{
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] OnStatsInitialized - calling SetAllStatEntries..."));
	SetAllStatEntries();
}

void UW_LevelUp::CacheWidgetReferences()
{
	// Get player pawn and cache component references
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValid(PlayerPawn))
	{
		// Use UStatManagerComponent (the correct class used by player)
		StatManagerComp = PlayerPawn->FindComponentByClass<UStatManagerComponent>();
		InventoryComponent = PlayerPawn->FindComponentByClass<UAC_InventoryManager>();

		// Get controller's inventory component (where currency is stored)
		if (APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController()))
		{
			ControllerInventoryComponent = PC->FindComponentByClass<UInventoryManagerComponent>();
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Found ControllerInventoryComponent: %s"),
				ControllerInventoryComponent ? TEXT("YES") : TEXT("NO"));
		}

		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] CacheWidgetReferences - StatManagerComp: %s, InventoryComponent: %s, ControllerInventoryComponent: %s"),
			StatManagerComp ? TEXT("found") : TEXT("NULL"),
			InventoryComponent ? TEXT("found") : TEXT("NULL"),
			ControllerInventoryComponent ? TEXT("found") : TEXT("NULL"));
	}

	// Cache StatBlock widget references (from UMG designer hierarchy)
	CachedPrimaryAttributes = Cast<UW_StatBlock_LevelUp>(GetWidgetFromName(TEXT("PrimaryAttributes")));
	CachedSecondaryStats = Cast<UW_StatBlock_LevelUp>(GetWidgetFromName(TEXT("SecondaryStats")));
	CachedMiscStats = Cast<UW_StatBlock_LevelUp>(GetWidgetFromName(TEXT("MiscStats")));
	CachedAttackStats = Cast<UW_StatBlock_LevelUp>(GetWidgetFromName(TEXT("AttackStats")));
	CachedNegationStats = Cast<UW_StatBlock_LevelUp>(GetWidgetFromName(TEXT("NegationStats")));
	CachedResistanceStats = Cast<UW_StatBlock_LevelUp>(GetWidgetFromName(TEXT("ResistanceStats")));

	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] CacheWidgetReferences - PrimaryAttributes: %s, SecondaryStats: %s"),
		CachedPrimaryAttributes ? TEXT("found") : TEXT("null"),
		CachedSecondaryStats ? TEXT("found") : TEXT("null"));

	// CRITICAL: Only PrimaryAttributes (Attribute Points) should have < > buttons
	// All other stat blocks show read-only values that update based on attribute changes
	if (CachedPrimaryAttributes)
	{
		CachedPrimaryAttributes->ShowUpgradeButtons = true;
	}
	if (CachedSecondaryStats)
	{
		CachedSecondaryStats->ShowUpgradeButtons = false;
	}
	if (CachedMiscStats)
	{
		CachedMiscStats->ShowUpgradeButtons = false;
	}
	if (CachedAttackStats)
	{
		CachedAttackStats->ShowUpgradeButtons = false;
	}
	if (CachedNegationStats)
	{
		CachedNegationStats->ShowUpgradeButtons = false;
	}
	if (CachedResistanceStats)
	{
		CachedResistanceStats->ShowUpgradeButtons = false;
	}
}

/**
 * HandleStatChanges - Process stat change preview
 *
 * BP_ONLY SOURCE: W_LevelUp.json lines 27923-30010 (HandleStatChanges graph)
 *
 * Blueprint Logic (EXACT from bp_only):
 * 1. Get StatInfo from StatObject
 * 2. Branch: Check if CurrentValue < MaxValue
 * 3. If TRUE (can increase):
 *    a. Select ValueType: if bShowMaxValue then MaxValue else CurrentValue
 *    b. SelectFloat Change: if Increase then +1.0 else -1.0
 *    c. CRITICAL: Call StatObject->AdjustValue(ValueType, Change, LevelUp=true, TriggerRegen=false)
 *    d. Map_Add to CurrentChanges
 * 4. AdjustValue fires OnStatUpdated delegate which updates widget display
 * 5. AdjustValue also cascades to affected stats (e.g., Vigor->HP) via StatsToAffect
 */
void UW_LevelUp::HandleStatChanges_Implementation(USLFStatBase* StatObject, bool Increase)
{
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] HandleStatChanges - Stat: %s, Increase: %s"),
		StatObject ? *StatObject->GetName() : TEXT("None"), Increase ? TEXT("true") : TEXT("false"));

	if (!IsValid(StatObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp] HandleStatChanges - StatObject is NULL!"));
		return;
	}

	// CURRENCY CHECK: Don't allow increases if player cannot afford level-up cost
	// bp_only: Buttons are disabled via OnLevelUpCostChanged, but we also validate here
	if (Increase && !bCanAffordLevelUp)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] HandleStatChanges - Cannot increase %s: bCanAffordLevelUp=false (insufficient currency)"),
			*StatObject->GetName());
		return;
	}

	// Get stat info for validation
	const FStatInfo& Info = StatObject->StatInfo;

	// CRITICAL: Check if can increase (bp_only pattern - Branch node)
	// bp_only: Break FStatInfo -> CurrentValue < MaxValue -> Branch
	if (Increase && Info.CurrentValue >= Info.MaxValue)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] HandleStatChanges - Cannot increase %s: CurrentValue (%.0f) >= MaxValue (%.0f)"),
			*StatObject->GetName(), Info.CurrentValue, Info.MaxValue);
		return;
	}

	// DECREASE CHECK: Can only decrease a stat that was previously increased this session
	// bp_only: You cannot decrease a stat below its original value when the menu opened
	if (!Increase)
	{
		FLevelChangeData* ExistingChange = CurrentChanges.Find(StatObject);
		if (!ExistingChange || ExistingChange->Change <= 0.0)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] HandleStatChanges - Cannot decrease %s: No pending increase to revert (ExistingChange=%s, Change=%.1f)"),
				*StatObject->GetName(),
				ExistingChange ? TEXT("found") : TEXT("not found"),
				ExistingChange ? ExistingChange->Change : 0.0);
			return;
		}
	}

	// ONE-ATTRIBUTE-PER-LEVEL LIMIT CHECK
	// bp_only: Can only increase ONE primary attribute per level-up
	// Calculate total pending changes across all stats
	int32 TotalPendingChanges = 0;
	for (const auto& Entry : CurrentChanges)
	{
		TotalPendingChanges += FMath::Abs(static_cast<int32>(Entry.Value.Change));
	}

	// If trying to increase but already have 1 point allocated, block it
	if (Increase && TotalPendingChanges >= 1)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] HandleStatChanges - Cannot increase %s: Already have %d attribute point(s) allocated (limit 1 per level)"),
			*StatObject->GetName(), TotalPendingChanges);
		return;
	}

	// DETERMINE VALUE TYPE (bp_only Select node)
	// bp_only: Select(Index=bShowMaxValue, Option0=CurrentValue, Option1=MaxValue)
	ESLFValueType ValueType = Info.bShowMaxValue ? ESLFValueType::MaxValue : ESLFValueType::CurrentValue;
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   ValueType: %s (bShowMaxValue=%s)"),
		ValueType == ESLFValueType::MaxValue ? TEXT("MaxValue") : TEXT("CurrentValue"),
		Info.bShowMaxValue ? TEXT("true") : TEXT("false"));

	// DETERMINE CHANGE AMOUNT (bp_only SelectFloat node)
	// bp_only: SelectFloat(Index=Increase, A=+1.0, B=-1.0)
	double Change = Increase ? 1.0 : -1.0;
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Change: %.1f"), Change);

	// CRITICAL: CALL ADJUSTVALUE ON THE STAT OBJECT
	// bp_only: StatObject->AdjustValue(ValueType, Change, LevelUp=true, TriggerRegen=false)
	// This is THE KEY that makes the system work:
	// 1. Updates the stat's value
	// 2. Fires OnStatUpdated delegate
	// 3. Widget's OnStatValueUpdatedHandler receives it
	// 4. Widget updates its display
	StatObject->AdjustValue(ValueType, Change, true, false);
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Called AdjustValue(ValueType=%d, Change=%.1f, LevelUp=true, TriggerRegen=false)"),
		static_cast<int32>(ValueType), Change);

	// CRITICAL FIX: Cascade to affected stats (e.g., Vigor -> HP)
	// bp_only pattern: After adjusting the primary stat, AdjustAffected is called to update derived stats
	// StatManagerComponent::AdjustAffectedStats reads StatModifiers.StatsToAffect and updates those stats
	if (IsValid(StatManagerComp))
	{
		StatManagerComp->AdjustAffectedStats(StatObject, Change, ValueType);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Called AdjustAffectedStats to cascade changes to derived stats"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_LevelUp]   StatManagerComp is NULL - cannot cascade to affected stats!"));
	}

	// ADD TO CURRENTCHANGES MAP (bp_only Map_Add node)
	FLevelChangeData* ExistingChange = CurrentChanges.Find(StatObject);
	if (ExistingChange)
	{
		// Update existing change
		ExistingChange->Change += Change;

		// Remove if change is back to 0
		if (FMath::IsNearlyZero(ExistingChange->Change))
		{
			CurrentChanges.Remove(StatObject);
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Removed from CurrentChanges (change is 0)"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Updated CurrentChanges: Change=%.1f"), ExistingChange->Change);
		}
	}
	else if (!FMath::IsNearlyZero(Change))
	{
		// Add new change entry
		FLevelChangeData NewChange;
		NewChange.Change = Change;
		NewChange.ValueType = ValueType;
		CurrentChanges.Add(StatObject, NewChange);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Added to CurrentChanges: Change=%.1f, ValueType=%d"),
			Change, static_cast<int32>(ValueType));
	}

	// NOTE: We don't call EventUpdateStatChanges here because AdjustValue already
	// fired OnStatUpdated which the widget bound to via OnStatValueUpdatedHandler
	// The widget will update itself automatically
}

/**
 * SetAllStatEntries - Initialize all stat entry widgets with current values
 *
 * Blueprint Logic (from bp_only):
 * 1. Clear AllStatEntries
 * 2. For each StatBlock (PrimaryAttributes, SecondaryStats, etc.)
 * 3. Call GetAllStatsInBlock() to get stat entry widgets
 * 4. Append to AllStatEntries
 * 5. For each stat entry, store current value in DefaultValues
 */
void UW_LevelUp::SetAllStatEntries_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::SetAllStatEntries"));

	// Clear existing values
	AllStatEntries.Empty();
	DefaultValues.Empty();
	CurrentChanges.Empty();

	// Helper to collect stats from a block
	auto CollectFromBlock = [this](UW_StatBlock_LevelUp* Block, const TCHAR* BlockName)
	{
		if (IsValid(Block))
		{
			TArray<UW_StatEntry_LevelUp*> BlockStats = Block->GetAllStatsInBlock();
			UE_LOG(LogTemp, Log, TEXT("  %s: %d entries"), BlockName, BlockStats.Num());
			AllStatEntries.Append(BlockStats);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  %s: null"), BlockName);
		}
	};

	// Collect from all stat blocks (order matches Blueprint)
	CollectFromBlock(CachedPrimaryAttributes, TEXT("PrimaryAttributes"));
	CollectFromBlock(CachedSecondaryStats, TEXT("SecondaryStats"));
	CollectFromBlock(CachedMiscStats, TEXT("MiscStats"));
	CollectFromBlock(CachedAttackStats, TEXT("AttackStats"));
	CollectFromBlock(CachedNegationStats, TEXT("NegationStats"));
	CollectFromBlock(CachedResistanceStats, TEXT("ResistanceStats"));

	// CRITICAL FIX: Reinitialize each entry to reset OldValue, text color, and highlights
	// This is needed because entries are cached widgets that may have stale state from previous level-ups
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Reinitializing %d stat entries..."), AllStatEntries.Num());
	for (UW_StatEntry_LevelUp* StatEntry : AllStatEntries)
	{
		if (IsValid(StatEntry) && IsValid(StatEntry->Stat))
		{
			// Get current stat value to set as OldValue (baseline for this session)
			double CurrentValue = StatEntry->Stat->StatInfo.bShowMaxValue
				? StatEntry->Stat->StatInfo.MaxValue
				: StatEntry->Stat->StatInfo.CurrentValue;

			// EventSetInitialValue sets OldValue AND calls InitStatEntry which resets colors/highlights
			StatEntry->EventSetInitialValue(CurrentValue);
		}
	}

	// Store default values and bind delegates for each stat entry (matches bp_only pattern)
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Binding delegates for %d stat entries..."), AllStatEntries.Num());
	for (UW_StatEntry_LevelUp* StatEntry : AllStatEntries)
	{
		if (IsValid(StatEntry) && IsValid(StatEntry->Stat))
		{
			FLevelChangeData DefaultChange;
			DefaultChange.Change = 0.0;
			DefaultChange.ValueType = ESLFValueType::MaxValue;
			DefaultValues.Add(StatEntry->Stat, DefaultChange);

			// Bind OnStatEntrySelected delegate to receive affected stats when entry is selected
			// This enables the highlighting of affected stats (e.g., selecting Vigor highlights HP)
			// Note: Dynamic delegates require value types, so we use a handler that forwards to the implementation
			StatEntry->OnStatEntrySelected.RemoveAll(this);  // Prevent duplicate bindings
			StatEntry->OnStatEntrySelected.AddDynamic(this, &UW_LevelUp::OnStatEntrySelectedHandler);

			// Bind OnStatChangeRequest delegate to handle < > button clicks
			StatEntry->OnStatChangeRequest.RemoveAll(this);  // Prevent duplicate bindings
			StatEntry->OnStatChangeRequest.AddDynamic(this, &UW_LevelUp::EventOnStatChangeRequested);

			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Bound delegates for: %s (Tag: %s)"),
				*StatEntry->Stat->GetName(), *StatEntry->Stat->StatInfo.Tag.ToString());
		}
	}

	// Reset navigation index and select first entry
	NavigationIndex = 0;
	if (AllStatEntries.Num() > 0 && IsValid(AllStatEntries[0]))
	{
		AllStatEntries[0]->SetLevelUpStatEntrySelected(true);
	}

	UE_LOG(LogTemp, Log, TEXT("  Total initialized: %d stat entries, delegates bound"), AllStatEntries.Num());

	// CRITICAL FIX: After populating AllStatEntries, apply current affordability state
	// This fixes the timing issue where OnLevelUpCostChanged fires BEFORE SetAllStatEntries
	// bp_only pattern: All stat entries must have their buttons enabled/disabled based on currency
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Applying bCanAffordLevelUp=%s to all %d stat entries"),
		bCanAffordLevelUp ? TEXT("true") : TEXT("false"), AllStatEntries.Num());

	for (UW_StatEntry_LevelUp* Entry : AllStatEntries)
	{
		if (IsValid(Entry))
		{
			Entry->EventToggleLevelUpEnabled(bCanAffordLevelUp);
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Applied EventToggleLevelUpEnabled(%s) to %s"),
				bCanAffordLevelUp ? TEXT("true") : TEXT("false"),
				Entry->Stat ? *Entry->Stat->GetName() : TEXT("None"));
		}
	}
}

/**
 * EventApplyNewStats - Finalize stat changes and refresh display
 *
 * FIX: bp_only pattern - HandleStatChanges already applies changes via AdjustValue
 * This function should NOT re-apply changes (that would be double-applying)
 * It only needs to:
 * 1. Clear CurrentChanges map (changes are already applied)
 * 2. Refresh stat entry displays with new base values
 */
void UW_LevelUp::EventApplyNewStats_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventApplyNewStats - %d changes were applied via HandleStatChanges"),
		CurrentChanges.Num());

	// FIX: Changes were ALREADY applied in HandleStatChanges via StatObject->AdjustValue
	// Do NOT re-apply them here or stats will be modified twice!

	// Log what was applied (for debugging)
	for (const auto& Entry : CurrentChanges)
	{
		USLFStatBase* StatObject = Entry.Key;
		const FLevelChangeData& ChangeData = Entry.Value;

		if (IsValid(StatObject))
		{
			UE_LOG(LogTemp, Log, TEXT("  Confirmed change: %.1f to %s"),
				ChangeData.Change, *StatObject->StatInfo.Tag.ToString());
		}
	}

	// Clear the changes map (changes are now permanent)
	CurrentChanges.Empty();

	// Refresh stat entries with new base values (OldValue becomes the new actual value)
	SetAllStatEntries();

	UE_LOG(LogTemp, Log, TEXT("  Level-up finalized successfully"));
}

/**
 * EventConfirmLevelUp - Confirm level up and apply changes
 *
 * bp_only Logic (EXACT):
 * 1. Get ConfirmButton -> GetIsEnabled -> Branch
 * 2. If enabled (TRUE):
 *    - Get currency from BPI_Controller.GetCurrency
 *    - Get required cost from AC_ProgressManager
 *    - Check CurrentCurrency >= RequiredCurrency
 *    - If can afford: AdjustCurrency(-Required), AdjustLevel(+1), ApplyNewStats
 */
void UW_LevelUp::EventConfirmLevelUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventConfirmLevelUp"));

	// bp_only: Get ConfirmButton -> GetIsEnabled -> Branch
	// Only proceed if the button is enabled (player can afford and has pending changes)
	if (UButton* ConfirmBtn = Cast<UButton>(GetWidgetFromName(TEXT("ConfirmButton"))))
	{
		if (!ConfirmBtn->GetIsEnabled())
		{
			UE_LOG(LogTemp, Log, TEXT("  ConfirmButton is disabled - cannot confirm level-up"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  ConfirmButton not found - checking bCanAffordLevelUp instead"));
		if (!bCanAffordLevelUp)
		{
			UE_LOG(LogTemp, Log, TEXT("  bCanAffordLevelUp is false - cannot confirm level-up"));
			return;
		}
	}

	// FIX: Only require StatManagerComp, not InventoryComponent
	// Currency can be retrieved via BPI_Controller interface or ControllerInventoryComponent
	// The Pawn's InventoryComponent (UAC_InventoryManager) may be NULL
	if (!IsValid(StatManagerComp))
	{
		UE_LOG(LogTemp, Warning, TEXT("  StatManagerComp not valid"));
		return;
	}

	// FIX: ProgressManager is on the PlayerController, not the Pawn
	// And the correct class is UProgressManagerComponent, not UAC_ProgressManager
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("  PlayerController not valid"));
		return;
	}

	UProgressManagerComponent* ProgressManager = PC->FindComponentByClass<UProgressManagerComponent>();
	if (!IsValid(ProgressManager))
	{
		UE_LOG(LogTemp, Warning, TEXT("  ProgressManager not found on PlayerController"));
		return;
	}

	// Get current currency via BPI_Controller interface (matches bp_only pattern)
	int32 CurrentCurrency = 0;
	bool bUsedBPIController = false;

	// PC is already retrieved above
	if (PC && PC->Implements<UBPI_Controller>())
	{
		IBPI_Controller::Execute_GetCurrency(PC, CurrentCurrency);
		bUsedBPIController = true;
	}
	else if (IsValid(ControllerInventoryComponent))
	{
		CurrentCurrency = ControllerInventoryComponent->GetCurrentCurrency();
	}
	else if (IsValid(InventoryComponent))
	{
		CurrentCurrency = InventoryComponent->GetCurrentCurency();
	}

	// Get required currency for next level
	int32 RequiredCurrency = ProgressManager->GetRequiredCurrencyForLevel(CurrentPlayerLevel + 1);

	UE_LOG(LogTemp, Log, TEXT("  Current currency: %d, Required: %d"), CurrentCurrency, RequiredCurrency);

	// Check if player has enough currency
	if (CurrentCurrency >= RequiredCurrency)
	{
		// Deduct currency via BPI_Controller interface (matches bp_only pattern)
		if (bUsedBPIController && PC)
		{
			IBPI_Controller::Execute_AdjustCurrency(PC, -RequiredCurrency);
		}
		else if (IsValid(ControllerInventoryComponent))
		{
			ControllerInventoryComponent->AdjustCurrency(-RequiredCurrency);
		}
		else if (IsValid(InventoryComponent))
		{
			InventoryComponent->AdjustCurrency(-RequiredCurrency);
		}

		// Increment level
		StatManagerComp->AdjustLevel(1);

		// Apply stat changes
		EventApplyNewStats();

		// Update local cached level
		CurrentPlayerLevel = StatManagerComp->Level;

		UE_LOG(LogTemp, Log, TEXT("  Level up successful! New level: %d"), CurrentPlayerLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Not enough currency for level up"));
	}
}

/**
 * EventDisregardPreview - Clear all preview changes
 *
 * Blueprint Logic:
 * 1. Clear CurrentChanges map
 * 2. Restore all stat entries to DefaultValues
 * 3. Broadcast OnPreviewDisregarded
 */
void UW_LevelUp::EventDisregardPreview_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventDisregardPreview"));

	// Clear all pending changes
	CurrentChanges.Empty();

	// Broadcast that preview was discarded
	OnPreviewDisregarded.Broadcast();

	// Update display to show default values
	EventUpdateStatChanges();

	UE_LOG(LogTemp, Log, TEXT("  Preview changes discarded"));
}

/**
 * EventExit - Exit the level up menu
 */
void UW_LevelUp::EventExit_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventExit"));

	// Discard any pending changes
	EventDisregardPreview();

	// Broadcast menu closed
	OnLevelUpMenuClosed.Broadcast();

	// Hide this widget
	SetVisibility(ESlateVisibility::Collapsed);
}

/**
 * EventNavigateCancel - Handle cancel navigation input
 */
void UW_LevelUp::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventNavigateCancel"));

	// Check if there are pending changes
	if (CurrentChanges.Num() > 0)
	{
		// Discard changes
		EventDisregardPreview();
	}
	else
	{
		// Exit menu
		EventExit();
	}
}

/**
 * EventNavigateDown - Navigate to next stat entry
 *
 * Blueprint Logic:
 * 1. Deselect current entry
 * 2. Increment NavigationIndex (wrap around)
 * 3. Select new entry
 */
void UW_LevelUp::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventNavigateDown"));

	if (AllStatEntries.Num() == 0)
	{
		return;
	}

	// Deselect current entry
	if (AllStatEntries.IsValidIndex(NavigationIndex))
	{
		UW_StatEntry_LevelUp* CurrentEntry = AllStatEntries[NavigationIndex];
		if (IsValid(CurrentEntry))
		{
			CurrentEntry->SetLevelUpStatEntrySelected(false);
		}
	}

	// Move to next entry
	NavigationIndex = (NavigationIndex + 1) % AllStatEntries.Num();

	// Select new entry
	if (AllStatEntries.IsValidIndex(NavigationIndex))
	{
		UW_StatEntry_LevelUp* NewEntry = AllStatEntries[NavigationIndex];
		if (IsValid(NewEntry))
		{
			NewEntry->SetLevelUpStatEntrySelected(true);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Navigation index: %d"), NavigationIndex);
}

/**
 * EventNavigateUp - Navigate to previous stat entry
 *
 * Blueprint Logic:
 * 1. Deselect current entry
 * 2. Decrement NavigationIndex (wrap around)
 * 3. Select new entry
 */
void UW_LevelUp::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventNavigateUp"));

	if (AllStatEntries.Num() == 0)
	{
		return;
	}

	// Deselect current entry
	if (AllStatEntries.IsValidIndex(NavigationIndex))
	{
		UW_StatEntry_LevelUp* CurrentEntry = AllStatEntries[NavigationIndex];
		if (IsValid(CurrentEntry))
		{
			CurrentEntry->SetLevelUpStatEntrySelected(false);
		}
	}

	// Move to previous entry
	NavigationIndex = (NavigationIndex - 1 + AllStatEntries.Num()) % AllStatEntries.Num();

	// Select new entry
	if (AllStatEntries.IsValidIndex(NavigationIndex))
	{
		UW_StatEntry_LevelUp* NewEntry = AllStatEntries[NavigationIndex];
		if (IsValid(NewEntry))
		{
			NewEntry->SetLevelUpStatEntrySelected(true);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Navigation index: %d"), NavigationIndex);
}

/**
 * EventNavigateLeft - Decrease stat value
 *
 * Blueprint Logic:
 * 1. Get currently selected stat entry by NavigationIndex
 * 2. Call HandleStatChanges with Increase=false to decrease the stat
 */
void UW_LevelUp::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventNavigateLeft"));

	// Get currently selected stat entry
	if (AllStatEntries.IsValidIndex(NavigationIndex))
	{
		UW_StatEntry_LevelUp* SelectedEntry = AllStatEntries[NavigationIndex];
		if (IsValid(SelectedEntry) && IsValid(SelectedEntry->Stat))
		{
			// Request stat decrease via HandleStatChanges
			HandleStatChanges(SelectedEntry->Stat, false);
			UE_LOG(LogTemp, Log, TEXT("  Decreased stat for entry %d: %s"), NavigationIndex, *SelectedEntry->Stat->GetName());
		}
	}
}

/**
 * EventNavigateRight - Increase stat value
 *
 * Blueprint Logic:
 * 1. Get currently selected stat entry by NavigationIndex
 * 2. Call HandleStatChanges with Increase=true to increase the stat
 */
void UW_LevelUp::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventNavigateRight"));

	// Get currently selected stat entry
	if (AllStatEntries.IsValidIndex(NavigationIndex))
	{
		UW_StatEntry_LevelUp* SelectedEntry = AllStatEntries[NavigationIndex];
		if (IsValid(SelectedEntry) && IsValid(SelectedEntry->Stat))
		{
			// Request stat increase via HandleStatChanges
			HandleStatChanges(SelectedEntry->Stat, true);
			UE_LOG(LogTemp, Log, TEXT("  Increased stat for entry %d: %s"), NavigationIndex, *SelectedEntry->Stat->GetName());
		}
	}
}

/**
 * EventNavigateOk - Confirm current selection/action
 */
void UW_LevelUp::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventNavigateOk"));

	// If there are pending changes, confirm level up
	if (CurrentChanges.Num() > 0)
	{
		EventConfirmLevelUp();
	}
}

/**
 * EventOnBackRequested - Handle back request
 */
void UW_LevelUp::EventOnBackRequested_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventOnBackRequested"));

	// Same as cancel
	EventNavigateCancel();
}

/**
 * EventOnPlayerCurrencyUpdated - Handle currency change notification
 */
void UW_LevelUp::EventOnPlayerCurrencyUpdated_Implementation(int32 NewCurrency)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventOnPlayerCurrencyUpdated - NewCurrency: %d"), NewCurrency);

	CurrentPlayerCurrency = NewCurrency;

	// Forward to LevelAndCurrencyBlock to update currency display
	if (UW_LevelCurrencyBlock_LevelUp* LevelCurrencyBlock = Cast<UW_LevelCurrencyBlock_LevelUp>(GetWidgetFromName(TEXT("LevelAndCurrencyBlock"))))
	{
		LevelCurrencyBlock->EventOnCurrencyUpdated(NewCurrency);
	}
}

/**
 * EventOnPlayerLevelUpdated - Handle level change notification
 */
void UW_LevelUp::EventOnPlayerLevelUpdated_Implementation(int32 NewLevel)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventOnPlayerLevelUpdated - NewLevel: %d"), NewLevel);

	CurrentPlayerLevel = NewLevel;

	// Forward to LevelAndCurrencyBlock to update level display
	if (UW_LevelCurrencyBlock_LevelUp* LevelCurrencyBlock = Cast<UW_LevelCurrencyBlock_LevelUp>(GetWidgetFromName(TEXT("LevelAndCurrencyBlock"))))
	{
		LevelCurrencyBlock->EventOnLevelUpdated(NewLevel);
	}
}

/**
 * EventOnStatChangeRequested - Handle stat change request from entry widget
 */
void UW_LevelUp::EventOnStatChangeRequested_Implementation(USLFStatBase* StatObject, bool Increase)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventOnStatChangeRequested - Stat: %s, Increase: %s"),
		StatObject ? *StatObject->GetName() : TEXT("None"), Increase ? TEXT("true") : TEXT("false"));

	// Delegate to HandleStatChanges
	HandleStatChanges(StatObject, Increase);
}

/**
 * EventOnStatEntrySelected - Handle stat entry selection
 *
 * Blueprint Logic (from bp_only):
 * When a stat entry is selected, its AffectedStats (from StatModifiers.StatsToAffect keys)
 * are broadcast. We search through all stat entries to find ones matching these tags
 * and highlight/unhighlight them accordingly.
 *
 * Example: Selecting Vigor broadcasts [HP tag] as AffectedStats
 *          We find the HP stat entry and call EventHighlight() on it
 */
void UW_LevelUp::EventOnStatEntrySelected_Implementation(const TArray<FGameplayTag>& AffectedStats, bool Selected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventOnStatEntrySelected - %d affected stats, Selected: %s"),
		AffectedStats.Num(), Selected ? TEXT("true") : TEXT("false"));

	// For each affected stat tag, find the matching stat entry and highlight/unhighlight it
	for (const FGameplayTag& AffectedTag : AffectedStats)
	{
		UE_LOG(LogTemp, Log, TEXT("  Looking for affected stat: %s"), *AffectedTag.ToString());

		// Search all stat entries for one with matching tag
		for (UW_StatEntry_LevelUp* Entry : AllStatEntries)
		{
			if (!IsValid(Entry) || !IsValid(Entry->Stat))
			{
				continue;
			}

			// Get this entry's stat tag
			FGameplayTag EntryTag = Entry->Stat->StatInfo.Tag;

			// Check if this entry's tag matches the affected tag
			// Use MatchesTagExact for precise matching (not hierarchical)
			if (EntryTag.MatchesTagExact(AffectedTag))
			{
				if (Selected)
				{
					// Highlight this entry to show it will be affected
					Entry->EventHighlight();
					UE_LOG(LogTemp, Log, TEXT("    Highlighted entry: %s"), *EntryTag.ToString());
				}
				else
				{
					// Remove highlight when selection is cleared
					Entry->EventRemoveHighlight();
					UE_LOG(LogTemp, Log, TEXT("    Removed highlight from: %s"), *EntryTag.ToString());
				}
				break;  // Found the matching entry, no need to continue searching
			}
		}
	}
}

/**
 * EventOnVisibilityChanged - Handle visibility change
 */
void UW_LevelUp::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventOnVisibilityChanged - Visibility: %d"), InVisibility);

	ESlateVisibility NewVisibility = static_cast<ESlateVisibility>(InVisibility);

	if (NewVisibility == ESlateVisibility::Visible || NewVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		// Refresh stat entries when becoming visible
		SetAllStatEntries();

		// Get current currency via BPI_Controller interface (matches bp_only pattern)
		APlayerController* PC = GetOwningPlayer();
		if (PC && PC->Implements<UBPI_Controller>())
		{
			int32 Currency = 0;
			IBPI_Controller::Execute_GetCurrency(PC, Currency);
			CurrentPlayerCurrency = Currency;
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] OnVisibilityChanged: Got currency via BPI_Controller: %d"), CurrentPlayerCurrency);
		}
		else if (IsValid(ControllerInventoryComponent))
		{
			CurrentPlayerCurrency = ControllerInventoryComponent->GetCurrentCurrency();
		}
		else if (IsValid(InventoryComponent))
		{
			CurrentPlayerCurrency = InventoryComponent->GetCurrentCurency();
		}
		if (IsValid(StatManagerComp))
		{
			CurrentPlayerLevel = StatManagerComp->Level;
		}

		// Update LevelAndCurrencyBlock with current values
		if (UW_LevelCurrencyBlock_LevelUp* LevelCurrencyBlock = Cast<UW_LevelCurrencyBlock_LevelUp>(GetWidgetFromName(TEXT("LevelAndCurrencyBlock"))))
		{
			LevelCurrencyBlock->EventInitializeLevelUpCost(CurrentPlayerLevel, CurrentPlayerCurrency);
			UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] Refreshed LevelAndCurrencyBlock - Level: %d, Currency: %d"),
				CurrentPlayerLevel, CurrentPlayerCurrency);
		}

		// Set focus for input
		SetKeyboardFocus();
	}
}

/**
 * EventUpdateStatChanges - Update stat changes display
 *
 * Blueprint Logic:
 * 1. For each stat entry in AllStatEntries
 * 2. Look up if there's a pending change for that stat in CurrentChanges
 * 3. Call EventOnStatUpdated on the entry with the change amount
 * 4. This triggers the entry to refresh its displayed value
 */
void UW_LevelUp::EventUpdateStatChanges_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LevelUp::EventUpdateStatChanges - %d pending changes"), CurrentChanges.Num());

	// Update all stat entry widgets to show current values + pending changes
	for (UW_StatEntry_LevelUp* StatEntry : AllStatEntries)
	{
		if (!IsValid(StatEntry) || !IsValid(StatEntry->Stat))
		{
			continue;
		}

		// Look up pending change for this stat
		const FLevelChangeData* PendingChange = CurrentChanges.Find(StatEntry->Stat);

		if (PendingChange)
		{
			// Call EventOnStatUpdated with the pending change amount
			// This triggers the entry to update its StatValueNew display
			StatEntry->EventOnStatUpdated(StatEntry->Stat, PendingChange->Change, false, static_cast<uint8>(PendingChange->ValueType));
			UE_LOG(LogTemp, Log, TEXT("  Updated entry %s with change: %.1f"), *StatEntry->Stat->GetName(), PendingChange->Change);
		}
		else
		{
			// No pending change - reset to original value
			StatEntry->EventOnStatUpdated(StatEntry->Stat, 0.0, false, 0);
		}
	}
}

/**
 * OnLevelUpCostChanged - Callback when affordability changes
 * Enables/disables stat adjustment buttons and confirm button
 */
void UW_LevelUp::OnLevelUpCostChanged(bool bCanAfford)
{
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] OnLevelUpCostChanged - CanAfford: %s"), bCanAfford ? TEXT("YES") : TEXT("NO"));

	bCanAffordLevelUp = bCanAfford;

	// Enable/disable all stat entry increase buttons
	for (UW_StatEntry_LevelUp* Entry : AllStatEntries)
	{
		if (IsValid(Entry))
		{
			Entry->EventToggleLevelUpEnabled(bCanAfford);
		}
	}

	// Enable/disable the confirm button
	if (UButton* ConfirmBtn = Cast<UButton>(GetWidgetFromName(TEXT("ConfirmButton"))))
	{
		ConfirmBtn->SetIsEnabled(bCanAfford);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] ConfirmButton SetIsEnabled: %s"), bCanAfford ? TEXT("true") : TEXT("false"));
	}
}

/**
 * OnStatEntrySelectedHandler - Delegate handler for OnStatEntrySelected
 * Dynamic delegates require value types, so this handler takes TArray by value
 * and forwards to EventOnStatEntrySelected which takes const ref
 */
void UW_LevelUp::OnStatEntrySelectedHandler(TArray<FGameplayTag> AffectedStats, bool Selected)
{
	// Forward to the implementation
	EventOnStatEntrySelected(AffectedStats, Selected);
}

/**
 * OnCurrencyUpdatedFromInventory - Callback when currency changes
 * This is called when the InventoryManagerComponent's OnCurrencyUpdated fires
 * (e.g., when death currency is picked up)
 */
void UW_LevelUp::OnCurrencyUpdatedFromInventory(int32 NewAmount)
{
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] OnCurrencyUpdatedFromInventory - NewAmount: %d (was: %d)"),
		NewAmount, CurrentPlayerCurrency);

	// Update our cached currency value
	CurrentPlayerCurrency = NewAmount;

	// Forward to the LevelAndCurrencyBlock which will recalculate affordability
	// and broadcast OnLevelUpCostChanged
	if (UW_LevelCurrencyBlock_LevelUp* LevelCurrencyBlock = Cast<UW_LevelCurrencyBlock_LevelUp>(GetWidgetFromName(TEXT("LevelAndCurrencyBlock"))))
	{
		LevelCurrencyBlock->EventOnCurrencyUpdated(NewAmount);
		UE_LOG(LogTemp, Log, TEXT("[W_LevelUp]   Forwarded to LevelAndCurrencyBlock"));
	}
}

/**
 * OnConfirmButtonClicked - Handler for ConfirmButton OnClicked
 * Triggers level-up confirmation when the button is clicked
 *
 * bp_only pattern: On Clicked (ConfirmButton) -> GetCurrency -> EventConfirmLevelUp
 * The click handler just calls EventConfirmLevelUp directly, which handles validation
 */
void UW_LevelUp::OnConfirmButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_LevelUp] OnConfirmButtonClicked - Calling EventConfirmLevelUp (CurrentChanges.Num: %d, bCanAffordLevelUp: %s)"),
		CurrentChanges.Num(), bCanAffordLevelUp ? TEXT("true") : TEXT("false"));

	// bp_only pattern: ConfirmButton click goes directly to EventConfirmLevelUp
	// EventConfirmLevelUp handles currency check and applies changes
	EventConfirmLevelUp();
}
