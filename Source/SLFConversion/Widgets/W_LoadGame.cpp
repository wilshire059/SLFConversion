// W_LoadGame.cpp
// C++ Widget implementation for W_LoadGame
//
// 20-PASS VALIDATION: 2026-01-07
// Source: BlueprintDNA/WidgetBlueprint/W_LoadGame.json

#include "Widgets/W_LoadGame.h"
#include "Widgets/W_LoadGame_Entry.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/BPI_GameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

UW_LoadGame::UW_LoadGame(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NavigationIndex = 0;
	SaveSlotsScrollBox = nullptr;
}

void UW_LoadGame::NativeConstruct()
{
	Super::NativeConstruct();

	// CRITICAL: Hide MainBlur widget - it's a BackgroundBlur at highest ZOrder that blurs everything
	if (UWidget* MainBlur = GetWidgetFromName(TEXT("MainBlur")))
	{
		MainBlur->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_LoadGame] Hidden MainBlur widget"));
	}

	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::NativeConstruct"));

	// Initialize save slots on construct
	InitializeSaveSlots();
}

void UW_LoadGame::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::NativeDestruct"));
}

void UW_LoadGame::InitializeSaveSlots_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::InitializeSaveSlots"));

	// Clear existing entries
	if (IsValid(SaveSlotsScrollBox))
	{
		SaveSlotsScrollBox->ClearChildren();
	}
	LoadSlotEntries.Empty();
	NavigationIndex = 0;

	// Get game instance
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No game instance available"));
		return;
	}

	// Check if game instance implements BPI_GameInstance
	if (!GameInstance->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("  Game instance does not implement BPI_GameInstance"));
		return;
	}

	// Get all save slots from game instance
	TArray<FString> SaveSlots;
	IBPI_GameInstance::Execute_GetAllSaveSlots(GameInstance, SaveSlots);

	UE_LOG(LogTemp, Log, TEXT("  Found %d save slots"), SaveSlots.Num());

	// Create entry widget for each save slot
	for (const FString& SlotName : SaveSlots)
	{
		// Try to create the entry widget
		UW_LoadGame_Entry* NewEntry = nullptr;

		if (LoadGameEntryClass)
		{
			// Use configured class
			NewEntry = CreateWidget<UW_LoadGame_Entry>(GetOwningPlayer(), LoadGameEntryClass);
		}
		else
		{
			// Try to load Blueprint class
			UClass* EntryClass = LoadClass<UW_LoadGame_Entry>(
				nullptr,
				TEXT("/Game/SoulslikeFramework/Widgets/MainMenu/W_LoadGame_Entry.W_LoadGame_Entry_C")
			);

			if (EntryClass)
			{
				NewEntry = CreateWidget<UW_LoadGame_Entry>(GetOwningPlayer(), EntryClass);
			}
		}

		if (!IsValid(NewEntry))
		{
			UE_LOG(LogTemp, Warning, TEXT("  Failed to create entry widget for slot: %s"), *SlotName);
			continue;
		}

		// Initialize the entry with slot name
		NewEntry->SetSaveSlotName(SlotName);

		// Add to array
		LoadSlotEntries.Add(NewEntry);

		// Add to scroll box
		if (IsValid(SaveSlotsScrollBox))
		{
			SaveSlotsScrollBox->AddChild(NewEntry);
		}

		// Bind to selection event
		NewEntry->OnSaveSlotSelected.AddDynamic(this, &UW_LoadGame::EventOnSaveSlotSelected);

		UE_LOG(LogTemp, Log, TEXT("  Added entry for slot: %s"), *SlotName);
	}

	// Update selection visuals
	UpdateSelectionVisuals();
}

void UW_LoadGame::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::EventNavigateCancel"));

	// Broadcast close event
	OnLoadGameClosed.Broadcast();
}

void UW_LoadGame::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::EventNavigateDown"));

	if (LoadSlotEntries.Num() == 0)
	{
		return;
	}

	// Move to next entry (wrap around)
	NavigationIndex = (NavigationIndex + 1) % LoadSlotEntries.Num();

	// Update visuals
	UpdateSelectionVisuals();

	// Scroll to selected entry
	if (IsValid(SaveSlotsScrollBox) && LoadSlotEntries.IsValidIndex(NavigationIndex))
	{
		SaveSlotsScrollBox->ScrollWidgetIntoView(LoadSlotEntries[NavigationIndex]);
	}
}

void UW_LoadGame::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::EventNavigateUp"));

	if (LoadSlotEntries.Num() == 0)
	{
		return;
	}

	// Move to previous entry (wrap around)
	NavigationIndex = (NavigationIndex - 1 + LoadSlotEntries.Num()) % LoadSlotEntries.Num();

	// Update visuals
	UpdateSelectionVisuals();

	// Scroll to selected entry
	if (IsValid(SaveSlotsScrollBox) && LoadSlotEntries.IsValidIndex(NavigationIndex))
	{
		SaveSlotsScrollBox->ScrollWidgetIntoView(LoadSlotEntries[NavigationIndex]);
	}
}

void UW_LoadGame::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::EventNavigateOk - Index: %d"), NavigationIndex);

	// Confirm selection on current entry
	if (LoadSlotEntries.IsValidIndex(NavigationIndex))
	{
		UW_LoadGame_Entry* SelectedEntry = LoadSlotEntries[NavigationIndex];
		if (IsValid(SelectedEntry))
		{
			// Trigger the entry's selection
			EventOnSaveSlotSelected(SelectedEntry);
		}
	}
}

void UW_LoadGame::EventOnSaveSlotSelected_Implementation(UW_LoadGame_Entry* Card)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::EventOnSaveSlotSelected"));

	if (!IsValid(Card))
	{
		return;
	}

	// Get slot name from entry
	FString SlotName = Card->GetSaveSlotName();
	UE_LOG(LogTemp, Log, TEXT("  Selected slot: %s"), *SlotName);

	// Set active slot in game instance
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (IsValid(GameInstance) && GameInstance->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		IBPI_GameInstance::Execute_SetActiveSlot(GameInstance, SlotName);
		UE_LOG(LogTemp, Log, TEXT("  Set active slot: %s"), *SlotName);
	}

	// Broadcast slot confirmed - the main menu will handle the actual level load
	OnSaveSlotConfirmed.Broadcast();
}

void UW_LoadGame::EventOnVisibilityChanged_Implementation(uint8 InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame::EventOnVisibilityChanged - %d"), InVisibility);

	// Refresh slots when becoming visible
	if (InVisibility == static_cast<uint8>(ESlateVisibility::Visible) ||
		InVisibility == static_cast<uint8>(ESlateVisibility::SelfHitTestInvisible))
	{
		InitializeSaveSlots();
	}
}

void UW_LoadGame::UpdateSelectionVisuals()
{
	// Update each entry's selection state
	for (int32 i = 0; i < LoadSlotEntries.Num(); ++i)
	{
		if (IsValid(LoadSlotEntries[i]))
		{
			bool bIsSelected = (i == NavigationIndex);
			LoadSlotEntries[i]->SetSelected(bIsSelected);
		}
	}
}
