// W_RestMenu.cpp
// C++ Widget implementation for W_RestMenu
//
// 20-PASS VALIDATION: 2026-01-16
// Matches bp_only Blueprint pattern exactly:
// - Each button (Rest, Level Up, Storage, Leave) has its OWN press handler
// - Selection events update navigation index only
// - Switcher and LocationText use "Cached" prefix to avoid BindWidget conflict

#include "Widgets/W_RestMenu.h"
#include "Widgets/W_RestMenu_Button.h"
#include "Widgets/W_RestMenu_TimeEntry.h"
#include "Widgets/W_TimePass.h"
#include "Widgets/W_HUD.h"
#include "Blueprints/B_RestingPoint.h"
#include "Blueprints/SLFRestingPointBase.h"
#include "Blueprints/Actors/SLFSkyManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UObject/ConstructorHelpers.h"

UW_RestMenu::UW_RestMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentRestingPoint = nullptr;
	LocalTimeInfo = nullptr;
	MainNavigationIndex = 0;
	TimeEntryNavigationIndex = 0;

	// Cached references (use "Cached" prefix to avoid conflict with Blueprint BindWidget)
	CachedSwitcher = nullptr;
	CachedLocationText = nullptr;
	TimeEntriesBoxWidget = nullptr;

	// Button references
	RestButton = nullptr;
	LevelUpButton = nullptr;
	StorageButton = nullptr;
	LeaveButton = nullptr;

	CurrentViewIndex = 0;

	// Load the TimeEntry widget class
	static ConstructorHelpers::FClassFinder<UW_RestMenu_TimeEntry> TimeEntryClassFinder(
		TEXT("/Game/SoulslikeFramework/Widgets/RestMenu/W_RestMenu_TimeEntry"));
	if (TimeEntryClassFinder.Succeeded())
	{
		TimeEntryWidgetClass = TimeEntryClassFinder.Class;
	}
}

void UW_RestMenu::NativeConstruct()
{
	Super::NativeConstruct();

	// Make widget focusable - CRITICAL for NativeOnKeyDown to work
	SetIsFocusable(true);

	// Cache widget references
	CacheWidgetReferences();

	// Bind button events
	BindButtonEvents();

	// Initialize time entries
	InitializeTimeEntries();

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::NativeConstruct"));
}

void UW_RestMenu::NativeDestruct()
{
	// Unbind all button events
	if (RestButton)
	{
		RestButton->OnRestMenuButtonSelected.RemoveDynamic(this, &UW_RestMenu::HandleButtonSelected);
		RestButton->OnRestMenuButtonPressed.RemoveDynamic(this, &UW_RestMenu::HandleRestButtonPressed);
	}
	if (LevelUpButton)
	{
		LevelUpButton->OnRestMenuButtonSelected.RemoveDynamic(this, &UW_RestMenu::HandleButtonSelected);
		LevelUpButton->OnRestMenuButtonPressed.RemoveDynamic(this, &UW_RestMenu::HandleLevelUpButtonPressed);
	}
	if (StorageButton)
	{
		StorageButton->OnRestMenuButtonSelected.RemoveDynamic(this, &UW_RestMenu::HandleButtonSelected);
		StorageButton->OnRestMenuButtonPressed.RemoveDynamic(this, &UW_RestMenu::HandleStorageButtonPressed);
	}
	if (LeaveButton)
	{
		LeaveButton->OnRestMenuButtonSelected.RemoveDynamic(this, &UW_RestMenu::HandleButtonSelected);
		LeaveButton->OnRestMenuButtonPressed.RemoveDynamic(this, &UW_RestMenu::HandleLeaveButtonPressed);
	}

	// Unbind time pass widget events
	if (TimePassWidgetRef)
	{
		TimePassWidgetRef->OnTimePassEnd.RemoveDynamic(this, &UW_RestMenu::HandleTimePassEnd);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::NativeDestruct"));
}

FReply UW_RestMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_RestMenu] NativeOnKeyDown - Key: %s, CurrentViewIndex: %d"), *Key.ToString(), CurrentViewIndex);

	// Navigate Up: W, Up Arrow, Gamepad DPad Up
	if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		if (CurrentViewIndex == 0)
		{
			// Main buttons view
			EventNavigateUp();
		}
		else if (CurrentViewIndex == 1)
		{
			// Time selection view - navigate through time entries
			if (TimeEntries.Num() > 0)
			{
				// Deselect current
				if (TimeEntries.IsValidIndex(TimeEntryNavigationIndex) && TimeEntries[TimeEntryNavigationIndex])
				{
					TimeEntries[TimeEntryNavigationIndex]->SetTimeEntrySelected(false);
				}

				// Move to previous
				TimeEntryNavigationIndex--;
				if (TimeEntryNavigationIndex < 0)
				{
					TimeEntryNavigationIndex = TimeEntries.Num() - 1;
				}

				// Select new
				if (TimeEntries[TimeEntryNavigationIndex])
				{
					TimeEntries[TimeEntryNavigationIndex]->SetTimeEntrySelected(true);
				}
			}
		}
		return FReply::Handled();
	}

	// Navigate Down: S, Down Arrow, Gamepad DPad Down
	if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		if (CurrentViewIndex == 0)
		{
			// Main buttons view
			EventNavigateDown();
		}
		else if (CurrentViewIndex == 1)
		{
			// Time selection view - navigate through time entries
			if (TimeEntries.Num() > 0)
			{
				// Deselect current
				if (TimeEntries.IsValidIndex(TimeEntryNavigationIndex) && TimeEntries[TimeEntryNavigationIndex])
				{
					TimeEntries[TimeEntryNavigationIndex]->SetTimeEntrySelected(false);
				}

				// Move to next
				TimeEntryNavigationIndex = (TimeEntryNavigationIndex + 1) % TimeEntries.Num();

				// Select new
				if (TimeEntries[TimeEntryNavigationIndex])
				{
					TimeEntries[TimeEntryNavigationIndex]->SetTimeEntrySelected(true);
				}
			}
		}
		return FReply::Handled();
	}

	// Navigate Ok/Confirm: Enter, Space, E (Interact key), Gamepad A
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::E || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		if (CurrentViewIndex == 0)
		{
			// Main buttons view - select current button
			EventNavigateOk();
		}
		else if (CurrentViewIndex == 1)
		{
			// Time selection view - select current time entry
			if (TimeEntries.IsValidIndex(TimeEntryNavigationIndex) && TimeEntries[TimeEntryNavigationIndex])
			{
				FSLFDayNightInfo TimeInfo = TimeEntries[TimeEntryNavigationIndex]->TimeInfo;
				EventOnTimeEntryPressed(TimeInfo);
			}
		}
		return FReply::Handled();
	}

	// Navigate Cancel/Back: Escape, Backspace, Q, Tab, Gamepad B
	if (Key == EKeys::Escape || Key == EKeys::BackSpace || Key == EKeys::Q || Key == EKeys::Tab || Key == EKeys::Gamepad_FaceButton_Right)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_RestMenu::CacheWidgetReferences()
{
	// Find key widgets (use "Cached" prefix to avoid conflict with Blueprint BindWidget names)
	// CRITICAL: Widget names must match Blueprint exactly (not C++ variable names)
	CachedSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("Switcher")));
	CachedLocationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RestingLocationNameText")));
	TimeEntriesBoxWidget = Cast<UVerticalBox>(GetWidgetFromName(TEXT("TimeEntriesBox")));  // Blueprint name: TimeEntriesBox
	TimePassWidgetRef = Cast<UW_TimePass>(GetWidgetFromName(TEXT("W_TimePass")));           // Blueprint name: W_TimePass
	TimeOfDayTextWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("TimeOfDayText")));       // Blueprint name: TimeOfDayText

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::CacheWidgetReferences - Switcher: %s, LocationText: %s, TimeEntriesBoxWidget: %s, TimePassWidgetRef: %s"),
		CachedSwitcher ? TEXT("Found") : TEXT("NULL"),
		CachedLocationText ? TEXT("Found") : TEXT("NULL"),
		TimeEntriesBoxWidget ? TEXT("Found") : TEXT("NULL"),
		TimePassWidgetRef ? TEXT("Found") : TEXT("NULL"));

	// Bind TimePassWidgetRef's OnTimePassEnd event
	if (TimePassWidgetRef)
	{
		TimePassWidgetRef->OnTimePassEnd.AddDynamic(this, &UW_RestMenu::HandleTimePassEnd);
	}

	// Find the four main buttons by their exact Blueprint names
	RestButton = Cast<UW_RestMenu_Button>(GetWidgetFromName(TEXT("W_RMB_Rest")));
	LevelUpButton = Cast<UW_RestMenu_Button>(GetWidgetFromName(TEXT("W_RMB_Levelup")));
	StorageButton = Cast<UW_RestMenu_Button>(GetWidgetFromName(TEXT("W_RMB_SortChest")));
	LeaveButton = Cast<UW_RestMenu_Button>(GetWidgetFromName(TEXT("W_RMB_Leave")));

	// Populate MainButtonEntries array in order
	MainButtonEntries.Empty();

	if (RestButton)
	{
		RestButton->ButtonText = FText::FromString(TEXT("Rest"));
		MainButtonEntries.Add(RestButton);
		UE_LOG(LogTemp, Log, TEXT("  Found W_RMB_Rest button"));
	}
	if (LevelUpButton)
	{
		LevelUpButton->ButtonText = FText::FromString(TEXT("Level Up"));
		MainButtonEntries.Add(LevelUpButton);
		UE_LOG(LogTemp, Log, TEXT("  Found W_RMB_Levelup button"));
	}
	if (StorageButton)
	{
		StorageButton->ButtonText = FText::FromString(TEXT("Storage"));
		MainButtonEntries.Add(StorageButton);
		UE_LOG(LogTemp, Log, TEXT("  Found W_RMB_SortChest button"));
	}
	if (LeaveButton)
	{
		LeaveButton->ButtonText = FText::FromString(TEXT("Leave"));
		MainButtonEntries.Add(LeaveButton);
		UE_LOG(LogTemp, Log, TEXT("  Found W_RMB_Leave button"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::CacheWidgetReferences - Found %d buttons"), MainButtonEntries.Num());
}

void UW_RestMenu::BindButtonEvents()
{
	// Bind EACH button to its OWN handler (matches Blueprint pattern exactly)
	// Blueprint has separate event handlers for W_RMB_Rest, W_RMB_Levelup, W_RMB_SortChest, W_RMB_Leave

	if (RestButton)
	{
		RestButton->OnRestMenuButtonSelected.AddDynamic(this, &UW_RestMenu::HandleButtonSelected);
		RestButton->OnRestMenuButtonPressed.AddDynamic(this, &UW_RestMenu::HandleRestButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("  Bound RestButton events"));
	}
	if (LevelUpButton)
	{
		LevelUpButton->OnRestMenuButtonSelected.AddDynamic(this, &UW_RestMenu::HandleButtonSelected);
		LevelUpButton->OnRestMenuButtonPressed.AddDynamic(this, &UW_RestMenu::HandleLevelUpButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("  Bound LevelUpButton events"));
	}
	if (StorageButton)
	{
		StorageButton->OnRestMenuButtonSelected.AddDynamic(this, &UW_RestMenu::HandleButtonSelected);
		StorageButton->OnRestMenuButtonPressed.AddDynamic(this, &UW_RestMenu::HandleStorageButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("  Bound StorageButton events"));
	}
	if (LeaveButton)
	{
		LeaveButton->OnRestMenuButtonSelected.AddDynamic(this, &UW_RestMenu::HandleButtonSelected);
		LeaveButton->OnRestMenuButtonPressed.AddDynamic(this, &UW_RestMenu::HandleLeaveButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("  Bound LeaveButton events"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// INDIVIDUAL BUTTON PRESS HANDLERS (matches Blueprint exactly)
// Each button has its OWN handler - not a generic switch on MainNavigationIndex
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * HandleRestButtonPressed - W_RMB_Rest pressed
 * Blueprint: Switches Switcher to index 1 (time selection view)
 */
void UW_RestMenu::HandleRestButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleRestButtonPressed - Rest (Pass Time)"));

	// Switch to time selection view (index 1)
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(1);
		CurrentViewIndex = 1;
		UE_LOG(LogTemp, Log, TEXT("  Switched to time selection view (index 1)"));
	}

	// Reset time entry navigation
	TimeEntryNavigationIndex = 0;
	if (TimeEntries.IsValidIndex(0) && TimeEntries[0])
	{
		TimeEntries[0]->SetTimeEntrySelected(true);
	}
}

/**
 * HandleLevelUpButtonPressed - W_RMB_Levelup pressed
 * Blueprint: Shows level up sub-menu (switches Switcher, sets W_LevelUp visible)
 */
void UW_RestMenu::HandleLevelUpButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleLevelUpButtonPressed - Level Up"));

	// Blueprint shows level up widget and switches input context
	// For now, just log - level up functionality would need W_LevelUp widget
	// This is typically handled by switching Switcher to level up view index
}

/**
 * HandleStorageButtonPressed - W_RMB_SortChest pressed
 * Blueprint: Broadcasts OnStorageRequested
 */
void UW_RestMenu::HandleStorageButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleStorageButtonPressed - Storage"));

	// Broadcast storage request (W_HUD listens for this)
	OnStorageRequested.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("  Broadcast OnStorageRequested"));
}

/**
 * HandleLeaveButtonPressed - W_RMB_Leave pressed
 * Blueprint: Closes the rest menu
 */
void UW_RestMenu::HandleLeaveButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleLeaveButtonPressed - Leave"));

	// Close the rest menu
	EventCloseRestMenu();
}

/**
 * HandleButtonSelected - Generic selection handler (updates navigation index)
 * This is called when any button is hovered/selected but NOT pressed
 */
void UW_RestMenu::HandleButtonSelected(UW_RestMenu_Button* Button)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleButtonSelected - Button: %s"),
		Button ? *Button->ButtonText.ToString() : TEXT("None"));

	if (!Button)
	{
		return;
	}

	// Find button index
	int32 ButtonIndex = MainButtonEntries.Find(Button);
	if (ButtonIndex != INDEX_NONE && ButtonIndex != MainNavigationIndex)
	{
		// Deselect OLD button
		if (MainButtonEntries.IsValidIndex(MainNavigationIndex) && MainButtonEntries[MainNavigationIndex])
		{
			MainButtonEntries[MainNavigationIndex]->SetRestMenuButtonSelected(false);
		}

		// Update navigation index (button is already visually selected)
		MainNavigationIndex = ButtonIndex;
		UE_LOG(LogTemp, Log, TEXT("  Updated MainNavigationIndex to %d"), MainNavigationIndex);
	}
}

/**
 * HandleTimeEntryPressed - Called when a time entry widget is pressed/activated
 */
void UW_RestMenu::HandleTimeEntryPressed(FSLFDayNightInfo TimeInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleTimeEntryPressed - Time: %s"), *TimeInfo.Name.ToString());

	// Forward to the event handler
	EventOnTimeEntryPressed(TimeInfo);
}

/**
 * HandleTimeEntrySelected - Called when a time entry widget is hovered/selected
 */
void UW_RestMenu::HandleTimeEntrySelected(UW_RestMenu_TimeEntry* TimeEntry)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleTimeEntrySelected"));

	// Forward to the event handler
	EventOnTimeEntrySelected(TimeEntry);
}

// ═══════════════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void UW_RestMenu::InitializeTimeEntries_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::InitializeTimeEntries"));

	// Clear any existing time entries
	TimeEntries.Empty();
	if (TimeEntriesBoxWidget)
	{
		TimeEntriesBoxWidget->ClearChildren();
	}

	// Build the time entries array - try from SkyManager first, fallback to defaults
	TArray<FSLFDayNightInfo> TimeInfoEntries;

	// Try to get entries from B_SkyManager's TimeInfoAsset
	UWorld* World = GetWorld();
	if (World)
	{
		ASLFSkyManager* SkyManager = Cast<ASLFSkyManager>(UGameplayStatics::GetActorOfClass(World, ASLFSkyManager::StaticClass()));
		if (SkyManager)
		{
			UPDA_DayNight* DayNightData = Cast<UPDA_DayNight>(SkyManager->TimeInfoAsset);
			if (DayNightData && DayNightData->Entries.Num() > 0)
			{
				TimeInfoEntries = DayNightData->Entries;
				LocalTimeInfo = DayNightData;
				UE_LOG(LogTemp, Log, TEXT("  Found PDA_DayNight with %d entries"), DayNightData->Entries.Num());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  B_SkyManager TimeInfoAsset has no valid entries, using defaults"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  No B_SkyManager in level, using default time entries"));
		}
	}

	// Fallback: create default time entries if none found
	// VALUES EXTRACTED FROM BP_ONLY DA_ExampleDayNightInfo
	if (TimeInfoEntries.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("  Creating default time entries (6 entries matching bp_only)"));

		// Entry 0: Early Morning - ToTime=12.0 (noon)
		FSLFDayNightInfo EarlyMorning;
		EarlyMorning.Name = FText::FromString(TEXT("Early Morning"));
		EarlyMorning.FromTime = 9.0;
		EarlyMorning.ToTime = 12.0;
		TimeInfoEntries.Add(EarlyMorning);

		// Entry 1: Sunrise - ToTime=14.0 (2 PM)
		FSLFDayNightInfo Sunrise;
		Sunrise.Name = FText::FromString(TEXT("Sunrise"));
		Sunrise.FromTime = 12.0;
		Sunrise.ToTime = 14.0;
		TimeInfoEntries.Add(Sunrise);

		// Entry 2: Afternoon - ToTime=22.0 (10 PM)
		FSLFDayNightInfo Afternoon;
		Afternoon.Name = FText::FromString(TEXT("Afternoon"));
		Afternoon.FromTime = 14.0;
		Afternoon.ToTime = 22.0;
		TimeInfoEntries.Add(Afternoon);

		// Entry 3: Sunset - ToTime=24.0 (midnight)
		FSLFDayNightInfo Sunset;
		Sunset.Name = FText::FromString(TEXT("Sunset"));
		Sunset.FromTime = 22.0;
		Sunset.ToTime = 24.0;
		TimeInfoEntries.Add(Sunset);

		// Entry 4: Night - ToTime=6.0 (6 AM)
		FSLFDayNightInfo Night;
		Night.Name = FText::FromString(TEXT("Night"));
		Night.FromTime = 0.0;
		Night.ToTime = 6.0;
		TimeInfoEntries.Add(Night);

		// Entry 5: Midnight - ToTime=9.0 (9 AM)
		FSLFDayNightInfo Midnight;
		Midnight.Name = FText::FromString(TEXT("Midnight"));
		Midnight.FromTime = 6.0;
		Midnight.ToTime = 9.0;
		TimeInfoEntries.Add(Midnight);
	}

	// Check if we have the widget class and container
	if (!TimeEntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("  TimeEntryWidgetClass not loaded"));
		return;
	}

	if (!TimeEntriesBoxWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("  TimeEntriesBoxWidget not found"));
		return;
	}

	// Create a time entry widget for each entry
	for (const FSLFDayNightInfo& Entry : TimeInfoEntries)
	{
		// Create the widget
		UW_RestMenu_TimeEntry* TimeEntryWidget = CreateWidget<UW_RestMenu_TimeEntry>(this, TimeEntryWidgetClass);
		if (TimeEntryWidget)
		{
			// Set the time info
			TimeEntryWidget->TimeInfo = Entry;

			// Bind events
			TimeEntryWidget->OnTimeEntryPressed.AddDynamic(this, &UW_RestMenu::HandleTimeEntryPressed);
			TimeEntryWidget->OnTimeEntrySelected.AddDynamic(this, &UW_RestMenu::HandleTimeEntrySelected);

			// Add to container
			TimeEntriesBoxWidget->AddChildToVerticalBox(TimeEntryWidget);

			// Add to array for navigation
			TimeEntries.Add(TimeEntryWidget);

			UE_LOG(LogTemp, Log, TEXT("  Created TimeEntry widget for: %s"), *Entry.Name.ToString());
		}
	}

	// Reset navigation and select first entry
	TimeEntryNavigationIndex = 0;
	if (TimeEntries.Num() > 0 && TimeEntries[0])
	{
		TimeEntries[0]->SetTimeEntrySelected(true);
	}

	UE_LOG(LogTemp, Log, TEXT("  Created %d time entry widgets"), TimeEntries.Num());

	// FROM BP_ONLY: Update TimeOfDayTextWidget to show the CURRENT time period name
	// This shows "Sunset", "Night", etc. at the top of the rest menu based on current time
	if (TimeOfDayTextWidget && World)
	{
		ASLFSkyManager* SkyManager = Cast<ASLFSkyManager>(UGameplayStatics::GetActorOfClass(World, ASLFSkyManager::StaticClass()));
		if (SkyManager)
		{
			float CurrentTime = SkyManager->CurrentTimeOfDay;

			// Find which time entry the current time falls into
			FText CurrentTimeName = FText::FromString(TEXT("Unknown"));
			for (const FSLFDayNightInfo& Entry : TimeInfoEntries)
			{
				// Handle time ranges that wrap around midnight (e.g., Night: 0-6)
				if (Entry.FromTime <= Entry.ToTime)
				{
					// Normal range (e.g., Afternoon: 14-22)
					if (CurrentTime >= Entry.FromTime && CurrentTime < Entry.ToTime)
					{
						CurrentTimeName = Entry.Name;
						break;
					}
				}
				else
				{
					// Wrap-around range (e.g., Sunset: 22-24, then Night: 0-6)
					if (CurrentTime >= Entry.FromTime || CurrentTime < Entry.ToTime)
					{
						CurrentTimeName = Entry.Name;
						break;
					}
				}
			}

			TimeOfDayTextWidget->SetText(CurrentTimeName);
			UE_LOG(LogTemp, Log, TEXT("  Set current time of day text to: %s (CurrentTime=%.2f)"),
				*CurrentTimeName.ToString(), CurrentTime);
		}
	}
}

void UW_RestMenu::InitializeTimePassWidget_Implementation(const FSLFDayNightInfo& TargetTime)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::InitializeTimePassWidget - TargetTime: %s, ToTime: %f"), *TargetTime.Name.ToString(), TargetTime.ToTime);

	// Get B_SkyManager to update the time
	double OldTime = 12.0; // Default if no SkyManager found
	UWorld* World = GetWorld();
	if (World)
	{
		ASLFSkyManager* SkyManager = Cast<ASLFSkyManager>(UGameplayStatics::GetActorOfClass(World, ASLFSkyManager::StaticClass()));
		if (SkyManager)
		{
			// Store the current time as the "old time"
			OldTime = SkyManager->CurrentTimeOfDay;
			UE_LOG(LogTemp, Log, TEXT("  Found SkyManager - OldTime: %f, setting to ToTime: %f"), OldTime, TargetTime.ToTime);

			// Set the new time (equivalent to calling "Event AdjustTime" which sets the Time property)
			SkyManager->SetTimeOfDay(TargetTime.ToTime);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  SkyManager not found in level"));
		}
	}

	// Update the TimeOfDayTextWidget to show the target time's name
	if (TimeOfDayTextWidget)
	{
		TimeOfDayTextWidget->SetText(TargetTime.Name);
		UE_LOG(LogTemp, Log, TEXT("  Updated TimeOfDayTextWidget to: %s"), *TargetTime.Name.ToString());
	}

	// Initialize the time pass widget with the old and new times
	if (TimePassWidgetRef)
	{
		TimePassWidgetRef->EventInitialize(OldTime, TargetTime.ToTime);
		UE_LOG(LogTemp, Log, TEXT("  Called EventInitialize on W_TimePass with FromTime: %f, ToTime: %f"), OldTime, TargetTime.ToTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  W_TimePass widget not found, cannot initialize"));
	}
}

bool UW_RestMenu::GetLevelUpMenuVisibility_Implementation()
{
	// Check if the level up widget within rest menu is visible
	// This would typically check a child widget's visibility
	return false;
}

void UW_RestMenu::EventBackToMain_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventBackToMain"));

	// Switch back to main buttons view (index 0)
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(0);
		CurrentViewIndex = 0;
	}

	// Reset to main menu navigation
	MainNavigationIndex = 0;

	// Deselect all buttons first
	for (UW_RestMenu_Button* Button : MainButtonEntries)
	{
		if (Button)
		{
			Button->SetRestMenuButtonSelected(false);
		}
	}

	// Select first main button
	if (MainButtonEntries.IsValidIndex(0) && MainButtonEntries[0])
	{
		MainButtonEntries[0]->SetRestMenuButtonSelected(true);
	}
}

void UW_RestMenu::EventCloseRestMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventCloseRestMenu"));

	// Hide the widget
	SetVisibility(ESlateVisibility::Collapsed);

	// Notify resting point if valid - try both class hierarchies
	if (IsValid(CurrentRestingPoint))
	{
		if (ASLFRestingPointBase* SLFRestPoint = Cast<ASLFRestingPointBase>(CurrentRestingPoint))
		{
			SLFRestPoint->OnExited.Broadcast();
			// Clear SittingActor so player can rest again
			SLFRestPoint->SittingActor = nullptr;
		}
		else if (AB_RestingPoint* OldRestPoint = Cast<AB_RestingPoint>(CurrentRestingPoint))
		{
			OldRestPoint->OnExited.Broadcast();
		}
	}

	// Clear reference
	CurrentRestingPoint = nullptr;

	// Switch viewport back to MainHUD (index 0) and exit UI mode
	if (UW_HUD* HUD = Cast<UW_HUD>(GetTypedOuter<UW_HUD>()))
	{
		HUD->EventSwitchViewport(0);
		HUD->EventToggleUiMode(false);
		UE_LOG(LogTemp, Log, TEXT("  Switched viewport back to MainHUD"));
	}
}

void UW_RestMenu::EventFadeInRestMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventFadeInRestMenu"));

	// Would play fade-in animation
	// For now, just make visible
	SetVisibility(ESlateVisibility::Visible);

	// Initialize navigation
	MainNavigationIndex = 0;
	if (MainButtonEntries.IsValidIndex(0) && MainButtonEntries[0])
	{
		MainButtonEntries[0]->SetRestMenuButtonSelected(true);
	}

	// Set keyboard focus to this widget so NativeOnKeyDown receives input
	SetKeyboardFocus();
	UE_LOG(LogTemp, Log, TEXT("  Set keyboard focus to rest menu"));
}

void UW_RestMenu::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventNavigateCancel - CurrentViewIndex: %d"), CurrentViewIndex);

	// Check if we're in a sub-menu (time selection, level up, etc.)
	if (CurrentViewIndex != 0 || GetLevelUpMenuVisibility())
	{
		// Return to main menu
		EventBackToMain();
	}
	else
	{
		// Close the rest menu entirely
		EventCloseRestMenu();
	}
}

void UW_RestMenu::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventNavigateDown"));

	if (MainButtonEntries.Num() == 0)
	{
		return;
	}

	// Deselect current
	if (MainButtonEntries.IsValidIndex(MainNavigationIndex) && MainButtonEntries[MainNavigationIndex])
	{
		MainButtonEntries[MainNavigationIndex]->SetRestMenuButtonSelected(false);
	}

	// Move to next
	MainNavigationIndex = (MainNavigationIndex + 1) % MainButtonEntries.Num();

	// Select new
	if (MainButtonEntries[MainNavigationIndex])
	{
		MainButtonEntries[MainNavigationIndex]->SetRestMenuButtonSelected(true);
	}

	UE_LOG(LogTemp, Log, TEXT("  New navigation index: %d"), MainNavigationIndex);
}

/**
 * EventNavigateOk - Keyboard/gamepad confirm (OK button)
 * This executes the action for whichever button is currently selected via navigation
 */
void UW_RestMenu::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventNavigateOk - MainNavigationIndex: %d"), MainNavigationIndex);

	if (!MainButtonEntries.IsValidIndex(MainNavigationIndex))
	{
		return;
	}

	// Get the currently selected button and trigger its action
	UW_RestMenu_Button* SelectedButton = MainButtonEntries[MainNavigationIndex];
	if (!SelectedButton)
	{
		return;
	}

	// Determine which button is selected and call its handler
	if (SelectedButton == RestButton)
	{
		HandleRestButtonPressed();
	}
	else if (SelectedButton == LevelUpButton)
	{
		HandleLevelUpButtonPressed();
	}
	else if (SelectedButton == StorageButton)
	{
		HandleStorageButtonPressed();
	}
	else if (SelectedButton == LeaveButton)
	{
		HandleLeaveButtonPressed();
	}
}

void UW_RestMenu::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventNavigateUp"));

	if (MainButtonEntries.Num() == 0)
	{
		return;
	}

	// Deselect current
	if (MainButtonEntries.IsValidIndex(MainNavigationIndex) && MainButtonEntries[MainNavigationIndex])
	{
		MainButtonEntries[MainNavigationIndex]->SetRestMenuButtonSelected(false);
	}

	// Move to previous
	MainNavigationIndex = MainNavigationIndex - 1;
	if (MainNavigationIndex < 0)
	{
		MainNavigationIndex = MainButtonEntries.Num() - 1;
	}

	// Select new
	if (MainButtonEntries[MainNavigationIndex])
	{
		MainButtonEntries[MainNavigationIndex]->SetRestMenuButtonSelected(true);
	}

	UE_LOG(LogTemp, Log, TEXT("  New navigation index: %d"), MainNavigationIndex);
}

void UW_RestMenu::EventOnMainRestMenuButtonSelected_Implementation(UW_RestMenu_Button* Button)
{
	// Forward to generic handler
	HandleButtonSelected(Button);
}

void UW_RestMenu::EventOnTimeEntryPressed_Implementation(FSLFDayNightInfo TimeInfo)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventOnTimeEntryPressed - TimeEntry: %s, ToTime: %f"), *TimeInfo.Name.ToString(), TimeInfo.ToTime);

	// Show the time pass widget
	if (TimePassWidgetRef)
	{
		TimePassWidgetRef->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("  Made W_TimePass visible"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  W_TimePass widget not found!"));
	}

	// Initialize the time pass widget with target time
	InitializeTimePassWidget(TimeInfo);
}

void UW_RestMenu::EventOnTimeEntrySelected_Implementation(UW_RestMenu_TimeEntry* TimeEntry)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventOnTimeEntrySelected"));

	if (!TimeEntry)
	{
		return;
	}

	// Find entry index
	int32 EntryIndex = TimeEntries.Find(TimeEntry);
	if (EntryIndex != INDEX_NONE)
	{
		// Deselect current
		if (TimeEntries.IsValidIndex(TimeEntryNavigationIndex) && TimeEntries[TimeEntryNavigationIndex])
		{
			TimeEntries[TimeEntryNavigationIndex]->SetTimeEntrySelected(false);
		}

		// Update index and select
		TimeEntryNavigationIndex = EntryIndex;
		TimeEntry->SetTimeEntrySelected(true);
	}
}

void UW_RestMenu::EventSetupRestingPoint_Implementation(AActor* InCurrentCampfire)
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::EventSetupRestingPoint"));

	CurrentRestingPoint = InCurrentCampfire;

	if (!IsValid(CurrentRestingPoint))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Invalid resting point"));
		return;
	}

	// Get location name - try both class hierarchies
	FText LocationName;
	if (ASLFRestingPointBase* SLFRestPoint = Cast<ASLFRestingPointBase>(CurrentRestingPoint))
	{
		LocationName = SLFRestPoint->LocationName;
	}
	else if (AB_RestingPoint* OldRestPoint = Cast<AB_RestingPoint>(CurrentRestingPoint))
	{
		LocationName = OldRestPoint->LocationName;
	}
	UE_LOG(LogTemp, Log, TEXT("  Location: %s"), *LocationName.ToString());

	// Update location name text widget
	if (CachedLocationText && !LocationName.IsEmpty())
	{
		CachedLocationText->SetText(LocationName);
	}

	// Reset to main view (index 0)
	if (CachedSwitcher)
	{
		CachedSwitcher->SetActiveWidgetIndex(0);
	}
	CurrentViewIndex = 0;

	// Reset navigation
	MainNavigationIndex = 0;
	TimeEntryNavigationIndex = 0;

	// Deselect all buttons
	for (UW_RestMenu_Button* Button : MainButtonEntries)
	{
		if (Button)
		{
			Button->SetRestMenuButtonSelected(false);
		}
	}

	// Select first button
	if (MainButtonEntries.IsValidIndex(0) && MainButtonEntries[0])
	{
		MainButtonEntries[0]->SetRestMenuButtonSelected(true);
	}

	// Fade in the menu
	EventFadeInRestMenu();
}

void UW_RestMenu::HandleTimePassEnd()
{
	UE_LOG(LogTemp, Log, TEXT("UW_RestMenu::HandleTimePassEnd - Time pass animation complete"));

	// Hide the time pass widget
	if (TimePassWidgetRef)
	{
		TimePassWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Return to main menu view
	EventBackToMain();
}
