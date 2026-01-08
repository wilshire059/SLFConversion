// W_LoadGame_Entry.cpp
// C++ Widget implementation for W_LoadGame_Entry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_LoadGame_Entry.h"

UW_LoadGame_Entry::UW_LoadGame_Entry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_LoadGame_Entry::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::NativeConstruct"));
}

void UW_LoadGame_Entry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::NativeDestruct"));
}

void UW_LoadGame_Entry::CacheWidgetReferences()
{
	// TODO: Cache any widget references needed for logic
}

void UW_LoadGame_Entry::SetSaveSlotSelected_Implementation(bool InSelected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::SetSaveSlotSelected - Selected: %s"), InSelected ? TEXT("true") : TEXT("false"));

	// Update selection state
	Selected = InSelected;

	// Visual update would happen in Blueprint or through additional logic
}

void UW_LoadGame_Entry::EventOnLoadEntryPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::EventOnLoadEntryPressed - Slot: %s"), *SaveSlotName);

	// Broadcast selection event
	OnSaveSlotSelected.Broadcast(this);
}

void UW_LoadGame_Entry::SetSaveSlotName(const FString& InSlotName)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::SetSaveSlotName - %s"), *InSlotName);

	SaveSlotName = InSlotName;

	// Would update display text here if there was a text widget
}

void UW_LoadGame_Entry::SetSelected(bool bInSelected)
{
	UE_LOG(LogTemp, Log, TEXT("UW_LoadGame_Entry::SetSelected - %s"), bInSelected ? TEXT("true") : TEXT("false"));

	Selected = bInSelected;

	// Call the BlueprintNativeEvent version for any Blueprint overrides
	SetSaveSlotSelected(bInSelected);
}
