// W_Inventory_ActionButton.cpp
// C++ Widget implementation for W_Inventory_ActionButton
//
// 20-PASS VALIDATION: 2026-01-08

#include "Widgets/W_Inventory_ActionButton.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UW_Inventory_ActionButton::UW_Inventory_ActionButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_Inventory_ActionButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Set the button text from ActionText property (originally in Blueprint PreConstruct)
	// Widget is named "Txt" in UMG designer, not "BtnText"
	if (UTextBlock* TxtWidget = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt"))))
	{
		TxtWidget->SetText(ActionText);
	}
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
	// Find and bind the UseButton widget from Blueprint UMG designer
	// bp_only: UseButton.OnPressed → Call OnActionButtonPressed
	// bp_only: UseButton.OnHovered → SetActionButtonSelected(true)
	// NOTE: Variable is named CachedUseButton to avoid collision with Blueprint-generated property
	CachedUseButton = Cast<UButton>(GetWidgetFromName(TEXT("UseButton")));
	if (CachedUseButton)
	{
		CachedUseButton->OnClicked.AddUniqueDynamic(this, &UW_Inventory_ActionButton::OnUseButtonClicked);
		CachedUseButton->OnHovered.AddUniqueDynamic(this, &UW_Inventory_ActionButton::OnUseButtonHovered);
		UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::CacheWidgetReferences - Bound UseButton events for %s"), *ActionText.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_Inventory_ActionButton::CacheWidgetReferences - UseButton not found!"));
	}
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

void UW_Inventory_ActionButton::OnUseButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::OnUseButtonClicked - %s"), *ActionText.ToString());
	// bp_only: On UseButton Pressed → Call OnActionButtonPressed
	EventActionPressed();
}

void UW_Inventory_ActionButton::OnUseButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Inventory_ActionButton::OnUseButtonHovered - %s"), *ActionText.ToString());
	// bp_only: On UseButton Hovered → SetActionButtonSelected(true)
	SetActionButtonSelected(true);
}
