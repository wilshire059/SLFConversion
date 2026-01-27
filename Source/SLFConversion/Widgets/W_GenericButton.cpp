// W_GenericButton.cpp
// C++ Widget implementation for W_GenericButton
//
// 20-PASS VALIDATION: 2026-01-08

#include "Widgets/W_GenericButton.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Styling/SlateBrush.h"

UW_GenericButton::UW_GenericButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedButton = nullptr;
	Selected = false;

	// Default colors matching Blueprint CDO (bp_only values)
	ButtonColor = FLinearColor(0.012f, 0.012f, 0.012f, 0.902f);
	SelectedButtonColor = FLinearColor(0.075f, 0.075f, 0.075f, 0.902f);

	// Default outline colors matching Blueprint CDO
	OutlineColor = FLinearColor(0.376f, 0.325f, 0.271f, 1.0f);
	SelectedOutlineColor = FLinearColor(0.859f, 0.745f, 0.620f, 1.0f);
	OutlineWidth = 2.0;
	OutlineCornerRadii = FVector4(1.0, 1.0, 1.0, 1.0);

	// Default text color matching Blueprint CDO
	TextColor = FSlateColor(FLinearColor(0.859f, 0.745f, 0.620f, 1.0f));
	TextPadding = FMargin(8.0f, 5.0f, 8.0f, 5.0f);
}

void UW_GenericButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::NativePreConstruct - %s (DesignTime=%s)"),
		*GetName(), IsDesignTime() ? TEXT("true") : TEXT("false"));

	// Apply styling to ButtonBorder - EXACT Blueprint PreConstruct pattern:
	// 1. SetBrushColor with ButtonColor
	// 2. Read existing Background brush from ButtonBorder
	// 3. Create new brush preserving existing settings, but with custom OutlineSettings
	// 4. SetBrush with the new brush
	if (UBorder* ButtonBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ButtonBorder"))))
	{
		UE_LOG(LogTemp, Log, TEXT("  Found ButtonBorder"));

		// Step 1: Set brush color (Blueprint does this first)
		ButtonBorder->SetBrushColor(ButtonColor);

		// Step 2: Get existing brush from ButtonBorder to preserve designer settings
		FSlateBrush ExistingBrush = ButtonBorder->Background;

		UE_LOG(LogTemp, Log, TEXT("  Existing brush DrawAs=%d, TintColor=(%f,%f,%f,%f)"),
			(int)ExistingBrush.DrawAs,
			ExistingBrush.TintColor.GetSpecifiedColor().R,
			ExistingBrush.TintColor.GetSpecifiedColor().G,
			ExistingBrush.TintColor.GetSpecifiedColor().B,
			ExistingBrush.TintColor.GetSpecifiedColor().A);

		// Step 3: Create new brush preserving existing properties
		FSlateBrush NewBrush;
		NewBrush.TintColor = ExistingBrush.TintColor;  // Use existing tint color
		NewBrush.DrawAs = ExistingBrush.DrawAs;        // Preserve DrawAs from designer
		NewBrush.Tiling = ExistingBrush.Tiling;
		NewBrush.Mirroring = ExistingBrush.Mirroring;
		NewBrush.ImageSize = ExistingBrush.ImageSize;
		NewBrush.Margin = ExistingBrush.Margin;
		NewBrush.SetResourceObject(ExistingBrush.GetResourceObject());

		// Step 4: Add custom OutlineSettings
		FSlateBrushOutlineSettings& OutlineSettings = NewBrush.OutlineSettings;
		OutlineSettings.CornerRadii = OutlineCornerRadii;
		OutlineSettings.Width = OutlineWidth;
		OutlineSettings.Color = FSlateColor(OutlineColor);
		OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		OutlineSettings.bUseBrushTransparency = true;

		// Step 5: Apply the new brush
		ButtonBorder->SetBrush(NewBrush);

		UE_LOG(LogTemp, Log, TEXT("  Applied brush - DrawAs=%d, OutlineWidth=%f, CornerRadii=(%f,%f,%f,%f)"),
			(int)NewBrush.DrawAs, OutlineWidth,
			OutlineCornerRadii.X, OutlineCornerRadii.Y, OutlineCornerRadii.Z, OutlineCornerRadii.W);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find ButtonBorder widget!"));
	}

	// Apply text styling
	if (UTextBlock* ButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ButtonText"))))
	{
		UE_LOG(LogTemp, Log, TEXT("  Found ButtonText"));

		// Set text color
		ButtonText->SetColorAndOpacity(TextColor);

		// Set text if provided
		if (!Text.IsEmpty())
		{
			ButtonText->SetText(Text);
		}

		// Apply text padding via the Border slot
		if (UBorder* ButtonBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ButtonBorder"))))
		{
			if (UBorderSlot* BorderSlot = Cast<UBorderSlot>(ButtonText->Slot))
			{
				BorderSlot->SetPadding(TextPadding);
				UE_LOG(LogTemp, Log, TEXT("  Applied TextPadding (%f,%f,%f,%f)"),
					TextPadding.Left, TextPadding.Top, TextPadding.Right, TextPadding.Bottom);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Could not find ButtonText widget!"));
	}
}

void UW_GenericButton::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::NativeConstruct - %s"), *GetName());

	// Cache widget references and bind button events
	CacheWidgetReferences();
}

void UW_GenericButton::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::NativeDestruct"));
}

void UW_GenericButton::CacheWidgetReferences()
{
	// Cache border and text widgets for visual updates

	// Find and bind the actual Button widget for mouse click handling
	CachedButton = Cast<UButton>(GetWidgetFromName(TEXT("Button")));
	if (CachedButton)
	{
		// Bind OnClicked to handle mouse clicks
		CachedButton->OnClicked.AddDynamic(this, &UW_GenericButton::HandleButtonClicked);

		// Bind OnHovered/OnUnhovered for hover selection
		CachedButton->OnHovered.AddDynamic(this, &UW_GenericButton::HandleButtonHovered);
		CachedButton->OnUnhovered.AddDynamic(this, &UW_GenericButton::HandleButtonUnhovered);

		UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::CacheWidgetReferences - Bound Button events"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_GenericButton::CacheWidgetReferences - Could not find 'Button' widget"));
	}
}

void UW_GenericButton::SetButtonSelected_Implementation(bool InSelected)
{
	// Update Selected state
	Selected = InSelected;

	// Update ButtonBorder - follow exact Blueprint pattern:
	// 1. Get existing brush to preserve designer settings
	// 2. Create new brush with updated colors based on selection
	// 3. Apply custom OutlineSettings
	if (UBorder* ButtonBorder = Cast<UBorder>(GetWidgetFromName(TEXT("ButtonBorder"))))
	{
		// Get existing brush to preserve DrawAs, Tiling, etc.
		FSlateBrush ExistingBrush = ButtonBorder->Background;

		// Create new brush preserving existing properties
		FSlateBrush NewBrush;
		NewBrush.DrawAs = ExistingBrush.DrawAs;
		NewBrush.Tiling = ExistingBrush.Tiling;
		NewBrush.Mirroring = ExistingBrush.Mirroring;
		NewBrush.ImageSize = ExistingBrush.ImageSize;
		NewBrush.Margin = ExistingBrush.Margin;
		NewBrush.SetResourceObject(ExistingBrush.GetResourceObject());

		// Set tint color based on Selected state
		FLinearColor TintColor = InSelected ? SelectedButtonColor : ButtonColor;
		NewBrush.TintColor = FSlateColor(TintColor);

		// Configure outline settings with selection-based color
		FSlateBrushOutlineSettings& OutlineSettings = NewBrush.OutlineSettings;
		OutlineSettings.CornerRadii = OutlineCornerRadii;
		OutlineSettings.Width = OutlineWidth;
		OutlineSettings.Color = FSlateColor(InSelected ? SelectedOutlineColor : OutlineColor);
		OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
		OutlineSettings.bUseBrushTransparency = true;

		ButtonBorder->SetBrush(NewBrush);
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

void UW_GenericButton::HandleButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::HandleButtonClicked - Mouse click received"));

	// Call the EventPressButton which broadcasts OnButtonPressed
	EventPressButton();
}

void UW_GenericButton::HandleButtonHovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::HandleButtonHovered"));

	// Blueprint logic: On hover, set button as selected
	SetButtonSelected(true);
}

void UW_GenericButton::HandleButtonUnhovered()
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericButton::HandleButtonUnhovered"));

	// Blueprint logic: On unhover, deselect button
	SetButtonSelected(false);
}
