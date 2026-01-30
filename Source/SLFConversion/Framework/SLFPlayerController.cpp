// SLFPlayerController.cpp
#include "SLFPlayerController.h"
#include "SLFGameInstance.h"
#include "Blueprints/B_SequenceActor.h"
#include "LevelSequence.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Widgets/W_HUD.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/StatManagerComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_ProgressManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/RadarManagerComponent.h"
#include "Widgets/W_Radar.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
// Includes for enemy reset on player death
#include "EngineUtils.h"
#include "Blueprints/SLFSoulslikeEnemy.h"
#include "Blueprints/B_Soulslike_Enemy.h"
#include "Blueprints/Actors/SLFBossDoor.h"
#include "Components/AICombatManagerComponent.h"
#include "Components/SLFAIStateMachineComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Interfaces/BPI_ExecutionIndicator.h"
#include "Interfaces/SLFExecutionIndicatorInterface.h"
#include "Blueprints/SLFBaseCharacter.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionComponent.h"

ASLFPlayerController::ASLFPlayerController()
{
	HUDWidgetRef = nullptr;
	HUDWidgetClass = nullptr;
	GameMenuWidgetClass = nullptr;
	GameMenuWidgetRef = nullptr;
	IMC_Gameplay = nullptr;
	IMC_NavigableMenu = nullptr;

	// Load IA_GameMenu input action
	static ConstructorHelpers::FObjectFinder<UInputAction> GameMenuActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu.IA_GameMenu"));
	if (GameMenuActionFinder.Succeeded())
	{
		IA_GameMenu = GameMenuActionFinder.Object;
	}
	else
	{
		IA_GameMenu = nullptr;
	}

	// Load navigation input actions
	static ConstructorHelpers::FObjectFinder<UInputAction> BackActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Back.IA_NavigableMenu_Back"));
	IA_NavigableMenu_Back = BackActionFinder.Succeeded() ? BackActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> OkActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Ok.IA_NavigableMenu_Ok"));
	IA_NavigableMenu_Ok = OkActionFinder.Succeeded() ? OkActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> UpActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Up.IA_NavigableMenu_Up"));
	IA_NavigableMenu_Up = UpActionFinder.Succeeded() ? UpActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> DownActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Down.IA_NavigableMenu_Down"));
	IA_NavigableMenu_Down = DownActionFinder.Succeeded() ? DownActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> LeftActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Left.IA_NavigableMenu_Left"));
	IA_NavigableMenu_Left = LeftActionFinder.Succeeded() ? LeftActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> RightActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Right.IA_NavigableMenu_Right"));
	IA_NavigableMenu_Right = RightActionFinder.Succeeded() ? RightActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> LeftCategoryActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Left_Category.IA_NavigableMenu_Left_Category"));
	IA_NavigableMenu_Left_Category = LeftCategoryActionFinder.Succeeded() ? LeftCategoryActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> RightCategoryActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Right_Category.IA_NavigableMenu_Right_Category"));
	IA_NavigableMenu_Right_Category = RightCategoryActionFinder.Succeeded() ? RightCategoryActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> UnequipActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Unequip.IA_NavigableMenu_Unequip"));
	IA_NavigableMenu_Unequip = UnequipActionFinder.Succeeded() ? UnequipActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> DetailedViewActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_DetailedView.IA_NavigableMenu_DetailedView"));
	IA_NavigableMenu_DetailedView = DetailedViewActionFinder.Succeeded() ? DetailedViewActionFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputAction> ResetToDefaultsActionFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_ResetToDefaults.IA_NavigableMenu_ResetToDefaults"));
	IA_NavigableMenu_ResetToDefaults = ResetToDefaultsActionFinder.Succeeded() ? ResetToDefaultsActionFinder.Object : nullptr;

	// Load input mapping contexts
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> GameplayIMCFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay"));
	IMC_Gameplay = GameplayIMCFinder.Succeeded() ? GameplayIMCFinder.Object : nullptr;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MenuIMCFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_NavigableMenu.IMC_NavigableMenu"));
	IMC_NavigableMenu = MenuIMCFinder.Succeeded() ? MenuIMCFinder.Object : nullptr;

	// NOTE: Don't load widget classes in constructor - they have compile errors
	// that cause the editor to hang. Widget search is done at runtime instead.
}

void ASLFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] BeginPlay - C++ PlayerController active"));

	// ═══════════════════════════════════════════════════════════════════════
	// STARTUP CUTSCENE - Check if first time on demo level
	// This replaces the Level Blueprint logic that had broken variable references
	// ═══════════════════════════════════════════════════════════════════════
	if (UWorld* World = GetWorld())
	{
		// Check if we're on the demo level
		FString LevelName = World->GetMapName();
		LevelName.RemoveFromStart(World->StreamingLevelsPrefix);
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Current level: %s"), *LevelName);

		if (LevelName.Contains(TEXT("Demo_Showcase")) || LevelName.Contains(TEXT("L_Demo")))
		{
			// Get game instance and check if first time
			if (USLFGameInstance* GI = Cast<USLFGameInstance>(GetGameInstance()))
			{
				UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] FirstTimeOnDemoLevel: %s"),
					GI->FirstTimeOnDemoLevel ? TEXT("true") : TEXT("false"));

				if (GI->FirstTimeOnDemoLevel)
				{
					// Load the sequence to play
					static const FSoftObjectPath ShowcaseSequencePath(TEXT("/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom.LS_ShowcaseRoom"));
					ULevelSequence* SequenceAsset = Cast<ULevelSequence>(ShowcaseSequencePath.TryLoad());

					if (SequenceAsset)
					{
						// Load B_SequenceActor Blueprint class
						static const FSoftClassPath SequenceActorBPPath(TEXT("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor.B_SequenceActor_C"));
						UClass* SequenceActorClass = SequenceActorBPPath.TryLoadClass<AB_SequenceActor>();

						if (SequenceActorClass)
						{
							// Spawn the sequence actor
							FActorSpawnParameters SpawnParams;
							SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

							AB_SequenceActor* SequenceActor = World->SpawnActor<AB_SequenceActor>(SequenceActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

							if (SequenceActor)
							{
								// Set the sequence to play
								SequenceActor->SequenceToPlay = SequenceAsset;
								SequenceActor->CanBeSkipped = true;

								UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] *** SPAWNED B_SequenceActor with LS_ShowcaseRoom ***"));

								// Toggle cinematic mode on HUD if available (after HUD is initialized)
								// Note: HUD isn't initialized yet at this point, so we schedule this
							}
							else
							{
								UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Failed to spawn B_SequenceActor"));
							}
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Could not load B_SequenceActor Blueprint class"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Could not load LS_ShowcaseRoom sequence"));
					}

					// Set first time to false so we don't play again
					GI->FirstTimeOnDemoLevel = false;
					UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Set FirstTimeOnDemoLevel = false"));
				}
			}
		}
	}

	// AAA Pattern: Pre-load widget classes FIRST to eliminate first-use freeze
	PreloadWidgetClasses();

	// From Blueprint DNA: BeginPlay → Sequence → Native_InitializeHUD
	// Initialize the HUD widget (creates W_HUD and adds to viewport)
	Native_InitializeHUD();

	// NOTE: Menu toggle (IA_GameMenu / Tab) is handled by the Blueprint's EventGraph
	// The Blueprint binds IA_GameMenu → W_HUD.EventShowGameMenu() which properly
	// toggles menu visibility and switches input contexts.
	// We do NOT override this in C++ to preserve the original Blueprint behavior.
}

void ASLFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SetupInputComponent"));

	// Get Enhanced Input Component and bind all input actions
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Gameplay input - game menu toggle
		if (IA_GameMenu)
		{
			EnhancedInput->BindAction(IA_GameMenu, ETriggerEvent::Started, this, &ASLFPlayerController::HandleGameMenuInput);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_GameMenu"));
		}

		// Navigation inputs - for menu navigation
		if (IA_NavigableMenu_Back)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Back, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateBack);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Back"));
		}

		if (IA_NavigableMenu_Ok)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Ok, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateOk);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Ok"));
		}

		if (IA_NavigableMenu_Up)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Up, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateUp);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Up"));
		}

		if (IA_NavigableMenu_Down)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Down, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateDown);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Down"));
		}

		if (IA_NavigableMenu_Left)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Left, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateLeft);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Left"));
		}

		if (IA_NavigableMenu_Right)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Right, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateRight);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Right"));
		}

		if (IA_NavigableMenu_Left_Category)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Left_Category, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateCategoryLeft);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Left_Category"));
		}

		if (IA_NavigableMenu_Right_Category)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Right_Category, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateCategoryRight);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Right_Category"));
		}

		if (IA_NavigableMenu_Unequip)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Unequip, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateUnequip);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_Unequip"));
		}

		if (IA_NavigableMenu_DetailedView)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_DetailedView, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateDetailedView);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_DetailedView"));
		}

		if (IA_NavigableMenu_ResetToDefaults)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_ResetToDefaults, ETriggerEvent::Started, this, &ASLFPlayerController::HandleNavigateResetToDefaults);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Bound IA_NavigableMenu_ResetToDefaults"));
		}
	}
}

void ASLFPlayerController::HandleGameMenuInput(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] HandleGameMenuInput - IA_GameMenu triggered"));

	// Toggle game menu via W_HUD
	if (HUDWidgetRef)
	{
		bool bIsMenuVisible = HUDWidgetRef->GetGameMenuVisibility();
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Menu currently visible: %s"),
			bIsMenuVisible ? TEXT("YES") : TEXT("NO"));

		if (bIsMenuVisible)
		{
			HUDWidgetRef->EventCloseGameMenu();
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Called EventCloseGameMenu"));
		}
		else
		{
			HUDWidgetRef->EventShowGameMenu();
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Called EventShowGameMenu"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] HUDWidgetRef is NULL, cannot toggle menu"));
	}
}

void ASLFPlayerController::Native_InitializeHUD()
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Initializing HUD"));

	// From Blueprint DNA PC_SoulslikeFramework.json:
	// Native_InitializeHUD flow:
	// 1. SetInputMode_GameOnly
	// 2. bShowMouseCursor = false
	// 3. CreateWidget<W_HUD_C> (class: /Game/SoulslikeFramework/Widgets/HUD/W_HUD.W_HUD_C)
	// 4. Set W_HUD variable
	// 5. AddToViewport

	// Step 1: Set input mode to game only
	SetInputMode(FInputModeGameOnly());
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Native_InitializeHUD - Set InputMode to GameOnly"));

	// Step 2: Hide mouse cursor
	bShowMouseCursor = false;
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Native_InitializeHUD - Set bShowMouseCursor = false"));

	// Step 3: Load W_HUD widget class at runtime (avoiding constructor loading issues)
	if (!HUDWidgetClass)
	{
		// Load the Blueprint widget class from content path
		const FString HUDWidgetPath = TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_HUD.W_HUD_C");
		HUDWidgetClass = LoadClass<UUserWidget>(nullptr, *HUDWidgetPath);

		if (HUDWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Loaded HUDWidgetClass: %s"), *HUDWidgetClass->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] Native_InitializeHUD - FAILED to load W_HUD class from: %s"), *HUDWidgetPath);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("FAILED to load W_HUD widget class!"));
			}
			return;
		}
	}

	// Step 4: Create widget
	if (HUDWidgetClass)
	{
		HUDWidgetRef = CreateWidget<UW_HUD>(this, HUDWidgetClass);

		if (HUDWidgetRef)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Created W_HUD widget: %s"), *HUDWidgetRef->GetName());

			// Step 5: Add to viewport
			HUDWidgetRef->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] Native_InitializeHUD - Added W_HUD to viewport"));

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("W_HUD created and added to viewport!"));
			}

			// Step 6: Schedule radar initialization after widget tree is fully constructed
			if (UWorld* World = GetWorld())
			{
				FTimerHandle RadarInitTimer;
				World->GetTimerManager().SetTimer(RadarInitTimer, this,
					&ASLFPlayerController::EventInitializeRadar, 0.2f, false);
				UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Scheduled radar initialization in 0.2s"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SLFPlayerController] Native_InitializeHUD - FAILED to create W_HUD widget instance"));
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("FAILED to create W_HUD widget instance!"));
			}
		}
	}
}

void ASLFPlayerController::EventInitializeRadar()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] EventInitializeRadar"));

	// Get the W_Radar widget from the HUD
	if (!HUDWidgetRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] EventInitializeRadar - No HUDWidgetRef!"));
		return;
	}

	UW_Radar* RadarWidget = HUDWidgetRef->GetCachedRadar();
	if (!RadarWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] EventInitializeRadar - No W_Radar widget in HUD!"));
		return;
	}

	// Find the RadarManager component on this controller
	URadarManagerComponent* RadarManager = FindComponentByClass<URadarManagerComponent>();
	if (RadarManager)
	{
		RadarManager->RadarWidget = RadarWidget;
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] EventInitializeRadar - Set RadarWidget on RadarManager"));

		// Call LateInitialize to complete full setup (player icon + cardinal markers)
		RadarManager->LateInitialize();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] EventInitializeRadar - No RadarManager component found!"));
	}
}

void ASLFPlayerController::PreloadWidgetClasses()
{
	// AAA Pattern: Pre-load commonly used widget Blueprint classes at startup
	// This loads them into memory once, eliminating the 2+ second freeze on first use
	// Classes are stored in UPROPERTY to prevent garbage collection

	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] PreloadWidgetClasses - Starting widget preload..."));

	double StartTime = FPlatformTime::Seconds();

	// Inventory slot - used heavily when opening inventory or picking up items
	CachedInventorySlotClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_InventorySlot.W_InventorySlot_C"));

	// Category entry - used for inventory category tabs
	CachedCategoryEntryClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory_CategoryEntry.W_Inventory_CategoryEntry_C"));

	// Loot notification - shown when items are picked up
	CachedLootNotificationClass = LoadClass<UUserWidget>(nullptr,
		TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_LootNotification.W_LootNotification_C"));

	double EndTime = FPlatformTime::Seconds();
	double LoadTime = (EndTime - StartTime) * 1000.0; // Convert to milliseconds

	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] PreloadWidgetClasses - Completed in %.1fms"), LoadTime);
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Preloaded: InventorySlot=%s, CategoryEntry=%s, LootNotification=%s"),
		CachedInventorySlotClass ? TEXT("OK") : TEXT("FAIL"),
		CachedCategoryEntryClass ? TEXT("OK") : TEXT("FAIL"),
		CachedLootNotificationClass ? TEXT("OK") : TEXT("FAIL"));
}

// ═══════════════════════════════════════════════════════════════════════════
// NAVIGATION INPUT HANDLERS
// ═══════════════════════════════════════════════════════════════════════════

void ASLFPlayerController::HandleNavigateBack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateBack - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateCancel(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateOk(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateOk - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateOk(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateUp(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateUp - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateUp(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateDown(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateDown - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateDown(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateLeft(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateLeft - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateLeft(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateRight(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateRight - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateRight(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateCategoryLeft(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateCategoryLeft - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateCategoryLeft(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateCategoryRight(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateCategoryRight - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateCategoryRight(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateUnequip(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateUnequip - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateUnequip(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateDetailedView(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateDetailedView - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateDetailedView(ActiveWidgetTag);
	}
}

void ASLFPlayerController::HandleNavigateResetToDefaults(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] HandleNavigateResetToDefaults - ActiveWidgetTag: %s"),
		*ActiveWidgetTag.ToString());

	if (HUDWidgetRef && ActiveWidgetTag.IsValid())
	{
		HUDWidgetRef->RouteNavigateResetToDefaults(ActiveWidgetTag);
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// INPUT CONTEXT SWITCHING
// ═══════════════════════════════════════════════════════════════════════════

void ASLFPlayerController::SwitchInputContextInternal(const TArray<UInputMappingContext*>& ContextsToEnable, const TArray<UInputMappingContext*>& ContextsToDisable)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SwitchInputContext - Enabling %d contexts, Disabling %d contexts"),
		ContextsToEnable.Num(), ContextsToDisable.Num());

	// Get the Enhanced Input Local Player Subsystem
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// First, disable contexts
			for (UInputMappingContext* Context : ContextsToDisable)
			{
				if (Context)
				{
					Subsystem->RemoveMappingContext(Context);
					UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SwitchInputContext - Disabled: %s"), *Context->GetName());
				}
			}

			// Then, enable contexts with priority 0
			for (UInputMappingContext* Context : ContextsToEnable)
			{
				if (Context)
				{
					Subsystem->AddMappingContext(Context, 0);
					UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SwitchInputContext - Enabled: %s"), *Context->GetName());
				}
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════
// BPI_CONTROLLER INTERFACE IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════

void ASLFPlayerController::StartRespawn_Implementation(double FadeDelay)
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] StartRespawn_Implementation - FadeDelay: %.2f"), FadeDelay);

	// Show "You Died" screen via HUD
	if (HUDWidgetRef)
	{
		HUDWidgetRef->ShowDeathScreen();
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] StartRespawn - Called HUD ShowDeathScreen"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] StartRespawn - HUDWidgetRef is NULL"));
	}

	// Disable player input during death
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	// Calculate timings for death sequence:
	// 1. Player dies, ragdoll/animation starts, "You Died" appears (0s)
	// 2. Wait for death animation to play (~1.5-2s)
	// 3. Start fade to black
	// 4. Complete respawn after fade finishes
	float TotalDelay = FadeDelay > 0 ? FadeDelay : 3.0f;
	float FadeOutDuration = 1.0f; // Fade to black over 1 second
	float TimeBeforeFade = 1.5f; // Wait 1.5 seconds for ragdoll/death animation before fade starts

	// Ensure total delay is long enough for animation + fade
	if (TotalDelay < TimeBeforeFade + FadeOutDuration + 0.5f)
	{
		TotalDelay = TimeBeforeFade + FadeOutDuration + 0.5f; // animation wait + fade + 0.5s buffer
	}

	// Start fade to black timer (before respawn completes)
	FTimerHandle FadeTimerHandle;
	GetWorldTimerManager().SetTimer(
		FadeTimerHandle,
		[this, FadeOutDuration]()
		{
			// Fade camera to black
			if (APlayerCameraManager* CameraMgr = PlayerCameraManager)
			{
				CameraMgr->StartCameraFade(0.0f, 1.0f, FadeOutDuration, FLinearColor::Black, false, true);
				UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] StartRespawn - Started camera fade to black"));
			}
		},
		TimeBeforeFade,
		false
	);

	// Start respawn timer
	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&ASLFPlayerController::CompleteRespawn,
		TotalDelay,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] StartRespawn - Respawn timer set for %.2f seconds, fade starts at %.2f"), TotalDelay, TimeBeforeFade);
}

void ASLFPlayerController::CompleteRespawn()
{
	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] CompleteRespawn - Respawning player"));

	// Hide death screen
	if (HUDWidgetRef)
	{
		HUDWidgetRef->HideDeathScreen();
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Hid death screen"));
	}

	// Get the pawn and reset its state
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		// Find player start or use last resting point
		AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(this);
		if (PlayerStart)
		{
			// Teleport pawn to player start (screen is black from fade)
			ControlledPawn->SetActorLocation(PlayerStart->GetActorLocation());
			ControlledPawn->SetActorRotation(PlayerStart->GetActorRotation());
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Teleported to %s"), *PlayerStart->GetName());
		}

		// CRITICAL: Reset IsDead flag on combat manager BEFORE restoring HP
		// This allows the player to attack again after respawning
		if (UAC_CombatManager* CombatManager = ControlledPawn->FindComponentByClass<UAC_CombatManager>())
		{
			CombatManager->IsDead = false;
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Reset IsDead flag on AC_CombatManager"));
		}

		// Reset ragdoll state - restore character mesh to normal physics mode
		if (ACharacter* CharacterPawn = Cast<ACharacter>(ControlledPawn))
		{
			// STEP 1: Re-enable capsule collision (disabled during ragdoll)
			if (UCapsuleComponent* Capsule = CharacterPawn->GetCapsuleComponent())
			{
				Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Re-enabled capsule collision"));
			}

			// STEP 2: Re-enable character movement (disabled during ragdoll)
			if (UCharacterMovementComponent* MovementComp = CharacterPawn->GetCharacterMovement())
			{
				MovementComp->SetMovementMode(MOVE_Walking);
				UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Re-enabled movement component"));
			}

			// STEP 3: Reset mesh from ragdoll state
			if (USkeletalMeshComponent* Mesh = CharacterPawn->GetMesh())
			{
				// Disable physics simulation and reset to kinematic
				Mesh->SetSimulatePhysics(false);
				Mesh->SetCollisionProfileName(FName("CharacterMesh"));

				// Reset mesh transform relative to capsule
				Mesh->AttachToComponent(CharacterPawn->GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
				Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // Standard UE character offset
				Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // Standard UE character rotation

				UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Reset mesh from ragdoll state"));
			}
		}

		// Reset character health via stat manager
		// Try to find and reset stats via interface
		if (ControlledPawn->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			// Get stat manager and reset HP to max
			UActorComponent* StatMgrComp = nullptr;
			IBPI_GenericCharacter::Execute_GetStatManager(ControlledPawn, StatMgrComp);
			if (UStatManagerComponent* StatMgr = Cast<UStatManagerComponent>(StatMgrComp))
			{
				FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"));
				UObject* StatObj = nullptr;
				FStatInfo StatInfo;
				if (StatMgr->GetStat(HPTag, StatObj, StatInfo))
				{
					// Reset HP to max
					double HPToRestore = StatInfo.MaxValue - StatInfo.CurrentValue;
					StatMgr->AdjustStat(HPTag, ESLFValueType::CurrentValue, HPToRestore, false, true);
					UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Restored HP to max (%.0f)"), StatInfo.MaxValue);
				}
			}
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// RESET ALL ENEMIES (Souls-like behavior - Elden Ring)
	// When player respawns, reset all enemies to their spawn locations
	// This happens while screen is black, so player sees fresh world when it fades in
	// ═══════════════════════════════════════════════════════════════════════════════
	UWorld* World = GetWorld();
	if (World)
	{
		int32 EnemiesReset = 0;

		// Reset enemies using both class hierarchies
		auto ResetEnemyCharacter = [&](ACharacter* EnemyChar)
		{
			// NOTE: Do NOT check IsHidden() here! Killed enemies ARE hidden by HandleDeath
			// and we need to respawn them. Only skip invalid actors.
			if (!IsValid(EnemyChar))
			{
				return;
			}

			// Reset combat manager state and teleport to spawn location
			if (UAICombatManagerComponent* AICombatMgr = EnemyChar->FindComponentByClass<UAICombatManagerComponent>())
			{
				AICombatMgr->bIsDead = false;
				AICombatMgr->bPoiseBroken = false;
				AICombatMgr->bHyperArmor = false;
				AICombatMgr->bInvincible = false;
				AICombatMgr->bHealthbarActive = false;

				// CRITICAL: Hide the execution indicator widget if poise was broken
				// The ExecutionWidget is a UWidgetComponent on ASLFBaseCharacter, not an interface on the actor
				if (ASLFBaseCharacter* BaseChar = Cast<ASLFBaseCharacter>(EnemyChar))
				{
					if (BaseChar->CachedExecutionWidget)
					{
						BaseChar->CachedExecutionWidget->SetVisibility(false);

						// Also call ToggleExecutionIcon on the internal widget to reset its state
						if (UUserWidget* Widget = BaseChar->CachedExecutionWidget->GetWidget())
						{
							if (Widget->GetClass()->ImplementsInterface(USLFExecutionIndicatorInterface::StaticClass()))
							{
								ISLFExecutionIndicatorInterface::Execute_ToggleExecutionIcon(Widget, false);
							}
						}
						UE_LOG(LogTemp, Log, TEXT("[Respawn] Hid execution indicator widget for %s"), *EnemyChar->GetName());
					}
				}

				// CRITICAL: Disable healthbar so it doesn't show stale damage
				// Will be re-enabled when enemy takes new damage
				AICombatMgr->DisableHealthbar();
				UE_LOG(LogTemp, Log, TEXT("[Respawn] Disabled healthbar for %s"), *EnemyChar->GetName());

				// Teleport enemy back to original spawn location
				FTransform OriginalSpawn = AICombatMgr->SpawnTransform;
				if (!OriginalSpawn.GetLocation().IsNearlyZero())
				{
					EnemyChar->SetActorTransform(OriginalSpawn);
					UE_LOG(LogTemp, Log, TEXT("[Respawn] Teleported %s to spawn: %s"),
						*EnemyChar->GetName(), *OriginalSpawn.GetLocation().ToString());
				}

				AICombatMgr->bHasBeenRespawned = true;
			}

			// Reset HP/Poise
			if (UStatManagerComponent* EnemyStatMgr = EnemyChar->FindComponentByClass<UStatManagerComponent>())
			{
				// Use false for bErrorIfNotFound to avoid crashing if tag doesn't exist
				FGameplayTag HPTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.HP"), false);
				if (HPTag.IsValid())
				{
					EnemyStatMgr->ResetStat(HPTag);
				}

				FGameplayTag PoiseTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Stat.Secondary.Poise"), false);
				if (PoiseTag.IsValid())
				{
					EnemyStatMgr->ResetStat(PoiseTag);
				}
			}

			// CRITICAL: Reset AI state machine using ResetFromDeath()
			// SetState(Idle) silently fails because the state machine blocks leaving Dead state
			// ResetFromDeath() bypasses this check and properly reinitializes everything
			if (USLFAIStateMachineComponent* AIStateMachine = EnemyChar->FindComponentByClass<USLFAIStateMachineComponent>())
			{
				AIStateMachine->ResetFromDeath();
				UE_LOG(LogTemp, Log, TEXT("[Respawn] Reset AI state machine for %s"), *EnemyChar->GetName());
			}

			// CRITICAL: Restart AI brain component (was stopped during HandleDeath)
			// Without this, the AI won't process perception or behavior tree
			if (AAIController* AIController = Cast<AAIController>(EnemyChar->GetController()))
			{
				if (UBrainComponent* BrainComp = AIController->GetBrainComponent())
				{
					BrainComp->RestartLogic();
					UE_LOG(LogTemp, Log, TEXT("[Respawn] Restarted AI brain for %s"), *EnemyChar->GetName());
				}

				// CRITICAL: Force perception component to re-scan for targets
				// After respawn, perception may have stale data or not re-detect the new player position
				if (UAIPerceptionComponent* PerceptionComp = AIController->GetAIPerceptionComponent())
				{
					PerceptionComp->RequestStimuliListenerUpdate();
					UE_LOG(LogTemp, Log, TEXT("[Respawn] Requested perception refresh for %s"), *EnemyChar->GetName());
				}
			}

			// CRITICAL: Make sure enemy is visible and fully functional (matches W_RestMenu::RespawnAllEnemies)
			EnemyChar->SetActorHiddenInGame(false);
			EnemyChar->SetActorEnableCollision(true);
			EnemyChar->SetActorTickEnabled(true);

			UE_LOG(LogTemp, Warning, TEXT("[Respawn] Set actor visible for %s - Hidden=%s, Collision=%s, Tick=%s"),
				*EnemyChar->GetName(),
				EnemyChar->IsHidden() ? TEXT("true") : TEXT("false"),
				EnemyChar->GetActorEnableCollision() ? TEXT("true") : TEXT("false"),
				EnemyChar->IsActorTickEnabled() ? TEXT("true") : TEXT("false"));

			// Re-enable collision on mesh and FULLY DISABLE RAGDOLL (matches W_RestMenu::RespawnAllEnemies)
			if (USkeletalMeshComponent* EnemyMesh = EnemyChar->GetMesh())
			{
				// Log mesh state BEFORE reset
				UE_LOG(LogTemp, Warning, TEXT("[Respawn] %s mesh BEFORE reset - Visible=%s, SimPhysics=%s, Location=%s"),
					*EnemyChar->GetName(),
					EnemyMesh->IsVisible() ? TEXT("true") : TEXT("false"),
					EnemyMesh->IsSimulatingPhysics() ? TEXT("true") : TEXT("false"),
					*EnemyMesh->GetComponentLocation().ToString());

				// CRITICAL: Disable ragdoll physics completely
				EnemyMesh->SetSimulatePhysics(false);
				EnemyMesh->SetAllBodiesSimulatePhysics(false);
				EnemyMesh->ResetAllBodiesSimulatePhysics();

				EnemyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				EnemyMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

				// CRITICAL: Explicitly set mesh visibility - SetActorHiddenInGame may not propagate to mesh
				EnemyMesh->SetVisibility(true, true);  // true = propagate to children
				EnemyMesh->SetHiddenInGame(false, true);  // true = propagate to children

				// Reattach mesh to capsule if it was detached for ragdoll
				if (UCapsuleComponent* Capsule = EnemyChar->GetCapsuleComponent())
				{
					EnemyMesh->AttachToComponent(Capsule, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

					// Also reset the relative transform to standard character offset
					EnemyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
					EnemyMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
				}

				// Log mesh state AFTER reset
				UE_LOG(LogTemp, Warning, TEXT("[Respawn] %s mesh AFTER reset - Visible=%s, SimPhysics=%s, RelLoc=%s"),
					*EnemyChar->GetName(),
					EnemyMesh->IsVisible() ? TEXT("true") : TEXT("false"),
					EnemyMesh->IsSimulatingPhysics() ? TEXT("true") : TEXT("false"),
					*EnemyMesh->GetRelativeLocation().ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[Respawn] %s has NO MESH!"), *EnemyChar->GetName());
			}

			// Re-enable capsule collision
			if (UCapsuleComponent* Capsule = EnemyChar->GetCapsuleComponent())
			{
				Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
				Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			}

			// Re-enable collision on ALL primitive components (like RestMenu does)
			TArray<UPrimitiveComponent*> PrimitiveComponents;
			EnemyChar->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
			for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
			{
				if (PrimComp)
				{
					PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				}
			}

			EnemiesReset++;
		};

		// Iterate both enemy class hierarchies
		for (TActorIterator<ASLFSoulslikeEnemy> It(World); It; ++It)
		{
			ResetEnemyCharacter(*It);
		}

		for (TActorIterator<AB_Soulslike_Enemy> It(World); It; ++It)
		{
			ResetEnemyCharacter(*It);
		}

		// CRITICAL: Clear player's execution target (the pink circle on HUD)
		// This must be done after resetting enemies so the player doesn't still think an enemy is staggered
		if (ControlledPawn)
		{
			if (UAC_CombatManager* PlayerCombatMgr = ControlledPawn->FindComponentByClass<UAC_CombatManager>())
			{
				PlayerCombatMgr->SetExecutionTarget(nullptr);
				UE_LOG(LogTemp, Log, TEXT("[Respawn] Cleared player execution target"));
			}
		}

		// SECOND PASS: Hide execution indicator widgets AFTER all resets complete
		// This is needed because the mesh/collision resets can cause widgets to reinitialize
		auto HideExecutionWidget = [](ACharacter* EnemyChar)
		{
			if (!IsValid(EnemyChar)) return;

			if (ASLFBaseCharacter* BaseChar = Cast<ASLFBaseCharacter>(EnemyChar))
			{
				if (BaseChar->CachedExecutionWidget)
				{
					// Force the widget component to be hidden
					BaseChar->CachedExecutionWidget->SetVisibility(false);

					// Also ensure the internal widget's icon is collapsed
					if (UUserWidget* Widget = BaseChar->CachedExecutionWidget->GetWidget())
					{
						if (Widget->GetClass()->ImplementsInterface(USLFExecutionIndicatorInterface::StaticClass()))
						{
							ISLFExecutionIndicatorInterface::Execute_ToggleExecutionIcon(Widget, false);
						}
					}
					UE_LOG(LogTemp, Log, TEXT("[Respawn] SECOND PASS: Hid execution widget for %s"), *EnemyChar->GetName());
				}
			}
		};

		for (TActorIterator<ASLFSoulslikeEnemy> It(World); It; ++It)
		{
			HideExecutionWidget(*It);
		}
		for (TActorIterator<AB_Soulslike_Enemy> It(World); It; ++It)
		{
			HideExecutionWidget(*It);
		}

		// Unseal boss doors
		for (TActorIterator<ASLFBossDoor> DoorIt(World); DoorIt; ++DoorIt)
		{
			if (IsValid(*DoorIt) && (*DoorIt)->bSealed)
			{
				(*DoorIt)->UnsealDoor();
				UE_LOG(LogTemp, Log, TEXT("[Respawn] Unsealed boss door: %s"), *(*DoorIt)->GetName());
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("[Respawn] Reset %d enemies on player respawn"), EnemiesReset);
	}

	// Fade camera back from black AFTER teleporting and resetting enemies
	float FadeInDuration = 1.0f;
	if (APlayerCameraManager* CameraMgr = PlayerCameraManager)
	{
		CameraMgr->StartCameraFade(1.0f, 0.0f, FadeInDuration, FLinearColor::Black, false, false);
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Started camera fade from black"));
	}

	// Re-enable player input after a short delay (let fade complete)
	FTimerHandle InputEnableTimerHandle;
	GetWorldTimerManager().SetTimer(
		InputEnableTimerHandle,
		[this]()
		{
			SetIgnoreMoveInput(false);
			SetIgnoreLookInput(false);
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Re-enabled player input"));
		},
		FadeInDuration * 0.5f, // Re-enable input halfway through fade
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] CompleteRespawn - Respawn complete!"));
}

void ASLFPlayerController::RequestAddToSaveData_Implementation(FGameplayTag SaveTag, const FInstancedStruct& Data)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] RequestAddToSaveData - Tag: %s"), *SaveTag.ToString());
}

void ASLFPlayerController::RequestUpdateSaveData_Implementation(FGameplayTag SaveTag, TArray<FInstancedStruct>& Data)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] RequestUpdateSaveData - Tag: %s"), *SaveTag.ToString());
}

void ASLFPlayerController::SerializeDataForSaving_Implementation(ESLFSaveBehavior Behavior, FGameplayTag SaveTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SerializeDataForSaving - Tag: %s"), *SaveTag.ToString());
}

void ASLFPlayerController::SerializeAllDataForSaving_Implementation(ESLFSaveBehavior Behavior)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SerializeAllDataForSaving"));
}

void ASLFPlayerController::SetActiveWidgetForNavigation_Implementation(FGameplayTag NavigableWidgetTag)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SetActiveWidgetForNavigation - Tag: %s"), *NavigableWidgetTag.ToString());
	ActiveWidgetTag = NavigableWidgetTag;
}

void ASLFPlayerController::SendBigScreenMessage_Implementation(const FText& Text, UMaterialInterface* GradientMaterial, bool bBackdrop, double PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] SendBigScreenMessage - Text: %s"), *Text.ToString());
}

void ASLFPlayerController::ShowSavingVisual_Implementation(double Length)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] ShowSavingVisual - Length: %.2f"), Length);
}

void ASLFPlayerController::ToggleRadarUpdateState_Implementation(bool bUpdateEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] ToggleRadarUpdateState - Enabled: %s"), bUpdateEnabled ? TEXT("true") : TEXT("false"));
}

void ASLFPlayerController::GetPlayerHUD_Implementation(UUserWidget*& HUD)
{
	HUD = HUDWidgetRef;
}

void ASLFPlayerController::SwitchInputContext_Implementation(TArray<UInputMappingContext*>& ContextsToEnable, TArray<UInputMappingContext*>& ContextsToDisable)
{
	// Call the internal version
	SwitchInputContextInternal(ContextsToEnable, ContextsToDisable);
}

void ASLFPlayerController::ToggleInput_Implementation(bool bEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] ToggleInput - Enabled: %s"), bEnabled ? TEXT("true") : TEXT("false"));
}

void ASLFPlayerController::GetCurrency_Implementation(int32& Currency)
{
	// Try to find InventoryManager component on the controller first
	UActorComponent* InventoryComp = FindComponentByClass<UInventoryManagerComponent>();

	// If not on controller, try to find on pawn
	if (!InventoryComp)
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			InventoryComp = ControlledPawn->FindComponentByClass<UInventoryManagerComponent>();
		}
	}

	// Get currency from the inventory manager
	if (UInventoryManagerComponent* InvMgr = Cast<UInventoryManagerComponent>(InventoryComp))
	{
		Currency = InvMgr->GetCurrentCurrency();
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] GetCurrency - Got %d from InventoryManager"), Currency);
	}
	else
	{
		Currency = 0;
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] GetCurrency - No InventoryManagerComponent found, returning 0"));
	}
}

void ASLFPlayerController::GetProgressManager_Implementation(UActorComponent*& ProgressManager)
{
	// Try to find AC_ProgressManager on the controller first
	ProgressManager = FindComponentByClass<UAC_ProgressManager>();

	// If not on controller, try to find on pawn
	if (!ProgressManager)
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			ProgressManager = ControlledPawn->FindComponentByClass<UAC_ProgressManager>();
		}
	}
}

void ASLFPlayerController::GetSoulslikeController_Implementation(APlayerController*& Controller)
{
	Controller = this;
}

void ASLFPlayerController::GetPawnFromController_Implementation(APawn*& OutPawn)
{
	OutPawn = GetPawn();
}

void ASLFPlayerController::GetInventoryComponent_Implementation(UActorComponent*& Inventory)
{
	// Try to find AC_InventoryManager on the controller first
	Inventory = FindComponentByClass<UAC_InventoryManager>();

	// If not on controller, try the legacy InventoryManagerComponent
	if (!Inventory)
	{
		Inventory = FindComponentByClass<UInventoryManagerComponent>();
	}

	// If not on controller, try to find on pawn
	if (!Inventory)
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			Inventory = ControlledPawn->FindComponentByClass<UAC_InventoryManager>();
			if (!Inventory)
			{
				Inventory = ControlledPawn->FindComponentByClass<UInventoryManagerComponent>();
			}
		}
	}
}

void ASLFPlayerController::AdjustCurrency_Implementation(int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] AdjustCurrency - Delta: %d"), Delta);

	// Try to find InventoryManager component on the controller first
	UActorComponent* InventoryComp = FindComponentByClass<UInventoryManagerComponent>();

	// If not on controller, try to find on pawn
	if (!InventoryComp)
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			InventoryComp = ControlledPawn->FindComponentByClass<UInventoryManagerComponent>();
		}
	}

	// Call AdjustCurrency on the inventory manager
	if (UInventoryManagerComponent* InvMgr = Cast<UInventoryManagerComponent>(InventoryComp))
	{
		InvMgr->AdjustCurrency(Delta);
		UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] Called InventoryManager->AdjustCurrency(%d)"), Delta);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFPlayerController] No InventoryManagerComponent found on controller or pawn"));
	}
}

void ASLFPlayerController::LootItemToInventory_Implementation(AActor* Item)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] LootItemToInventory - Item: %s"), Item ? *Item->GetName() : TEXT("null"));
}

void ASLFPlayerController::BlendViewTarget_Implementation(AActor* TargetActor, double BlendTime, double BlendExp, bool bLockOutgoing)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] BlendViewTarget - Target: %s"), TargetActor ? *TargetActor->GetName() : TEXT("null"));
}
