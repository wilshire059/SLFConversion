// W_MainMenu.cpp
// C++ Widget implementation for W_MainMenu
//
// 20-PASS VALIDATION: 2026-01-03

#include "Widgets/W_MainMenu.h"
#include "Widgets/W_MainMenu_Button.h"
#include "Widgets/W_LoadingScreen.h"
#include "Widgets/W_CharacterSelection.h"
#include "Widgets/W_LoadGame.h"
#include "Widgets/W_Settings.h"
#include "Widgets/W_Credits.h"
#include "Widgets/W_WorldMap.h"
#include "Interfaces/SLFMainMenuInterface.h"
#include "Interfaces/BPI_GameInstance.h"
#include "SLFPrimaryDataAssets.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
	, BtnMap(nullptr)
	, BtnQuitGame(nullptr)
	, WorldMapOverlay(nullptr)
	, DynamicMapButton(nullptr)
	, W_CharacterSelection(nullptr)
	, W_LoadGame(nullptr)
	, W_Settings(nullptr)
	, W_Credits(nullptr)
	, ActiveOverlay(nullptr)
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

	// Handle CanContinueGame - show or hide Continue and LoadGame buttons
	// NOTE: These buttons default to Collapsed in UMG designer, so we MUST explicitly
	// set them Visible when a save exists
	if (CanContinueGame)
	{
		if (BtnContinue)
		{
			BtnContinue->SetVisibility(ESlateVisibility::Visible);
		}
		if (BtnLoadGame)
		{
			BtnLoadGame->SetVisibility(ESlateVisibility::Visible);
		}
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Save exists - showing Continue and LoadGame buttons (BtnContinue: %s, BtnLoadGame: %s)"),
			BtnContinue ? TEXT("OK") : TEXT("NULL"), BtnLoadGame ? TEXT("OK") : TEXT("NULL"));
	}
	else
	{
		if (BtnContinue)
		{
			BtnContinue->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (BtnLoadGame)
		{
			BtnLoadGame->SetVisibility(ESlateVisibility::Collapsed);
		}
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] No save game - hiding Continue and LoadGame buttons"));
	}

	// Initialize buttons from ButtonsBox
	InitializeButtons();

	// Bind button events
	BindButtonEvents();

	// Make all buttons visible via PlayButtonInitAnimation with staggered timing
	float StartDelay = 0.0f;
	for (UWidget* ButtonWidget : Buttons)
	{
		if (ButtonWidget && ButtonWidget->Implements<USLFMainMenuInterface>())
		{
			ISLFMainMenuInterface::Execute_PlayButtonInitAnimation(ButtonWidget, StartDelay, 1, EUMGSequencePlayMode::Forward, 1.0);
			StartDelay += 0.15;
		}
	}

	// Set first button as selected
	if (Buttons.Num() > 0)
	{
		NavigationIndex = 0;
		UpdateButtonSelection();
	}

	// Setup overlay widgets (embedded children in UMG hierarchy)
	ActiveOverlay = nullptr;

	if (W_CharacterSelection)
	{
		W_CharacterSelection->SetVisibility(ESlateVisibility::Collapsed);
		W_CharacterSelection->OnClassCardClicked.AddDynamic(this, &UW_MainMenu::OnCharacterClassSelected);
		W_CharacterSelection->OnCharacterSelectionClosed.AddDynamic(this, &UW_MainMenu::OnCharacterSelectionClosed);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] W_CharacterSelection bound (embedded child)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_CharacterSelection NOT found (BindWidgetOptional)"));
	}

	if (W_LoadGame)
	{
		W_LoadGame->SetVisibility(ESlateVisibility::Collapsed);
		W_LoadGame->OnLoadGameClosed.AddDynamic(this, &UW_MainMenu::OnLoadGameClosed);
		W_LoadGame->OnSaveSlotConfirmed.AddDynamic(this, &UW_MainMenu::OnSaveSlotConfirmed);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] W_LoadGame bound (embedded child)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_LoadGame NOT found (BindWidgetOptional)"));
	}

	if (W_Settings)
	{
		W_Settings->SetVisibility(ESlateVisibility::Collapsed);
		W_Settings->OnSettingsClosed.AddDynamic(this, &UW_MainMenu::OnSettingsClosed);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] W_Settings bound (embedded child)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_Settings NOT found (BindWidgetOptional)"));
	}

	if (W_Credits)
	{
		W_Credits->SetVisibility(ESlateVisibility::Collapsed);
		W_Credits->OnCreditsClosed.AddDynamic(this, &UW_MainMenu::OnCreditsClosed);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] W_Credits bound (embedded child)"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_Credits NOT found (BindWidgetOptional)"));
	}

	// Create WorldMap overlay dynamically (pure C++ widget, no Blueprint needed)
	if (APlayerController* PC = GetOwningPlayer())
	{
		WorldMapOverlay = CreateWidget<UW_WorldMap>(PC);
		if (WorldMapOverlay)
		{
			WorldMapOverlay->SetVisibility(ESlateVisibility::Collapsed);
			WorldMapOverlay->OnMapClosed.AddDynamic(this, &UW_MainMenu::OnWorldMapClosed);

			// Add to the root overlay if one exists, otherwise add to viewport separately
			UWidget* Root = GetRootWidget();
			UOverlay* RootOverlay = Cast<UOverlay>(Root);
			if (RootOverlay)
			{
				UOverlaySlot* MapSlot = RootOverlay->AddChildToOverlay(WorldMapOverlay);
				if (MapSlot)
				{
					MapSlot->SetHorizontalAlignment(HAlign_Fill);
					MapSlot->SetVerticalAlignment(VAlign_Fill);
				}
			}
			else if (UPanelWidget* RootPanel = Cast<UPanelWidget>(Root))
			{
				RootPanel->AddChild(WorldMapOverlay);
			}

			UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] WorldMap overlay created and added"));
		}
	}

	// Create dynamic Map button if BtnMap wasn't bound from Blueprint
	if (!BtnMap && ButtonsBox && WidgetTree)
	{
		DynamicMapButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("DynamicMapButton"));
		if (DynamicMapButton)
		{
			// Style: dark semi-transparent background
			FButtonStyle Style = DynamicMapButton->GetStyle();
			FLinearColor DarkBg(0.02f, 0.02f, 0.05f, 0.8f);
			Style.Normal.TintColor = FSlateColor(DarkBg);
			Style.Hovered.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.15f, 0.9f));
			Style.Pressed.TintColor = FSlateColor(FLinearColor(0.15f, 0.15f, 0.2f, 1.0f));
			DynamicMapButton->SetStyle(Style);

			// Create text label
			UTextBlock* MapLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MapLabel"));
			if (MapLabel)
			{
				MapLabel->SetText(FText::FromString(TEXT("Map")));
				MapLabel->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				FSlateFontInfo Font = MapLabel->GetFont();
				Font.Size = 24;
				MapLabel->SetFont(Font);
				MapLabel->SetJustification(ETextJustify::Center);
				DynamicMapButton->AddChild(MapLabel);
			}

			// Insert before the last button (QuitGame) in ButtonsBox
			int32 InsertIndex = ButtonsBox->GetChildrenCount();
			if (BtnQuitGame)
			{
				InsertIndex = ButtonsBox->GetChildIndex(BtnQuitGame);
				if (InsertIndex == INDEX_NONE)
				{
					InsertIndex = ButtonsBox->GetChildrenCount();
				}
			}
			ButtonsBox->InsertChildAt(InsertIndex, DynamicMapButton);

			// Set padding to match other buttons
			if (UVerticalBoxSlot* VBSlot = Cast<UVerticalBoxSlot>(DynamicMapButton->Slot))
			{
				VBSlot->SetPadding(FMargin(0.0f, 4.0f, 0.0f, 4.0f));
				VBSlot->SetHorizontalAlignment(HAlign_Fill);
			}

			DynamicMapButton->OnClicked.AddDynamic(this, &UW_MainMenu::OnDynamicMapButtonClicked);
			UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Dynamic Map button created at index %d"), InsertIndex);
		}
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

	// Unbind overlay events
	if (W_CharacterSelection)
	{
		W_CharacterSelection->OnClassCardClicked.RemoveAll(this);
		W_CharacterSelection->OnCharacterSelectionClosed.RemoveAll(this);
	}
	if (W_LoadGame)
	{
		W_LoadGame->OnLoadGameClosed.RemoveAll(this);
		W_LoadGame->OnSaveSlotConfirmed.RemoveAll(this);
	}
	if (W_Settings)
	{
		W_Settings->OnSettingsClosed.RemoveAll(this);
	}
	if (W_Credits)
	{
		W_Credits->OnCreditsClosed.RemoveAll(this);
	}
	if (WorldMapOverlay)
	{
		WorldMapOverlay->OnMapClosed.RemoveAll(this);
	}
	if (DynamicMapButton)
	{
		DynamicMapButton->OnClicked.RemoveAll(this);
	}

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

	// Navigate Left: A, Left Arrow, Gamepad DPad Left, Left Stick Left
	if (Key == EKeys::A || Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
	{
		EventNavigateLeft();
		return FReply::Handled();
	}

	// Navigate Right: D, Right Arrow, Gamepad DPad Right, Left Stick Right
	if (Key == EKeys::D || Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
	{
		EventNavigateRight();
		return FReply::Handled();
	}

	// Navigate Ok/Confirm: Enter, Space, Gamepad A (FaceButton_Bottom)
	if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
	{
		EventNavigateOk();
		return FReply::Handled();
	}

	// Navigate Cancel/Back: Escape, Gamepad B (FaceButton_Right)
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	// Category Left/Right: Q/E or LB/RB (for Settings tab switching)
	if (Key == EKeys::Q || Key == EKeys::Gamepad_LeftShoulder)
	{
		if (ActiveOverlay && W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateCategoryLeft();
			return FReply::Handled();
		}
	}
	if (Key == EKeys::E || Key == EKeys::Gamepad_RightShoulder)
	{
		if (ActiveOverlay && W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateCategoryRight();
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_MainMenu::InitializeButtons()
{
	Buttons.Empty();

	// Use the individually bound button references
	// Order matters for navigation: Continue, NewGame, LoadGame, Settings, Credits, QuitGame
	// Only add visible buttons (Continue/LoadGame hidden when no save exists)
	if (BtnContinue && BtnContinue->GetVisibility() != ESlateVisibility::Collapsed && BtnContinue->GetVisibility() != ESlateVisibility::Hidden)
	{
		Buttons.Add(BtnContinue);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnContinue"));
	}

	if (BtnNewGame)
	{
		Buttons.Add(BtnNewGame);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnNewGame"));
	}

	if (BtnLoadGame && BtnLoadGame->GetVisibility() != ESlateVisibility::Collapsed && BtnLoadGame->GetVisibility() != ESlateVisibility::Hidden)
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

	if (BtnMap)
	{
		Buttons.Add(BtnMap);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: BtnMap"));
	}
	else if (DynamicMapButton)
	{
		Buttons.Add(DynamicMapButton);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Added button: DynamicMapButton"));
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
	if (BtnMap)
	{
		BtnMap->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnMapClicked);
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
	if (BtnMap) BtnMap->OnButtonClicked.RemoveAll(this);
	if (BtnQuitGame) BtnQuitGame->OnButtonClicked.RemoveAll(this);
}

void UW_MainMenu::OnContinueClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Continue button clicked"));

	// bp_only flow: SetLastSlotNameToActive → OpenLevel via LoadingScreen
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (GI && GI->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		IBPI_GameInstance::Execute_SetLastSlotNameToActive(GI);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] SetLastSlotNameToActive called on GameInstance"));
	}

	LoadGameLevel();
}

void UW_MainMenu::OnNewGameClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] New Game button clicked - showing character selection"));
	if (W_CharacterSelection)
	{
		ShowOverlay(W_CharacterSelection);
	}
	else
	{
		// Fallback: broadcast to PC if no overlay available
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_CharacterSelection not available, broadcasting to PC"));
		OnMenuButtonClicked.Broadcast(FName(TEXT("NewGame")));
	}
}

void UW_MainMenu::OnLoadGameClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Load Game button clicked - showing load game screen"));
	if (W_LoadGame)
	{
		ShowOverlay(W_LoadGame);
	}
	else
	{
		// Fallback: broadcast to PC if no overlay available
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_LoadGame not available, broadcasting to PC"));
		OnMenuButtonClicked.Broadcast(FName(TEXT("LoadGame")));
	}
}

void UW_MainMenu::OnSettingsClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Settings button clicked - showing settings overlay"));
	if (W_Settings)
	{
		ShowOverlay(W_Settings);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_Settings not available, broadcasting to PC"));
		OnMenuButtonClicked.Broadcast(FName(TEXT("Settings")));
	}
}

void UW_MainMenu::OnCreditsClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Credits button clicked - showing credits overlay"));
	if (W_Credits)
	{
		W_Credits->EventInitializeCredits();
		ShowOverlay(W_Credits);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] W_Credits not available, broadcasting to PC"));
		OnMenuButtonClicked.Broadcast(FName(TEXT("Credits")));
	}
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

	// Block main menu navigation when an overlay is active
	if (ActiveOverlay)
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
	// Forward to active overlay
	if (ActiveOverlay)
	{
		if (W_LoadGame && ActiveOverlay == W_LoadGame)
		{
			W_LoadGame->EventNavigateDown();
		}
		else if (W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateDown();
		}
		else if (W_Credits && ActiveOverlay == W_Credits)
		{
			W_Credits->EventNavigateDown();
		}
		// CharacterSelection uses Left/Right, not Up/Down
		return;
	}

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
	// Forward to active overlay
	if (ActiveOverlay)
	{
		if (W_LoadGame && ActiveOverlay == W_LoadGame)
		{
			W_LoadGame->EventNavigateUp();
		}
		else if (W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateUp();
		}
		else if (W_Credits && ActiveOverlay == W_Credits)
		{
			W_Credits->EventNavigateUp();
		}
		// CharacterSelection uses Left/Right, not Up/Down
		return;
	}

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
	// Forward to active overlay
	if (ActiveOverlay)
	{
		if (W_CharacterSelection && ActiveOverlay == W_CharacterSelection)
		{
			W_CharacterSelection->EventNavigateOk();
		}
		else if (W_LoadGame && ActiveOverlay == W_LoadGame)
		{
			W_LoadGame->EventNavigateOk();
		}
		else if (W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateOk();
		}
		// WorldMap has no confirm action - do nothing
		return;
	}

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

void UW_MainMenu::EventNavigateCancel_Implementation()
{
	// Forward to active overlay
	if (ActiveOverlay)
	{
		if (W_CharacterSelection && ActiveOverlay == W_CharacterSelection)
		{
			W_CharacterSelection->EventNavigateCancel();
		}
		else if (W_LoadGame && ActiveOverlay == W_LoadGame)
		{
			W_LoadGame->EventNavigateCancel();
		}
		else if (W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateCancel();
		}
		else if (W_Credits && ActiveOverlay == W_Credits)
		{
			W_Credits->EventNavigateCancel();
		}
		else if (WorldMapOverlay && ActiveOverlay == WorldMapOverlay)
		{
			WorldMapOverlay->EventNavigateCancel();
		}
		return;
	}

	// No overlay active - cancel on main menu does nothing
}

void UW_MainMenu::EventNavigateLeft_Implementation()
{
	// Forward to active overlay
	if (ActiveOverlay)
	{
		if (W_CharacterSelection && ActiveOverlay == W_CharacterSelection)
		{
			W_CharacterSelection->EventNavigateLeft();
		}
		else if (W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateLeft();
		}
		return;
	}
}

void UW_MainMenu::EventNavigateRight_Implementation()
{
	// Forward to active overlay
	if (ActiveOverlay)
	{
		if (W_CharacterSelection && ActiveOverlay == W_CharacterSelection)
		{
			W_CharacterSelection->EventNavigateRight();
		}
		else if (W_Settings && ActiveOverlay == W_Settings)
		{
			W_Settings->EventNavigateRight();
		}
		return;
	}
}

// ═══════════════════════════════════════════════════════════════════════
// OVERLAY MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════

void UW_MainMenu::ShowOverlay(UWidget* Overlay)
{
	if (!Overlay)
	{
		return;
	}

	ActiveOverlay = Overlay;

	if (FadeMenuOnly)
	{
		// Play fade-out animation for menu buttons, then show overlay
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &UW_MainMenu::OnFadeMenuOnlyFinished);
		BindToAnimationFinished(FadeMenuOnly, Delegate);
		PlayAnimationForward(FadeMenuOnly);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Playing FadeMenuOnly, will show overlay on finish"));
	}
	else
	{
		// No animation available, show immediately
		Overlay->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Showing overlay immediately (no FadeMenuOnly animation)"));
	}
}

void UW_MainMenu::OnFadeMenuOnlyFinished()
{
	UnbindAllFromAnimationFinished(FadeMenuOnly);

	if (ActiveOverlay)
	{
		ActiveOverlay->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] FadeMenuOnly finished - overlay now visible"));
	}
}

void UW_MainMenu::ReturnFromOverlay()
{
	if (ActiveOverlay)
	{
		ActiveOverlay->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Overlay hidden"));
	}
	ActiveOverlay = nullptr;

	// Fade menu back in (plays FadeMenuOnly in reverse)
	EventFadeInMenu(1.0f);

	// Restore focus to main menu for keyboard navigation
	SetFocus();
}

void UW_MainMenu::OnCharacterClassSelected(UPrimaryDataAsset* ClassAsset)
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Character class selected: %s"), *GetNameSafe(ClassAsset));

	// Hide overlay
	if (ActiveOverlay)
	{
		ActiveOverlay->SetVisibility(ESlateVisibility::Collapsed);
		ActiveOverlay = nullptr;
	}

	// bp_only flow: SetSelectedClass → GetUniqueSlotName → SetActiveSlot → OpenLevel
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (GI && GI->GetClass()->ImplementsInterface(UBPI_GameInstance::StaticClass()))
	{
		// 1. Set the selected character class
		IBPI_GameInstance::Execute_SetSelectedClass(GI, ClassAsset);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] SetSelectedClass: %s"), *GetNameSafe(ClassAsset));

		// 2. Generate a unique slot name: "SLF_{CharacterName}_{index}"
		FString NewSlotName = GenerateUniqueSlotName(ClassAsset);

		// 3. Register the new slot in SGO_Slots so Continue/Load can find it
		IBPI_GameInstance::Execute_AddAndSaveSlots(GI, NewSlotName);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] AddAndSaveSlots: %s"), *NewSlotName);

		// 4. Set this as the active slot
		IBPI_GameInstance::Execute_SetActiveSlot(GI, NewSlotName);
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] SetActiveSlot: %s"), *NewSlotName);
	}

	// 5. Load the game level
	LoadGameLevel();
}

void UW_MainMenu::OnCharacterSelectionClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Character selection closed - returning to main menu"));
	ReturnFromOverlay();
}

void UW_MainMenu::OnLoadGameClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Load game closed (cancelled) - returning to main menu"));
	ReturnFromOverlay();
}

void UW_MainMenu::OnSaveSlotConfirmed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Save slot confirmed - loading game level"));

	// Hide overlay
	if (ActiveOverlay)
	{
		ActiveOverlay->SetVisibility(ESlateVisibility::Collapsed);
		ActiveOverlay = nullptr;
	}

	// Active slot was already set by W_LoadGame::EventOnSaveSlotSelected
	// Just open the level
	LoadGameLevel();
}

void UW_MainMenu::OnSettingsClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Settings closed - returning to main menu"));
	ReturnFromOverlay();
}

void UW_MainMenu::OnCreditsClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Credits closed - returning to main menu"));
	ReturnFromOverlay();
}

void UW_MainMenu::OnMapClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Map button clicked - showing world map"));
	if (WorldMapOverlay)
	{
		ShowOverlay(WorldMapOverlay);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] WorldMapOverlay not available"));
		OnMenuButtonClicked.Broadcast(FName(TEXT("Map")));
	}
}

void UW_MainMenu::OnDynamicMapButtonClicked()
{
	OnMapClicked();
}

void UW_MainMenu::OnWorldMapClosed()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] World map closed - returning to main menu"));
	ReturnFromOverlay();
}

// ═══════════════════════════════════════════════════════════════════════
// SAVE/LOAD HELPERS
// ═══════════════════════════════════════════════════════════════════════

FString UW_MainMenu::GenerateUniqueSlotName(UPrimaryDataAsset* CharacterClass)
{
	// bp_only format: "SLF_{CharacterClassName}_{index}"
	// Uses BFL_Helper::GetUniqueSlotName logic: loop until DoesSaveGameExist returns false

	FString CharName = TEXT("Character");

	if (UPDA_BaseCharacterInfo* CharInfo = Cast<UPDA_BaseCharacterInfo>(CharacterClass))
	{
		// Use DisplayName (inherited from UPDA_Base), which maps to bp_only CharacterClassName
		FString DisplayStr = CharInfo->DisplayName.ToString();
		if (!DisplayStr.IsEmpty())
		{
			CharName = DisplayStr;
		}
	}
	else if (CharacterClass)
	{
		// Fallback to asset name
		CharName = CharacterClass->GetName();
	}

	// Find unique index
	for (int32 Index = 0; Index < 999; ++Index)
	{
		FString SlotName = FString::Printf(TEXT("SLF_%s_%d"), *CharName, Index);
		if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
		{
			UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Generated unique slot name: %s"), *SlotName);
			return SlotName;
		}
	}

	// Fallback with timestamp if somehow 999 slots exist
	FString Fallback = FString::Printf(TEXT("SLF_%s_%lld"), *CharName, FDateTime::Now().GetTicks());
	UE_LOG(LogTemp, Warning, TEXT("[W_MainMenu] All slot indices taken, using timestamp: %s"), *Fallback);
	return Fallback;
}

void UW_MainMenu::LoadGameLevel()
{
	UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] LoadGameLevel - using LoadingScreen: %s"),
		LoadingScreen ? TEXT("YES") : TEXT("NO"));

	// Use loading screen if available (matches bp_only: EventOpenLevelByNameAndFadeOut)
	if (LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);
		LoadingScreen->EventOpenLevelByNameAndFadeOut(FName(TEXT("L_Demo_Showcase")), true, TEXT(""));
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Loading L_Demo_Showcase via LoadingScreen"));
	}
	else
	{
		// Fallback: direct level load
		UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("L_Demo_Showcase")));
		UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Loading L_Demo_Showcase directly (no LoadingScreen)"));
	}
}
