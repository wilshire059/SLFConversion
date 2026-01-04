// W_MainMenu.cpp
// C++ Widget implementation for W_MainMenu
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_MainMenu.h"
#include "Widgets/W_MainMenu_Button.h"
#include "Widgets/W_LoadingScreen.h"
#include "Interfaces/SLFMainMenuInterface.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"

UW_MainMenu::UW_MainMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MediaPlayer(nullptr)
	, CanContinueGame(false)
	, LoadingScreen(nullptr)
	, MainMenuAsset(nullptr)
	, NavigationIndex(0)
	, ButtonsBox(nullptr)
	, BtnContinue(nullptr)
	, BtnNewGame(nullptr)
	, BtnLoadGame(nullptr)
	, BtnSettings(nullptr)
	, BtnCredits(nullptr)
	, BtnQuitGame(nullptr)
	, Fade(nullptr)
	, FadeMenuOnly(nullptr)
{
}

void UW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] NativeConstruct"));

	// Initialize buttons from ButtonsBox
	InitializeButtons();

	// Bind button events
	BindButtonEvents();

	// Set first button as selected
	if (Buttons.Num() > 0)
	{
		NavigationIndex = 0;
		UpdateButtonSelection();
	}

	// Play fade-in animation
	if (Fade)
	{
		PlayAnimationForward(Fade);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Playing Fade animation"));
	}
}

void UW_MainMenu::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] NativeDestruct"));
}

void UW_MainMenu::InitializeButtons()
{
	Buttons.Empty();

	// Use the individually bound button references
	// Order matters for navigation: Continue, NewGame, LoadGame, Settings, Credits, QuitGame
	if (BtnContinue && BtnContinue->IsVisible())
	{
		Buttons.Add(BtnContinue);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnContinue"));
	}

	if (BtnNewGame)
	{
		Buttons.Add(BtnNewGame);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnNewGame"));
	}

	if (BtnLoadGame)
	{
		Buttons.Add(BtnLoadGame);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnLoadGame"));
	}

	if (BtnSettings)
	{
		Buttons.Add(BtnSettings);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnSettings"));
	}

	if (BtnCredits)
	{
		Buttons.Add(BtnCredits);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnCredits"));
	}

	if (BtnQuitGame)
	{
		Buttons.Add(BtnQuitGame);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnQuitGame"));
	}

	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Initialized %d buttons"), Buttons.Num());
}

void UW_MainMenu::BindButtonEvents()
{
	// Button events are bound in Blueprint via event dispatchers
	// The OnSelected event on each button calls back to OnButtonSelected
	// This is handled by the Blueprint widget bindings
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Button events will be handled via Blueprint bindings"));
}

void UW_MainMenu::UpdateButtonSelection()
{
	if (Buttons.Num() == 0)
	{
		return;
	}

	// Clamp navigation index
	NavigationIndex = FMath::Clamp(NavigationIndex, 0, Buttons.Num() - 1);

	// Get the selected button
	UWidget* SelectedWidget = Buttons.IsValidIndex(NavigationIndex) ? Buttons[NavigationIndex] : nullptr;

	// Update all buttons via interface
	for (UWidget* ButtonWidget : Buttons)
	{
		if (ButtonWidget && ButtonWidget->Implements<USLFMainMenuInterface>())
		{
			bool bIsSelected = (ButtonWidget == SelectedWidget);
			ISLFMainMenuInterface::Execute_SetMenuButtonSelected(ButtonWidget, bIsSelected);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Navigation index: %d"), NavigationIndex);
}

bool UW_MainMenu::CanNavigate_Implementation()
{
	// Check if widget is visible and not in a loading state
	if (!IsVisible())
	{
		return false;
	}

	// Check if loading screen is visible
	if (LoadingScreen && LoadingScreen->IsVisible())
	{
		return false;
	}

	return true;
}

void UW_MainMenu::OnButtonSelected_Implementation(UW_MainMenu_Button* Button)
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] OnButtonSelected: %s"), Button ? *Button->GetName() : TEXT("null"));

	if (!Button)
	{
		return;
	}

	// Loop through all buttons and update their selection state
	for (UWidget* ButtonWidget : Buttons)
	{
		if (ButtonWidget && ButtonWidget->Implements<USLFMainMenuInterface>())
		{
			// Set selected = true for the selected button, false for others
			bool bIsSelected = (ButtonWidget == Button);
			ISLFMainMenuInterface::Execute_SetMenuButtonSelected(ButtonWidget, bIsSelected);
		}
	}

	// Update navigation index to match the selected button
	int32 FoundIndex = Buttons.Find(Button);
	if (FoundIndex != INDEX_NONE)
	{
		NavigationIndex = FoundIndex;
	}
}

void UW_MainMenu::EventFadeInMenu_Implementation(float PlaybackSpeed)
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] EventFadeInMenu (PlaybackSpeed: %.2f)"), PlaybackSpeed);

	if (FadeMenuOnly)
	{
		PlayAnimationReverse(FadeMenuOnly, PlaybackSpeed);
	}
}

void UW_MainMenu::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] EventNavigateDown"));

	if (!CanNavigate())
	{
		return;
	}

	if (Buttons.Num() == 0)
	{
		return;
	}

	// Increment navigation index (clamped in UpdateButtonSelection)
	NavigationIndex++;

	// Wrap around or clamp
	if (NavigationIndex >= Buttons.Num())
	{
		NavigationIndex = Buttons.Num() - 1; // Clamp to last
	}

	UpdateButtonSelection();
}

void UW_MainMenu::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] EventNavigateUp"));

	if (!CanNavigate())
	{
		return;
	}

	if (Buttons.Num() == 0)
	{
		return;
	}

	// Decrement navigation index
	NavigationIndex--;

	// Clamp to first
	if (NavigationIndex < 0)
	{
		NavigationIndex = 0;
	}

	UpdateButtonSelection();
}

void UW_MainMenu::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] EventNavigateOk"));

	if (!CanNavigate())
	{
		return;
	}

	if (!Buttons.IsValidIndex(NavigationIndex))
	{
		return;
	}

	// Get the current button and call its pressed handler
	UWidget* CurrentButton = Buttons[NavigationIndex];
	if (CurrentButton && CurrentButton->Implements<USLFMainMenuInterface>())
	{
		ISLFMainMenuInterface::Execute_OnMenuButtonPressed(CurrentButton);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Called OnMenuButtonPressed on button %d"), NavigationIndex);
	}
}
