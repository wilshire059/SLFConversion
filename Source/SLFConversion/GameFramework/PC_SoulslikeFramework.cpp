// PC_SoulslikeFramework.cpp
// C++ implementation for Game Framework class PC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation

#include "GameFramework/PC_SoulslikeFramework.h"
#include "Widgets/W_HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/SaveLoadManagerComponent.h"
#include "Components/RadarManagerComponent.h"
#include "Components/ProgressManagerComponent.h"
#include "Widgets/W_Radar.h"
#include "Blueprints/B_RestingPoint.h"

APC_SoulslikeFramework::APC_SoulslikeFramework()
{
	W_HUD = nullptr;
	ActiveSequencePlayer = nullptr;

	// Load Input Mapping Contexts
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> GameplayContextFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_Gameplay"));
	if (GameplayContextFinder.Succeeded())
	{
		GameplayMappingContext = GameplayContextFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> NavigableMenuContextFinder(
		TEXT("/Game/SoulslikeFramework/Input/IMC_NavigableMenu"));
	if (NavigableMenuContextFinder.Succeeded())
	{
		NavigableMenuMappingContext = NavigableMenuContextFinder.Object;
	}

	// Load Navigation Input Actions
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_UpFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Up"));
	if (IA_UpFinder.Succeeded())
	{
		IA_NavigableMenu_Up = IA_UpFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DownFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Down"));
	if (IA_DownFinder.Succeeded())
	{
		IA_NavigableMenu_Down = IA_DownFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LeftFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Left"));
	if (IA_LeftFinder.Succeeded())
	{
		IA_NavigableMenu_Left = IA_LeftFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_RightFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Right"));
	if (IA_RightFinder.Succeeded())
	{
		IA_NavigableMenu_Right = IA_RightFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_OkFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Ok"));
	if (IA_OkFinder.Succeeded())
	{
		IA_NavigableMenu_Ok = IA_OkFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_BackFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Back"));
	if (IA_BackFinder.Succeeded())
	{
		IA_NavigableMenu_Back = IA_BackFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LeftCategoryFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Left_Category"));
	if (IA_LeftCategoryFinder.Succeeded())
	{
		IA_NavigableMenu_Left_Category = IA_LeftCategoryFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_RightCategoryFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Right_Category"));
	if (IA_RightCategoryFinder.Succeeded())
	{
		IA_NavigableMenu_Right_Category = IA_RightCategoryFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_UnequipFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_Unequip"));
	if (IA_UnequipFinder.Succeeded())
	{
		IA_NavigableMenu_Unequip = IA_UnequipFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_DetailedViewFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_DetailedView"));
	if (IA_DetailedViewFinder.Succeeded())
	{
		IA_NavigableMenu_DetailedView = IA_DetailedViewFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ResetToDefaultsFinder(
		TEXT("/Game/SoulslikeFramework/Input/Actions/MainMenu/IA_NavigableMenu_ResetToDefaults"));
	if (IA_ResetToDefaultsFinder.Succeeded())
	{
		IA_NavigableMenu_ResetToDefaults = IA_ResetToDefaultsFinder.Object;
	}

	// Create components
	AC_InventoryManager = CreateDefaultSubobject<UInventoryManagerComponent>(TEXT("AC_InventoryManager"));
	AC_EquipmentManager = CreateDefaultSubobject<UEquipmentManagerComponent>(TEXT("AC_EquipmentManager"));
	AC_SaveLoadManager = CreateDefaultSubobject<USaveLoadManagerComponent>(TEXT("AC_SaveLoadManager"));
	AC_RadarManager = CreateDefaultSubobject<URadarManagerComponent>(TEXT("AC_RadarManager"));
	AC_ProgressManager = CreateDefaultSubobject<UProgressManagerComponent>(TEXT("AC_ProgressManager"));
}

void APC_SoulslikeFramework::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] BeginPlay"));

	// Setup Enhanced Input mapping context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (GameplayMappingContext)
		{
			Subsystem->AddMappingContext(GameplayMappingContext, 0);
			UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Added GameplayMappingContext"));
		}
	}

	// Create HUD widget
	// Note: The actual HUD widget class should be set via DefaultHUDClass or created here
	// Blueprint does: Create Widget (W_HUD class) -> Add to Viewport -> Store reference
	// For now, we'll leave W_HUD creation to Blueprint or subclass configuration

	// Broadcast HUD initialized
	OnHudInitialized.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] OnHudInitialized broadcast"));

	// Give player starting items (Health Flask for tools slot)
	if (AC_InventoryManager)
	{
		// Load and add Health Flask
		static const FSoftObjectPath HealthFlaskPath(TEXT("/Game/SoulslikeFramework/Data/Items/DA_HealthFlask.DA_HealthFlask"));
		if (UObject* FlaskAsset = HealthFlaskPath.TryLoad())
		{
			if (UDataAsset* FlaskData = Cast<UDataAsset>(FlaskAsset))
			{
				AC_InventoryManager->AddItem(FlaskData, 5, false); // 5 flasks, no loot UI
				UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Added 5x Health Flask to inventory"));

				// Equip the flask to the first tool slot so it can be used
				if (AC_EquipmentManager)
				{
					FGameplayTag ToolSlot1 = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Equipment.SlotType.Tools 1"));
					AC_EquipmentManager->EquipToolToSlot(ToolSlot1, FlaskData, false);
					UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Equipped Health Flask to Tools 1 slot"));
				}
			}
		}
	}

	// Schedule radar initialization after HUD is ready (delayed to ensure widgets are constructed)
	// Note: Uses 0.2s delay instead of 1.0s for faster radar visibility
	if (UWorld* World = GetWorld())
	{
		FTimerHandle RadarInitTimer;
		World->GetTimerManager().SetTimer(RadarInitTimer, this,
			&APC_SoulslikeFramework::EventInitializeRadar_Implementation, 0.2f, false);
		UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Scheduled radar initialization"));
	}
}

void APC_SoulslikeFramework::SetupInputComponent()
{
	Super::SetupInputComponent();

	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] SetupInputComponent"));

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind navigation input actions
		if (IA_NavigableMenu_Up)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Up, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateUp);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Up"));
		}
		if (IA_NavigableMenu_Down)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Down, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateDown);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Down"));
		}
		if (IA_NavigableMenu_Left)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Left, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateLeft);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Left"));
		}
		if (IA_NavigableMenu_Right)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Right, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateRight);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Right"));
		}
		if (IA_NavigableMenu_Ok)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Ok, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateOk);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Ok"));
		}
		if (IA_NavigableMenu_Back)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Back, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateBack);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Back"));
		}
		if (IA_NavigableMenu_Left_Category)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Left_Category, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateCategoryLeft);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Left_Category"));
		}
		if (IA_NavigableMenu_Right_Category)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Right_Category, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateCategoryRight);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Right_Category"));
		}
		if (IA_NavigableMenu_Unequip)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_Unequip, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateUnequip);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_Unequip"));
		}
		if (IA_NavigableMenu_DetailedView)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_DetailedView, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateDetailedView);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_DetailedView"));
		}
		if (IA_NavigableMenu_ResetToDefaults)
		{
			EnhancedInput->BindAction(IA_NavigableMenu_ResetToDefaults, ETriggerEvent::Started, this, &APC_SoulslikeFramework::HandleNavigateResetToDefaults);
			UE_LOG(LogTemp, Log, TEXT("  Bound IA_NavigableMenu_ResetToDefaults"));
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// NAVIGATION INPUT HANDLERS
// ═══════════════════════════════════════════════════════════════════════════════

void APC_SoulslikeFramework::HandleNavigateUp()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateUp - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateUp(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateDown()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateDown - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateDown(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateLeft()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateLeft - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateLeft(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateRight()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateRight - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateRight(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateOk()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateOk - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateOk(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateBack()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateBack - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateCancel(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateCategoryLeft()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateCategoryLeft - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateCategoryLeft(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateCategoryRight()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateCategoryRight - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateCategoryRight(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateUnequip()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateUnequip - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateUnequip(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateDetailedView()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateDetailedView - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateDetailedView(ActiveWidgetTag);
	}
}

void APC_SoulslikeFramework::HandleNavigateResetToDefaults()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleNavigateResetToDefaults - ActiveWidgetTag: %s"), *ActiveWidgetTag.ToString());
	if (W_HUD)
	{
		W_HUD->RouteNavigateResetToDefaults(ActiveWidgetTag);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOCAL FUNCTIONS [2/2]
// ═══════════════════════════════════════════════════════════════════════════════

void APC_SoulslikeFramework::HandleMainMenuRequest_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] HandleMainMenuRequest"));

	// If HUD is not valid, return early
	if (!W_HUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PC_SoulslikeFramework] HandleMainMenuRequest - No HUD"));
		return;
	}

	// JSON Logic: Check dialog first, then game menu
	// "If Dialog Window is active, first finish dialog. Then on second press, show game menu."
	bool bDialogVisible = W_HUD->GetDialogWindowVisibility();
	if (bDialogVisible)
	{
		W_HUD->EventFinishDialog();
		UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Closed dialog window"));
		return;
	}

	// Check if game menu is visible - if so, close it
	bool bGameMenuVisible = W_HUD->GetGameMenuVisibility();
	if (bGameMenuVisible)
	{
		W_HUD->EventCloseGameMenu();
		UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Closed game menu"));
		return;
	}

	// Otherwise, show the game menu
	W_HUD->EventShowGameMenu();
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Opened game menu"));
}

void APC_SoulslikeFramework::GetNearestRestingPoint_Implementation(bool& OutSuccess, AActor*& OutPoint)
{
	// JSON Logic: GetAllActorsOfClass(B_RestingPoint) -> GetPawn -> GetActorLocation -> FindNearestActor
	TArray<AActor*> RestingPoints;
	UGameplayStatics::GetAllActorsOfClass(this, AB_RestingPoint::StaticClass(), RestingPoints);

	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] GetNearestRestingPoint - Found %d resting points"), RestingPoints.Num());

	if (RestingPoints.Num() == 0)
	{
		OutSuccess = false;
		OutPoint = nullptr;
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		OutSuccess = false;
		OutPoint = nullptr;
		return;
	}

	FVector Origin = ControlledPawn->GetActorLocation();
	float NearestDistance = 0.0f;
	AActor* NearestActor = UGameplayStatics::FindNearestActor(Origin, RestingPoints, NearestDistance);

	OutSuccess = (NearestActor != nullptr);
	OutPoint = NearestActor;

	if (NearestActor)
	{
		UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] Nearest resting point: %s at distance %.0f"),
			*NearestActor->GetName(), NearestDistance);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_Controller INTERFACE IMPLEMENTATIONS [20/20]
// ═══════════════════════════════════════════════════════════════════════════════

// --- Save Functions ---

void APC_SoulslikeFramework::RequestAddToSaveData_Implementation(FGameplayTag SaveTag, const FInstancedStruct& Data)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] RequestAddToSaveData - Tag: %s"), *SaveTag.ToString());

	if (AC_SaveLoadManager)
	{
		// Forward to SaveLoadManager component
		// JSON Logic: Get AC_SaveLoadManager -> AddSaveData(SaveTag, Data)
		// Component handles the actual save data storage
	}
}

void APC_SoulslikeFramework::RequestUpdateSaveData_Implementation(FGameplayTag SaveTag, TArray<FInstancedStruct>& Data)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] RequestUpdateSaveData - Tag: %s"), *SaveTag.ToString());

	if (AC_SaveLoadManager)
	{
		// Forward to SaveLoadManager component
	}
}

void APC_SoulslikeFramework::SerializeDataForSaving_Implementation(ESLFSaveBehavior Behavior, FGameplayTag SaveTag)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] SerializeDataForSaving - Tag: %s"), *SaveTag.ToString());

	if (AC_SaveLoadManager)
	{
		// Forward to SaveLoadManager component
	}
}

void APC_SoulslikeFramework::SerializeAllDataForSaving_Implementation(ESLFSaveBehavior Behavior)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] SerializeAllDataForSaving"));

	if (AC_SaveLoadManager)
	{
		// Forward to SaveLoadManager component
	}
}

// --- UI Functions ---

void APC_SoulslikeFramework::SetActiveWidgetForNavigation_Implementation(FGameplayTag NavigableWidgetTag)
{
	// JSON Logic: Set ActiveWidgetTag = NavigableWidgetTag
	ActiveWidgetTag = NavigableWidgetTag;
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] SetActiveWidgetForNavigation - Tag: %s"), *NavigableWidgetTag.ToString());
}

void APC_SoulslikeFramework::SendBigScreenMessage_Implementation(const FText& Text, UMaterialInterface* GradientMaterial, bool bBackdrop, double PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] SendBigScreenMessage - %s"), *Text.ToString());

	if (W_HUD)
	{
		// JSON Logic: W_HUD -> EventBigScreenMessage(Text, GradientMaterial, bBackdrop, PlayRate)
		W_HUD->EventBigScreenMessage(Text, GradientMaterial, bBackdrop, static_cast<float>(PlayRate));
	}
}

void APC_SoulslikeFramework::ShowSavingVisual_Implementation(double Length)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] ShowSavingVisual - Length: %.2f"), Length);

	if (W_HUD)
	{
		// JSON Logic: W_HUD -> EventShowAutoSaveIcon(Length)
		W_HUD->EventShowAutoSaveIcon(static_cast<float>(Length));
	}
}

void APC_SoulslikeFramework::ToggleRadarUpdateState_Implementation(bool bUpdateEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] ToggleRadarUpdateState - %s"), bUpdateEnabled ? TEXT("Enabled") : TEXT("Disabled"));

	if (AC_RadarManager)
	{
		// JSON Logic: AC_RadarManager -> ToggleUpdateState(bUpdateEnabled)
		AC_RadarManager->ToggleUpdateState(bUpdateEnabled);
	}
}

void APC_SoulslikeFramework::GetPlayerHUD_Implementation(UUserWidget*& HUD)
{
	// JSON Logic: Return W_HUD
	HUD = W_HUD;
}

// --- Input Functions ---

void APC_SoulslikeFramework::SwitchInputContext_Implementation(TArray<UInputMappingContext*>& ContextsToEnable, TArray<UInputMappingContext*>& ContextsToDisable)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] SwitchInputContext - Enable: %d, Disable: %d"),
		ContextsToEnable.Num(), ContextsToDisable.Num());

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// Remove contexts to disable
		for (UInputMappingContext* Context : ContextsToDisable)
		{
			if (Context)
			{
				Subsystem->RemoveMappingContext(Context);
			}
		}

		// Add contexts to enable
		for (UInputMappingContext* Context : ContextsToEnable)
		{
			if (Context)
			{
				Subsystem->AddMappingContext(Context, 0);
			}
		}
	}
}

void APC_SoulslikeFramework::ToggleInput_Implementation(bool bEnabled)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] ToggleInput - %s"), bEnabled ? TEXT("Enabled") : TEXT("Disabled"));

	// JSON Logic: EnableInput/DisableInput based on bEnabled
	if (bEnabled)
	{
		EnableInput(this);
	}
	else
	{
		DisableInput(this);
	}
}

// --- Getter Functions ---

void APC_SoulslikeFramework::GetCurrency_Implementation(int32& Currency)
{
	// JSON Logic: AC_InventoryManager -> GetCurrentCurrency() -> Return
	if (AC_InventoryManager)
	{
		Currency = AC_InventoryManager->GetCurrentCurrency();
	}
	else
	{
		Currency = 0;
	}
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] GetCurrency: %d"), Currency);
}

void APC_SoulslikeFramework::GetProgressManager_Implementation(UActorComponent*& ProgressManager)
{
	// JSON Logic: Return AC_ProgressManager
	ProgressManager = AC_ProgressManager;
}

void APC_SoulslikeFramework::GetSoulslikeController_Implementation(APlayerController*& Controller)
{
	// JSON Logic: Return self
	Controller = this;
}

void APC_SoulslikeFramework::GetPawnFromController_Implementation(APawn*& OutPawn)
{
	// JSON Logic: GetPawn() -> Return
	OutPawn = GetPawn();
}

void APC_SoulslikeFramework::GetInventoryComponent_Implementation(UActorComponent*& Inventory)
{
	// JSON Logic: IsValid(AC_InventoryManager) -> Return
	if (IsValid(AC_InventoryManager))
	{
		Inventory = AC_InventoryManager;
	}
	else
	{
		Inventory = nullptr;
	}
}

// --- Loot Functions ---

void APC_SoulslikeFramework::AdjustCurrency_Implementation(int32 Delta)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] AdjustCurrency - Delta: %d"), Delta);

	if (AC_InventoryManager)
	{
		// JSON Logic: AC_InventoryManager -> AdjustCurrency(Delta)
		AC_InventoryManager->AdjustCurrency(Delta);
	}
}

void APC_SoulslikeFramework::LootItemToInventory_Implementation(AActor* Item)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] LootItemToInventory - Item: %s"),
		Item ? *Item->GetName() : TEXT("null"));

	if (AC_InventoryManager && Item)
	{
		// JSON Logic: Get Item data asset and amount from pickup actor via BPI_Item interface
		// Then call AC_InventoryManager -> LootPickupItem(ItemAsset, Amount)
		// For now, the actual loot logic depends on item interface implementation
		// This forwards to inventory manager which handles the BPI_Item interface call
	}
}

// --- Other Functions ---

void APC_SoulslikeFramework::StartRespawn_Implementation(double FadeDelay)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] StartRespawn - FadeDelay: %.2f"), FadeDelay);

	// JSON Logic:
	// 1. W_HUD -> EventFade(FadeOut, FadeDelay)
	// 2. SetTimerByEvent -> RespawnPlayer
	if (W_HUD)
	{
		W_HUD->EventFade(ESLFFadeTypes::FadeOut, FadeDelay);
	}

	// Setup timer for respawn (would need to implement RespawnPlayer function)
}

void APC_SoulslikeFramework::BlendViewTarget_Implementation(AActor* TargetActor, double BlendTime, double BlendExp, bool bLockOutgoing)
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] BlendViewTarget - Target: %s, Time: %.2f"),
		TargetActor ? *TargetActor->GetName() : TEXT("null"), BlendTime);

	if (TargetActor)
	{
		// JSON Logic: SetViewTargetWithBlend(TargetActor, BlendTime, Cubic, BlendExp, bLockOutgoing)
		FViewTargetTransitionParams Params;
		Params.BlendTime = BlendTime;
		Params.BlendFunction = EViewTargetBlendFunction::VTBlend_Cubic;
		Params.BlendExp = BlendExp;
		Params.bLockOutgoing = bLockOutgoing;

		SetViewTargetWithBlend(TargetActor, BlendTime, Params.BlendFunction, Params.BlendExp, Params.bLockOutgoing);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// RADAR INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════════

void APC_SoulslikeFramework::EventInitializeRadar_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] EventInitializeRadar"));

	// Get the W_Radar widget from the HUD
	if (!W_HUD)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PC_SoulslikeFramework] EventInitializeRadar - No W_HUD!"));
		return;
	}

	UW_Radar* RadarWidget = W_HUD->GetCachedRadar();
	if (!RadarWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PC_SoulslikeFramework] EventInitializeRadar - No W_Radar widget in HUD!"));
		return;
	}

	// Set the radar widget on the RadarManager
	if (AC_RadarManager)
	{
		AC_RadarManager->RadarWidget = RadarWidget;
		UE_LOG(LogTemp, Log, TEXT("[PC_SoulslikeFramework] EventInitializeRadar - Set RadarWidget on RadarManager"));

		// Call LateInitialize to complete full setup (player icon + cardinal markers)
		// This is called here instead of relying on RadarManager's timer because
		// the widget reference needs to be set first
		AC_RadarManager->LateInitialize();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PC_SoulslikeFramework] EventInitializeRadar - No AC_RadarManager!"));
	}
}
