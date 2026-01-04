// W_GameMenu.cpp
// C++ Widget implementation for W_GameMenu
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_GameMenu.h"
#include "Widgets/W_GameMenu_Button.h"
#include "Components/VerticalBox.h"
#include "Animation/WidgetAnimation.h"

UW_GameMenu::UW_GameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NavigationIndex(0)
	, ButtonsBox(nullptr)
	, FadeIn(nullptr)
{
}

void UW_GameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] NativeConstruct"));

	// Initialize menu buttons from ButtonsBox
	InitializeMenuButtons();

	// Set first button as selected
	if (MenuButtons.Num() > 0)
	{
		NavigationIndex = 0;
		UpdateButtonSelection();
	}

	// Play fade in animation if available
	if (FadeIn)
	{
		PlayAnimationForward(FadeIn);
	}
}

void UW_GameMenu::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] NativeDestruct"));
}

void UW_GameMenu::InitializeMenuButtons()
{
	MenuButtons.Empty();

	if (!ButtonsBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_GameMenu] ButtonsBox is null"));
		return;
	}

	// Get all children from ButtonsBox that are GameMenu buttons
	TArray<UWidget*> Children = ButtonsBox->GetAllChildren();

	for (UWidget* Child : Children)
	{
		if (UW_GameMenu_Button* Button = Cast<UW_GameMenu_Button>(Child))
		{
			if (Button->IsVisible())
			{
				MenuButtons.Add(Button);
				UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Added button: %s"), *Button->GetName());
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Initialized %d menu buttons"), MenuButtons.Num());
}

void UW_GameMenu::UpdateButtonSelection()
{
	if (MenuButtons.Num() == 0)
	{
		return;
	}

	// Clamp navigation index
	NavigationIndex = FMath::Clamp(NavigationIndex, 0, MenuButtons.Num() - 1);

	// Get the selected button
	UW_GameMenu_Button* SelectedButton = MenuButtons.IsValidIndex(NavigationIndex) ? MenuButtons[NavigationIndex] : nullptr;

	// Update all buttons
	for (UW_GameMenu_Button* Button : MenuButtons)
	{
		if (Button)
		{
			bool bIsSelected = (Button == SelectedButton);
			Button->SetGameMenuButtonSelected(bIsSelected);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Navigation index: %d"), NavigationIndex);
}

void UW_GameMenu::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateCancel"));

	// Hide the menu
	SetVisibility(ESlateVisibility::Collapsed);

	// Broadcast that the menu is closed
	OnGameMenuClosed.Broadcast();
}

void UW_GameMenu::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateDown"));

	if (MenuButtons.Num() == 0)
	{
		return;
	}

	// Increment navigation index
	NavigationIndex++;

	// Clamp to last button (no wrap)
	if (NavigationIndex >= MenuButtons.Num())
	{
		NavigationIndex = MenuButtons.Num() - 1;
	}

	UpdateButtonSelection();
}

void UW_GameMenu::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateUp"));

	if (MenuButtons.Num() == 0)
	{
		return;
	}

	// Decrement navigation index
	NavigationIndex--;

	// Clamp to first button (no wrap)
	if (NavigationIndex < 0)
	{
		NavigationIndex = 0;
	}

	UpdateButtonSelection();
}

void UW_GameMenu::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateOk"));

	if (!MenuButtons.IsValidIndex(NavigationIndex))
	{
		return;
	}

	// Get the current button and trigger its pressed event
	UW_GameMenu_Button* CurrentButton = MenuButtons[NavigationIndex];
	if (CurrentButton)
	{
		CurrentButton->EventOnGameMenuButtonPressed();
	}
}

void UW_GameMenu::EventOnMenuButtonPressed_Implementation(FGameplayTag WidgetTag)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventOnMenuButtonPressed - Tag: %s"), *WidgetTag.ToString());

	// Broadcast the widget request
	OnGameMenuWidgetRequest.Broadcast(WidgetTag);
}

void UW_GameMenu::EventOnMenuButtonSelected_Implementation(UW_GameMenu_Button* Button)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventOnMenuButtonSelected: %s"), Button ? *Button->GetName() : TEXT("null"));

	if (!Button)
	{
		return;
	}

	// Update navigation index to match the selected button
	int32 FoundIndex = MenuButtons.Find(Button);
	if (FoundIndex != INDEX_NONE)
	{
		NavigationIndex = FoundIndex;

		// Update all buttons to reflect new selection
		for (UW_GameMenu_Button* MenuButton : MenuButtons)
		{
			if (MenuButton)
			{
				bool bIsSelected = (MenuButton == Button);
				MenuButton->SetGameMenuButtonSelected(bIsSelected);
			}
		}
	}
}

void UW_GameMenu::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventOnVisibilityChanged: %d"), static_cast<int32>(InVisibility));

	// When becoming visible, select the first button
	if (InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		if (MenuButtons.Num() > 0)
		{
			NavigationIndex = 0;
			UpdateButtonSelection();
		}

		// Play fade in animation
		if (FadeIn)
		{
			PlayAnimationForward(FadeIn);
		}
	}
}
