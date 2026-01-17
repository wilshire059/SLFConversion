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

	// Make focusable for keyboard input
	SetIsFocusable(true);

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
	// Unbind button events before destruction
	UnbindButtonEvents();

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] NativeDestruct"));
}

FReply UW_MainMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] NativeOnKeyDown - Key: %s"), *Key.ToString());

	// Navigate Up: W, Up Arrow, Gamepad DPad Up, Left Stick Up
	if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
	{
		EventNavigateUp();
		return FReply::Handled();
	}

	// Navigate Down: S, Down Arrow, Gamepad DPad Down, Left Stick Down
	if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
	{
		EventNavigateDown();
		return FReply::Handled();
	}

	// Navigate Ok/Confirm: Enter, Space, Gamepad A (FaceButton_Bottom)
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Binding button events"));

	// Bind OnButtonClicked events from each button
	if (BtnContinue)
	{
		BtnContinue->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnContinueClicked);
	}
	if (BtnNewGame)
	{
		BtnNewGame->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnNewGameClicked);
	}
	if (BtnLoadGame)
	{
		BtnLoadGame->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnLoadGameClicked);
	}
	if (BtnSettings)
	{
		BtnSettings->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnSettingsClicked);
	}
	if (BtnCredits)
	{
		BtnCredits->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnCreditsClicked);
	}
	if (BtnQuitGame)
	{
		BtnQuitGame->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnQuitGameClicked);
	}
}

void UW_MainMenu::UnbindButtonEvents()
{
	if (BtnContinue) BtnContinue->OnButtonClicked.RemoveAll(this);
	if (BtnNewGame) BtnNewGame->OnButtonClicked.RemoveAll(this);
	if (BtnLoadGame) BtnLoadGame->OnButtonClicked.RemoveAll(this);
	if (BtnSettings) BtnSettings->OnButtonClicked.RemoveAll(this);
	if (BtnCredits) BtnCredits->OnButtonClicked.RemoveAll(this);
	if (BtnQuitGame) BtnQuitGame->OnButtonClicked.RemoveAll(this);
}

void UW_MainMenu::OnContinueClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Continue button clicked"));
	OnMenuButtonClicked.Broadcast(FName(TEXT("Continue")));
}

void UW_MainMenu::OnNewGameClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] New Game button clicked"));
	OnMenuButtonClicked.Broadcast(FName(TEXT("NewGame")));
}

void UW_MainMenu::OnLoadGameClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Load Game button clicked"));
	OnMenuButtonClicked.Broadcast(FName(TEXT("LoadGame")));
}

void UW_MainMenu::OnSettingsClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Settings button clicked"));
	OnMenuButtonClicked.Broadcast(FName(TEXT("Settings")));
}

void UW_MainMenu::OnCreditsClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Credits button clicked"));
	OnMenuButtonClicked.Broadcast(FName(TEXT("Credits")));
}

void UW_MainMenu::OnQuitGameClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Quit Game button clicked"));
	OnMenuButtonClicked.Broadcast(FName(TEXT("QuitGame")));
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

	// Increment navigation index with wrap-around
	NavigationIndex++;
	if (NavigationIndex >= Buttons.Num())
	{
		NavigationIndex = 0;  // Wrap to first
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

	// Decrement navigation index with wrap-around
	NavigationIndex--;
	if (NavigationIndex < 0)
	{
		NavigationIndex = Buttons.Num() - 1;  // Wrap to last
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
