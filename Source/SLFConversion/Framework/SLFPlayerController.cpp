// SLFPlayerController.cpp
#include "SLFPlayerController.h"
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
#include "Widgets/W_HUD.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/StatManagerComponent.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"

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

	// Start respawn timer
	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&ASLFPlayerController::CompleteRespawn,
		FadeDelay > 0 ? FadeDelay : 3.0f, // Default 3 second delay
		false
	);

	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] StartRespawn - Respawn timer set for %.2f seconds"), FadeDelay > 0 ? FadeDelay : 3.0f);
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

	// Re-enable player input
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);

	// Get the pawn and reset its state
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		// Find player start or use last resting point
		AActor* PlayerStart = GetWorld()->GetAuthGameMode()->FindPlayerStart(this);
		if (PlayerStart)
		{
			// Teleport pawn to player start
			ControlledPawn->SetActorLocation(PlayerStart->GetActorLocation());
			ControlledPawn->SetActorRotation(PlayerStart->GetActorRotation());
			UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] CompleteRespawn - Teleported to %s"), *PlayerStart->GetName());
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
	Currency = 0; // TODO: Get from progress manager
}

void ASLFPlayerController::GetProgressManager_Implementation(UActorComponent*& ProgressManager)
{
	ProgressManager = nullptr; // TODO: Find progress manager component
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
	Inventory = FindComponentByClass<UActorComponent>(); // TODO: Find specific inventory component
}

void ASLFPlayerController::AdjustCurrency_Implementation(int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] AdjustCurrency - Delta: %d"), Delta);
}

void ASLFPlayerController::LootItemToInventory_Implementation(AActor* Item)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] LootItemToInventory - Item: %s"), Item ? *Item->GetName() : TEXT("null"));
}

void ASLFPlayerController::BlendViewTarget_Implementation(AActor* TargetActor, double BlendTime, double BlendExp, bool bLockOutgoing)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFPlayerController] BlendViewTarget - Target: %s"), TargetActor ? *TargetActor->GetName() : TEXT("null"));
}
