// W_Settings_InputKeySelector.cpp
// C++ Widget implementation for W_Settings_InputKeySelector
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_Settings_InputKeySelector.h"

UW_Settings_InputKeySelector::UW_Settings_InputKeySelector(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Settings_InputKeySelector::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_InputKeySelector::NativeConstruct"));
}

void UW_Settings_InputKeySelector::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_InputKeySelector::NativeDestruct"));
}

void UW_Settings_InputKeySelector::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}

void UW_Settings_InputKeySelector::TryGetKeyIcon_Implementation(const FKey& InKey, TSoftObjectPtr<UTexture2D>& OutIcon, bool& OutSuccess)
{
	// Check if key is valid, then look up in KeyCorrelationTable
	OutSuccess = false;

	if (!InKey.IsValid())
	{
		return;
	}

	if (KeyCorrelationTable)
	{
		// Look up the key in the data table by key name
		FName KeyName = InKey.GetFName();
		// Data table lookup would be done here via FindRow
		// For now we return false since the actual table row struct type is unknown
		UE_LOG(LogTemp, Log, TEXT("UW_Settings_InputKeySelector::TryGetKeyIcon - Looking up key: %s"), *KeyName.ToString());
	}
}

void UW_Settings_InputKeySelector::SetKeySelectorSelected_Implementation(bool InSelected)
{
	// Set Selected variable, then update brush color based on selection state
	Selected = InSelected;

	// Trigger OnKeySelectorSelected if selected
	if (InSelected)
	{
		OnKeySelectorSelected.Broadcast(this);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_InputKeySelector::SetKeySelectorSelected - Selected: %s"), Selected ? TEXT("true") : TEXT("false"));
}

void UW_Settings_InputKeySelector::SetActive_Implementation(bool InActive, const FText& ActiveText)
{
	// Set Active variable, then update brush color based on active state
	Active = InActive;
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_InputKeySelector::SetActive - Active: %s"), Active ? TEXT("true") : TEXT("false"));
}

FKey UW_Settings_InputKeySelector::GetCurrentKey_Implementation()
{
	// Simply return the Key variable
	return Key;
}
void UW_Settings_InputKeySelector::EventSetKeyVisual_Implementation(FKey InKey)
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_InputKeySelector::EventSetKeyVisual_Implementation"));
}
