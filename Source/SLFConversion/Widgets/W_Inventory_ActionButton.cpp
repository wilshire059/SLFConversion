// W_Inventory_ActionButton.cpp
// C++ Widget implementation for W_Inventory_ActionButton
//
// 20-PASS VALIDATION: 2026-01-08

#include "Widgets/W_Inventory_ActionButton.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

UW_Inventory_ActionButton::UW_Inventory_ActionButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Inventory_ActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::NativeConstruct"));
}

void UW_Inventory_ActionButton::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::NativeDestruct"));
}

void UW_Inventory_ActionButton::CacheWidgetReferences()
{
	// Cache border and text widgets for visual updates
}

void UW_Inventory_ActionButton::SetActionButtonSelected_Implementation(bool InSelected)
{
	// Update Selected state
	Selected = InSelected;

	// Update Btn Border color based on selection
	if (UBorder* BtnBorder = Cast<UBorder>(GetWidgetFromName(TEXT("Btn"))))
	{
		FLinearColor BrushColor = InSelected ? SelectedColor : Color;
		BtnBorder->SetBrushColor(BrushColor);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::SetActionButtonSelected - Selected: %s"), InSelected ? TEXT("true") : TEXT("false"));
}

void UW_Inventory_ActionButton::EventActionPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::EventActionPressed_Implementation"));

	// Broadcast OnActionButtonPressed event
	OnActionButtonPressed.Broadcast();
}
