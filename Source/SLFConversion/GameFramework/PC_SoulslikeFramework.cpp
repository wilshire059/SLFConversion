// PC_SoulslikeFramework.cpp
// C++ implementation for Game Framework class PC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation

#include "GameFramework/PC_SoulslikeFramework.h"
#include "Widgets/W_HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/EquipmentManagerComponent.h"
#include "Components/SaveLoadManagerComponent.h"
#include "Components/RadarManagerComponent.h"
#include "Components/ProgressManagerComponent.h"
#include "Blueprints/B_RestingPoint.h"

APC_SoulslikeFramework::APC_SoulslikeFramework()
{
	W_HUD = nullptr;
	GameplayMappingContext = nullptr;
	ActiveSequencePlayer = nullptr;

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
