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
class UW_Dialog;
class UW_InteractableWidget;
class UW_BossBar;
class UW_Boss_Healthbar;
class UW_ItemWheel;
class UW_ItemWheelSlot;
class UW_AbilityDisplay;
class UW_Interaction;
class UW_LootNotification;
class UW_FirstLootNotification;
class UW_Resources;
class UW_StatusEffectBar;
class UW_StatusEffectNotification;
class UVerticalBox;
class UW_Radar;

// Forward declarations for Blueprint types
class UB_Stat;
class USLFStatBase;
class UB_StatusEffect;
class UPDA_Buff;
class UPDA_Vendor;
class AB_Interactable;
class AB_RestingPoint;
class UAIInteractionManagerComponent;
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
	UW_Resources* CachedW_Resources;
	UW_RestMenu* CachedW_RestMenu;
	UW_Radar* CachedW_Radar;

	// NPC dialog widgets - cached from UMG
	UW_NPC_Window* CachedW_NPC_Window;
	UW_Dialog* CachedW_Dialog;

	// Loot notification widgets - cached from UMG
	UVerticalBox* CachedItemLootNotificationsBox;
	UW_FirstLootNotification* CachedW_FirstLootNotification;

	// Status effect widgets - cached from UMG
	UVerticalBox* CachedStatusEffectBox;
	UW_StatusEffectNotification* CachedW_StatusEffectNotification;

	// Boss health bar widget - cached from UMG
	UW_Boss_Healthbar* CachedBossHealthbar;

	// Loot notification widget class for dynamic creation
	UPROPERTY()
	TSubclassOf<UW_LootNotification> LootNotificationWidgetClass;

	// Status effect bar widget class for dynamic creation
	UPROPERTY()
	TSubclassOf<UW_StatusEffectBar> StatusEffectBarWidgetClass;

	// Item wheel widgets - cached to configure SlotsToTrack if needed
	UW_ItemWheelSlot* CachedItemWheelTools;

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
	void BindToStatUpdate(const TArray<USLFStatBase*>& AllStats, const TArray<TSubclassOf<USLFStatBase>>& StatsToListenFor);
	virtual void BindToStatUpdate_Implementation(const TArray<USLFStatBase*>& AllStats, const TArray<TSubclassOf<USLFStatBase>>& StatsToListenFor);
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

	/** Internal handler bound to USLFStatBase::OnStatUpdated delegates */
	UFUNCTION()
	void OnStatUpdatedHandler(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatUpdated(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);
	virtual void EventOnStatUpdated_Implementation(USLFStatBase* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

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
	void EventSetupRestingPointWidget(AActor* TargetCampfire);
	virtual void EventSetupRestingPointWidget_Implementation(AActor* TargetCampfire);

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
	void EventSetupNpcWindow(const FText& NpcName, UDataAsset* InVendorAsset, UAIInteractionManagerComponent* DialogComponent);
	virtual void EventSetupNpcWindow_Implementation(const FText& NpcName, UDataAsset* InVendorAsset, UAIInteractionManagerComponent* DialogComponent);

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
	void EventToggleCinematicMode(UPARAM(DisplayName = "Active?") bool bActive, bool bFadeOutIfNotCinematicMode);
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

	// ═══════════════════════════════════════════════════════════════════════
	// NAVIGATION ROUTING (Called by PlayerController based on ActiveWidgetTag)
	// ═══════════════════════════════════════════════════════════════════════

	/** Route navigation cancel (Back button/B button) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateCancel(const FGameplayTag& ActiveWidgetTag);

	/** Route navigation OK (A button/Enter) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateOk(const FGameplayTag& ActiveWidgetTag);

	/** Route navigation up (DPad Up/W key) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateUp(const FGameplayTag& ActiveWidgetTag);

	/** Route navigation down (DPad Down/S key) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateDown(const FGameplayTag& ActiveWidgetTag);

	/** Route navigation left (DPad Left/A key) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateLeft(const FGameplayTag& ActiveWidgetTag);

	/** Route navigation right (DPad Right/D key) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateRight(const FGameplayTag& ActiveWidgetTag);

	/** Route category left (LB/Q key) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateCategoryLeft(const FGameplayTag& ActiveWidgetTag);

	/** Route category right (RB/E key) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateCategoryRight(const FGameplayTag& ActiveWidgetTag);

	/** Route unequip action (Y button) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateUnequip(const FGameplayTag& ActiveWidgetTag);

	/** Route detailed view toggle (X button) to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateDetailedView(const FGameplayTag& ActiveWidgetTag);

	/** Route reset to defaults to active widget */
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Navigation")
	void RouteNavigateResetToDefaults(const FGameplayTag& ActiveWidgetTag);

	// Getters for cached widgets (used by PlayerController)
	UW_GameMenu* GetCachedGameMenu() const { return CachedW_GameMenu; }
	UW_Inventory* GetCachedInventory() const { return CachedW_Inventory; }
	UW_Equipment* GetCachedEquipment() const { return CachedW_Equipment; }
	UW_Crafting* GetCachedCrafting() const { return CachedW_Crafting; }
	UW_Status* GetCachedStatus() const { return CachedW_Status; }
	UW_Settings* GetCachedSettings() const { return CachedW_Settings; }
	UW_RestMenu* GetCachedRestMenu() const { return CachedW_RestMenu; }
	UW_Radar* GetCachedRadar() const { return CachedW_Radar; }

	// Death Screen
	UFUNCTION(BlueprintCallable, Category = "W_HUD|Death")
	void ShowDeathScreen();

	UFUNCTION(BlueprintCallable, Category = "W_HUD|Death")
	void HideDeathScreen();

	// Status Effect Widget Management
	UFUNCTION(BlueprintCallable, Category = "W_HUD|StatusEffects")
	void ClearStatusEffectWidgets();

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

	/** Internal handler bound to W_Settings::OnQuitRequested */
	UFUNCTION()
	void OnQuitRequestedHandler(bool Desktop);

	UFUNCTION()
	void OnRestMenuClosedHandler();

	/** Internal handler bound to W_RestMenu::OnStorageRequested */
	UFUNCTION()
	void OnStorageRequestedHandler();

	/** Internal handler bound to StatusEffectManagerComponent::OnStatusEffectAdded (takes UObject*) */
	UFUNCTION()
	void OnStatusEffectAddedFromComponent(UObject* StatusEffect);

	/** Internal handler bound to ItemWheel_Tools::OnItemWheelSlotSelected - sets active tool slot */
	UFUNCTION()
	void OnItemWheelToolsSlotSelected(FGameplayTag SlotTag);
};
