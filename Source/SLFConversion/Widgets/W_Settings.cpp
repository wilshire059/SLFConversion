// W_Settings.cpp
// C++ Widget implementation for W_Settings
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_Settings.h"
#include "Widgets/W_Settings_CategoryEntry.h"
#include "Widgets/W_Settings_Entry.h"
#include "Components/PanelWidget.h"

UW_Settings::UW_Settings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CategoriesBox(nullptr)
	, CategorySwitcher(nullptr)
	, CategoryNavigationIndex(0)
	, EntryNavigationIndex(0)
	, QuitToDesktop(false)
	, ActiveQuitButton(nullptr)
{
}

void UW_Settings::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] NativeConstruct"));

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
}

void UW_Settings::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] NativeDestruct"));
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

			// Bind OnSelected event
			CategoryEntry->OnSelected.AddDynamic(this, &UW_Settings::HandleCategorySelected);

			UE_LOG(LogTemp, Log, TEXT("[W_Settings] Added category: %s at index %d"), *CategoryEntry->GetName(), Index);
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

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] Initialized %d categories"), CategoryEntries.Num());
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
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCancel"));

	// Broadcast settings closed event
	OnSettingsClosed.Broadcast();
}

void UW_Settings::EventNavigateCategoryLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCategoryLeft"));

	if (CategoryEntries.Num() == 0)
	{
		return;
	}

	// Move to previous category (wrap around)
	CategoryNavigationIndex--;
	if (CategoryNavigationIndex < 0)
	{
		CategoryNavigationIndex = CategoryEntries.Num() - 1;
	}

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

	// Move to next category (wrap around)
	CategoryNavigationIndex++;
	if (CategoryNavigationIndex >= CategoryEntries.Num())
	{
		CategoryNavigationIndex = 0;
	}

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

	// Activate the current entry (for button entries, toggles, etc.)
	if (SettingEntries.IsValidIndex(EntryNavigationIndex))
	{
		UW_Settings_Entry* Entry = SettingEntries[EntryNavigationIndex];
		if (Entry)
		{
			// For button entries, this would trigger their action
			UE_LOG(LogTemp, Log, TEXT("[W_Settings] OK pressed on entry: %s"), *Entry->GetName());
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
	UpdateCategorySelection();

	// Switch the category switcher
	if (CategorySwitcher)
	{
		CategorySwitcher->SetActiveWidgetIndex(CategoryNavigationIndex);
	}

	// Re-initialize entries for new category
	InitializeEntries();

	// Reset entry navigation
	EntryNavigationIndex = 0;
	UpdateEntrySelection();
}

void UW_Settings::EventOnEntrySelected_Implementation(UW_Settings_Entry* InEntry)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventOnEntrySelected"));

	if (!InEntry)
	{
		return;
	}

	// Find the index of this entry
	for (int32 i = 0; i < SettingEntries.Num(); i++)
	{
		if (SettingEntries[i] == InEntry)
		{
			EntryNavigationIndex = i;
			UpdateEntrySelection();
			break;
		}
	}
}

void UW_Settings::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventOnVisibilityChanged: %d"), InVisibility);

	ESlateVisibility NewVisibility = static_cast<ESlateVisibility>(InVisibility);

	if (NewVisibility == ESlateVisibility::Visible || NewVisibility == ESlateVisibility::SelfHitTestInvisible)
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

	// Update category switcher
	if (CategorySwitcher && CategorySwitcher->GetNumWidgets() > CategoryNavigationIndex)
	{
		CategorySwitcher->SetActiveWidgetIndex(CategoryNavigationIndex);
	}

	UE_LOG(LogTemp, Log, TEXT("[W_Settings] UpdateCategorySelection: index %d"), CategoryNavigationIndex);
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
	TArray<UWidget*> EntryWidgets = VerticalBox->GetAllChildren();
	for (UWidget* Widget : EntryWidgets)
	{
		if (UW_Settings_Entry* Entry = Cast<UW_Settings_Entry>(Widget))
		{
			OutEntries.Add(Entry);
		}
	}
}

void UW_Settings::HandleCategorySelected(UW_Settings_CategoryEntry* CategoryEntry, int32 Index)
{
	EventOnCategorySelected(CategoryEntry, Index);
}

void UW_Settings::HandleEntrySelected(UW_Settings_Entry* Entry)
{
	EventOnEntrySelected(Entry);
}
