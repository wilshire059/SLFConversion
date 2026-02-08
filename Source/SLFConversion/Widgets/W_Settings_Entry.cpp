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
#include "Engine/Blueprint.h"

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

	// CRITICAL DEBUG: Log ALL widget bindings and property values
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] ============= NativeConstruct ============="));
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] SettingName: %s"), *SettingName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] SettingTag: %s"), *SettingTag.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] EntryType: %d (0=DropDown, 1=SingleBtn, 2=DoubleBtn, 3=Slider, 4=KeySelector)"), static_cast<int32>(EntryType));
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] DropDown ptr: %s"), DropDown ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] DropdownView ptr: %s"), DropdownView ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] DecreaseButton: %s, IncreaseButton: %s, Slider: %s"),
		DecreaseButton ? TEXT("OK") : TEXT("NULL"),
		IncreaseButton ? TEXT("OK") : TEXT("NULL"),
		Slider ? TEXT("OK") : TEXT("NULL"));

	// Bind button events for Decrease/Increase (used in DoubleButtonView)
	if (DecreaseButton)
	{
		DecreaseButton->OnClicked.AddDynamic(this, &UW_Settings_Entry::OnLeftArrowClicked);
	}

	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.AddDynamic(this, &UW_Settings_Entry::OnRightArrowClicked);
	}

	if (Slider)
	{
		Slider->OnValueChanged.AddDynamic(this, &UW_Settings_Entry::OnSliderValueChanged);
	}

	// Bind ComboBox widget generation delegates
	if (DropDown)
	{
		DropDown->OnGenerateItemWidget.BindDynamic(this, &UW_Settings_Entry::OnGenerateItemWidget);
		DropDown->OnGenerateContentWidget.BindDynamic(this, &UW_Settings_Entry::OnGenerateContentWidget);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Bound ComboBox delegates for %s"), *SettingName.ToString());

		// Populate dropdown options based on setting tag
		PopulateDropdownOptions();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] DropDown is NULL for %s - cannot populate!"), *SettingName.ToString());
	}

	// Set initial entry type (shows/hides appropriate views)
	SetEntryType(EntryType);

	// Initially not selected
	SetEntrySelected(false);

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] ============= NativeConstruct DONE ============="));
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
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] SetEntryType(%d) for %s - DropdownView=%s"),
		static_cast<int32>(Type),
		*SettingName.ToString(),
		DropdownView ? TEXT("VALID") : TEXT("NULL"));

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
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Showing ButtonView for %s"), *SettingName.ToString());
		break;
	case ESLFSettingEntry::DoubleButton:
		ShowView(DoubleButtonView, true);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Showing DoubleButtonView for %s"), *SettingName.ToString());
		break;
	case ESLFSettingEntry::Slider:
		ShowView(SliderView, true);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Showing SliderView for %s"), *SettingName.ToString());
		break;
	case ESLFSettingEntry::DropDown:
		ShowView(DropdownView, true);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Showing DropdownView for %s"), *SettingName.ToString());
		break;
	case ESLFSettingEntry::InputKeySelector:
		ShowView(KeySelectorView, true);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Showing KeySelectorView for %s"), *SettingName.ToString());
		break;
	default:
		// Handle invalid/stale enum values from Blueprint assets
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] UNKNOWN EntryType: %d for %s, defaulting to DropDown"), static_cast<int32>(Type), *SettingName.ToString());
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

void UW_Settings_Entry::SetCurrentValueInt_Implementation(int32 InCurrentValue)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentValueInt: %d"), InCurrentValue);

	// Convert int to string and call the string version
	SetCurrentValue(FString::FromInt(InCurrentValue));
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

void UW_Settings_Entry::SetCurrentValueBool_Implementation(bool InCurrentValue)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetCurrentValueBool: %s"), InCurrentValue ? TEXT("true") : TEXT("false"));

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
	Selected = bSelected;

	// Background border color changes based on selection (not visibility)
	if (BackgroundBorder)
	{
		FLinearColor TargetColor = Selected ? HoveredColor : UnhoveredColor;
		BackgroundBorder->SetBrushColor(TargetColor);

		if (Selected)
		{
			UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] SetEntrySelected: true - %s - Color: R=%.3f G=%.3f B=%.3f"),
				*SettingName.ToString(), TargetColor.R, TargetColor.G, TargetColor.B);
		}
	}
	else if (Selected)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] SetEntrySelected: true - %s - BackgroundBorder is NULL!"), *SettingName.ToString());
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

	if (CachedDropdownOptions.Num() == 0 || !DropDown)
	{
		return;
	}

	// Find current option index
	FName CurrentOption = DropDown->GetSelectedOption();
	int32 CurrentIndex = CachedDropdownOptions.IndexOfByKey(CurrentOption);
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = 0;
	}

	// Decrement with wrap
	int32 NewIndex = (CurrentIndex - 1 + CachedDropdownOptions.Num()) % CachedDropdownOptions.Num();
	FName NewOption = CachedDropdownOptions[NewIndex];

	DropDown->SetSelectedOption(NewOption);
	SetCurrentValue(NewOption.ToString());
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Dropdown scrolled left: %s -> %s"), *CurrentOption.ToString(), *NewOption.ToString());
}

void UW_Settings_Entry::EventScrollDropdownRight_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventScrollDropdownRight"));

	if (CachedDropdownOptions.Num() == 0 || !DropDown)
	{
		return;
	}

	// Find current option index
	FName CurrentOption = DropDown->GetSelectedOption();
	int32 CurrentIndex = CachedDropdownOptions.IndexOfByKey(CurrentOption);
	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = 0;
	}

	// Increment with wrap
	int32 NewIndex = (CurrentIndex + 1) % CachedDropdownOptions.Num();
	FName NewOption = CachedDropdownOptions[NewIndex];

	DropDown->SetSelectedOption(NewOption);
	SetCurrentValue(NewOption.ToString());
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Dropdown scrolled right: %s -> %s"), *CurrentOption.ToString(), *NewOption.ToString());
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
			SetCurrentValueBool(!bCurrentBool);
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
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] OnGenerateItemWidget: %s"), *Item.ToString());

	// Load the Blueprint widget class (has visual elements defined in UMG)
	static TSubclassOf<UW_Settings_CenteredText> CachedWidgetClass;
	if (!CachedWidgetClass)
	{
		// Try loading via Blueprint asset first
		const TCHAR* BlueprintPath = TEXT("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText");
		UBlueprint* WidgetBP = LoadObject<UBlueprint>(nullptr, BlueprintPath);

		if (WidgetBP && WidgetBP->GeneratedClass)
		{
			CachedWidgetClass = WidgetBP->GeneratedClass;
			UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Loaded W_Settings_CenteredText Blueprint class: %s"),
				*CachedWidgetClass->GetName());
		}
		else
		{
			// Try direct class path
			CachedWidgetClass = LoadClass<UW_Settings_CenteredText>(
				nullptr,
				TEXT("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText.W_Settings_CenteredText_C")
			);
		}

		if (!CachedWidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] Failed to load W_Settings_CenteredText - using C++ fallback"));
			CachedWidgetClass = UW_Settings_CenteredText::StaticClass();
		}
	}

	UW_Settings_CenteredText* CenteredTextWidget = CreateWidget<UW_Settings_CenteredText>(GetOwningPlayer(), CachedWidgetClass);

	if (CenteredTextWidget)
	{
		// Set the text to the item name
		FText ItemText = FText::FromName(Item);
		CenteredTextWidget->EventSetText(ItemText);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Created CenteredText widget, set text to: %s"), *ItemText.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] Failed to create CenteredText widget!"));
	}

	return CenteredTextWidget;
}

UWidget* UW_Settings_Entry::OnGenerateContentWidget(FName Item)
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] OnGenerateContentWidget: %s"), *Item.ToString());

	// Same logic as OnGenerateItemWidget for content display
	return OnGenerateItemWidget(Item);
}

void UW_Settings_Entry::PopulateDropdownOptions()
{
	if (!DropDown)
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] PopulateDropdownOptions - No DropDown widget!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] >>>>>> PopulateDropdownOptions CALLED"));
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] SettingName: %s"), *SettingName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] SettingTag: '%s' (IsValid=%s)"),
		*SettingTag.ToString(),
		SettingTag.IsValid() ? TEXT("YES") : TEXT("NO"));

	// Clear existing options
	DropDown->ClearOptions();
	DropDown->ClearSelection();
	Resolutions.Empty();
	WindowModes.Empty();
	CachedDropdownOptions.Empty();

	// Check what type of setting this is based on the SettingTag
	FString TagString = SettingTag.ToString();

	// CRITICAL: If SettingTag is empty/invalid, we can't populate
	if (!SettingTag.IsValid() || TagString.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] SettingTag is INVALID/EMPTY for %s - cannot determine dropdown options!"), *SettingName.ToString());
		return;
	}

	if (TagString.Contains(TEXT("Resolution")))
	{
		// Populate resolution options
		TArray<FIntPoint> AvailableResolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);
		SupportedResolutions = AvailableResolutions;

		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Found %d supported resolutions"), SupportedResolutions.Num());

		for (const FIntPoint& Res : SupportedResolutions)
		{
			FString ResString = FString::Printf(TEXT("%dx%d"), Res.X, Res.Y);
			FName ResName(*ResString);
			Resolutions.Add(ResName, Res);
			DropDown->AddOption(ResName);
			CachedDropdownOptions.Add(ResName);
		}

		// Set current resolution as selected
		if (GEngine && GEngine->GameUserSettings)
		{
			FIntPoint CurrentRes = GEngine->GameUserSettings->GetScreenResolution();
			FString CurrentResString = FString::Printf(TEXT("%dx%d"), CurrentRes.X, CurrentRes.Y);
			DropDown->SetSelectedOption(FName(*CurrentResString));
			SetCurrentValue(CurrentResString);
		}
	}
	else if (TagString.Contains(TEXT("ScreenMode")))
	{
		// Populate window mode options
		WindowModes.Add(FName(TEXT("Fullscreen")), EWindowMode::Fullscreen);
		WindowModes.Add(FName(TEXT("Windowed Fullscreen")), EWindowMode::WindowedFullscreen);
		WindowModes.Add(FName(TEXT("Windowed")), EWindowMode::Windowed);

		for (const auto& Pair : WindowModes)
		{
			DropDown->AddOption(Pair.Key);
			CachedDropdownOptions.Add(Pair.Key);
		}

		// Set current mode as selected
		if (GEngine && GEngine->GameUserSettings)
		{
			EWindowMode::Type CurrentMode = GEngine->GameUserSettings->GetFullscreenMode();
			FName ModeName;
			switch (CurrentMode)
			{
			case EWindowMode::Fullscreen:
				ModeName = FName(TEXT("Fullscreen"));
				break;
			case EWindowMode::WindowedFullscreen:
				ModeName = FName(TEXT("Windowed Fullscreen"));
				break;
			case EWindowMode::Windowed:
			default:
				ModeName = FName(TEXT("Windowed"));
				break;
			}
			DropDown->SetSelectedOption(ModeName);
			SetCurrentValue(ModeName.ToString());
		}
	}
	else if (TagString.Contains(TEXT("TextureQuality")) ||
			 TagString.Contains(TEXT("AntiAliasing")) ||
			 TagString.Contains(TEXT("PostProcessing")) ||
			 TagString.Contains(TEXT("ShadowQuality")) ||
			 TagString.Contains(TEXT("Reflections")) ||
			 TagString.Contains(TEXT("ShaderQuality")) ||
			 TagString.Contains(TEXT("GlobalIllumination")) ||
			 TagString.Contains(TEXT("ViewDistance")) ||
			 TagString.Contains(TEXT("FoliageQuality")))
	{
		// Quality settings use Low, Medium, High, Ultra, Epic
		DropDown->AddOption(FName(TEXT("Low")));    CachedDropdownOptions.Add(FName(TEXT("Low")));
		DropDown->AddOption(FName(TEXT("Medium"))); CachedDropdownOptions.Add(FName(TEXT("Medium")));
		DropDown->AddOption(FName(TEXT("High")));   CachedDropdownOptions.Add(FName(TEXT("High")));
		DropDown->AddOption(FName(TEXT("Ultra")));  CachedDropdownOptions.Add(FName(TEXT("Ultra")));
		DropDown->AddOption(FName(TEXT("Epic")));   CachedDropdownOptions.Add(FName(TEXT("Epic")));

		// Get current quality level from GameUserSettings
		if (GEngine && GEngine->GameUserSettings)
		{
			int32 QualityLevel = 2; // Default to High
			if (TagString.Contains(TEXT("TextureQuality")))
				QualityLevel = GEngine->GameUserSettings->GetTextureQuality();
			else if (TagString.Contains(TEXT("AntiAliasing")))
				QualityLevel = GEngine->GameUserSettings->GetAntiAliasingQuality();
			else if (TagString.Contains(TEXT("PostProcessing")))
				QualityLevel = GEngine->GameUserSettings->GetPostProcessingQuality();
			else if (TagString.Contains(TEXT("ShadowQuality")))
				QualityLevel = GEngine->GameUserSettings->GetShadowQuality();
			else if (TagString.Contains(TEXT("ViewDistance")))
				QualityLevel = GEngine->GameUserSettings->GetViewDistanceQuality();
			else if (TagString.Contains(TEXT("FoliageQuality")))
				QualityLevel = GEngine->GameUserSettings->GetFoliageQuality();
			// Note: Reflections, Shader, GlobalIllumination might need custom handling

			FName QualityName;
			switch (QualityLevel)
			{
			case 0: QualityName = FName(TEXT("Low")); break;
			case 1: QualityName = FName(TEXT("Medium")); break;
			case 2: QualityName = FName(TEXT("High")); break;
			case 3: QualityName = FName(TEXT("Ultra")); break;
			case 4: QualityName = FName(TEXT("Epic")); break;
			default: QualityName = FName(TEXT("High")); break;
			}
			DropDown->SetSelectedOption(QualityName);
			SetCurrentValue(QualityName.ToString());
		}
	}
	else if (TagString.Contains(TEXT("InvertCamX")) || TagString.Contains(TEXT("InvertCamY")))
	{
		// Invert camera X/Y is No/Yes dropdown (bp_only order: No first, then Yes)
		DropDown->AddOption(FName(TEXT("No")));  CachedDropdownOptions.Add(FName(TEXT("No")));
		DropDown->AddOption(FName(TEXT("Yes"))); CachedDropdownOptions.Add(FName(TEXT("Yes")));
		DropDown->SetSelectedOption(FName(TEXT("No")));
		SetCurrentValue(TEXT("No"));
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Populated InvertCam dropdown with No/Yes options"));
	}
	else if (TagString.Contains(TEXT("CamSpeed")))
	{
		// Camera speed is a slider-like dropdown with numeric options
		for (int32 i = 0; i <= 100; i += 10)
		{
			FName OptName(*FString::FromInt(i));
			DropDown->AddOption(OptName);
			CachedDropdownOptions.Add(OptName);
		}
		DropDown->SetSelectedOption(FName(TEXT("50")));
		SetCurrentValue(TEXT("50"));
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Populated CamSpeed dropdown with 0-100 options"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[W_Settings_Entry] Unknown setting tag: '%s' - no dropdown options added for %s"), *TagString, *SettingName.ToString());
	}

	// Force refresh by re-selecting the current option (workaround for ComboBoxKey not showing initial selection)
	FName CurrentSelection = DropDown->GetSelectedOption();
	if (!CurrentSelection.IsNone())
	{
		// Clear and re-set to trigger content widget generation
		DropDown->SetSelectedOption(NAME_None);
		DropDown->SetSelectedOption(CurrentSelection);
		UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] Re-selected option '%s' to force refresh"), *CurrentSelection.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("[W_Settings_Entry] <<<<<< PopulateDropdownOptions DONE for %s (SettingTag=%s)"), *SettingName.ToString(), *SettingTag.ToString());
}
