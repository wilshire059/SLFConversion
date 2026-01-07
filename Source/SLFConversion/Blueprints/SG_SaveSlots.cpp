// SG_SaveSlots.cpp
// C++ implementation for Blueprint SG_SaveSlots
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/SG_SaveSlots.json

#include "Blueprints/SG_SaveSlots.h"

USG_SaveSlots::USG_SaveSlots()
{
}

void USG_SaveSlots::AddSlot_Implementation(const FString& SlotName)
{
	Slots.AddUnique(SlotName);
	LastSavedSlot = SlotName;
}

FString USG_SaveSlots::GetLastSaveSlot_Implementation()
{
	return LastSavedSlot;
}

TArray<FString> USG_SaveSlots::GetAllSlots_Implementation()
{
	return Slots;
}
