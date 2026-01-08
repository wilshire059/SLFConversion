// W_GenericButton.cpp
// C++ Widget implementation for W_GenericButton
//
// 20-PASS VALIDATION: 2026-01-08

#include "Widgets/W_GenericButton.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

UW_GenericButton::UW_GenericButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_GenericButton::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::NativeConstruct"));
}

void UW_GenericButton::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::NativeDestruct"));
}

void UW_GenericButton::CacheWidgetReferences()
{
	// Cache border and text widgets for visual updates
}

void UW_GenericButton::SetButtonSelected_Implementation(bool InSelected)
{
	// Update Selected state
	Selected = InSelected;

	// Update ButtonBorder color based on selection
	if (UBorder* ButtonBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ButtonBorder"))))
	{
		FLinearColor BrushColor = InSelected ? SelectedButtonColor : ButtonColor;
		ButtonBorder->SetBrushColor(BrushColor);
	}

	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::SetButtonSelected - Selected: %s"), InSelected ? TEXT("true") : TEXT("false"));
}

void UW_GenericButton::EventPressButton_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::EventPressButton_Implementation"));

	// Broadcast OnButtonPressed event
	OnButtonPressed.Broadcast();
}

void UW_GenericButton::EventSetButtonText_Implementation(const FText& InText)
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::EventSetButtonText_Implementation - %s"), *InText.ToString());

	// Update the Text property
	Text = InText;

	// Update the TextBlock widget if it exists
	if (UTextBlock* ButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ButtonText"))))
	{
		ButtonText->SetText(InText);
	}
}
