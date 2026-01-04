// PC_Menu_SoulslikeFramework.cpp
// C++ implementation for Game Framework class PC_Menu_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-03

#include "GameFramework/PC_Menu_SoulslikeFramework.h"
#include "Widgets/W_MainMenu.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

APC_Menu_SoulslikeFramework::APC_Menu_SoulslikeFramework()
	: W_MainMenu(nullptr)
	, SaveGameExists(false)
	, MainMenuWidgetClass(nullptr)
	, MenuInputMappingContext(nullptr)
	, IA_NavigableMenu_Up(nullptr)
	, IA_NavigableMenu_Down(nullptr)
	, IA_NavigableMenu_Ok(nullptr)
	, IA_NavigableMenu_Back(nullptr)
{
}

void APC_Menu_SoulslikeFramework::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] BeginPlay"));

	// Load input actions and bind them
	LoadInputActions();
	BindInputActions();

	// Check if save game exists
	SaveGameExists = CheckSaveGameExists();
	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] SaveGameExists: %s"), SaveGameExists ? TEXT("true") : TEXT("false"));

	// Create and display the main menu widget
	CreateMainMenuWidget();

	// Setup menu input
	SetupMenuInput();

	// Show mouse cursor for menu
	bShowMouseCursor = true;

	// Set input mode to Game and UI
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(true);
	if (W_MainMenu)
	{
		InputMode.SetWidgetToFocus(W_MainMenu->TakeWidget());
	}
	SetInputMode(InputMode);

	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Menu setup complete"));
}

void APC_Menu_SoulslikeFramework::CreateMainMenuWidget()
{
	// Try to load the widget class if not set
	if (!MainMenuWidgetClass)
	{
		// Load the Blueprint widget class at runtime
		UClass* LoadedClass = LoadClass<UW_MainMenu>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/MainMenu/W_MainMenu.W_MainMenu_C"));
		if (LoadedClass)
		{
			MainMenuWidgetClass = LoadedClass;
		}
	}

	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[PC_Menu_SoulslikeFramework] MainMenuWidgetClass not set and failed to load!"));
		return;
	}

	// Create the widget
	W_MainMenu = CreateWidget<UW_MainMenu>(this, MainMenuWidgetClass);
	if (!W_MainMenu)
	{
		UE_LOG(LogTemp, Error, TEXT("[PC_Menu_SoulslikeFramework] Failed to create W_MainMenu widget!"));
		return;
	}

	// Set CanContinueGame based on save game existence
	W_MainMenu->CanContinueGame = SaveGameExists;

	// Add to viewport
	W_MainMenu->AddToViewport(0);

	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] W_MainMenu created and added to viewport"));
}

void APC_Menu_SoulslikeFramework::SetupMenuInput()
{
	// Get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Try to load the menu input mapping context if not set
		if (!MenuInputMappingContext)
		{
			MenuInputMappingContext = LoadObject<UInputMappingContext>(nullptr,
				TEXT("/Game/SoulslikeFramework/Input/IMC_MainMenu.IMC_MainMenu"));
		}

		if (MenuInputMappingContext)
		{
			// Add the menu input mapping context
			Subsystem->AddMappingContext(MenuInputMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Menu input mapping context added"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[PC_Menu_SoulslikeFramework] MenuInputMappingContext not set"));
		}
	}
}

bool APC_Menu_SoulslikeFramework::CheckSaveGameExists()
{
	// Check for any save game slot
	// The original Blueprint called DoesAnySaveExist on the GameInstance
	// For now, just check if a default save slot exists
	return UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot_0"), 0) ||
		   UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot_1"), 0) ||
		   UGameplayStatics::DoesSaveGameExist(TEXT("SaveSlot_2"), 0);
}

void APC_Menu_SoulslikeFramework::SetActiveWidgetForNavigation_Implementation(const FGameplayTag& InNavigableWidgetTag)
{
	NavigableWidgetTag = InNavigableWidgetTag;
	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] SetActiveWidgetForNavigation: %s"), *InNavigableWidgetTag.ToString());
}

void APC_Menu_SoulslikeFramework::LoadInputActions()
{
	const FString BasePath = TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/");

	if (!IA_NavigableMenu_Up)
	{
		IA_NavigableMenu_Up = LoadObject<UInputAction>(nullptr,
			*(BasePath + TEXT("IA_NavigableMenu_Up.IA_NavigableMenu_Up")));
	}

	if (!IA_NavigableMenu_Down)
	{
		IA_NavigableMenu_Down = LoadObject<UInputAction>(nullptr,
			*(BasePath + TEXT("IA_NavigableMenu_Down.IA_NavigableMenu_Down")));
	}

	if (!IA_NavigableMenu_Ok)
	{
		IA_NavigableMenu_Ok = LoadObject<UInputAction>(nullptr,
			*(BasePath + TEXT("IA_NavigableMenu_Ok.IA_NavigableMenu_Ok")));
	}

	if (!IA_NavigableMenu_Back)
	{
		IA_NavigableMenu_Back = LoadObject<UInputAction>(nullptr,
			*(BasePath + TEXT("IA_NavigableMenu_Back.IA_NavigableMenu_Back")));
	}

	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Input actions loaded - Up:%s Down:%s Ok:%s Back:%s"),
		IA_NavigableMenu_Up ? TEXT("Yes") : TEXT("No"),
		IA_NavigableMenu_Down ? TEXT("Yes") : TEXT("No"),
		IA_NavigableMenu_Ok ? TEXT("Yes") : TEXT("No"),
		IA_NavigableMenu_Back ? TEXT("Yes") : TEXT("No"));
}

void APC_Menu_SoulslikeFramework::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void APC_Menu_SoulslikeFramework::BindInputActions()
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_NavigableMenu_Up)
		{
			EnhancedInputComponent->BindAction(IA_NavigableMenu_Up, ETriggerEvent::Triggered, this, &APC_Menu_SoulslikeFramework::OnNavigateUp);
			UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Bound IA_NavigableMenu_Up"));
		}

		if (IA_NavigableMenu_Down)
		{
			EnhancedInputComponent->BindAction(IA_NavigableMenu_Down, ETriggerEvent::Triggered, this, &APC_Menu_SoulslikeFramework::OnNavigateDown);
			UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Bound IA_NavigableMenu_Down"));
		}

		if (IA_NavigableMenu_Ok)
		{
			EnhancedInputComponent->BindAction(IA_NavigableMenu_Ok, ETriggerEvent::Triggered, this, &APC_Menu_SoulslikeFramework::OnNavigateOk);
			UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Bound IA_NavigableMenu_Ok"));
		}

		if (IA_NavigableMenu_Back)
		{
			EnhancedInputComponent->BindAction(IA_NavigableMenu_Back, ETriggerEvent::Triggered, this, &APC_Menu_SoulslikeFramework::OnNavigateBack);
			UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] Bound IA_NavigableMenu_Back"));
		}
	}
}

void APC_Menu_SoulslikeFramework::OnNavigateUp(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] OnNavigateUp"));

	if (W_MainMenu)
	{
		W_MainMenu->EventNavigateUp();
	}
}

void APC_Menu_SoulslikeFramework::OnNavigateDown(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] OnNavigateDown"));

	if (W_MainMenu)
	{
		W_MainMenu->EventNavigateDown();
	}
}

void APC_Menu_SoulslikeFramework::OnNavigateOk(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] OnNavigateOk"));

	if (W_MainMenu)
	{
		W_MainMenu->EventNavigateOk();
	}
}

void APC_Menu_SoulslikeFramework::OnNavigateBack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_Menu_SoulslikeFramework] OnNavigateBack"));

	// In main menu, back typically does nothing or could quit
	// For now just log it
}
