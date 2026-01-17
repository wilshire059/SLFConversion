// W_HUD.cpp
// C++ Widget implementation for W_HUD
//
// 20-PASS VALIDATION: 2026-01-07
// Base implementation with child widget visibility management

#include "Widgets/W_HUD.h"
#include "Blueprints/B_Stat.h"
#include "Blueprints/SLFStatBase.h"
#include "Blueprints/B_StatusEffect.h"
#include "Components/StatManagerComponent.h"
#include "Blueprints/B_Interactable.h"
#include "Blueprints/B_RestingPoint.h"
#include "Blueprints/Actors/SLFInteractableBase.h"
#include "Blueprints/Actors/SLFBossDoor.h"
#include "Blueprints/SLFPickupItemBase.h"
#include "SLFPrimaryDataAssets.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "Interfaces/BPI_Interactable.h"
#include "Components/AC_AI_InteractionManager.h"
#include "Widgets/W_LoadingScreen.h"
#include "Widgets/W_Inventory.h"
#include "Widgets/W_Equipment.h"
#include "Widgets/W_Crafting.h"
#include "Widgets/W_GameMenu.h"
#include "Widgets/W_Status.h"
#include "Widgets/W_Settings.h"
#include "Widgets/W_RestMenu.h"
#include "Widgets/W_NPC_Window.h"
#include "Widgets/W_Dialog.h"
#include "Widgets/W_Interaction.h"
#include "Widgets/W_LootNotification.h"
#include "Widgets/W_FirstLootNotification.h"
#include "Widgets/W_Resources.h"
#include "Widgets/W_BigScreenMessage.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InventoryManagerComponent.h"
#include "GameFramework/PC_SoulslikeFramework.h"
#include "Framework/SLFPlayerController.h"

UW_HUD::UW_HUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LoadingScreen = nullptr;
	CachedW_Interaction = nullptr;
	CachedW_GameMenu = nullptr;
	CachedW_Inventory = nullptr;
	CachedW_Equipment = nullptr;
	CachedW_Crafting = nullptr;
	CachedW_Status = nullptr;
	CachedW_Settings = nullptr;
	CachedW_Resources = nullptr;
	CachedItemLootNotificationsBox = nullptr;
	CachedW_FirstLootNotification = nullptr;
	LootNotificationWidgetClass = nullptr;
	IsDialogActive = false;
	CinematicMode = false;
}

void UW_HUD::NativeConstruct()
{
	Super::NativeConstruct();
	CacheWidgetReferences();
	InitializeBindings();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::NativeConstruct"));
}

void UW_HUD::NativeDestruct()
{
	Super::NativeDestruct();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::NativeDestruct"));
}

void UW_HUD::CacheWidgetReferences()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - START"));

	if (!LoadingScreen)
	{
		LoadingScreen = Cast<UW_LoadingScreen>(GetWidgetFromName(TEXT("LoadingScreen")));
	}

	if (!CachedW_Interaction)
	{
		CachedW_Interaction = Cast<UW_Interaction>(GetWidgetFromName(TEXT("W_Interaction")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Interaction: %s"),
		CachedW_Interaction ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find CachedW_GameMenu if not already set
	if (!CachedW_GameMenu)
	{
		CachedW_GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("W_GameMenu")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_GameMenu: %s"),
		CachedW_GameMenu ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find child menu widgets
	if (!CachedW_Inventory)
	{
		CachedW_Inventory = Cast<UW_Inventory>(GetWidgetFromName(TEXT("W_Inventory")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Inventory: %s"),
		CachedW_Inventory ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_Equipment)
	{
		CachedW_Equipment = Cast<UW_Equipment>(GetWidgetFromName(TEXT("W_Equipment")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Equipment: %s"),
		CachedW_Equipment ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_Crafting)
	{
		CachedW_Crafting = Cast<UW_Crafting>(GetWidgetFromName(TEXT("W_Crafting")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Crafting: %s"),
		CachedW_Crafting ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_Status)
	{
		CachedW_Status = Cast<UW_Status>(GetWidgetFromName(TEXT("W_Status")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Status: %s"),
		CachedW_Status ? TEXT("Found") : TEXT("NOT FOUND"));

	// Bind to CachedW_GameMenu's OnGameMenuWidgetRequest dispatcher
	if (CachedW_GameMenu)
	{
		CachedW_GameMenu->OnGameMenuWidgetRequest.AddUniqueDynamic(this, &UW_HUD::OnGameMenuWidgetRequestHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to OnGameMenuWidgetRequest"));
	}

	// Bind to child widget closed dispatchers
	if (CachedW_Inventory)
	{
		CachedW_Inventory->OnInventoryClosed.AddUniqueDynamic(this, &UW_HUD::OnInventoryClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Inventory::OnInventoryClosed"));
	}

	if (CachedW_Equipment)
	{
		CachedW_Equipment->OnEquipmentClosed.AddUniqueDynamic(this, &UW_HUD::OnEquipmentClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Equipment::OnEquipmentClosed"));
	}

	if (CachedW_Crafting)
	{
		CachedW_Crafting->OnCraftingClosed.AddUniqueDynamic(this, &UW_HUD::OnCraftingClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Crafting::OnCraftingClosed"));
	}

	if (CachedW_Status)
	{
		CachedW_Status->OnStatusClosed.AddUniqueDynamic(this, &UW_HUD::OnStatusClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Status::OnStatusClosed"));
	}

	// Find CachedW_Settings widget
	if (!CachedW_Settings)
	{
		CachedW_Settings = Cast<UW_Settings>(GetWidgetFromName(TEXT("W_Settings")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Settings: %s"),
		CachedW_Settings ? TEXT("Found") : TEXT("NOT FOUND"));

	// Find CachedW_Resources widget (stat bars)
	if (!CachedW_Resources)
	{
		CachedW_Resources = Cast<UW_Resources>(GetWidgetFromName(TEXT("W_Resources")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_Resources: %s"),
		CachedW_Resources ? TEXT("Found") : TEXT("NOT FOUND"));

	if (CachedW_Settings)
	{
		CachedW_Settings->OnSettingsClosed.AddUniqueDynamic(this, &UW_HUD::OnSettingsClosedHandler);
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - Bound to CachedW_Settings::OnSettingsClosed"));
	}

	// Cache loot notification widgets
	if (!CachedItemLootNotificationsBox)
	{
		CachedItemLootNotificationsBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("ItemLootNotificationsBox")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedItemLootNotificationsBox: %s"),
		CachedItemLootNotificationsBox ? TEXT("Found") : TEXT("NOT FOUND"));

	if (!CachedW_FirstLootNotification)
	{
		CachedW_FirstLootNotification = Cast<UW_FirstLootNotification>(GetWidgetFromName(TEXT("W_FirstLootNotification")));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - CachedW_FirstLootNotification: %s"),
		CachedW_FirstLootNotification ? TEXT("Found") : TEXT("NOT FOUND"));

	// Pre-load loot notification widget class
	if (!LootNotificationWidgetClass)
	{
		LootNotificationWidgetClass = LoadClass<UW_LootNotification>(nullptr,
			TEXT("/Game/SoulslikeFramework/Widgets/HUD/W_LootNotification.W_LootNotification_C"));
	}
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - LootNotificationWidgetClass: %s"),
		LootNotificationWidgetClass ? TEXT("Loaded") : TEXT("NOT LOADED"));

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CacheWidgetReferences - END"));
}

bool UW_HUD::GetTargetWidgetVisibility_Implementation(UUserWidget* Widget)
{
	// NOTE: Don't check IsInViewport() - child widgets inside W_HUD are not
	// added to viewport separately, so IsInViewport() returns false for them.
	// Just check the widget's visibility directly.
	if (Widget)
	{
		ESlateVisibility WidgetVisibility = Widget->GetVisibility();
		return WidgetVisibility == ESlateVisibility::Visible ||
			   WidgetVisibility == ESlateVisibility::SelfHitTestInvisible ||
			   WidgetVisibility == ESlateVisibility::HitTestInvisible;
	}
	return false;
}

void UW_HUD::BindToStatUpdate_Implementation(const TArray<USLFStatBase*>& AllStats, const TArray<TSubclassOf<USLFStatBase>>& StatsToListenFor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::BindToStatUpdate - Binding to %d stats, filtering by %d types"),
		AllStats.Num(), StatsToListenFor.Num());

	for (USLFStatBase* Stat : AllStats)
	{
		if (!Stat)
		{
			continue;
		}

		bool bShouldBind = false;
		if (StatsToListenFor.Num() == 0)
		{
			bShouldBind = true;
		}
		else
		{
			UClass* StatClass = Stat->GetClass();
			for (const TSubclassOf<USLFStatBase>& FilterClass : StatsToListenFor)
			{
				if (FilterClass && StatClass->IsChildOf(FilterClass))
				{
					bShouldBind = true;
					break;
				}
			}
		}

		if (bShouldBind)
		{
			Stat->OnStatUpdated.AddUniqueDynamic(this, &UW_HUD::OnStatUpdatedHandler);
			UE_LOG(LogTemp, Log, TEXT("  Bound to stat: %s (%s)"),
				*Stat->StatInfo.Tag.ToString(),
				*Stat->GetClass()->GetName());
		}
	}
}

bool UW_HUD::GetGameMenuVisibility_Implementation()
{
	if (CachedW_GameMenu)
	{
		return GetTargetWidgetVisibility(CachedW_GameMenu);
	}
	else if (UW_GameMenu* GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("W_GameMenu"))))
	{
		return GetTargetWidgetVisibility(GameMenu);
	}
	return false;
}

bool UW_HUD::IsRestMenuHud_Implementation()
{
	return false;
}

void UW_HUD::InitializeBindings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings"));

	// Also try to bind CachedW_GameMenu here in case CacheWidgetReferences missed it
	if (!CachedW_GameMenu)
	{
		CachedW_GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("W_GameMenu")));
		if (CachedW_GameMenu)
		{
			CachedW_GameMenu->OnGameMenuWidgetRequest.AddUniqueDynamic(this, &UW_HUD::OnGameMenuWidgetRequestHandler);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to CachedW_GameMenu::OnGameMenuWidgetRequest"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - CachedW_GameMenu not found!"));
		}
	}

	// Bind to InventoryManager's OnItemLooted event
	// Use FindComponentByClass to work with any controller type (Blueprint or C++)
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (UInventoryManagerComponent* InvMgr = PC->FindComponentByClass<UInventoryManagerComponent>())
		{
			InvMgr->OnItemLooted.AddUniqueDynamic(this, &UW_HUD::OnItemLootedHandler);
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to InventoryManager::OnItemLooted"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - InventoryManagerComponent not found on controller"));
		}

		// Bind to StatManager's OnStatsInitialized event and stat updates
		if (APawn* Pawn = PC->GetPawn())
		{
			if (UStatManagerComponent* StatManager = Pawn->FindComponentByClass<UStatManagerComponent>())
			{
				// Bind to OnStatsInitialized for future initializations
				StatManager->OnStatsInitialized.AddUniqueDynamic(this, &UW_HUD::EventOnStatsInitialized);
				UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Bound to StatManager::OnStatsInitialized"));

				// If stats are already initialized (component already ran BeginPlay), bind now
				TArray<UObject*> StatObjects;
				TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
				StatManager->GetAllStats(StatObjects, StatClassesAndCategories);

				if (StatObjects.Num() > 0)
				{
					UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeBindings - Stats already initialized, binding now..."));
					EventOnStatsInitialized();
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - StatManagerComponent not found on Pawn"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UW_HUD::InitializeBindings - No Pawn available yet"));
		}
	}
}

void UW_HUD::SerializeItemWheelData_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::SerializeItemWheelData"));
}

void UW_HUD::InitializeLoadedItemWheelSlotData_Implementation(const TArray<FSLFItemWheelSaveInfo>& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::InitializeLoadedItemWheelSlotData - %d items"), LoadedData.Num());
}

bool UW_HUD::GetDialogWindowVisibility_Implementation()
{
	return IsDialogActive;
}

void UW_HUD::CloseAllMenus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::CloseAllMenus"));
	EventCloseInventory();
	EventCloseEquipment();
	EventCloseCrafting();
	EventCloseStatus();
	EventCloseSettings();
	EventCloseGameMenu();
}

void UW_HUD::EventToggleUiMode_Implementation(bool bToggled)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventToggleUiMode - bToggled: %s"), bToggled ? TEXT("true") : TEXT("false"));

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		if (bToggled)
		{
			PC->SetShowMouseCursor(true);
			FInputModeUIOnly InputMode;
			PC->SetInputMode(InputMode);
		}
		else
		{
			PC->SetShowMouseCursor(false);
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
}

void UW_HUD::EventShowInventory_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInventory"));
	if (UWidget* InventoryWidget = GetWidgetFromName(TEXT("W_Inventory")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInventory - Found CachedW_Inventory, setting visible"));
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowInventory - CachedW_Inventory not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseInventory_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseInventory"));
	if (UWidget* InventoryWidget = GetWidgetFromName(TEXT("W_Inventory")))
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowEquipment_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowEquipment"));
	if (UWidget* EquipmentWidget = GetWidgetFromName(TEXT("W_Equipment")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowEquipment - Found CachedW_Equipment, setting visible"));
		EquipmentWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowEquipment - CachedW_Equipment not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseEquipment_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseEquipment"));
	if (UWidget* EquipmentWidget = GetWidgetFromName(TEXT("W_Equipment")))
	{
		EquipmentWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowStatus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowStatus"));
	if (UWidget* StatusWidget = GetWidgetFromName(TEXT("W_Status")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowStatus - Found CachedW_Status, setting visible"));
		StatusWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowStatus - CachedW_Status not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseStatus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseStatus"));
	if (UWidget* StatusWidget = GetWidgetFromName(TEXT("W_Status")))
	{
		StatusWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowCrafting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowCrafting"));
	if (UWidget* CraftingWidget = GetWidgetFromName(TEXT("W_Crafting")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowCrafting - Found CachedW_Crafting, setting visible"));
		CraftingWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowCrafting - CachedW_Crafting not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseCrafting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseCrafting"));
	if (UWidget* CraftingWidget = GetWidgetFromName(TEXT("W_Crafting")))
	{
		CraftingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSettings"));
	if (UWidget* SettingsWidget = GetWidgetFromName(TEXT("W_Settings")))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSettings - Found CachedW_Settings, setting visible"));
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowSettings - CachedW_Settings not found!"));
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseSettings"));
	if (UWidget* SettingsWidget = GetWidgetFromName(TEXT("W_Settings")))
	{
		SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowGameMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowGameMenu"));
	if (CachedW_GameMenu)
	{
		CachedW_GameMenu->SetVisibility(ESlateVisibility::Visible);
		// Set keyboard focus so gamepad/keyboard input works after returning from sub-menu
		CachedW_GameMenu->SetKeyboardFocus();
	}
	else if (UWidget* GameMenuWidget = GetWidgetFromName(TEXT("W_GameMenu")))
	{
		GameMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);

	// Set ActiveWidgetTag and switch input context on PlayerController
	if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetOwningPlayer()))
	{
		// Set active widget tag for navigation routing
		PC->ActiveWidgetTag = FGameplayTag::RequestGameplayTag(FName(TEXT("SoulslikeFramework.Backend.Widgets.GameMenu")));
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowGameMenu - Set ActiveWidgetTag to GameMenu"));

		// Switch input context: Enable menu context, disable gameplay context
		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (PC->IMC_NavigableMenu)
		{
			ToEnable.Add(PC->IMC_NavigableMenu);
		}
		if (PC->IMC_Gameplay)
		{
			ToDisable.Add(PC->IMC_Gameplay);
		}

		PC->SwitchInputContextInternal(ToEnable, ToDisable);
	}
}

void UW_HUD::EventCloseGameMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseGameMenu"));
	if (CachedW_GameMenu)
	{
		CachedW_GameMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (UWidget* GameMenuWidget = GetWidgetFromName(TEXT("W_GameMenu")))
	{
		GameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	EventToggleUiMode(false);

	// Clear ActiveWidgetTag and switch input context back to gameplay
	if (ASLFPlayerController* PC = Cast<ASLFPlayerController>(GetOwningPlayer()))
	{
		// Clear active widget tag
		PC->ActiveWidgetTag = FGameplayTag();
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseGameMenu - Cleared ActiveWidgetTag"));

		// Switch input context: Enable gameplay context, disable menu context
		TArray<UInputMappingContext*> ToEnable;
		TArray<UInputMappingContext*> ToDisable;

		if (PC->IMC_Gameplay)
		{
			ToEnable.Add(PC->IMC_Gameplay);
		}
		if (PC->IMC_NavigableMenu)
		{
			ToDisable.Add(PC->IMC_NavigableMenu);
		}

		PC->SwitchInputContextInternal(ToEnable, ToDisable);
	}
}

void UW_HUD::EventOnExitStorage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnExitStorage"));
	EventCloseInventory();
	EventToggleUiMode(false);
}

void UW_HUD::OnStatUpdatedHandler(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	EventOnStatUpdated(UpdatedStat, Change, bUpdateAffectedStats, ValueType);
}

void UW_HUD::EventOnStatUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	if (!UpdatedStat)
	{
		return;
	}

	FGameplayTag StatTag = UpdatedStat->StatInfo.Tag;
	double CurrentValue = UpdatedStat->StatInfo.CurrentValue;
	double MaxValue = UpdatedStat->StatInfo.MaxValue;
	double Percent = UpdatedStat->CalculatePercent();

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatUpdated - Stat: %s, Value: %.1f/%.1f (%.1f%%), Change: %.1f"),
		*StatTag.ToString(), CurrentValue, MaxValue, Percent, Change);

	// Forward stat update to W_Resources for visual update
	if (CachedW_Resources)
	{
		CachedW_Resources->EventUpdateStat(UpdatedStat->StatInfo);
	}
}

void UW_HUD::EventOnStatusEffectAdded_Implementation(UB_StatusEffect* StatusEffect)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatusEffectAdded - Effect: %s"),
		StatusEffect ? *StatusEffect->GetName() : TEXT("None"));
}

void UW_HUD::EventOnStatsInitialized_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatsInitialized - Binding to player stats"));

	// Get the player pawn's StatManagerComponent
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatsInitialized - No PlayerController!"));
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatsInitialized - No Pawn!"));
		return;
	}

	UStatManagerComponent* StatManager = Pawn->FindComponentByClass<UStatManagerComponent>();
	if (!StatManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnStatsInitialized - No StatManagerComponent!"));
		return;
	}

	// Get all stats and bind to their OnStatUpdated delegates
	TArray<UObject*> StatObjects;
	TMap<TSubclassOf<UObject>, FGameplayTag> StatClassesAndCategories;
	StatManager->GetAllStats(StatObjects, StatClassesAndCategories);

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatsInitialized - Found %d stats"), StatObjects.Num());

	for (UObject* StatObj : StatObjects)
	{
		USLFStatBase* Stat = Cast<USLFStatBase>(StatObj);
		if (Stat)
		{
			Stat->OnStatUpdated.AddUniqueDynamic(this, &UW_HUD::OnStatUpdatedHandler);
			UE_LOG(LogTemp, Log, TEXT("  Bound to stat: %s (%.1f/%.1f)"),
				*Stat->StatInfo.Tag.ToString(),
				Stat->StatInfo.CurrentValue,
				Stat->StatInfo.MaxValue);
		}
	}
}

void UW_HUD::EventOnBuffDetected_Implementation(UPDA_Buff* Buff, bool bAdded)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnBuffDetected - Buff: %s, Added: %s"),
		Buff ? *Buff->GetName() : TEXT("None"),
		bAdded ? TEXT("true") : TEXT("false"));
}

void UW_HUD::EventOnItemLooted_Implementation(const FSLFItemInfo& InItem, int32 InAmount, bool bExists)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Item: %s, Amount: %d, Exists: %s"),
		*InItem.DisplayName.ToString(), InAmount, bExists ? TEXT("true") : TEXT("false"));

	// If this is a new item (doesn't exist in inventory yet), show the "NEW" notification
	if (!bExists && CachedW_FirstLootNotification)
	{
		// Get icon texture from item
		UTexture2D* IconTexture = nullptr;
		if (!InItem.IconSmall.IsNull())
		{
			IconTexture = InItem.IconSmall.LoadSynchronous();
		}

		// Show the "NEW" first loot notification with item name and icon
		CachedW_FirstLootNotification->EventShowNotification(InItem.DisplayName, IconTexture, 3.0);

		UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Showing NEW notification for first-time item: %s"),
			*InItem.DisplayName.ToString());
	}

	// Create and show the regular loot notification
	if (CachedItemLootNotificationsBox && LootNotificationWidgetClass)
	{
		UW_LootNotification* LootNotif = CreateWidget<UW_LootNotification>(GetOwningPlayer(), LootNotificationWidgetClass);
		if (LootNotif)
		{
			// Get icon texture
			UTexture2D* IconTexture = nullptr;
			if (!InItem.IconSmall.IsNull())
			{
				IconTexture = InItem.IconSmall.LoadSynchronous();
			}

			// Setup the notification with item info
			LootNotif->ItemName = InItem.DisplayName;
			LootNotif->ItemLootedAmount = InAmount;
			LootNotif->ItemTexture = InItem.IconSmall;  // Copy the soft object ptr directly
			LootNotif->Duration = 3.0;

			// Add to the notifications box
			CachedItemLootNotificationsBox->AddChild(LootNotif);

			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Created loot notification widget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventOnItemLooted - Cannot create notification: Box=%s, Class=%s"),
			CachedItemLootNotificationsBox ? TEXT("OK") : TEXT("NULL"),
			LootNotificationWidgetClass ? TEXT("OK") : TEXT("NULL"));
	}
}

void UW_HUD::EventOnAbilityInfoInvalidated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnAbilityInfoInvalidated"));
}

void UW_HUD::EventOnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnDataLoaded"));
}

void UW_HUD::EventShowInteractableWidget_Implementation(AActor* Interactable)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Interactable: %s"),
		Interactable ? *Interactable->GetName() : TEXT("None"));

	// Cache reference if not set
	if (!CachedW_Interaction)
	{
		CachedW_Interaction = Cast<UW_Interaction>(GetWidgetFromName(TEXT("W_Interaction")));
	}

	if (!CachedW_Interaction)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::EventShowInteractableWidget - CachedW_Interaction widget not found!"));
		return;
	}

	if (!Interactable)
	{
		return;
	}

	// Try C++ interface first (ISLFInteractableInterface::TryGetItemInfo returns FSLFItemInfo)
	if (Interactable->GetClass()->ImplementsInterface(USLFInteractableInterface::StaticClass()))
	{
		FSLFItemInfo ItemInfo = ISLFInteractableInterface::Execute_TryGetItemInfo(Interactable);
		// Check DisplayName instead of ItemTag (ItemTag wasn't migrated, but DisplayName was)
		if (!ItemInfo.DisplayName.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Via ISLFInteractableInterface, showing item: %s"),
				*ItemInfo.DisplayName.ToString());
			CachedW_Interaction->EventOnItemOverlap(ItemInfo);
			return;
		}
	}

	// Fallback: Try Blueprint interface (IBPI_Interactable::TryGetItemInfo has out param signature)
	if (Interactable->GetClass()->ImplementsInterface(UBPI_Interactable::StaticClass()))
	{
		FSLFItemInfo ItemInfo;
		IBPI_Interactable::Execute_TryGetItemInfo(Interactable, ItemInfo);
		// Check DisplayName instead of ItemTag
		if (!ItemInfo.DisplayName.IsEmpty())
		{
			UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Via IBPI_Interactable, showing item: %s"),
				*ItemInfo.DisplayName.ToString());
			CachedW_Interaction->EventOnItemOverlap(ItemInfo);
			return;
		}
	}

	// Get interaction text from the actor - check multiple types
	FText InteractionText = FText::GetEmpty();
	if (ASLFInteractableBase* InteractableBase = Cast<ASLFInteractableBase>(Interactable))
	{
		InteractionText = InteractableBase->InteractionText;
	}
	else if (ASLFBossDoor* BossDoor = Cast<ASLFBossDoor>(Interactable))
	{
		InteractionText = BossDoor->InteractionText;
	}

	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Showing interactable overlap for text: %s"),
		*InteractionText.ToString());
	CachedW_Interaction->EventOnInteractableOverlap(InteractionText);
}

void UW_HUD::EventHideInteractionWidget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventHideInteractionWidget"));

	// Cache reference if not set
	if (!CachedW_Interaction)
	{
		CachedW_Interaction = Cast<UW_Interaction>(GetWidgetFromName(TEXT("W_Interaction")));
	}

	if (CachedW_Interaction)
	{
		CachedW_Interaction->EventHide();
	}
}

void UW_HUD::EventSetupRestingPointWidget_Implementation(AB_RestingPoint* TargetCampfire)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupRestingPointWidget - Campfire: %s"),
		TargetCampfire ? *TargetCampfire->GetName() : TEXT("None"));
	if (UW_RestMenu* RestMenuWidget = Cast<UW_RestMenu>(GetWidgetFromName(TEXT("RestMenu"))))
	{
		RestMenuWidget->EventSetupRestingPoint(TargetCampfire);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventShowNpcInteractionWidget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowNpcInteractionWidget"));
	if (UWidget* NpcWindow = GetWidgetFromName(TEXT("NpcWindow")))
	{
		NpcWindow->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventSetupDialog_Implementation(const FText& InText)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupDialog"));
	IsDialogActive = true;
	if (UWidget* DialogWindow = GetWidgetFromName(TEXT("DialogWindow")))
	{
		DialogWindow->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventFinishDialog_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventFinishDialog"));
	IsDialogActive = false;
	if (UWidget* DialogWindow = GetWidgetFromName(TEXT("DialogWindow")))
	{
		DialogWindow->SetVisibility(ESlateVisibility::Collapsed);
	}
	OnDialogWindowClosed.Broadcast();
}

void UW_HUD::EventSetupNpcWindow_Implementation(const FText& NpcName, UPDA_Vendor* InVendorAsset, UAC_AI_InteractionManager* DialogComponent)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSetupNpcWindow - NPC: %s"), *NpcName.ToString());
	if (UWidget* NpcWindow = GetWidgetFromName(TEXT("NpcWindow")))
	{
		NpcWindow->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventFade_Implementation(ESLFFadeTypes FadeType, float PlayRate)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventFade - Type: %d, PlayRate: %f"), (int32)FadeType, PlayRate);
}

void UW_HUD::EventSwitchViewport_Implementation(int32 InIndex)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSwitchViewport - Index: %d"), InIndex);
	if (UWidgetSwitcher* ViewportSwitcher = Cast<UWidgetSwitcher>(GetWidgetFromName(TEXT("ViewportSwitcher"))))
	{
		ViewportSwitcher->SetActiveWidgetIndex(InIndex);
	}
}

void UW_HUD::EventSendInteractionErrorMessage_Implementation(const FText& InMessage, float Time)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventSendInteractionErrorMessage - Message: %s, Time: %f"),
		*InMessage.ToString(), Time);
}

void UW_HUD::EventBigScreenMessage_Implementation(const FText& InMessage, UMaterialInterface* GradientMaterial, bool bHasBackdrop, float AnimationRateScale)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventBigScreenMessage - Message: %s, AnimationRate: %f"),
		*InMessage.ToString(), AnimationRateScale);

	// Find and show the W_BigScreenMessage widget
	UW_BigScreenMessage* BigScreenMsg = Cast<UW_BigScreenMessage>(GetWidgetFromName(TEXT("W_BigScreenMessage")));
	if (BigScreenMsg)
	{
		// Call EventShowMessage on the widget
		BigScreenMsg->EventShowMessage(InMessage, GradientMaterial, bHasBackdrop, AnimationRateScale);
		UE_LOG(LogTemp, Log, TEXT("[W_HUD] Forwarded to W_BigScreenMessage"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_HUD] W_BigScreenMessage widget not found!"));
	}
}

void UW_HUD::ShowDeathScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("UW_HUD::ShowDeathScreen - Showing 'YOU DIED' screen"));

	// Call EventBigScreenMessage with "YOU DIED" text
	EventBigScreenMessage(FText::FromString(TEXT("YOU DIED")), nullptr, true, 1.0f);

	// Also display on-screen message as fallback
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("YOU DIED"));
	}
}

void UW_HUD::HideDeathScreen()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::HideDeathScreen - Hiding death screen"));

	// Hide the big screen message widget (use correct widget name "W_BigScreenMessage")
	if (UWidget* BigScreenMessage = GetWidgetFromName(TEXT("W_BigScreenMessage")))
	{
		BigScreenMessage->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[W_HUD] Hid W_BigScreenMessage widget"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[W_HUD] HideDeathScreen - W_BigScreenMessage not found!"));
	}

	// Clear any on-screen debug messages
	if (GEngine)
	{
		GEngine->ClearOnScreenDebugMessages();
	}
}

void UW_HUD::EventShowAutoSaveIcon_Implementation(float InDuration)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowAutoSaveIcon - Duration: %f"), InDuration);
	if (UWidget* SaveIcon = GetWidgetFromName(TEXT("AutoSaveIcon")))
	{
		SaveIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventRemoveAutoSaveIcon_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventRemoveAutoSaveIcon"));
	if (UWidget* SaveIcon = GetWidgetFromName(TEXT("AutoSaveIcon")))
	{
		SaveIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventCreateLoadingScreen_Implementation(float AdditionalWaitDuration, bool bAutoRemove)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCreateLoadingScreen - Duration: %f, AutoRemove: %s"),
		AdditionalWaitDuration, bAutoRemove ? TEXT("true") : TEXT("false"));
	if (LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventOnLoadingScreenFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnLoadingScreenFinished"));
	if (LoadingScreen)
	{
		LoadingScreen->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventToggleCinematicMode_Implementation(bool bActive, bool bFadeOutIfNotCinematicMode)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventToggleCinematicMode - Active: %s, FadeOut: %s"),
		bActive ? TEXT("true") : TEXT("false"),
		bFadeOutIfNotCinematicMode ? TEXT("true") : TEXT("false"));
	CinematicMode = bActive;
	if (UWidget* HudContent = GetWidgetFromName(TEXT("HudContent")))
	{
		HudContent->SetVisibility(bActive ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

void UW_HUD::EventShowSkipCinematicNotification_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSkipCinematicNotification"));
	if (UWidget* SkipNotification = GetWidgetFromName(TEXT("SkipCinematicNotification")))
	{
		SkipNotification->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventShowBossBar_Implementation(const FText& BossName, AActor* BossActor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowBossBar - Boss: %s"), *BossName.ToString());
	if (UWidget* BossBar = GetWidgetFromName(TEXT("BossBar")))
	{
		BossBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventHideBossBar_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventHideBossBar"));
	if (UWidget* BossBar = GetWidgetFromName(TEXT("BossBar")))
	{
		BossBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventOnBossDestroyed_Implementation(AActor* DestroyedActor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnBossDestroyed"));
	EventHideBossBar();
}

void UW_HUD::OnGameMenuWidgetRequestHandler(FGameplayTag WidgetTag)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Tag: %s"), *WidgetTag.ToString());

	// Switch on gameplay tag to show the appropriate widget
	static const FGameplayTag InventoryTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Inventory"));
	static const FGameplayTag EquipmentTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Equipment"));
	static const FGameplayTag CraftingTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Crafting"));
	static const FGameplayTag StatusTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.Status"));
	static const FGameplayTag SystemTag = FGameplayTag::RequestGameplayTag(FName("SoulslikeFramework.Backend.Widgets.System"));

	if (WidgetTag.MatchesTag(InventoryTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Inventory"));
		EventShowInventory();
	}
	else if (WidgetTag.MatchesTag(EquipmentTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Equipment"));
		EventShowEquipment();
	}
	else if (WidgetTag.MatchesTag(CraftingTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Crafting"));
		EventShowCrafting();
	}
	else if (WidgetTag.MatchesTag(StatusTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Status"));
		EventShowStatus();
	}
	else if (WidgetTag.MatchesTag(SystemTag))
	{
		UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Showing Settings"));
		EventShowSettings();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::OnGameMenuWidgetRequestHandler - Unknown tag: %s"), *WidgetTag.ToString());
	}
}

void UW_HUD::OnInventoryClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnInventoryClosedHandler - Closing Inventory, showing GameMenu"));
	EventCloseInventory();
	EventShowGameMenu();
}

void UW_HUD::OnEquipmentClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnEquipmentClosedHandler - Closing Equipment, showing GameMenu"));
	EventCloseEquipment();
	EventShowGameMenu();
}

void UW_HUD::OnCraftingClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnCraftingClosedHandler - Closing Crafting, showing GameMenu"));
	EventCloseCrafting();
	EventShowGameMenu();
}

void UW_HUD::OnStatusClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnStatusClosedHandler - Closing Status, showing GameMenu"));
	EventCloseStatus();
	EventShowGameMenu();
}

void UW_HUD::OnSettingsClosedHandler()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnSettingsClosedHandler - Closing Settings, showing GameMenu"));
	EventCloseSettings();
	EventShowGameMenu();
}

void UW_HUD::OnItemLootedHandler(UDataAsset* ItemAsset, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::OnItemLootedHandler - Item: %s, Amount: %d"),
		ItemAsset ? *ItemAsset->GetName() : TEXT("null"), Amount);

	if (!ItemAsset)
	{
		return;
	}

	// Get item info from the data asset
	FSLFItemInfo ItemInfo;
	bool bExists = false;

	// Try to cast to PDA_Item to get the item information
	if (UPDA_Item* ItemData = Cast<UPDA_Item>(ItemAsset))
	{
		ItemInfo = ItemData->ItemInformation;

		// Check if item existed in inventory before this add (Amount > 1 means it existed)
		// Note: The inventory manager broadcasts AFTER adding, so if the new amount is
		// greater than the amount just added, it means the item already existed
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (UInventoryManagerComponent* InvMgr = PC->FindComponentByClass<UInventoryManagerComponent>())
			{
				// Get the current amount from inventory
				int32 CurrentAmount = InvMgr->GetOverallCountForItem(ItemAsset);
				// If current amount > Amount we just picked up, item already existed
				bExists = (CurrentAmount > Amount);
			}
		}
	}

	// Call the main event handler with transformed parameters
	EventOnItemLooted(ItemInfo, Amount, bExists);
}

// ═══════════════════════════════════════════════════════════════════════════
// NAVIGATION ROUTING FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void UW_HUD::RouteNavigateCancel(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateCancel - ActiveWidgetTag: %s"), *TagString);

	// Route based on active widget tag
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("Status")))
	{
		if (CachedW_Status) CachedW_Status->EventNavigateCancel();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateCancel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_HUD::RouteNavigateCancel - Unknown widget tag: %s"), *TagString);
	}
}

void UW_HUD::RouteNavigateOk(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateOk - ActiveWidgetTag: %s"), *TagString);

	// Note: W_Status only has EventNavigateCancel
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateOk();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateOk();
	}
}

void UW_HUD::RouteNavigateUp(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateUp - ActiveWidgetTag: %s"), *TagString);

	// Note: W_Status only has EventNavigateCancel, no Up/Down navigation
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateUp();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateUp();
	}
}

void UW_HUD::RouteNavigateDown(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateDown - ActiveWidgetTag: %s"), *TagString);

	// Note: W_Status only has EventNavigateCancel, no Up/Down navigation
	if (TagString.Contains(TEXT("GameMenu")))
	{
		if (CachedW_GameMenu) CachedW_GameMenu->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateDown();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateDown();
	}
}

void UW_HUD::RouteNavigateLeft(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateLeft - ActiveWidgetTag: %s"), *TagString);

	// Note: W_GameMenu and W_Status don't have Left navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateLeft();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateLeft();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateLeft();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateLeft();
	}
}

void UW_HUD::RouteNavigateRight(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateRight - ActiveWidgetTag: %s"), *TagString);

	// Note: W_GameMenu and W_Status don't have Right navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateRight();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateRight();
	}
	else if (TagString.Contains(TEXT("Crafting")))
	{
		if (CachedW_Crafting) CachedW_Crafting->EventNavigateRight();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateRight();
	}
}

void UW_HUD::RouteNavigateCategoryLeft(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateCategoryLeft - ActiveWidgetTag: %s"), *TagString);

	// Only Inventory and Settings have category navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateCategoryLeft();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateCategoryLeft();
	}
}

void UW_HUD::RouteNavigateCategoryRight(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateCategoryRight - ActiveWidgetTag: %s"), *TagString);

	// Only Inventory and Settings have category navigation
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateCategoryRight();
	}
	else if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateCategoryRight();
	}
}

void UW_HUD::RouteNavigateUnequip(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateUnequip - ActiveWidgetTag: %s"), *TagString);

	// Only Equipment has unequip navigation
	if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateUnequip();
	}
}

void UW_HUD::RouteNavigateDetailedView(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateDetailedView - ActiveWidgetTag: %s"), *TagString);

	// Inventory and Equipment have detailed view
	if (TagString.Contains(TEXT("Inventory")))
	{
		if (CachedW_Inventory) CachedW_Inventory->EventNavigateDetailedView();
	}
	else if (TagString.Contains(TEXT("Equipment")))
	{
		if (CachedW_Equipment) CachedW_Equipment->EventNavigateDetailedView();
	}
}

void UW_HUD::RouteNavigateResetToDefaults(const FGameplayTag& ActiveWidgetTag)
{
	FString TagString = ActiveWidgetTag.ToString();
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::RouteNavigateResetToDefaults - ActiveWidgetTag: %s"), *TagString);

	// Only Settings has reset to defaults (note: function is EventNavigateResetToDefault, singular)
	if (TagString.Contains(TEXT("System")) || TagString.Contains(TEXT("Settings")))
	{
		if (CachedW_Settings) CachedW_Settings->EventNavigateResetToDefault();
	}
}
