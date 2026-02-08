// W_Settings.cpp
// C++ Widget implementation for W_Settings
//
// 20-PASS VALIDATION: 2026-01-05
// NO REFLECTION - direct property access for all child widgets

#include "Widgets/W_Settings.h"
#include "Widgets/W_Settings_CategoryEntry.h"
#include "Widgets/W_Settings_Entry.h"
#include "Widgets/W_GenericButton.h"
#include "Widgets/W_Settings_QuitConfirmation.h"
#include "Components/PanelWidget.h"
#include "Components/Border.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Texture2D.h"

UW_Settings::UW_Settings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CategoriesBox(nullptr)
	, CategorySwitcher(nullptr)
	, CategoryNavigationIndex(0)
	, EntryNavigationIndex(0)
	, QuitToDesktop(false)
	, ActiveQuitButton(nullptr)
	, bHandlingCategorySelection(false)
	, CachedQuitBtn(nullptr)
	, CachedDesktopBtn(nullptr)
	, CachedQuitConfirmation(nullptr)
	, CachedConfirmationBorder(nullptr)
{
}

void UW_Settings::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide blur widgets - BackgroundBlur at highest ZOrder that blurs everything
	// W_Settings has TWO blur widgets: "Blur" and "MainBlur"
	if (UWidget* Blur = GetWidgetFromName(TEXT("Blur")))
	{
		Blur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Hidden Blur widget"));
	}
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Hidden MainBlur widget"));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] NativeConstruct"));

	// CRITICAL: Configure all embedded widgets with their proper settings
	// This sets EntryType, SettingTag, SettingName, Icon, SwitcherIndex for each widget
	ConfigureEmbeddedWidgets();

	// Initialize categories from CategoriesBox
	InitializeCategories();

	// Initialize entries for default category
	InitializeEntries();

	// Set first category as selected
	if (CategoryEntries.Num() > 0)
	{
		CategoryNavigationIndex = 0;
		UpdateCategorySelection();
	}

	// Set first entry as selected
	if (SettingEntries.Num() > 0)
	{
		EntryNavigationIndex = 0;
		UpdateEntrySelection();
	}

	// Setup quit button and confirmation bindings
	SetupQuitBindings();

	// CRITICAL: Take keyboard/gamepad focus so NativeOnPreviewKeyDown works
	SetIsFocusable(true);
	if (APlayerController* PC = GetOwningPlayer())
	{
		SetUserFocus(PC);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Set user focus in NativeConstruct"));
	}
}

void UW_Settings::NativeDestruct()
{
	// Unbind quit buttons
	if (CachedQuitBtn)
	{
		CachedQuitBtn->OnButtonPressed.RemoveDynamic(this, &UW_Settings::HandleQuitButtonPressed);
	}
	if (CachedDesktopBtn)
	{
		CachedDesktopBtn->OnButtonPressed.RemoveDynamic(this, &UW_Settings::HandleDesktopButtonPressed);
	}
	if (CachedQuitConfirmation)
	{
		CachedQuitConfirmation->OnQuitGameConfirmed.RemoveDynamic(this, &UW_Settings::HandleQuitConfirmed);
		CachedQuitConfirmation->OnQuitGameCanceled.RemoveDynamic(this, &UW_Settings::HandleQuitCanceled);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] NativeDestruct"));
}

FReply UW_Settings::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	// When quit confirmation is visible, forward input directly to the child widget
	// UMG tunneling won't route to the embedded widget, so we call its handler explicitly
	if (CachedConfirmationBorder && CachedConfirmationBorder->GetVisibility() == ESlateVisibility::Visible)
	{
		// Only allow LB/RB to still change categories
		if (Key == EKeys::Gamepad_LeftShoulder || Key == EKeys::Gamepad_RightShoulder)
		{
			// Fall through to category handling below
		}
		else if (CachedQuitConfirmation)
		{
			// Forward input directly to quit confirmation widget
			return CachedQuitConfirmation->NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
		}
		else
		{
			return FReply::Unhandled();
		}
	}

	// CRITICAL: Gamepad D-pad and stick events are consumed by UMG's built-in focus navigation
	// (moves focus between ComboBoxKey, Buttons, Sliders) before reaching NativeOnKeyDown.
	// NativeOnPreviewKeyDown intercepts them during the tunneling phase, before child widgets process them.
	// Keyboard arrow keys are NOT consumed by UMG navigation, so they're handled in NativeOnKeyDown.

	// Gamepad D-pad / stick navigation
	if (Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad Up"));
		EventNavigateUp();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad Down"));
		EventNavigateDown();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad Left"));
		EventNavigateLeft();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad Right"));
		EventNavigateRight();
		return FReply::Handled();
	}

	// Gamepad face buttons
	if (Key == EKeys::Gamepad_FaceButton_Bottom) // A button / Cross
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad OK (A)"));
		EventNavigateOk();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_FaceButton_Right) // B button / Circle
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad Cancel (B)"));
		EventNavigateCancel();
		return FReply::Handled();
	}

	// Gamepad shoulder buttons (for category tab cycling)
	if (Key == EKeys::Gamepad_LeftShoulder)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad L1"));
		EventNavigateCategoryLeft();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_RightShoulder)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Preview - Gamepad R1"));
		EventNavigateCategoryRight();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UW_Settings::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	// Keyboard input (gamepad is handled in NativeOnPreviewKeyDown above)

	// Handle Tab/Escape to close settings
	if (Key == EKeys::Tab || Key == EKeys::Escape)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Tab/Escape pressed - calling EventNavigateCancel"));
		EventNavigateCancel();
		return FReply::Handled();
	}

	// Handle arrow keys for navigation
	if (Key == EKeys::Up)
	{
		EventNavigateUp();
		return FReply::Handled();
	}
	if (Key == EKeys::Down)
	{
		EventNavigateDown();
		return FReply::Handled();
	}
	if (Key == EKeys::Left)
	{
		EventNavigateLeft();
		return FReply::Handled();
	}
	if (Key == EKeys::Right)
	{
		EventNavigateRight();
		return FReply::Handled();
	}

	// Handle category navigation with Q/E
	if (Key == EKeys::Q)
	{
		EventNavigateCategoryLeft();
		return FReply::Handled();
	}
	if (Key == EKeys::E)
	{
		EventNavigateCategoryRight();
		return FReply::Handled();
	}

	// Handle OK/Enter
	if (Key == EKeys::Enter)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_Settings::InitializeCategories_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] InitializeCategories"));

	CategoryEntries.Empty();

	if (!CategoriesBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings] CategoriesBox is null"));
		return;
	}

	// Get all children from CategoriesBox
	TArray<UWidget*> Children = CategoriesBox->GetAllChildren();

	for (int32 Index = 0; Index < Children.Num(); Index++)
	{
		UWidget* Child = Children[Index];
		if (UW_Settings_CategoryEntry* CategoryEntry = Cast<UW_Settings_CategoryEntry>(Child))
		{
			CategoryEntries.Add(CategoryEntry);

			// CRITICAL: Set CategoryIndex = position in the list (for navigation)
			// SwitcherIndex = panel index in WidgetSwitcher (for content display)
			CategoryEntry->CategoryIndex = CategoryEntries.Num() - 1;

			// Bind OnSelected event
			CategoryEntry->OnSelected.AddDynamic(this, &UW_Settings::HandleCategorySelected);

			// CRITICAL: Log both indices to verify they're set correctly
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Added category[%d]: %s, CategoryIndex=%d, SwitcherIndex=%d"),
				Index, *CategoryEntry->GetName(), CategoryEntry->CategoryIndex, CategoryEntry->SwitcherIndex);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Child[%d] cast failed: %s (class: %s)"),
				Index, *Child->GetName(), *Child->GetClass()->GetName());
		}
	}

	// Hide categories in CategoriesToHide
	for (const FGameplayTag& TagToHide : CategoriesToHide)
	{
		for (UW_Settings_CategoryEntry* CategoryEntry : CategoryEntries)
		{
			if (CategoryEntry && CategoryEntry->SettingCategory.MatchesTag(TagToHide))
			{
				CategoryEntry->SetVisibility(ESlateVisibility::Collapsed);
				UE_LOG(LogTemp, Log, TEXT("[W_Settings] Hidden category with tag: %s"), *TagToHide.ToString());
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Initialized %d categories"), CategoryEntries.Num());

	// Log CategorySwitcher info
	if (CategorySwitcher)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings] CategorySwitcher has %d panels"), CategorySwitcher->GetNumWidgets());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings] CategorySwitcher is NULL!"));
	}
}

void UW_Settings::InitializeEntries_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] InitializeEntries"));

	SettingEntries.Empty();

	// Get entries for the active category
	GetEntriesForActiveCategory(SettingEntries);

	// Bind OnEntrySelected for each entry
	for (UW_Settings_Entry* Entry : SettingEntries)
	{
		if (Entry)
		{
			Entry->OnEntrySelected.AddDynamic(this, &UW_Settings::HandleEntrySelected);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] Initialized %d entries for category %d"), SettingEntries.Num(), CategoryNavigationIndex);

	// On the quit tab (no entries), select the first quit button by default
	if (SettingEntries.Num() == 0 && CachedQuitBtn)
	{
		CachedQuitBtn->SetButtonSelected(true);
		if (CachedDesktopBtn)
		{
			CachedDesktopBtn->SetButtonSelected(false);
		}
		ActiveQuitButton = CachedQuitBtn;
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Quit tab - selected W_GB_Quit by default"));
	}
}

int32 UW_Settings::GetEntryCountForActiveCategory_Implementation()
{
	if (!CategorySwitcher)
	{
		return 0;
	}

	UWidget* ActiveWidget = CategorySwitcher->GetActiveWidget();
	if (!ActiveWidget)
	{
		return 0;
	}

	// Cast to ScrollBox
	UScrollBox* ScrollBox = Cast<UScrollBox>(ActiveWidget);
	if (!ScrollBox)
	{
		return 0;
	}

	// Get first child which should be a VerticalBox
	TArray<UWidget*> ScrollChildren = ScrollBox->GetAllChildren();
	if (ScrollChildren.Num() == 0)
	{
		return 0;
	}

	UVerticalBox* VerticalBox = Cast<UVerticalBox>(ScrollChildren[0]);
	if (!VerticalBox)
	{
		return 0;
	}

	// Count W_Settings_Entry children
	TArray<UWidget*> EntryWidgets = VerticalBox->GetAllChildren();
	int32 Count = 0;
	for (UWidget* Widget : EntryWidgets)
	{
		if (Cast<UW_Settings_Entry>(Widget))
		{
			Count++;
		}
	}

	return Count;
}

void UW_Settings::EventCancelConfirmation_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventCancelConfirmation"));

	// Cancel any confirmation dialogs
	// This would typically hide quit confirmation, etc.
}

void UW_Settings::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCancel - discarding unsaved changes"));

	// Cancel should NOT save - it discards changes
	// If user wants to save, they should use an explicit Apply button or rely on
	// immediate apply behavior in EventNavigateLeft/Right

	// Broadcast settings closed event (without saving)
	OnSettingsClosed.Broadcast();
}

void UW_Settings::EventNavigateCategoryLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCategoryLeft"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to previous category (wrap around), skipping categories with no content panel (SwitcherIndex == -1)
	int32 StartIndex = CategoryNavigationIndex;
	do
	{
		CategoryNavigationIndex--;
		if (CategoryNavigationIndex < 0)
		{
			CategoryNavigationIndex = CategoryEntries.Num() - 1;
		}
		// Safety: if we wrapped all the way around, break to avoid infinite loop
		if (CategoryNavigationIndex == StartIndex)
		{
			break;
		}
	} while (CategoryEntries[CategoryNavigationIndex] && CategoryEntries[CategoryNavigationIndex]->SwitcherIndex < 0);

	UpdateCategorySelection();

	// Re-initialize entries for new category
	InitializeEntries();

	// Reset entry navigation
	EntryNavigationIndex = 0;
	UpdateEntrySelection();
}

void UW_Settings::EventNavigateCategoryRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCategoryRight"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to next category (wrap around), skipping categories with no content panel (SwitcherIndex == -1)
	int32 StartIndex = CategoryNavigationIndex;
	do
	{
		CategoryNavigationIndex++;
		if (CategoryNavigationIndex >= CategoryEntries.Num())
		{
			CategoryNavigationIndex = 0;
		}
		// Safety: if we wrapped all the way around, break to avoid infinite loop
		if (CategoryNavigationIndex == StartIndex)
		{
			break;
		}
	} while (CategoryEntries[CategoryNavigationIndex] && CategoryEntries[CategoryNavigationIndex]->SwitcherIndex < 0);

	UpdateCategorySelection();

	// Re-initialize entries for new category
	InitializeEntries();

	// Reset entry navigation
	EntryNavigationIndex = 0;
	UpdateEntrySelection();
}

void UW_Settings::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateDown"));

	if (SettingEntries.Num() == 0)
	{
		// Quit tab: toggle between Quit and Desktop buttons
		ToggleQuitTabButton();
		return;
	}

	// Move to next entry (wrap around)
	EntryNavigationIndex++;
	if (EntryNavigationIndex >= SettingEntries.Num())
	{
		EntryNavigationIndex = 0;
	}

	UpdateEntrySelection();
}

void UW_Settings::EventNavigateLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateLeft"));

	// Decrease the current entry's value
	if (SettingEntries.IsValidIndex(EntryNavigationIndex))
	{
		UW_Settings_Entry* Entry = SettingEntries[EntryNavigationIndex];
		if (Entry)
		{
			Entry->EventDecreaseSetting();
		}
	}
}

void UW_Settings::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateOk"));

	// Check if we're on the Quit tab (no settings entries = quit panel)
	if (SettingEntries.Num() == 0)
	{
		UW_Settings_CategoryEntry* CurrentCategory = CategoryEntries.IsValidIndex(CategoryNavigationIndex)
			? CategoryEntries[CategoryNavigationIndex]
			: nullptr;

		if (CurrentCategory && CurrentCategory->SwitcherIndex == 3)
		{
			// Show quit confirmation using ActiveQuitButton to determine desktop vs main menu
			QuitToDesktop = (ActiveQuitButton == CachedDesktopBtn);
			if (CachedConfirmationBorder)
			{
				CachedConfirmationBorder->SetVisibility(ESlateVisibility::Visible);
			}
			// Keep focus on W_Settings - NativeOnPreviewKeyDown forwards to QuitConfirmation
			// DO NOT call CachedQuitConfirmation->SetKeyboardFocus() as it moves focus
			// away from W_Settings, preventing NativeOnPreviewKeyDown from being called
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] Quit tab - showing confirmation (Desktop=%s)"),
				QuitToDesktop ? TEXT("true") : TEXT("false"));
			return;
		}
	}

	// Activate the current entry (for button entries, toggles, etc.)
	if (SettingEntries.IsValidIndex(EntryNavigationIndex))
	{
		UW_Settings_Entry* Entry = SettingEntries[EntryNavigationIndex];
		if (Entry)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] Activating entry: %s"), *Entry->GetName());
			Entry->EventActivateEntry();
		}
	}
}

void UW_Settings::EventNavigateResetToDefault_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateResetToDefault"));

	// Reset current entry to default value
	// This would typically be handled by each entry type
}

void UW_Settings::EventNavigateRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateRight"));

	// Increase the current entry's value
	if (SettingEntries.IsValidIndex(EntryNavigationIndex))
	{
		UW_Settings_Entry* Entry = SettingEntries[EntryNavigationIndex];
		if (Entry)
		{
			Entry->EventIncreaseSetting();
		}
	}
}

void UW_Settings::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateUp"));

	if (SettingEntries.Num() == 0)
	{
		// Quit tab: toggle between Quit and Desktop buttons
		ToggleQuitTabButton();
		return;
	}

	// Move to previous entry (wrap around)
	EntryNavigationIndex--;
	if (EntryNavigationIndex < 0)
	{
		EntryNavigationIndex = SettingEntries.Num() - 1;
	}

	UpdateEntrySelection();
}

void UW_Settings::EventOnCategorySelected_Implementation(UW_Settings_CategoryEntry* CategoryEntry, int32 InIndex)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventOnCategorySelected: index %d"), InIndex);

	if (!CategoryEntry)
	{
		return;
	}

	CategoryNavigationIndex = InIndex;

	// UpdateCategorySelection handles the switcher using SwitcherIndex, not nav index
	UpdateCategorySelection();

	// Re-initialize entries for new category (only if this category has content)
	if (CategoryEntry->SwitcherIndex >= 0)
	{
		InitializeEntries();

		// Reset entry navigation
		EntryNavigationIndex = 0;
		UpdateEntrySelection();
	}
	else
	{
		// No content panel for this category - clear entries
		SettingEntries.Empty();
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Category %d has no content panel, cleared entries"), InIndex);
	}
}

void UW_Settings::EventOnEntrySelected_Implementation(UW_Settings_Entry* InEntry)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventOnEntrySelected - Entry: %s"),
		InEntry ? *InEntry->GetName() : TEXT("null"));

	if (!InEntry)
	{
		return;
	}

	// Find the index of this entry and update navigation index
	// DO NOT call UpdateEntrySelection() here - that would cause infinite recursion:
	// UpdateEntrySelection -> SetEntrySelected -> OnEntrySelected -> EventOnEntrySelected -> UpdateEntrySelection
	for (int32 i = 0; i < SettingEntries.Num(); i++)
	{
		if (SettingEntries[i] == InEntry)
		{
			EntryNavigationIndex = i;
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] Entry navigation index set to: %d"), EntryNavigationIndex);
			break;
		}
	}
}

void UW_Settings::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventOnVisibilityChanged: %d"), static_cast<int32>(InVisibility));

	if (InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		// Reinitialize when becoming visible
		InitializeCategories();
		InitializeEntries();

		if (CategoryEntries.Num() > 0)
		{
			CategoryNavigationIndex = FMath::Clamp(CategoryNavigationIndex, 0, CategoryEntries.Num() - 1);
			UpdateCategorySelection();
		}

		if (SettingEntries.Num() > 0)
		{
			EntryNavigationIndex = FMath::Clamp(EntryNavigationIndex, 0, SettingEntries.Num() - 1);
			UpdateEntrySelection();
		}

		// CRITICAL: Take keyboard focus so gamepad/keyboard input works
		// Without this, W_GameMenu keeps focus and receives shoulder button input
		SetIsFocusable(true);
		if (APlayerController* PC = GetOwningPlayer())
		{
			SetUserFocus(PC);
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] Set user focus to Settings widget"));
		}
	}
}

void UW_Settings::UpdateCategorySelection()
{
	CategoryNavigationIndex = FMath::Clamp(CategoryNavigationIndex, 0, FMath::Max(0, CategoryEntries.Num() - 1));

	UW_Settings_CategoryEntry* SelectedCategory = CategoryEntries.IsValidIndex(CategoryNavigationIndex)
		? CategoryEntries[CategoryNavigationIndex]
		: nullptr;

	for (int32 i = 0; i < CategoryEntries.Num(); i++)
	{
		UW_Settings_CategoryEntry* Category = CategoryEntries[i];
		if (Category)
		{
			bool bIsSelected = (Category == SelectedCategory);
			Category->SetCategorySelected(bIsSelected);
		}
	}

	// Update category switcher using the category's SwitcherIndex, NOT the navigation index
	// CRITICAL: SwitcherIndex of -1 means no content panel for this category
	if (CategorySwitcher && SelectedCategory)
	{
		int32 SwitcherIdx = SelectedCategory->SwitcherIndex;
		if (SwitcherIdx >= 0 && SwitcherIdx < CategorySwitcher->GetNumWidgets())
		{
			CategorySwitcher->SetActiveWidgetIndex(SwitcherIdx);
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] UpdateCategorySelection: navIndex=%d, switcherIndex=%d"),
				CategoryNavigationIndex, SwitcherIdx);
		}
		else
		{
			// SwitcherIndex is -1 (no content panel) - don't change the switcher
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] UpdateCategorySelection: navIndex=%d has no content panel (switcherIndex=%d)"),
				CategoryNavigationIndex, SwitcherIdx);
		}
	}
}

void UW_Settings::UpdateEntrySelection()
{
	EntryNavigationIndex = FMath::Clamp(EntryNavigationIndex, 0, FMath::Max(0, SettingEntries.Num() - 1));

	UW_Settings_Entry* SelectedEntry = SettingEntries.IsValidIndex(EntryNavigationIndex)
		? SettingEntries[EntryNavigationIndex]
		: nullptr;

	for (UW_Settings_Entry* Entry : SettingEntries)
	{
		if (Entry)
		{
			bool bIsSelected = (Entry == SelectedEntry);
			Entry->SetEntrySelected(bIsSelected);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] UpdateEntrySelection: index %d"), EntryNavigationIndex);
}

UScrollBox* UW_Settings::GetActiveScrollBox() const
{
	if (!CategorySwitcher)
	{
		return nullptr;
	}

	UWidget* ActiveWidget = CategorySwitcher->GetActiveWidget();
	return Cast<UScrollBox>(ActiveWidget);
}

void UW_Settings::GetEntriesForActiveCategory(TArray<UW_Settings_Entry*>& OutEntries) const
{
	OutEntries.Empty();

	UScrollBox* ScrollBox = GetActiveScrollBox();
	if (!ScrollBox)
	{
		return;
	}

	// Get first child which should be a VerticalBox containing entries
	TArray<UWidget*> ScrollChildren = ScrollBox->GetAllChildren();
	if (ScrollChildren.Num() == 0)
	{
		return;
	}

	UVerticalBox* VerticalBox = Cast<UVerticalBox>(ScrollChildren[0]);
	if (!VerticalBox)
	{
		return;
	}

	// Get all W_Settings_Entry children
	// NOTE: Blueprint W_Settings_Entry_C may NOT inherit from C++ UW_Settings_Entry if restored from bp_only.
	// So we try both the C++ class cast AND check for the Blueprint class by name.
	TArray<UWidget*> EntryWidgets = VerticalBox->GetAllChildren();
	for (UWidget* Widget : EntryWidgets)
	{
		// First try direct cast to C++ class
		if (UW_Settings_Entry* Entry = Cast<UW_Settings_Entry>(Widget))
		{
			OutEntries.Add(Entry);
		}
		// Fallback: Check if it's a Blueprint-based entry by class name
		else if (Widget && Widget->GetClass()->GetName().Contains(TEXT("W_Settings_Entry")))
		{
			// Cast to UW_Settings_Entry won't work for Blueprint classes, but we can still use it
			// since they share the same interface. However, for now just log that we found one.
			// TODO: Consider reparenting W_Settings_Entry Blueprint to C++ class for proper integration.
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Found Blueprint-based W_Settings_Entry: %s (class: %s) - cast failed, skipping"),
				*Widget->GetName(), *Widget->GetClass()->GetName());
		}
	}
}

void UW_Settings::HandleCategorySelected(UW_Settings_CategoryEntry* CategoryEntry, int32 Index)
{
	// CRITICAL: Re-entrance guard. When UpdateCategorySelection calls SetCategorySelected on entries,
	// that can broadcast OnSelected again, re-entering this handler and corrupting CategoryNavigationIndex.
	if (bHandlingCategorySelection)
	{
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] HandleCategorySelected - BLOCKED re-entrant call (index=%d)"), Index);
		return;
	}

	bHandlingCategorySelection = true;
	EventOnCategorySelected(CategoryEntry, Index);
	bHandlingCategorySelection = false;
}

void UW_Settings::HandleEntrySelected(UW_Settings_Entry* Entry)
{
	EventOnEntrySelected(Entry);
}

void UW_Settings::ApplyAndSaveSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] ApplyAndSaveSettings"));

	UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
	if (!GameSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Failed to get GameUserSettings!"));
		return;
	}

	// Iterate through all setting entries and apply their values
	// This is a generic implementation - specific settings should be handled by SettingTag
	for (UW_Settings_Entry* Entry : SettingEntries)
	{
		if (!Entry)
		{
			continue;
		}

		FGameplayTag Tag = Entry->SettingTag;
		FString Value = Entry->CurrentValue;

		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Applying setting: %s = %s"), *Tag.ToString(), *Value);

		// Handle specific setting tags
		// Note: Add more cases as needed for your specific settings tags
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.Resolution"))))
		{
			// Parse resolution string "WxH" or "W x H" (handles spaces)
			FString Width, Height;
			if (Value.Split(TEXT("x"), &Width, &Height))
			{
				// Trim whitespace from both parts to handle "1920 x 1080" format
				Width.TrimStartAndEndInline();
				Height.TrimStartAndEndInline();

				int32 WidthInt = FCString::Atoi(*Width);
				int32 HeightInt = FCString::Atoi(*Height);

				// Only apply if we got valid values
				if (WidthInt > 0 && HeightInt > 0)
				{
					FIntPoint Resolution(WidthInt, HeightInt);
					GameSettings->SetScreenResolution(Resolution);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Invalid resolution format: %s"), *Value);
				}
			}
		}
		else if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.WindowMode"))))
		{
			int32 ModeInt = FCString::Atoi(*Value);
			EWindowMode::Type WindowMode = static_cast<EWindowMode::Type>(ModeInt);
			GameSettings->SetFullscreenMode(WindowMode);
		}
		else if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.VSync"))))
		{
			bool bVSync = (Value == TEXT("1") || Value.ToLower() == TEXT("true"));
			GameSettings->SetVSyncEnabled(bVSync);
		}
		else if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.FrameRateLimit"))))
		{
			float FrameRate = FCString::Atof(*Value);
			GameSettings->SetFrameRateLimit(FrameRate);
		}
		// Add audio, gameplay, and other settings as needed
	}

	// Apply and save all changes
	GameSettings->ApplySettings(false);  // false = don't check for command line overrides
	GameSettings->SaveSettings();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] Settings applied and saved"));
}

void UW_Settings::ConfigureEmbeddedWidgets()
{
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings] ConfigureEmbeddedWidgets - Configuring embedded widgets"));

	// ═══════════════════════════════════════════════════════════════════════
	// CONFIGURE CATEGORY ENTRIES
	// ═══════════════════════════════════════════════════════════════════════

	// Category icons paths
	const FString IconBasePath = TEXT("/Game/SoulslikeFramework/Widgets/_Textures/");

	// Configure each category entry by name
	struct FCategoryConfig
	{
		FString WidgetName;
		int32 SwitcherIndex;
		FString IconName;
	};

	// CRITICAL: SwitcherIndex must match bp_only values!
	// -1 means no content panel (category has no settings page)
	TArray<FCategoryConfig> CategoryConfigs = {
		{ TEXT("W_Settings_CategoryEntry_Display"), 0, TEXT("T_Computer") },    // Has content panel at index 0
		{ TEXT("W_Settings_CategoryEntry_Camera"), 1, TEXT("T_Camera") },       // Has content panel at index 1
		{ TEXT("W_Settings_CategoryEntry_Gameplay"), -1, TEXT("T_Controller") }, // NO content panel (-1)
		{ TEXT("W_Settings_CategoryEntry_Keybinds"), 2, TEXT("T_Keybinds") },   // Has content panel at index 2
		{ TEXT("W_Settings_CategoryEntry_Sounds"), -1, TEXT("T_Volume") },       // NO content panel (-1)
		{ TEXT("W_Settings_CategoryEntry_Quit"), 3, TEXT("T_Quit") },           // Has content panel at index 3
	};

	for (const FCategoryConfig& Config : CategoryConfigs)
	{
		if (UWidget* Widget = GetWidgetFromName(*Config.WidgetName))
		{
			if (UW_Settings_CategoryEntry* CategoryEntry = Cast<UW_Settings_CategoryEntry>(Widget))
			{
				FString IconPath = IconBasePath + Config.IconName;
				ConfigureCategoryEntry(CategoryEntry, Config.SwitcherIndex, IconPath);
				UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Configured category: %s, SwitcherIndex=%d"),
					*Config.WidgetName, Config.SwitcherIndex);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Found widget %s but cast to UW_Settings_CategoryEntry failed (class: %s)"),
					*Config.WidgetName, *Widget->GetClass()->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Could not find category widget: %s"), *Config.WidgetName);
		}
	}

	// ═══════════════════════════════════════════════════════════════════════
	// CONFIGURE SETTINGS ENTRIES
	// ═══════════════════════════════════════════════════════════════════════

	struct FEntryConfig
	{
		FString WidgetName;
		FString SettingTagName;
		FText DisplayName;
		ESLFSettingEntry EntryType;
	};

	// CRITICAL: SettingTag and EntryType MUST match bp_only exactly!
	// All bp_only entries use EntryType=0 (DropDown)
	// Tags use "SoulslikeFramework.Settings.*" prefix
	TArray<FEntryConfig> EntryConfigs = {
		// Display Settings
		{ TEXT("ScreenMode"), TEXT("SoulslikeFramework.Settings.ScreenMode"), NSLOCTEXT("Settings", "ScreenMode", "Screen Mode"), ESLFSettingEntry::DropDown },
		{ TEXT("Resolution"), TEXT("SoulslikeFramework.Settings.Resolution"), NSLOCTEXT("Settings", "Resolution", "Resolution"), ESLFSettingEntry::DropDown },
		{ TEXT("TextureQuality"), TEXT("SoulslikeFramework.Settings.TextureQuality"), NSLOCTEXT("Settings", "TextureQuality", "Texture Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("AntialiasingQuality"), TEXT("SoulslikeFramework.Settings.AntiAliasing"), NSLOCTEXT("Settings", "AntialiasingQuality", "Antialiasing Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("PostprocessQuality"), TEXT("SoulslikeFramework.Settings.PostProcessing"), NSLOCTEXT("Settings", "PostprocessQuality", "Postprocessing Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("ShadowQuality"), TEXT("SoulslikeFramework.Settings.ShadowQuality"), NSLOCTEXT("Settings", "ShadowQuality", "Shadow Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("ReflectionQuality"), TEXT("SoulslikeFramework.Settings.Reflections"), NSLOCTEXT("Settings", "ReflectionQuality", "Reflections Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("ShaderQuality"), TEXT("SoulslikeFramework.Settings.ShaderQuality"), NSLOCTEXT("Settings", "ShaderQuality", "Shader Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("GlobalIlluminationQuality"), TEXT("SoulslikeFramework.Settings.GlobalIllumination"), NSLOCTEXT("Settings", "GlobalIlluminationQuality", "Global Illuimination Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("ViewDistanceQuality"), TEXT("SoulslikeFramework.Settings.ViewDistance"), NSLOCTEXT("Settings", "ViewDistanceQuality", "View Distance Quality"), ESLFSettingEntry::DropDown },
		{ TEXT("FoliageQuality"), TEXT("SoulslikeFramework.Settings.FoliageQuality"), NSLOCTEXT("Settings", "FoliageQuality", "Foliage Quality"), ESLFSettingEntry::DropDown },

		// Camera Settings - ALL are DropDown in bp_only, use custom tags
		{ TEXT("InvertCameraX"), TEXT("SoulslikeFramework.Settings.Custom.InvertCamX"), NSLOCTEXT("Settings", "InvertCameraX", "Invert Camera X-Axis?"), ESLFSettingEntry::DropDown },
		{ TEXT("InvertCameraY"), TEXT("SoulslikeFramework.Settings.Custom.InvertCamY"), NSLOCTEXT("Settings", "InvertCameraY", "Invert Camera Y-Axis?"), ESLFSettingEntry::DropDown },
		{ TEXT("CameraSpeed"), TEXT("SoulslikeFramework.Settings.Custom.CamSpeed"), NSLOCTEXT("Settings", "CameraSpeed", "Camera Speed"), ESLFSettingEntry::DropDown },

		// Keybind Settings
		{ TEXT("KeyMappingSettings"), TEXT("SoulslikeFramework.Settings.Keymappings"), NSLOCTEXT("Settings", "KeyMappingSettings", "Controls"), ESLFSettingEntry::DropDown },
	};

	for (const FEntryConfig& Config : EntryConfigs)
	{
		if (UWidget* Widget = GetWidgetFromName(*Config.WidgetName))
		{
			if (UW_Settings_Entry* Entry = Cast<UW_Settings_Entry>(Widget))
			{
				ConfigureSettingsEntry(Entry, Config.SettingTagName, Config.DisplayName, Config.EntryType);
				UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Configured entry: %s, Type=%d"),
					*Config.WidgetName, static_cast<int32>(Config.EntryType));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Could not find widget: %s"), *Config.WidgetName);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings] ConfigureEmbeddedWidgets complete - Configured %d settings entries"), EntryConfigs.Num());
}

void UW_Settings::ConfigureCategoryEntry(UW_Settings_CategoryEntry* Entry, int32 Index, const FString& IconPath)
{
	if (!Entry)
	{
		return;
	}

	// Set switcher index - this is CRITICAL for tab switching to work
	Entry->SwitcherIndex = Index;

	// NOTE: Do NOT set Icon here - W_Settings_CategoryEntry::ApplyIconToWidget() handles icon loading
	// based on widget name patterns (Display→T_Computer, Camera→T_Camera, etc.)

	// Set colors (matching bp_only values from settings_current.json)
	Entry->SelectedColor = FLinearColor(0.859375f, 0.7441949844360352f, 0.618340015411377f, 1.0f);
	Entry->DeselectedColor = FLinearColor(0.49411800503730774f, 0.3921569883823395f, 0.282353013753891f, 1.0f);
}

void UW_Settings::ConfigureSettingsEntry(UW_Settings_Entry* Entry, const FString& SettingTagName, const FText& DisplayName, ESLFSettingEntry Type)
{
	if (!Entry)
	{
		return;
	}

	// Set the setting tag
	Entry->SettingTag = FGameplayTag::RequestGameplayTag(FName(*SettingTagName), false);

	// Set display name
	Entry->SettingName = DisplayName;

	// Set entry type
	Entry->EntryType = Type;

	// Set colors for selection highlighting
	// NOTE: bp_only extracted values (0.047 vs 0.065) were nearly identical and invisible.
	// Using a clearly visible warm highlight that matches the Soulslike UI theme.
	Entry->UnhoveredColor = FLinearColor(0.04705899953842163f, 0.04705899953842163f, 0.04705899953842163f, 0.9019610285758972f);
	Entry->HoveredColor = FLinearColor(0.18f, 0.14f, 0.08f, 1.0f); // Warm gold-brown, clearly visible

	// Call SetEntryType to update the visual (shows/hides the appropriate view)
	Entry->SetEntryType(Type);
}

void UW_Settings::SetupQuitBindings()
{
	// Find quit buttons by name in the quit panel
	if (UWidget* QuitWidget = GetWidgetFromName(TEXT("W_GB_Quit")))
	{
		CachedQuitBtn = Cast<UW_GenericButton>(QuitWidget);
	}
	if (UWidget* DesktopWidget = GetWidgetFromName(TEXT("W_GB_Desktop")))
	{
		CachedDesktopBtn = Cast<UW_GenericButton>(DesktopWidget);
	}

	// Find confirmation border and sub-widget
	CachedConfirmationBorder = GetWidgetFromName(TEXT("ConfirmationBorder"));

	if (UWidget* ConfWidget = GetWidgetFromName(TEXT("W_Settings_QuitConfirmation")))
	{
		CachedQuitConfirmation = Cast<UW_Settings_QuitConfirmation>(ConfWidget);
	}

	// Bind to quit/desktop button presses
	if (CachedQuitBtn)
	{
		CachedQuitBtn->OnButtonPressed.AddDynamic(this, &UW_Settings::HandleQuitButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Bound to W_GB_Quit.OnButtonPressed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Could not find W_GB_Quit button"));
	}

	if (CachedDesktopBtn)
	{
		CachedDesktopBtn->OnButtonPressed.AddDynamic(this, &UW_Settings::HandleDesktopButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Bound to W_GB_Desktop.OnButtonPressed"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Could not find W_GB_Desktop button"));
	}

	// Bind to confirmation result
	if (CachedQuitConfirmation)
	{
		CachedQuitConfirmation->OnQuitGameConfirmed.AddDynamic(this, &UW_Settings::HandleQuitConfirmed);
		CachedQuitConfirmation->OnQuitGameCanceled.AddDynamic(this, &UW_Settings::HandleQuitCanceled);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] Bound to QuitConfirmation dispatchers"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Could not find W_Settings_QuitConfirmation widget"));
	}

	// Initially hide confirmation border
	if (CachedConfirmationBorder)
	{
		CachedConfirmationBorder->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings] ConfirmationBorder initially hidden"));
	}
}

void UW_Settings::ToggleQuitTabButton()
{
	if (!CachedQuitBtn || !CachedDesktopBtn)
	{
		return;
	}

	// Toggle: if Quit is active, select Desktop and vice versa
	if (ActiveQuitButton == CachedDesktopBtn)
	{
		CachedQuitBtn->SetButtonSelected(true);
		CachedDesktopBtn->SetButtonSelected(false);
		ActiveQuitButton = CachedQuitBtn;
	}
	else
	{
		CachedDesktopBtn->SetButtonSelected(true);
		CachedQuitBtn->SetButtonSelected(false);
		ActiveQuitButton = CachedDesktopBtn;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] ToggleQuitTabButton - Active: %s"),
		ActiveQuitButton == CachedQuitBtn ? TEXT("Quit") : TEXT("Desktop"));
}

void UW_Settings::HandleQuitButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] W_GB_Quit pressed - showing confirmation (Desktop=false)"));
	QuitToDesktop = false;
	if (CachedConfirmationBorder)
	{
		CachedConfirmationBorder->SetVisibility(ESlateVisibility::Visible);
	}
	// Keep focus on W_Settings - forwarding handles QuitConfirmation input
}

void UW_Settings::HandleDesktopButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] W_GB_Desktop pressed - showing confirmation (Desktop=true)"));
	QuitToDesktop = true;
	if (CachedConfirmationBorder)
	{
		CachedConfirmationBorder->SetVisibility(ESlateVisibility::Visible);
	}
	// Keep focus on W_Settings - forwarding handles QuitConfirmation input
}

void UW_Settings::HandleQuitConfirmed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] Quit CONFIRMED - broadcasting OnQuitRequested(Desktop=%s)"),
		QuitToDesktop ? TEXT("true") : TEXT("false"));

	// Hide confirmation
	if (CachedConfirmationBorder)
	{
		CachedConfirmationBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Broadcast quit request to parent (W_HUD)
	OnQuitRequested.Broadcast(QuitToDesktop);
}

void UW_Settings::HandleQuitCanceled()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] Quit CANCELED - hiding confirmation"));

	// Just hide confirmation, return to quit panel
	if (CachedConfirmationBorder)
	{
		CachedConfirmationBorder->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Return focus to W_Settings for gamepad navigation
	SetKeyboardFocus();
}
