// W_HUD.h
// C++ Widget class for W_HUD
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_HUD.json
// Parent: UUserWidget
// Variables: 3 | Functions: 9 | Dispatchers: 1

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFStatTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_HUD.generated.h"

// Forward declarations for widget types
class UW_LoadingScreen;
class UW_Inventory;
class UW_Equipment;
class UW_Crafting;
class UW_GameMenu;
class UW_Status;
class UW_Settings;
class UW_RestMenu;
class UW_NPC_Window;
class UW_DialogWindow;
class UW_InteractableWidget;
class UW_BossBar;
class UW_ItemWheel;
class UW_AbilityDisplay;
class UW_Interaction;
class UW_LootNotification;
class UW_FirstLootNotification;
class UVerticalBox;

// Forward declarations for Blueprint types
class UB_Stat;
class UB_StatusEffect;
class UPDA_Buff;
class UPDA_Vendor;
class AB_Interactable;
class AB_RestingPoint;
class UAC_AI_InteractionManager;
class UMaterialInterface;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_HUD_OnDialogWindowClosed);

UCLASS()
class SLFCONVERSION_API UW_HUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_HUD(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════
	// NOTE: Widget pointers (W_Interaction, W_GameMenu, etc.) are NOT declared here
	// because the Blueprint UMG designer already has BindWidget variables with these names.
	// Declaring them in C++ causes "property already exists" compiler errors.
	// These widgets are cached via GetWidgetFromName in CacheWidgetReferences().

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_LoadingScreen* LoadingScreen;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool IsDialogActive;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool CinematicMode;

protected:
	// Cached widget pointers - NOT UPROPERTY to avoid conflict with Blueprint BindWidget variables
	UW_Interaction* CachedW_Interaction;
	UW_GameMenu* CachedW_GameMenu;
	UW_Inventory* CachedW_Inventory;
	UW_Equipment* CachedW_Equipment;
	UW_Crafting* CachedW_Crafting;
	UW_Status* CachedW_Status;
	UW_Settings* CachedW_Settings;

	// Loot notification widgets - cached from UMG
	UVerticalBox* CachedItemLootNotificationsBox;
	UW_FirstLootNotification* CachedW_FirstLootNotification;

	// Loot notification widget class for dynamic creation
	UPROPERTY()
	TSubclassOf<UW_LootNotification> LootNotificationWidgetClass;

public:

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_HUD_OnDialogWindowClosed OnDialogWindowClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (9)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	bool GetTargetWidgetVisibility(UUserWidget* Widget);
	virtual bool GetTargetWidgetVisibility_Implementation(UUserWidget* Widget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	void BindToStatUpdate(const TArray<UB_Stat*>& AllStats, const TArray<TSubclassOf<UB_Stat>>& StatsToListenFor);
	virtual void BindToStatUpdate_Implementation(const TArray<UB_Stat*>& AllStats, const TArray<TSubclassOf<UB_Stat>>& StatsToListenFor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	bool GetGameMenuVisibility();
	virtual bool GetGameMenuVisibility_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	bool IsRestMenuHud();
	virtual bool IsRestMenuHud_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	void InitializeBindings();
	virtual void InitializeBindings_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	void SerializeItemWheelData();
	virtual void SerializeItemWheelData_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	void InitializeLoadedItemWheelSlotData(const TArray<FSLFItemWheelSaveInfo>& LoadedData);
	virtual void InitializeLoadedItemWheelSlotData_Implementation(const TArray<FSLFItemWheelSaveInfo>& LoadedData);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	bool GetDialogWindowVisibility();
	virtual bool GetDialogWindowVisibility_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD")
	void CloseAllMenus();
	virtual void CloseAllMenus_Implementation();

	// Menu Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventToggleUiMode(bool bToggled);
	virtual void EventToggleUiMode_Implementation(bool bToggled);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowInventory();
	virtual void EventShowInventory_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseInventory();
	virtual void EventCloseInventory_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseEquipment();
	virtual void EventCloseEquipment_Implementation();


	// Additional Menu Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowEquipment();
	virtual void EventShowEquipment_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowStatus();
	virtual void EventShowStatus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowCrafting();
	virtual void EventShowCrafting_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowSettings();
	virtual void EventShowSettings_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowGameMenu();
	virtual void EventShowGameMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseCrafting();
	virtual void EventCloseCrafting_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseStatus();
	virtual void EventCloseStatus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseSettings();
	virtual void EventCloseSettings_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseGameMenu();
	virtual void EventCloseGameMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventOnExitStorage();
	virtual void EventOnExitStorage_Implementation();

	// Stat/Status Events

	/** Internal handler bound to UB_Stat::OnStatUpdated delegates */
	UFUNCTION()
	void OnStatUpdatedHandler(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatUpdated(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);
	virtual void EventOnStatUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatusEffectAdded(UB_StatusEffect* StatusEffect);
	virtual void EventOnStatusEffectAdded_Implementation(UB_StatusEffect* StatusEffect);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatsInitialized();
	virtual void EventOnStatsInitialized_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnBuffDetected(UPDA_Buff* Buff, bool bAdded);
	virtual void EventOnBuffDetected_Implementation(UPDA_Buff* Buff, bool bAdded);

	// Item Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Items")
	void EventOnItemLooted(const FSLFItemInfo& InItem, int32 InAmount, bool bExists);
	virtual void EventOnItemLooted_Implementation(const FSLFItemInfo& InItem, int32 InAmount, bool bExists);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Items")
	void EventOnAbilityInfoInvalidated();
	virtual void EventOnAbilityInfoInvalidated_Implementation();

	// Save/Load Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|SaveLoad")
	void EventOnDataLoaded(const FSLFSaveGameInfo& LoadedData);
	virtual void EventOnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData);

	// Interaction Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventShowInteractableWidget(AActor* Interactable);
	virtual void EventShowInteractableWidget_Implementation(AActor* Interactable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventHideInteractionWidget();
	virtual void EventHideInteractionWidget_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventSetupRestingPointWidget(AB_RestingPoint* TargetCampfire);
	virtual void EventSetupRestingPointWidget_Implementation(AB_RestingPoint* TargetCampfire);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventShowNpcInteractionWidget();
	virtual void EventShowNpcInteractionWidget_Implementation();

	// Dialog Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Dialog")
	void EventSetupDialog(const FText& InText);
	virtual void EventSetupDialog_Implementation(const FText& InText);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Dialog")
	void EventFinishDialog();
	virtual void EventFinishDialog_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Dialog")
	void EventSetupNpcWindow(const FText& NpcName, UPDA_Vendor* InVendorAsset, UAC_AI_InteractionManager* DialogComponent);
	virtual void EventSetupNpcWindow_Implementation(const FText& NpcName, UPDA_Vendor* InVendorAsset, UAC_AI_InteractionManager* DialogComponent);

	// Visual Effects Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventFade(ESLFFadeTypes FadeType, float PlayRate);
	virtual void EventFade_Implementation(ESLFFadeTypes FadeType, float PlayRate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventSwitchViewport(int32 InIndex);
	virtual void EventSwitchViewport_Implementation(int32 InIndex);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventSendInteractionErrorMessage(const FText& InMessage, float Time);
	virtual void EventSendInteractionErrorMessage_Implementation(const FText& InMessage, float Time);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventBigScreenMessage(const FText& InMessage, UMaterialInterface* GradientMaterial, bool bHasBackdrop, float AnimationRateScale);
	virtual void EventBigScreenMessage_Implementation(const FText& InMessage, UMaterialInterface* GradientMaterial, bool bHasBackdrop, float AnimationRateScale);

	// Save Icon Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|SaveIcon")
	void EventShowAutoSaveIcon(float InDuration);
	virtual void EventShowAutoSaveIcon_Implementation(float InDuration);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|SaveIcon")
	void EventRemoveAutoSaveIcon();
	virtual void EventRemoveAutoSaveIcon_Implementation();

	// Loading Screen Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Loading")
	void EventCreateLoadingScreen(float AdditionalWaitDuration, bool bAutoRemove);
	virtual void EventCreateLoadingScreen_Implementation(float AdditionalWaitDuration, bool bAutoRemove);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Loading")
	void EventOnLoadingScreenFinished();
	virtual void EventOnLoadingScreenFinished_Implementation();

	// Cinematic Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Cinematic")
	void EventToggleCinematicMode(bool bActive, bool bFadeOutIfNotCinematicMode);
	virtual void EventToggleCinematicMode_Implementation(bool bActive, bool bFadeOutIfNotCinematicMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Cinematic")
	void EventShowSkipCinematicNotification();
	virtual void EventShowSkipCinematicNotification_Implementation();

	// Boss Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Boss")
	void EventShowBossBar(const FText& BossName, AActor* BossActor);
	virtual void EventShowBossBar_Implementation(const FText& BossName, AActor* BossActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Boss")
	void EventHideBossBar();
	virtual void EventHideBossBar_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Boss")
	void EventOnBossDestroyed(AActor* DestroyedActor);
	virtual void EventOnBossDestroyed_Implementation(AActor* DestroyedActor);

protected:
	// Cache references
	void CacheWidgetReferences();

	/** Internal handler bound to InventoryManager::OnItemLooted - bridges to EventOnItemLooted */
	UFUNCTION()
	void OnItemLootedHandler(UDataAsset* ItemAsset, int32 Amount);

	/** Internal handler bound to W_GameMenu::OnGameMenuWidgetRequest */
	UFUNCTION()
	void OnGameMenuWidgetRequestHandler(FGameplayTag WidgetTag);

	/** Internal handlers bound to child widget closed dispatchers */
	UFUNCTION()
	void OnInventoryClosedHandler();

	UFUNCTION()
	void OnEquipmentClosedHandler();

	UFUNCTION()
	void OnCraftingClosedHandler();

	UFUNCTION()
	void OnStatusClosedHandler();

	UFUNCTION()
	void OnSettingsClosedHandler();
};
