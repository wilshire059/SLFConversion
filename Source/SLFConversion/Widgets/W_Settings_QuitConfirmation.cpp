// W_Settings_QuitConfirmation.cpp
// C++ Widget implementation for W_Settings_QuitConfirmation
//
// Source: BlueprintDNA_v2/WidgetBlueprint/W_Settings_QuitConfirmation.json

#include "Widgets/W_Settings_QuitConfirmation.h"
#include "Widgets/W_GenericButton.h"
#include "Components/TextBlock.h"
#include "GameFramework/InputSettings.h"

UW_Settings_QuitConfirmation::UW_Settings_QuitConfirmation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ActiveBtn(nullptr)
	, CachedYesBtn(nullptr)
	, CachedNoBtn(nullptr)
{
}

void UW_Settings_QuitConfirmation::NativeConstruct()
{
	Super::NativeConstruct();

	CacheWidgetReferences();

	// Bind button events
	if (CachedYesBtn)
	{
		CachedYesBtn->OnButtonPressed.AddDynamic(this, &UW_Settings_QuitConfirmation::OnYesButtonPressed);
		CachedYesBtn->OnButtonSelected.AddDynamic(this, &UW_Settings_QuitConfirmation::OnYesButtonSelected);
	}
	if (CachedNoBtn)
	{
		CachedNoBtn->OnButtonPressed.AddDynamic(this, &UW_Settings_QuitConfirmation::OnNoButtonPressed);
		CachedNoBtn->OnButtonSelected.AddDynamic(this, &UW_Settings_QuitConfirmation::OnNoButtonSelected);
	}

	// Select YES by default for gamepad/keyboard navigation
	if (CachedYesBtn)
	{
		CachedYesBtn->SetButtonSelected(true);
		ActiveBtn = CachedYesBtn;
	}

	// Ensure this widget is focusable for keyboard/gamepad input
	SetIsFocusable(true);

	UE_LOG(LogTemp, Log, TEXT("[W_Settings_QuitConfirmation] NativeConstruct - YES: %s, NO: %s"),
		CachedYesBtn ? TEXT("OK") : TEXT("NULL"), CachedNoBtn ? TEXT("OK") : TEXT("NULL"));
}

void UW_Settings_QuitConfirmation::NativeDestruct()
{
	// Unbind button events
	if (CachedYesBtn)
	{
		CachedYesBtn->OnButtonPressed.RemoveDynamic(this, &UW_Settings_QuitConfirmation::OnYesButtonPressed);
		CachedYesBtn->OnButtonSelected.RemoveDynamic(this, &UW_Settings_QuitConfirmation::OnYesButtonSelected);
	}
	if (CachedNoBtn)
	{
		CachedNoBtn->OnButtonPressed.RemoveDynamic(this, &UW_Settings_QuitConfirmation::OnNoButtonPressed);
		CachedNoBtn->OnButtonSelected.RemoveDynamic(this, &UW_Settings_QuitConfirmation::OnNoButtonSelected);
	}

	Super::NativeDestruct();
}

void UW_Settings_QuitConfirmation::CacheWidgetReferences()
{
	// Find W_GB_YES and W_GB_NO by name from the widget tree
	if (!CachedYesBtn)
	{
		if (UWidget* Widget = GetWidgetFromName(TEXT("W_GB_YES")))
		{
			CachedYesBtn = Cast<UW_GenericButton>(Widget);
		}
	}
	if (!CachedNoBtn)
	{
		if (UWidget* Widget = GetWidgetFromName(TEXT("W_GB_NO")))
		{
			CachedNoBtn = Cast<UW_GenericButton>(Widget);
		}
	}
}

void UW_Settings_QuitConfirmation::OnYesButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_QuitConfirmation] YES pressed - broadcasting OnQuitGameConfirmed"));
	OnQuitGameConfirmed.Broadcast();
}

void UW_Settings_QuitConfirmation::OnNoButtonPressed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_Settings_QuitConfirmation] NO pressed - broadcasting OnQuitGameCanceled"));
	OnQuitGameCanceled.Broadcast();
}

void UW_Settings_QuitConfirmation::OnYesButtonSelected(UW_GenericButton* Button)
{
	ActiveBtn = Button;
	// Deselect NO when YES is selected (mutual exclusion)
	if (CachedNoBtn)
	{
		CachedNoBtn->SetButtonSelected(false);
	}
}

void UW_Settings_QuitConfirmation::OnNoButtonSelected(UW_GenericButton* Button)
{
	ActiveBtn = Button;
	// Deselect YES when NO is selected (mutual exclusion)
	if (CachedYesBtn)
	{
		CachedYesBtn->SetButtonSelected(false);
	}
}

FReply UW_Settings_QuitConfirmation::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	// D-pad Left/Right, Left Stick Left/Right, or Left/Right arrows: toggle between YES and NO
	if (Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left || Key == EKeys::Left ||
		Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right || Key == EKeys::Right)
	{
		// Toggle: if YES is active, select NO and vice versa
		if (ActiveBtn == CachedYesBtn && CachedNoBtn)
		{
			CachedNoBtn->SetButtonSelected(true);
		}
		else if (CachedYesBtn)
		{
			CachedYesBtn->SetButtonSelected(true);
		}

		UE_LOG(LogTemp, Log, TEXT("[W_Settings_QuitConfirmation] Gamepad navigate - ActiveBtn: %s"),
			ActiveBtn == CachedYesBtn ? TEXT("YES") : TEXT("NO"));
		return FReply::Handled();
	}

	// Gamepad A / Enter / Space: press the active button
	if (Key == EKeys::Gamepad_FaceButton_Bottom || Key == EKeys::Enter || Key == EKeys::SpaceBar)
	{
		if (ActiveBtn)
		{
			ActiveBtn->EventPressButton();
			UE_LOG(LogTemp, Log, TEXT("[W_Settings_QuitConfirmation] Gamepad confirm - pressed %s"),
				ActiveBtn == CachedYesBtn ? TEXT("YES") : TEXT("NO"));
		}
		return FReply::Handled();
	}

	// Gamepad B / Escape: cancel (same as pressing NO)
	if (Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Escape)
	{
		OnNoButtonPressed();
		UE_LOG(LogTemp, Log, TEXT("[W_Settings_QuitConfirmation] Gamepad cancel"));
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}
