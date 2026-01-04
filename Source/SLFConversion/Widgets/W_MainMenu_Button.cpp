// W_MainMenu_Button.cpp
// C++ Widget implementation for W_MainMenu_Button
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_MainMenu_Button.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"

UW_MainMenu_Button::UW_MainMenu_Button(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Width(0.0f)
	, Selected(false)
	, Sizer(nullptr)
	, SelectedBorder(nullptr)
	, Button(nullptr)
	, Loadup(nullptr)
{
}

void UW_MainMenu_Button::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Set width override on the sizer based on Width variable
	if (Sizer)
	{
		Sizer->SetWidthOverride(Width);
	}
}

void UW_MainMenu_Button::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_MainMenu_Button::NativeConstruct"));

	// Start hidden - will be made visible via PlayButtonInitAnimation
	SetVisibility(ESlateVisibility::Hidden);

	// Bind button events
	if (Button)
	{
		Button->OnHovered.AddDynamic(this, &UW_MainMenu_Button::OnButtonHovered);
		Button->OnClicked.AddDynamic(this, &UW_MainMenu_Button::OnButtonClicked_Internal);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu_Button] Button events bound"));
	}

	// Initially hide the selected border
	if (SelectedBorder)
	{
		SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_MainMenu_Button::NativeDestruct()
{
	// Unbind button events
	if (Button)
	{
		Button->OnHovered.RemoveDynamic(this, &UW_MainMenu_Button::OnButtonHovered);
		Button->OnClicked.RemoveDynamic(this, &UW_MainMenu_Button::OnButtonClicked_Internal);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_MainMenu_Button::NativeDestruct"));
}

void UW_MainMenu_Button::SetMenuButtonSelected_Implementation(bool InSelected)
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu_Button] SetMenuButtonSelected: %s"), InSelected ? TEXT("true") : TEXT("false"));

	// Set the selected state
	Selected = InSelected;

	// Update SelectedBorder visibility based on selection state
	if (SelectedBorder)
	{
		if (Selected)
		{
			SelectedBorder->SetVisibility(ESlateVisibility::Visible);
			// Broadcast the OnSelected event
			OnSelected.Broadcast();
		}
		else
		{
			SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UW_MainMenu_Button::PlayButtonInitAnimation_Implementation(double StartTime, int32 Loops, EUMGSequencePlayMode::Type Mode, double PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu_Button] PlayButtonInitAnimation (StartTime: %.2f, Loops: %d, PlayRate: %.2f)"), StartTime, Loops, PlayRate);

	// Play the Loadup animation
	if (Loadup)
	{
		PlayAnimation(Loadup, StartTime, Loops, Mode, PlayRate);
	}

	// Make the button visible
	SetVisibility(ESlateVisibility::Visible);
}

void UW_MainMenu_Button::OnMenuButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu_Button] OnMenuButtonPressed"));

	// Broadcast the OnButtonClicked event
	OnButtonClicked.Broadcast();
}

void UW_MainMenu_Button::OnButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu_Button] OnButtonHovered"));

	// When hovered, set this button as selected
	SetMenuButtonSelected(true);
}

void UW_MainMenu_Button::OnButtonClicked_Internal()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu_Button] OnButtonClicked_Internal"));

	// Broadcast the OnButtonClicked event
	OnButtonClicked.Broadcast();
}
