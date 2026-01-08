// W_HUD.cpp
// C++ Widget implementation for W_HUD
//
// 20-PASS VALIDATION: 2026-01-07
// Base implementation with child widget visibility management

#include "Widgets/W_HUD.h"
#include "Blueprints/B_Stat.h"
#include "Blueprints/B_StatusEffect.h"
#include "Blueprints/B_Interactable.h"
#include "Blueprints/B_RestingPoint.h"
#include "Components/AC_AI_InteractionManager.h"
#include "Widgets/W_LoadingScreen.h"
#include "Widgets/W_Inventory.h"
#include "Widgets/W_Equipment.h"
#include "Widgets/W_Crafting.h"
#include "Widgets/W_GameMenu.h"
#include "Widgets/W_Status.h"
#include "Widgets/W_RestMenu.h"
#include "Widgets/W_NPC_Window.h"
#include "Widgets/W_Dialog.h"
#include "Widgets/W_Interaction.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"

UW_HUD::UW_HUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LoadingScreen = nullptr;
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
	if (!LoadingScreen)
	{
		LoadingScreen = Cast<UW_LoadingScreen>(GetWidgetFromName(TEXT("LoadingScreen")));
	}
}

bool UW_HUD::GetTargetWidgetVisibility_Implementation(UUserWidget* Widget)
{
	if (Widget && Widget->IsInViewport())
	{
		ESlateVisibility WidgetVisibility = Widget->GetVisibility();
		return WidgetVisibility == ESlateVisibility::Visible ||
			   WidgetVisibility == ESlateVisibility::SelfHitTestInvisible ||
			   WidgetVisibility == ESlateVisibility::HitTestInvisible;
	}
	return false;
}

void UW_HUD::BindToStatUpdate_Implementation(const TArray<UB_Stat*>& AllStats, const TArray<TSubclassOf<UB_Stat>>& StatsToListenFor)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::BindToStatUpdate - Binding to %d stats, filtering by %d types"),
		AllStats.Num(), StatsToListenFor.Num());

	for (UB_Stat* Stat : AllStats)
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
			for (const TSubclassOf<UB_Stat>& FilterClass : StatsToListenFor)
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
	if (UW_GameMenu* GameMenu = Cast<UW_GameMenu>(GetWidgetFromName(TEXT("GameMenu"))))
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
	if (UWidget* InventoryWidget = GetWidgetFromName(TEXT("InventoryWidget")))
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseInventory_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseInventory"));
	if (UWidget* InventoryWidget = GetWidgetFromName(TEXT("InventoryWidget")))
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowEquipment_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowEquipment"));
	if (UWidget* EquipmentWidget = GetWidgetFromName(TEXT("EquipmentWidget")))
	{
		EquipmentWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseEquipment_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseEquipment"));
	if (UWidget* EquipmentWidget = GetWidgetFromName(TEXT("EquipmentWidget")))
	{
		EquipmentWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowStatus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowStatus"));
	if (UWidget* StatusWidget = GetWidgetFromName(TEXT("StatusWidget")))
	{
		StatusWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseStatus_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseStatus"));
	if (UWidget* StatusWidget = GetWidgetFromName(TEXT("StatusWidget")))
	{
		StatusWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowCrafting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowCrafting"));
	if (UWidget* CraftingWidget = GetWidgetFromName(TEXT("CraftingWidget")))
	{
		CraftingWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseCrafting_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseCrafting"));
	if (UWidget* CraftingWidget = GetWidgetFromName(TEXT("CraftingWidget")))
	{
		CraftingWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowSettings"));
	if (UWidget* SettingsWidget = GetWidgetFromName(TEXT("SettingsWidget")))
	{
		SettingsWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseSettings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseSettings"));
	if (UWidget* SettingsWidget = GetWidgetFromName(TEXT("SettingsWidget")))
	{
		SettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UW_HUD::EventShowGameMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowGameMenu"));
	if (UWidget* GameMenuWidget = GetWidgetFromName(TEXT("GameMenu")))
	{
		GameMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
	EventToggleUiMode(true);
}

void UW_HUD::EventCloseGameMenu_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventCloseGameMenu"));
	if (UWidget* GameMenuWidget = GetWidgetFromName(TEXT("GameMenu")))
	{
		GameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	EventToggleUiMode(false);
}

void UW_HUD::EventOnExitStorage_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnExitStorage"));
	EventCloseInventory();
	EventToggleUiMode(false);
}

void UW_HUD::OnStatUpdatedHandler(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
{
	EventOnStatUpdated(UpdatedStat, Change, bUpdateAffectedStats, ValueType);
}

void UW_HUD::EventOnStatUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType)
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
}

void UW_HUD::EventOnStatusEffectAdded_Implementation(UB_StatusEffect* StatusEffect)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatusEffectAdded - Effect: %s"),
		StatusEffect ? *StatusEffect->GetName() : TEXT("None"));
}

void UW_HUD::EventOnStatsInitialized_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnStatsInitialized"));
}

void UW_HUD::EventOnBuffDetected_Implementation(UPDA_Buff* Buff, bool bAdded)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnBuffDetected - Buff: %s, Added: %s"),
		Buff ? *Buff->GetName() : TEXT("None"),
		bAdded ? TEXT("true") : TEXT("false"));
}

void UW_HUD::EventOnItemLooted_Implementation(const FSLFItemInfo& InItem, int32 InAmount, bool bExists)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnItemLooted - Amount: %d, Exists: %s"),
		InAmount, bExists ? TEXT("true") : TEXT("false"));
}

void UW_HUD::EventOnAbilityInfoInvalidated_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnAbilityInfoInvalidated"));
}

void UW_HUD::EventOnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventOnDataLoaded"));
}

void UW_HUD::EventShowInteractableWidget_Implementation(AB_Interactable* Interactable)
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventShowInteractableWidget - Interactable: %s"),
		Interactable ? *Interactable->GetName() : TEXT("None"));
	if (UWidget* InteractionWidget = GetWidgetFromName(TEXT("InteractionWidget")))
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HUD::EventHideInteractionWidget_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_HUD::EventHideInteractionWidget"));
	if (UWidget* InteractionWidget = GetWidgetFromName(TEXT("InteractionWidget")))
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
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
