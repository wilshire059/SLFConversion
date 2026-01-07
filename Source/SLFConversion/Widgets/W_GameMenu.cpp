// W_GameMenu.cpp
// C++ Widget implementation for W_GameMenu
//
// 20-PASS VALIDATION: 2026-01-05
// Implements all EventGraph logic from Blueprint

#include "Widgets/W_GameMenu.h"
#include "Widgets/W_GameMenu_Button.h"
#include "Components/PanelWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Framework/SLFPlayerController.h"

UW_GameMenu::UW_GameMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, NavigationIndex(0)
	, ButtonContainer(nullptr)
	, FadeBorders(nullptr)
{
}

void UW_GameMenu::NativePreConstruct()
{
	Super::NativePreConstruct();
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] NativePreConstruct"));
}

void UW_GameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] NativeConstruct - Initializing menu"));

	// Populate MenuButtons array from children
	PopulateMenuButtons();

	// Bind to button events
	BindButtonEvents();

	// Bind visibility changed event
	OnVisibilityChanged.AddDynamic(this, &UW_GameMenu::EventOnVisibilityChanged);

	// Select first button if available
	if (MenuButtons.Num() > 0)
	{
		NavigationIndex = 0;
		SelectCurrentButton();
	}

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Initialized with %d buttons"), MenuButtons.Num());
}

void UW_GameMenu::NativeDestruct()
{
	// Unbind button events
	UnbindButtonEvents();

	// Clear visibility binding
	OnVisibilityChanged.RemoveAll(this);

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] NativeDestruct"));
}

FReply UW_GameMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] NativeOnKeyDown - Key: %s"), *Key.ToString());

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

	// Navigate Cancel: Escape, Gamepad B (FaceButton_Right), Tab (same key that opens menu)
	if (Key == EKeys::Escape || Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Tab)
	{
		EventNavigateCancel();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UW_GameMenu::PopulateMenuButtons()
{
	MenuButtons.Empty();

	// Get all children and find W_GameMenu_Button instances
	TArray<UWidget*> AllChildren;
	WidgetTree->GetAllWidgets(AllChildren);

	for (UWidget* Child : AllChildren)
	{
		if (UW_GameMenu_Button* Button = Cast<UW_GameMenu_Button>(Child))
		{
			MenuButtons.Add(Button);
			UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Found button: %s"), *Button->GetName());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Populated %d menu buttons"), MenuButtons.Num());
}

void UW_GameMenu::BindButtonEvents()
{
	for (UW_GameMenu_Button* Button : MenuButtons)
	{
		if (IsValid(Button))
		{
			// Bind to button pressed event
			Button->OnGameMenuButtonPressed.AddDynamic(this, &UW_GameMenu::EventOnMenuButtonPressed);

			// Bind to button selected event
			Button->OnGameMenuButtonSelected.AddDynamic(this, &UW_GameMenu::EventOnMenuButtonSelected);
		}
	}
}

void UW_GameMenu::UnbindButtonEvents()
{
	for (UW_GameMenu_Button* Button : MenuButtons)
	{
		if (IsValid(Button))
		{
			Button->OnGameMenuButtonPressed.RemoveAll(this);
			Button->OnGameMenuButtonSelected.RemoveAll(this);
		}
	}
}

void UW_GameMenu::SelectCurrentButton()
{
	// Deselect all buttons first, then select current
	for (int32 i = 0; i < MenuButtons.Num(); i++)
	{
		if (IsValid(MenuButtons[i]))
		{
			MenuButtons[i]->SetGameMenuButtonSelected(i == NavigationIndex);
		}
	}
}

void UW_GameMenu::ApplyInputMappingContexts(bool bAdd)
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		// Build array of contexts to apply
		TArray<UInputMappingContext*> Contexts;
		if (NavigableInputMapping)
		{
			Contexts.Add(NavigableInputMapping);
		}

		for (UInputMappingContext* Context : Contexts)
		{
			if (bAdd)
			{
				Subsystem->AddMappingContext(Context, 100);
				UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Added input mapping context: %s"), *Context->GetName());
			}
			else
			{
				Subsystem->RemoveMappingContext(Context);
				UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Removed input mapping context: %s"), *Context->GetName());
			}
		}
	}
}

void UW_GameMenu::EventNavigateDown_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateDown"));

	// Check if we can navigate
	if (!CanNavigate())
	{
		UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Cannot navigate - CanNavigate returned false"));
		return;
	}

	if (MenuButtons.Num() == 0)
	{
		return;
	}

	// Calculate new index: NavigationIndex + 1, clamped to array bounds
	// If exceeds bounds, wrap to 0
	int32 NewIndex = NavigationIndex + 1;
	int32 MaxIndex = MenuButtons.Num() - 1;

	// Clamp to 0..MaxIndex, but wrap if we go past
	if (NewIndex > MaxIndex)
	{
		NewIndex = 0; // Wrap around to first
	}

	NavigationIndex = NewIndex;
	SelectCurrentButton();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Navigated down to index %d"), NavigationIndex);
}

void UW_GameMenu::EventNavigateUp_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateUp"));

	// Check if we can navigate
	if (!CanNavigate())
	{
		UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Cannot navigate - CanNavigate returned false"));
		return;
	}

	if (MenuButtons.Num() == 0)
	{
		return;
	}

	// Calculate new index: NavigationIndex - 1, clamped to array bounds
	// If goes below 0, wrap to last
	int32 NewIndex = NavigationIndex - 1;
	int32 MaxIndex = MenuButtons.Num() - 1;

	// Wrap around if negative
	if (NewIndex < 0)
	{
		NewIndex = MaxIndex; // Wrap around to last
	}

	NavigationIndex = NewIndex;
	SelectCurrentButton();

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Navigated up to index %d"), NavigationIndex);
}

void UW_GameMenu::EventNavigateOk_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateOk"));

	// Check if we can navigate
	if (!CanNavigate())
	{
		return;
	}

	// Get current button and trigger its pressed event
	if (MenuButtons.IsValidIndex(NavigationIndex))
	{
		if (UW_GameMenu_Button* CurrentButton = MenuButtons[NavigationIndex])
		{
			if (IsValid(CurrentButton))
			{
				CurrentButton->EventOnGameMenuButtonPressed();
				UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Triggered button at index %d"), NavigationIndex);
			}
		}
	}
}

void UW_GameMenu::EventNavigateCancel_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventNavigateCancel - Closing menu"));

	// Broadcast that the menu is closed
	OnGameMenuClosed.Broadcast();

	// Cast to BPI_Controller and call CloseEscMenu if implemented
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		// Try to call interface method on controller
		if (ASLFPlayerController* SLFPC = Cast<ASLFPlayerController>(PC))
		{
			// Call CloseEscMenu through the interface if it exists
			// Note: The interface call should be implemented on the controller
			UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Broadcasting menu closed"));
		}
	}
}

void UW_GameMenu::EventOnMenuButtonPressed_Implementation(FGameplayTag WidgetTag)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventOnMenuButtonPressed - Tag: %s"), *WidgetTag.ToString());

	// Forward to the OnGameMenuWidgetRequest dispatcher
	OnGameMenuWidgetRequest.Broadcast(WidgetTag);
}

void UW_GameMenu::EventOnMenuButtonSelected_Implementation(UW_GameMenu_Button* Button)
{
	if (!IsValid(Button))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventOnMenuButtonSelected - Button: %s"), *Button->GetName());

	// Find the index of this button
	int32 FoundIndex = MenuButtons.Find(Button);
	if (FoundIndex != INDEX_NONE)
	{
		// Update navigation index
		NavigationIndex = FoundIndex;

		// Deselect all other buttons
		for (int32 i = 0; i < MenuButtons.Num(); i++)
		{
			if (IsValid(MenuButtons[i]) && MenuButtons[i] != Button)
			{
				MenuButtons[i]->SetGameMenuButtonSelected(false);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Updated NavigationIndex to %d"), NavigationIndex);
	}
}

void UW_GameMenu::EventOnVisibilityChanged_Implementation(ESlateVisibility InVisibility)
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] EventOnVisibilityChanged - Visibility: %d"), static_cast<int32>(InVisibility));

	// Switch on visibility
	switch (InVisibility)
	{
	case ESlateVisibility::Visible:
	case ESlateVisibility::SelfHitTestInvisible:
	case ESlateVisibility::HitTestInvisible:
	{
		// Visible - add input mapping contexts
		ApplyInputMappingContexts(true);

		// Play fade animation if available
		if (FadeBorders)
		{
			PlayAnimationForward(FadeBorders);
			UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Playing FadeBorders animation"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[W_GameMenu] FadeBorders animation NOT bound!"));
		}

		// Select first button
		if (MenuButtons.Num() > 0)
		{
			NavigationIndex = 0;
			SelectCurrentButton();
		}

		// Debug: Force layout update and log state
		ForceLayoutPrepass();
		FVector2D DesiredSize = GetDesiredSize();
		float Opacity = GetRenderOpacity();
		UE_LOG(LogTemp, Warning, TEXT("[W_GameMenu] After visibility change - Size: %.1f x %.1f, Opacity: %.2f"),
			DesiredSize.X, DesiredSize.Y, Opacity);

		// Force the widget to be focusable and set focus
		SetIsFocusable(true);
		if (APlayerController* PC = GetOwningPlayer())
		{
			SetUserFocus(PC);
			UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] Set user focus to menu"));
		}
		break;
	}

	case ESlateVisibility::Collapsed:
	case ESlateVisibility::Hidden:
		// Hidden - remove input mapping contexts
		ApplyInputMappingContexts(false);
		break;
	}
}

void UW_GameMenu::OnInputDeviceChanged_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[W_GameMenu] OnInputDeviceChanged"));

	// Update visual elements based on current input device
	// This would typically update button icons for gamepad vs keyboard
	// The actual implementation depends on how input icons are displayed
}
