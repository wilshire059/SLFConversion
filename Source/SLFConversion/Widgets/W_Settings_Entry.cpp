// W_Settings_Entry.cpp
// C++ Widget implementation for W_Settings_Entry
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_Settings_Entry.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

UW_Settings_Entry::UW_Settings_Entry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EntryType(ESLFSettingEntry::Slider)
	, UnhoveredColor(FLinearColor::White)
	, HoveredColor(FLinearColor::Yellow)
	, Selected(false)
	, CustomSettingsAsset(nullptr)
	, SelectedBorder(nullptr)
	, SettingNameText(nullptr)
	, ValueText(nullptr)
	, EntrySwitcher(nullptr)
	, ValueSlider(nullptr)
	, LeftArrow(nullptr)
	, RightArrow(nullptr)
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

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] NativeConstruct - Type: %d"), static_cast<int32>(EntryType));

	// Bind button events
	if (LeftArrow)
	{
		LeftArrow->OnClicked.AddDynamic(this, &UW_Settings_Entry::OnLeftArrowClicked);
	}

	if (RightArrow)
	{
		RightArrow->OnClicked.AddDynamic(this, &UW_Settings_Entry::OnRightArrowClicked);
	}

	if (ValueSlider)
	{
		ValueSlider->OnValueChanged.AddDynamic(this, &UW_Settings_Entry::OnSliderValueChanged);
	}

	// Set initial entry type
	SetEntryType(EntryType);

	// Initially not selected
	SetEntrySelected(false);
}

void UW_Settings_Entry::NativeDestruct()
{
	// Unbind events
	if (LeftArrow)
	{
		LeftArrow->OnClicked.RemoveDynamic(this, &UW_Settings_Entry::OnLeftArrowClicked);
	}

	if (RightArrow)
	{
		RightArrow->OnClicked.RemoveDynamic(this, &UW_Settings_Entry::OnRightArrowClicked);
	}

	if (ValueSlider)
	{
		ValueSlider->OnValueChanged.RemoveDynamic(this, &UW_Settings_Entry::OnSliderValueChanged);
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

	// Initially hide selected border
	if (SelectedBorder)
	{
		SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_Settings_Entry::SetEntryType_Implementation(ESLFSettingEntry InType)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetEntryType: %d"), static_cast<int32>(InType));

	EntryType = InType;

	// Switch the widget switcher to the appropriate panel based on entry type
	if (EntrySwitcher)
	{
		int32 PanelIndex = 0;
		switch (EntryType)
		{
		case ESLFSettingEntry::DropDown:
			PanelIndex = 0;
			break;
		case ESLFSettingEntry::SingleButton:
			PanelIndex = 1;
			break;
		case ESLFSettingEntry::DoubleButton:
			PanelIndex = 2;
			break;
		case ESLFSettingEntry::Slider:
			PanelIndex = 3;
			break;
		case ESLFSettingEntry::InputKeySelector:
			PanelIndex = 4;
			break;
		default:
			// Handle invalid/stale enum values from Blueprint assets
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Unknown EntryType: %d, defaulting to DropDown"), static_cast<int32>(EntryType));
			PanelIndex = 0;
			break;
		}
		EntrySwitcher->SetActiveWidgetIndex(PanelIndex);
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

	// Update the value text display
	if (ValueText)
	{
		ValueText->SetText(FText::FromString(CurrentValue));
	}

	// Update slider if applicable
	if (ValueSlider && EntryType == ESLFSettingEntry::Slider)
	{
		int32 IntValue = FCString::Atoi(*CurrentValue);
		ValueSlider->SetValue(static_cast<float>(IntValue) / 100.0f);
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

UWidget* UW_Settings_Entry::OnGenerateItemWidget_Implementation(const FName& InItem)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnGenerateItemWidget: %s"), *InItem.ToString());

	// Create a text block for the dropdown item
	// This would typically be done with a widget pool in production
	return nullptr;
}

void UW_Settings_Entry::SetCurrentBoolValue_Implementation(bool InCurrentValue)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentBoolValue: %s"), InCurrentValue ? TEXT("true") : TEXT("false"));

	CurrentValue = InCurrentValue ? TEXT("1") : TEXT("0");

	if (ValueText)
	{
		ValueText->SetText(FText::FromString(InCurrentValue ? TEXT("On") : TEXT("Off")));
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

	if (ValueText)
	{
		ValueText->SetText(FText::FromString(ModeText));
	}
}

void UW_Settings_Entry::SetCurrentResolutionValue_Implementation(const FIntPoint& Resolution)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentResolutionValue: %dx%d"), Resolution.X, Resolution.Y);

	CurrentValue = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);

	if (ValueText)
	{
		ValueText->SetText(FText::FromString(CurrentValue));
	}
}

void UW_Settings_Entry::SetEntrySelected_Implementation(bool InSelected)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetEntrySelected: %s"), InSelected ? TEXT("true") : TEXT("false"));

	Selected = InSelected;

	if (SelectedBorder)
	{
		if (Selected)
		{
			SelectedBorder->SetVisibility(ESlateVisibility::Visible);
			SelectedBorder->SetBrushColor(HoveredColor);
			OnEntrySelected.Broadcast(this);
		}
		else
		{
			SelectedBorder->SetVisibility(ESlateVisibility::Collapsed);
			SelectedBorder->SetBrushColor(UnhoveredColor);
		}
	}
}

void UW_Settings_Entry::SetCameraInvertValue_Implementation(bool TargetBool)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCameraInvertValue: %s"), TargetBool ? TEXT("true") : TEXT("false"));

	SetCurrentBoolValue(TargetBool);
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
