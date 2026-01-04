// W_GameMenu_Button.cpp
// C++ Widget implementation for W_GameMenu_Button
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_GameMenu_Button.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UW_GameMenu_Button::UW_GameMenu_Button(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ImageColor(FLinearColor::White)
	, SelectedImageColor(FLinearColor::White)
	, ImageBorderColor(FLinearColor::White)
	, SelectedImageBorderColor(FLinearColor::White)
	, HighlightColor(FLinearColor::White)
	, Selected(false)
	, EscMenuButton(nullptr)
	, SelectedBorder(nullptr)
	, ButtonIcon(nullptr)
	, ButtonLabel(nullptr)
{
}

void UW_GameMenu_Button::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Apply visual configuration from exposed properties
	ApplyVisualConfig();
}

void UW_GameMenu_Button::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GameMenu_Button::NativeConstruct"));

	// Bind button events
	if (EscMenuButton)
	{
		EscMenuButton->OnHovered.AddDynamic(this, &UW_GameMenu_Button::OnButtonHovered);
		EscMenuButton->OnPressed.AddDynamic(this, &UW_GameMenu_Button::OnButtonPressed);
		UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] Button events bound"));
	}

	// Initially not selected
	SetGameMenuButtonSelected(false);
}

void UW_GameMenu_Button::NativeDestruct()
{
	// Unbind button events
	if (EscMenuButton)
	{
		EscMenuButton->OnHovered.RemoveDynamic(this, &UW_GameMenu_Button::OnButtonHovered);
		EscMenuButton->OnPressed.RemoveDynamic(this, &UW_GameMenu_Button::OnButtonPressed);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GameMenu_Button::NativeDestruct"));
}

void UW_GameMenu_Button::ApplyVisualConfig()
{
	// Set button text if label widget exists
	if (ButtonLabel)
	{
		ButtonLabel->SetText(ButtonText);
	}

	// Set button image if icon widget exists and texture is valid
	if (ButtonIcon && !ButtonImage.IsNull())
	{
		if (UTexture2D* Texture = ButtonImage.LoadSynchronous())
		{
			ButtonIcon->SetBrushFromTexture(Texture);
			ButtonIcon->SetColorAndOpacity(ImageColor);
		}
	}

	// Set border color
	if (SelectedBorder)
	{
		SelectedBorder->SetBrushColor(ImageBorderColor);
		SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_GameMenu_Button::SetGameMenuButtonSelected_Implementation(bool InSelected)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] SetGameMenuButtonSelected: %s"), InSelected ? TEXT("true") : TEXT("false"));

	// Update selected state
	Selected = InSelected;

	if (SelectedBorder)
	{
		if (Selected)
		{
			// Show selected border with selected color
			SelectedBorder->SetVisibility(ESlateVisibility::Visible);
			SelectedBorder->SetBrushColor(SelectedImageBorderColor);
		}
		else
		{
			// Hide selected border
			SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);
			SelectedBorder->SetBrushColor(ImageBorderColor);
		}
	}

	// Update image color if applicable
	if (ButtonIcon)
	{
		ButtonIcon->SetColorAndOpacity(Selected ? SelectedImageColor : ImageColor);
	}

	// Broadcast selection changed
	OnGameMenuButtonSelected.Broadcast(this);
}

void UW_GameMenu_Button::EventOnGameMenuButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] EventOnGameMenuButtonPressed - Tag: %s"), *TargetWidgetTag.ToString());

	// Broadcast the pressed event with the target widget tag
	OnGameMenuButtonPressed.Broadcast(TargetWidgetTag);
}

void UW_GameMenu_Button::OnButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] OnButtonHovered"));

	// When hovered, set this button as selected
	SetGameMenuButtonSelected(true);
}

void UW_GameMenu_Button::OnButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] OnButtonPressed"));

	// Trigger the button pressed event
	EventOnGameMenuButtonPressed();
}
