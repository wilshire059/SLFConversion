// W_Settings_Entry.cpp
// C++ Widget implementation for W_Settings_Entry
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_Settings_Entry.h"
#include "Widgets/W_Settings_CenteredText.h"
#include "SLFPrimaryDataAssets.h"  // For UPDA_CustomSettings
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ComboBoxKey.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/WidgetTree.h"

UW_Settings_Entry::UW_Settings_Entry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EntryType(ESLFSettingEntry::Slider)
	, UnhoveredColor(FLinearColor::White)
	, HoveredColor(FLinearColor::Yellow)
	, Selected(false)
	, CustomSettingsAsset(nullptr)
	, BackgroundBorder(nullptr)
	, SettingNameText(nullptr)
	, SliderValue(nullptr)
	, ButtonsValue(nullptr)
	, Slider(nullptr)
	, DecreaseButton(nullptr)
	, IncreaseButton(nullptr)
	, ButtonView(nullptr)
	, DoubleButtonView(nullptr)
	, SliderView(nullptr)
	, DropdownView(nullptr)
	, KeySelectorView(nullptr)
	, DropDown(nullptr)
{
}

void UW_Settings_Entry::NativePreConstruct()
{
	Super::NativePreConstruct();

	ApplyVisualConfig();
}

void UW_Settings_Entry::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] NativeConstruct - Type: %d, Decrease=%s, Increase=%s, Slider=%s"),
		static_cast<int32>(EntryType),
		DecreaseButton ? TEXT("OK") : TEXT("NULL"),
		IncreaseButton ? TEXT("OK") : TEXT("NULL"),
		Slider ? TEXT("OK") : TEXT("NULL"));

	// Bind button events for Decrease/Increase (used in DoubleButtonView)
	if (DecreaseButton)
	{
		DecreaseButton->OnClicked.AddDynamic(this, &UW_Settings_Entry::OnLeftArrowClicked);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Bound DecreaseButton.OnClicked"));
	}

	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.AddDynamic(this, &UW_Settings_Entry::OnRightArrowClicked);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Bound IncreaseButton.OnClicked"));
	}

	if (Slider)
	{
		Slider->OnValueChanged.AddDynamic(this, &UW_Settings_Entry::OnSliderValueChanged);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Bound Slider.OnValueChanged"));
	}

	// Bind ComboBox widget generation delegates
	if (DropDown)
	{
		DropDown->OnGenerateItemWidget.BindDynamic(this, &UW_Settings_Entry::OnGenerateItemWidget);
		DropDown->OnGenerateContentWidget.BindDynamic(this, &UW_Settings_Entry::OnGenerateContentWidget);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Bound ComboBox widget generation delegates"));
	}

	// Set initial entry type (shows/hides appropriate views)
	SetEntryType(EntryType);

	// Initially not selected
	SetEntrySelected(false);
}

void UW_Settings_Entry::NativeDestruct()
{
	// Unbind events
	if (DecreaseButton)
	{
		DecreaseButton->OnClicked.RemoveDynamic(this, &UW_Settings_Entry::OnLeftArrowClicked);
	}

	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.RemoveDynamic(this, &UW_Settings_Entry::OnRightArrowClicked);
	}

	if (Slider)
	{
		Slider->OnValueChanged.RemoveDynamic(this, &UW_Settings_Entry::OnSliderValueChanged);
	}

	// Unbind ComboBox delegates
	if (DropDown)
	{
		DropDown->OnGenerateItemWidget.Unbind();
		DropDown->OnGenerateContentWidget.Unbind();
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] NativeDestruct"));
}

void UW_Settings_Entry::ApplyVisualConfig()
{
	// Set setting name text
	if (SettingNameText)
	{
		SettingNameText->SetText(SettingName);
	}

	// Background border is always visible but color changes on selection
	if (BackgroundBorder)
	{
		BackgroundBorder->SetBrushColor(UnhoveredColor);
	}
}

void UW_Settings_Entry::SetEntryType_Implementation(ESLFSettingEntry Type)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetEntryType: %d"), static_cast<int32>(Type));

	EntryType = Type;

	// Show/hide views based on entry type (Blueprint uses visibility, not WidgetSwitcher)
	auto ShowView = [](UWidget* View, bool bShow)
	{
		if (View)
		{
			View->SetVisibility(bShow ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	};

	// Hide all views first
	ShowView(ButtonView, false);
	ShowView(DoubleButtonView, false);
	ShowView(SliderView, false);
	ShowView(DropdownView, false);
	ShowView(KeySelectorView, false);

	// Show the appropriate view
	switch (EntryType)
	{
	case ESLFSettingEntry::SingleButton:
		ShowView(ButtonView, true);
		break;
	case ESLFSettingEntry::DoubleButton:
		ShowView(DoubleButtonView, true);
		break;
	case ESLFSettingEntry::Slider:
		ShowView(SliderView, true);
		break;
	case ESLFSettingEntry::DropDown:
		ShowView(DropdownView, true);
		break;
	case ESLFSettingEntry::InputKeySelector:
		ShowView(KeySelectorView, true);
		break;
	default:
		// Handle invalid/stale enum values from Blueprint assets
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Unknown EntryType: %d, defaulting to DropDown"), static_cast<int32>(Type));
		ShowView(DropdownView, true);
		break;
	}
}

FText UW_Settings_Entry::GetQualityLevelText_Implementation(int32 QualityLevel)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] GetQualityLevelText: %d"), QualityLevel);

	switch (QualityLevel)
	{
	case 0:
		return FText::FromString(TEXT("Low"));
	case 1:
		return FText::FromString(TEXT("Medium"));
	case 2:
		return FText::FromString(TEXT("High"));
	case 3:
		return FText::FromString(TEXT("Epic"));
	case 4:
		return FText::FromString(TEXT("Cinematic"));
	default:
		return FText::FromString(TEXT("Custom"));
	}
}

void UW_Settings_Entry::SetCurrentValue_Implementation(const FString& InCurrentValue)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentValue: %s"), *InCurrentValue);

	CurrentValue = InCurrentValue;

	// Update the appropriate value text display based on entry type
	if (EntryType == ESLFSettingEntry::Slider)
	{
		if (SliderValue)
		{
			SliderValue->SetText(FText::FromString(CurrentValue));
		}
		// Update slider position
		if (Slider)
		{
			int32 IntValue = FCString::Atoi(*CurrentValue);
			Slider->SetValue(static_cast<float>(IntValue) / 100.0f);
		}
	}
	else
	{
		// DoubleButton, DropDown use ButtonsValue
		if (ButtonsValue)
		{
			ButtonsValue->SetText(FText::FromString(CurrentValue));
		}
	}
}

int32 UW_Settings_Entry::GetIncrementedValue_Implementation(int32 MaxClamp)
{
	int32 IntValue = FCString::Atoi(*CurrentValue);
	int32 NewValue = FMath::Clamp(IntValue + 1, 0, MaxClamp);

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] GetIncrementedValue: %d -> %d (Max: %d)"), IntValue, NewValue, MaxClamp);

	return NewValue;
}

int32 UW_Settings_Entry::GetDecrementedValue_Implementation(int32 MaxClamp)
{
	int32 IntValue = FCString::Atoi(*CurrentValue);
	int32 NewValue = FMath::Clamp(IntValue - 1, 0, MaxClamp);

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] GetDecrementedValue: %d -> %d (Max: %d)"), IntValue, NewValue, MaxClamp);

	return NewValue;
}

// NOTE: OnGenerateItemWidget is implemented in Blueprint (creates W_Settings_CenteredText)

void UW_Settings_Entry::SetCurrentBoolValue_Implementation(bool InCurrentValue)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentBoolValue: %s"), InCurrentValue ? TEXT("true") : TEXT("false"));

	CurrentValue = InCurrentValue ? TEXT("1") : TEXT("0");

	// Bool values are typically shown in ButtonsValue (DoubleButton view)
	if (ButtonsValue)
	{
		ButtonsValue->SetText(FText::FromString(InCurrentValue ? TEXT("On") : TEXT("Off")));
	}
}

void UW_Settings_Entry::SetCurrentScreenModeValue_Implementation(EWindowMode::Type FullscreenMode)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentScreenModeValue: %d"), static_cast<int32>(FullscreenMode));

	CurrentValue = FString::FromInt(static_cast<int32>(FullscreenMode));

	FString ModeText;
	switch (FullscreenMode)
	{
	case EWindowMode::Fullscreen:
		ModeText = TEXT("Fullscreen");
		break;
	case EWindowMode::WindowedFullscreen:
		ModeText = TEXT("Borderless");
		break;
	case EWindowMode::Windowed:
		ModeText = TEXT("Windowed");
		break;
	default:
		ModeText = TEXT("Unknown");
		break;
	}

	// Screen mode is shown in ButtonsValue (DropDown/DoubleButton view)
	if (ButtonsValue)
	{
		ButtonsValue->SetText(FText::FromString(ModeText));
	}
}

void UW_Settings_Entry::SetCurrentResolutionValue_Implementation(const FIntPoint& Resolution)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentResolutionValue: %dx%d"), Resolution.X, Resolution.Y);

	CurrentValue = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);

	// Resolution is shown in ButtonsValue (DropDown view)
	if (ButtonsValue)
	{
		ButtonsValue->SetText(FText::FromString(CurrentValue));
	}
}

void UW_Settings_Entry::SetEntrySelected_Implementation(bool bSelected)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetEntrySelected: %s"), bSelected ? TEXT("true") : TEXT("false"));

	Selected = bSelected;

	// Background border color changes based on selection (not visibility)
	if (BackgroundBorder)
	{
		BackgroundBorder->SetBrushColor(Selected ? HoveredColor : UnhoveredColor);
	}

	if (Selected)
	{
		OnEntrySelected.Broadcast(this);
	}
}

void UW_Settings_Entry::SetCameraInvertValue_Implementation(bool TargetBool)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCameraInvertValue: %s"), TargetBool ? TEXT("true") : TEXT("false"));

	// Camera invert shows "Inverted" or "Normal" (not "On"/"Off" like regular bools)
	// This matches the Blueprint's SelectText("Inverted", "Normal", bPickA=TargetBool)
	CurrentValue = TargetBool ? TEXT("1") : TEXT("0");

	if (ButtonsValue)
	{
		FText DisplayText = TargetBool ? FText::FromString(TEXT("Inverted")) : FText::FromString(TEXT("Normal"));
		ButtonsValue->SetText(DisplayText);
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCameraInvertValue - ButtonsValue set to: %s"), TargetBool ? TEXT("Inverted") : TEXT("Normal"));
	}
}

void UW_Settings_Entry::EventDecreaseSetting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventDecreaseSetting"));

	// Trigger the appropriate decrease based on entry type
	switch (EntryType)
	{
	case ESLFSettingEntry::Slider:
		EventOnSliderSettingDecrease();
		break;
	case ESLFSettingEntry::DropDown:
		EventScrollDropdownLeft();
		break;
	default:
		EventOnButtonSettingDecrease();
		break;
	}
}

void UW_Settings_Entry::EventIncreaseSetting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventIncreaseSetting"));

	// Trigger the appropriate increase based on entry type
	switch (EntryType)
	{
	case ESLFSettingEntry::Slider:
		EventOnSliderSettingIncrease();
		break;
	case ESLFSettingEntry::DropDown:
		EventScrollDropdownRight();
		break;
	default:
		EventOnButtonSettingIncrease();
		break;
	}
}

void UW_Settings_Entry::EventOnButtonSettingDecrease_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventOnButtonSettingDecrease"));

	int32 NewValue = GetDecrementedValue(4);
	SetCurrentValue(FString::FromInt(NewValue));
}

void UW_Settings_Entry::EventOnButtonSettingIncrease_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventOnButtonSettingIncrease"));

	int32 NewValue = GetIncrementedValue(4);
	SetCurrentValue(FString::FromInt(NewValue));
}

void UW_Settings_Entry::EventOnSliderSettingDecrease_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventOnSliderSettingDecrease"));

	int32 CurrentInt = FCString::Atoi(*CurrentValue);
	int32 NewValue = FMath::Clamp(CurrentInt - 10, 0, 100);
	SetCurrentValue(FString::FromInt(NewValue));
}

void UW_Settings_Entry::EventOnSliderSettingIncrease_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventOnSliderSettingIncrease"));

	int32 CurrentInt = FCString::Atoi(*CurrentValue);
	int32 NewValue = FMath::Clamp(CurrentInt + 10, 0, 100);
	SetCurrentValue(FString::FromInt(NewValue));
}

void UW_Settings_Entry::EventRefreshResolutions_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventRefreshResolutions"));

	// Get supported resolutions from game user settings
	SupportedResolutions.Empty();

	TArray<FIntPoint> AvailableResolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);
	SupportedResolutions = AvailableResolutions;

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Found %d supported resolutions"), SupportedResolutions.Num());
}

void UW_Settings_Entry::EventScrollDropdownLeft_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventScrollDropdownLeft"));

	int32 NewValue = GetDecrementedValue(10);
	SetCurrentValue(FString::FromInt(NewValue));
}

void UW_Settings_Entry::EventScrollDropdownRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventScrollDropdownRight"));

	int32 NewValue = GetIncrementedValue(10);
	SetCurrentValue(FString::FromInt(NewValue));
}

void UW_Settings_Entry::EventActivateEntry_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventActivateEntry - Type: %d"), static_cast<int32>(EntryType));

	// Handle activation based on entry type
	switch (EntryType)
	{
	case ESLFSettingEntry::SingleButton:
		// Broadcast the single button pressed event
		OnSingleButtonEntryPressed.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Broadcast OnSingleButtonEntryPressed"));
		break;

	case ESLFSettingEntry::DoubleButton:
		// For double buttons, toggle the value (On/Off)
		{
			bool bCurrentBool = (CurrentValue == TEXT("1") || CurrentValue.ToLower() == TEXT("true"));
			SetCurrentBoolValue(!bCurrentBool);
		}
		break;

	case ESLFSettingEntry::DropDown:
		// For dropdowns, cycle to next option
		EventScrollDropdownRight();
		break;

	case ESLFSettingEntry::Slider:
		// For sliders, OK typically does nothing (use left/right to adjust)
		break;

	case ESLFSettingEntry::InputKeySelector:
		// For key selectors, this would start listening for key input
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] InputKeySelector activation - would start key listening"));
		break;

	default:
		break;
	}
}

void UW_Settings_Entry::OnLeftArrowClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnLeftArrowClicked"));
	EventDecreaseSetting();
}

void UW_Settings_Entry::OnRightArrowClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnRightArrowClicked"));
	EventIncreaseSetting();
}

void UW_Settings_Entry::OnEntryHovered()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnEntryHovered"));
	SetEntrySelected(true);
}

void UW_Settings_Entry::OnSliderValueChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnSliderValueChanged: %.2f"), Value);

	int32 IntValue = FMath::RoundToInt(Value * 100.0f);
	SetCurrentValue(FString::FromInt(IntValue));
}

UWidget* UW_Settings_Entry::OnGenerateItemWidget(FName Item)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnGenerateItemWidget: %s"), *Item.ToString());

	// Create W_Settings_CenteredText widget
	UClass* WidgetClass = UW_Settings_CenteredText::StaticClass();
	UW_Settings_CenteredText* CenteredTextWidget = CreateWidget<UW_Settings_CenteredText>(GetOwningPlayer(), WidgetClass);

	if (CenteredTextWidget)
	{
		// Set the text to the item name
		FText ItemText = FText::FromName(Item);
		CenteredTextWidget->EventSetText(ItemText);
	}

	return CenteredTextWidget;
}

UWidget* UW_Settings_Entry::OnGenerateContentWidget(FName Item)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnGenerateContentWidget: %s"), *Item.ToString());

	// Same logic as OnGenerateItemWidget for content display
	return OnGenerateItemWidget(Item);
}
