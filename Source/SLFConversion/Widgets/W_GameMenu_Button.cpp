// W_GameMenu_Button.cpp
// C++ Widget implementation for W_GameMenu_Button
//
// 20-PASS VALIDATION: 2026-01-05
// Source: BlueprintDNA_v2/WidgetBlueprint/W_GameMenu_Button.json
//
// PreConstruct flow from Blueprint:
//   1. BtnText.SetText(ButtonText)
//   2. Img.SetBrushFromSoftTexture(ButtonImage, false)
//   3. Img.SetColorAndOpacity(ImageColor)
//   4. ImgBorder.SetBrushColor(ImageBorderColor)
//
// SetGameMenuButtonSelected flow from Blueprint:
//   1. Set Selected? = bSelected
//   2. Branch on Selected?:
//      TRUE:  Img.SetColorAndOpacity(SelectedImageColor)
//             ImgBorder.SetBrushColor(SelectedImageBorderColor)
//             Broadcast OnGameMenuButtonSelected(self)
//      FALSE: Img.SetColorAndOpacity(ImageColor)
//             ImgBorder.SetBrushColor(ImageBorderColor)

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
	, ImgBorder(nullptr)
	, Img(nullptr)
	, BtnText(nullptr)
{
}

void UW_GameMenu_Button::NativePreConstruct()
{
	Super::NativePreConstruct();

	// PreConstruct: Apply visual configuration from per-instance properties
	// This matches the Blueprint PreConstruct event graph exactly
	ApplyVisualConfig();
}

void UW_GameMenu_Button::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GameMenu_Button::NativeConstruct"));

	// Bind button events (from EventGraph: On Pressed, On Hovered)
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
	// From Blueprint PreConstruct:
	// 1. BtnText.SetText(ButtonText)
	if (BtnText)
	{
		BtnText->SetText(ButtonText);
		UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] Set text to: %s"), *ButtonText.ToString());
	}

	// 2. Img.SetBrushFromSoftTexture(ButtonImage, bMatchSize=false)
	// 3. Img.SetColorAndOpacity(ImageColor)
	if (Img)
	{
		if (!ButtonImage.IsNull())
		{
			Img->SetBrushFromSoftTexture(ButtonImage, false);
		}
		Img->SetColorAndOpacity(ImageColor);
	}

	// 4. ImgBorder.SetBrushColor(ImageBorderColor)
	if (ImgBorder)
	{
		ImgBorder->SetBrushColor(ImageBorderColor);
	}
}

void UW_GameMenu_Button::SetGameMenuButtonSelected_Implementation(bool bSelected)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] SetGameMenuButtonSelected: %s"), bSelected ? TEXT("true") : TEXT("false"));

	// 1. Set Selected? = bSelected
	Selected = bSelected;

	// 2. Branch on Selected?
	if (Selected)
	{
		// TRUE branch:
		// Img.SetColorAndOpacity(SelectedImageColor)
		if (Img)
		{
			Img->SetColorAndOpacity(SelectedImageColor);
		}

		// ImgBorder.SetBrushColor(SelectedImageBorderColor)
		if (ImgBorder)
		{
			ImgBorder->SetBrushColor(SelectedImageBorderColor);
		}

		// Broadcast OnGameMenuButtonSelected(self)
		OnGameMenuButtonSelected.Broadcast(this);
	}
	else
	{
		// FALSE branch:
		// Img.SetColorAndOpacity(ImageColor)
		if (Img)
		{
			Img->SetColorAndOpacity(ImageColor);
		}

		// ImgBorder.SetBrushColor(ImageBorderColor)
		if (ImgBorder)
		{
			ImgBorder->SetBrushColor(ImageBorderColor);
		}
	}
}

void UW_GameMenu_Button::EventOnGameMenuButtonPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] EventOnGameMenuButtonPressed - Tag: %s"), *TargetWidgetTag.ToString());

	// From Blueprint: Broadcast OnGameMenuButtonPressed(TargetWidgetTag)
	OnGameMenuButtonPressed.Broadcast(TargetWidgetTag);
}

void UW_GameMenu_Button::OnButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] OnButtonHovered"));

	// From Blueprint EventGraph: On Hovered (EscMenuButton) -> SetGameMenuButtonSelected(true)
	SetGameMenuButtonSelected(true);
}

void UW_GameMenu_Button::OnButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu_Button] OnButtonPressed"));

	// From Blueprint EventGraph: On Pressed (EscMenuButton) -> Call OnGameMenuButtonPressed
	EventOnGameMenuButtonPressed();
}
