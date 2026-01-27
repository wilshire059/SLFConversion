// W_InventoryActionAmount.cpp
// C++ Widget implementation for W_InventoryActionAmount
//
// 20-PASS VALIDATION: 2026-01-19
// Full implementation matching bp_only logic exactly

#include "Widgets/W_InventoryActionAmount.h"
#include "Widgets/W_InventorySlot.h"
#include "Widgets/W_GenericButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SLFPrimaryDataAssets.h"

UW_InventoryActionAmount::UW_InventoryActionAmount(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SelectedSlot = nullptr;
	CurrentAmount = 1;
	MaxAmount = 1;
	CurrentItem = nullptr;
	OperationType = ESLFInventoryAmountedActionType::LeaveAmount;

	// Initialize cached pointers
	CachedBtnUp = nullptr;
	CachedBtnDown = nullptr;
	CachedAmountText = nullptr;
	CachedMaxAmountText = nullptr;
	CachedYesButton = nullptr;
	CachedNoButton = nullptr;
	ConfirmButtonIndex = 0;  // Start with YES selected
}

void UW_InventoryActionAmount::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	// Bind button events
	BindButtonEvents();

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::NativeConstruct"));
}

void UW_InventoryActionAmount::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::NativeDestruct"));
}

void UW_InventoryActionAmount::CacheWidgetReferences()
{
	// bp_only widget names from WidgetTree:
	// BtnUp, BtnDown, AmountText, MaxAmountText, W_GB_YES, W_GB_NO
	CachedBtnUp = Cast<UButton>(GetWidgetFromName(TEXT("BtnUp")));
	CachedBtnDown = Cast<UButton>(GetWidgetFromName(TEXT("BtnDown")));
	CachedAmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("AmountText")));
	CachedMaxAmountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MaxAmountText")));
	CachedYesButton = Cast<UW_GenericButton>(GetWidgetFromName(TEXT("W_GB_YES")));
	CachedNoButton = Cast<UW_GenericButton>(GetWidgetFromName(TEXT("W_GB_NO")));

	// bp_only: Set button text labels - these were instance properties on the parent widget
	// that are lost during reparenting, so we must set them explicitly
	if (CachedYesButton)
	{
		CachedYesButton->EventSetButtonText(FText::FromString(TEXT("YES")));
	}
	if (CachedNoButton)
	{
		CachedNoButton->EventSetButtonText(FText::FromString(TEXT("NO")));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::CacheWidgetReferences - BtnUp: %s, BtnDown: %s, AmountText: %s, MaxAmountText: %s, YES: %s, NO: %s"),
		CachedBtnUp ? TEXT("found") : TEXT("null"),
		CachedBtnDown ? TEXT("found") : TEXT("null"),
		CachedAmountText ? TEXT("found") : TEXT("null"),
		CachedMaxAmountText ? TEXT("found") : TEXT("null"),
		CachedYesButton ? TEXT("found") : TEXT("null"),
		CachedNoButton ? TEXT("found") : TEXT("null"));
}

void UW_InventoryActionAmount::BindButtonEvents()
{
	// Bind BtnUp pressed event
	if (CachedBtnUp)
	{
		CachedBtnUp->OnPressed.AddUniqueDynamic(this, &UW_InventoryActionAmount::OnBtnUpPressed);
	}

	// Bind BtnDown pressed event
	if (CachedBtnDown)
	{
		CachedBtnDown->OnPressed.AddUniqueDynamic(this, &UW_InventoryActionAmount::OnBtnDownPressed);
	}

	// Bind YES button pressed event
	if (CachedYesButton)
	{
		CachedYesButton->OnButtonPressed.AddUniqueDynamic(this, &UW_InventoryActionAmount::OnYesButtonPressed);
	}

	// Bind NO button pressed event
	if (CachedNoButton)
	{
		CachedNoButton->OnButtonPressed.AddUniqueDynamic(this, &UW_InventoryActionAmount::OnNoButtonPressed);
	}
}

void UW_InventoryActionAmount::UpdateAmountDisplay()
{
	// Update AmountText to show CurrentAmount
	if (CachedAmountText)
	{
		CachedAmountText->SetText(FText::AsNumber(CurrentAmount));
	}

	// Update MaxAmountText to show MaxAmount
	if (CachedMaxAmountText)
	{
		CachedMaxAmountText->SetText(FText::AsNumber(MaxAmount));
	}

	// bp_only: Enable/disable up/down buttons based on amount
	if (CachedBtnUp)
	{
		CachedBtnUp->SetIsEnabled(CurrentAmount < MaxAmount);
	}
	if (CachedBtnDown)
	{
		CachedBtnDown->SetIsEnabled(CurrentAmount > 1);
	}
}

void UW_InventoryActionAmount::OnBtnUpPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::OnBtnUpPressed"));
	EventNavigateAmountUp();
}

void UW_InventoryActionAmount::OnBtnDownPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::OnBtnDownPressed"));
	EventNavigateAmountDown();
}

void UW_InventoryActionAmount::OnYesButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::OnYesButtonPressed - Confirming with Amount: %d, OperationType: %d"),
		CurrentAmount, static_cast<int32>(OperationType));

	// bp_only: On YES button pressed -> broadcast OnRequestConfirmed
	OnRequestConfirmed.Broadcast(CurrentAmount, OperationType);
}

void UW_InventoryActionAmount::OnNoButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::OnNoButtonPressed - Canceling OperationType: %d"),
		static_cast<int32>(OperationType));

	// bp_only: On NO button pressed -> broadcast OnRequestCanceled
	OnRequestCanceled.Broadcast(OperationType);
}

/**
 * EventAmountConfirmButtonPressed - Trigger the currently selected confirm button
 *
 * bp_only logic:
 * Gets the currently selected button (YES or NO) and calls EventPressButton on it
 */
void UW_InventoryActionAmount::EventAmountConfirmButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::EventAmountConfirmButtonPressed - ConfirmButtonIndex: %d"),
		ConfirmButtonIndex);

	// bp_only: Select between YES and NO based on navigation state, then press
	UW_GenericButton* TargetButton = (ConfirmButtonIndex == 0) ? CachedYesButton : CachedNoButton;
	if (TargetButton)
	{
		TargetButton->EventPressButton();
	}
}

/**
 * EventInitializeForSlot - Initialize the amount selector for a slot
 *
 * bp_only logic:
 * 1. Set OperationType from input
 * 2. If Slot is valid:
 *    a. Set SelectedSlot
 *    b. Get AssignedItem from slot -> set CurrentItem
 *    c. Get Count from slot -> set MaxAmount
 *    d. Set CurrentAmount = 1 (or clamp to MaxAmount)
 * 3. Update display
 * 4. Select YES button by default
 */
void UW_InventoryActionAmount::EventInitializeForSlot_Implementation(uint8 InOperationType, UW_InventorySlot* InSlot)
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::EventInitializeForSlot - OperationType: %d, Slot: %s"),
		InOperationType,
		InSlot ? *InSlot->GetName() : TEXT("null"));

	// Set operation type
	OperationType = static_cast<ESLFInventoryAmountedActionType>(InOperationType);

	// bp_only: Check if slot is valid
	if (!IsValid(InSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Slot is not valid - aborting initialization"));
		return;
	}

	// Store slot reference
	SelectedSlot = InSlot;

	// Get item from slot
	CurrentItem = InSlot->AssignedItem;

	// Get count from slot as max amount
	MaxAmount = InSlot->Count;
	if (MaxAmount < 1)
	{
		MaxAmount = 1;
	}

	// Set initial amount to 1 (bp_only pattern)
	CurrentAmount = 1;

	UE_LOG(LogTemp, Log, TEXT("  Initialized - CurrentItem: %s, MaxAmount: %d, CurrentAmount: %d"),
		CurrentItem ? *CurrentItem->GetName() : TEXT("null"),
		MaxAmount,
		CurrentAmount);

	// Update the display
	UpdateAmountDisplay();

	// bp_only: Select YES button by default
	ConfirmButtonIndex = 0;
	if (CachedYesButton)
	{
		CachedYesButton->SetButtonSelected(true);
	}
	if (CachedNoButton)
	{
		CachedNoButton->SetButtonSelected(false);
	}
}

/**
 * EventNavigateAmountDown - Decrease the selected amount
 *
 * bp_only logic:
 * CurrentAmount = Clamp(CurrentAmount - 1, 1, MaxAmount)
 * Update display
 */
void UW_InventoryActionAmount::EventNavigateAmountDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::EventNavigateAmountDown - Before: %d"), CurrentAmount);

	// bp_only: Clamp(CurrentAmount - 1, Min=1, Max=MaxAmount)
	CurrentAmount = FMath::Clamp(CurrentAmount - 1, 1, MaxAmount);

	UE_LOG(LogTemp, Log, TEXT("  After: %d"), CurrentAmount);

	// Update display
	UpdateAmountDisplay();
}

/**
 * EventNavigateAmountUp - Increase the selected amount
 *
 * bp_only logic:
 * CurrentAmount = Clamp(CurrentAmount + 1, 1, MaxAmount)
 * Update display
 */
void UW_InventoryActionAmount::EventNavigateAmountUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::EventNavigateAmountUp - Before: %d"), CurrentAmount);

	// bp_only: Clamp(CurrentAmount + 1, Min=1, Max=MaxAmount)
	CurrentAmount = FMath::Clamp(CurrentAmount + 1, 1, MaxAmount);

	UE_LOG(LogTemp, Log, TEXT("  After: %d"), CurrentAmount);

	// Update display
	UpdateAmountDisplay();
}

/**
 * EventNavigateConfirmButtonsHorizontal - Toggle between YES and NO buttons
 *
 * bp_only logic:
 * Toggle selection between YES (index 0) and NO (index 1)
 * Update visual state
 */
void UW_InventoryActionAmount::EventNavigateConfirmButtonsHorizontal_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_InventoryActionAmount::EventNavigateConfirmButtonsHorizontal - Before: %d"), ConfirmButtonIndex);

	// Toggle between 0 and 1
	ConfirmButtonIndex = (ConfirmButtonIndex == 0) ? 1 : 0;

	UE_LOG(LogTemp, Log, TEXT("  After: %d"), ConfirmButtonIndex);

	// Update button selection states
	if (CachedYesButton)
	{
		CachedYesButton->SetButtonSelected(ConfirmButtonIndex == 0);
	}
	if (CachedNoButton)
	{
		CachedNoButton->SetButtonSelected(ConfirmButtonIndex == 1);
	}
}
