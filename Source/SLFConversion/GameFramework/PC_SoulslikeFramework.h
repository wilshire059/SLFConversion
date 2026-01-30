// PC_SoulslikeFramework.h
// C++ Game Framework class for PC_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation
// Source: BlueprintDNA/GameFramework/PC_SoulslikeFramework.json
// Parent: APlayerController
// Variables: 6 | Functions: 22 | Dispatchers: 2 | Components: 5

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Interfaces/BPI_Controller.h"
#include "PC_SoulslikeFramework.generated.h"

// Forward declarations
class UW_HUD;
class ULevelSequencePlayer;
class UInventoryManagerComponent;
class UEquipmentManagerComponent;
class USaveLoadManagerComponent;
class URadarManagerComponent;
class UProgressManagerComponent;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPC_OnInputDetected, bool, bIsGamepad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPC_OnHudInitialized);

UCLASS()
class SLFCONVERSION_API APC_SoulslikeFramework : public APlayerController, public IBPI_Controller
{
	GENERATED_BODY()

public:
	APC_SoulslikeFramework();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Navigation input handlers
	void HandleNavigateUp();
	void HandleNavigateDown();
	void HandleNavigateLeft();
	void HandleNavigateRight();
	void HandleNavigateOk();
	void HandleNavigateBack();
	void HandleNavigateCategoryLeft();
	void HandleNavigateCategoryRight();
	void HandleNavigateUnequip();
	void HandleNavigateDetailedView();
	void HandleNavigateResetToDefaults();

public:
	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES [6/6]
	// ═══════════════════════════════════════════════════════════════════════

	/** HUD widget reference */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	UW_HUD* W_HUD;

	/** Gameplay input mapping context */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* GameplayMappingContext;

	/** Navigable menu input mapping context (for UI navigation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* NavigableMenuMappingContext;

	// ═══════════════════════════════════════════════════════════════════════
	// NAVIGATION INPUT ACTIONS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Down;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Ok;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Back;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Left_Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Right_Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_Unequip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_DetailedView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Navigation")
	UInputAction* IA_NavigableMenu_ResetToDefaults;

	/** Active level sequence player for cinematics */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
	ULevelSequencePlayer* ActiveSequencePlayer;

	/** Tag of currently active navigable widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime|Navigation")
	FGameplayTag ActiveWidgetTag;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS [5/5]
	// ═══════════════════════════════════════════════════════════════════════

	/** Inventory manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryManagerComponent* AC_InventoryManager;

	/** Equipment manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEquipmentManagerComponent* AC_EquipmentManager;

	/** Save/Load manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USaveLoadManagerComponent* AC_SaveLoadManager;

	/** Radar manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URadarManagerComponent* AC_RadarManager;

	/** Progress manager component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProgressManagerComponent* AC_ProgressManager;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS [2/2]
	// ═══════════════════════════════════════════════════════════════════════

	/** Broadcast when input type changes (gamepad vs keyboard/mouse) */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FPC_OnInputDetected OnInputDetected;

	/** Broadcast when HUD is initialized */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FPC_OnHudInitialized OnHudInitialized;

	// ═══════════════════════════════════════════════════════════════════════
	// LOCAL FUNCTIONS [2/2]
	// ═══════════════════════════════════════════════════════════════════════

	/** Handle ESC/Start button - toggle game menu or close dialogs */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PC_SoulslikeFramework")
	void HandleMainMenuRequest();
	virtual void HandleMainMenuRequest_Implementation();

	/** Find the nearest resting point to the player */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PC_SoulslikeFramework")
	void GetNearestRestingPoint(bool& OutSuccess, AActor*& OutPoint);
	virtual void GetNearestRestingPoint_Implementation(bool& OutSuccess, AActor*& OutPoint);

	/** Initialize radar by connecting W_Radar widget to RadarManager */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PC_SoulslikeFramework")
	void EventInitializeRadar();
	virtual void EventInitializeRadar_Implementation();

	// ═══════════════════════════════════════════════════════════════════════
	// BPI_Controller INTERFACE IMPLEMENTATIONS [20/20]
	// ═══════════════════════════════════════════════════════════════════════

	// --- Save Functions ---
	virtual void RequestAddToSaveData_Implementation(FGameplayTag SaveTag, const FInstancedStruct& Data) override;
	virtual void RequestUpdateSaveData_Implementation(FGameplayTag SaveTag, TArray<FInstancedStruct>& Data) override;
	virtual void SerializeDataForSaving_Implementation(ESLFSaveBehavior Behavior, FGameplayTag SaveTag) override;
	virtual void SerializeAllDataForSaving_Implementation(ESLFSaveBehavior Behavior) override;

	// --- UI Functions ---
	virtual void SetActiveWidgetForNavigation_Implementation(FGameplayTag NavigableWidgetTag) override;
	virtual void SendBigScreenMessage_Implementation(const FText& Text, UMaterialInterface* GradientMaterial, bool bBackdrop, double PlayRate) override;
	virtual void ShowSavingVisual_Implementation(double Length) override;
	virtual void ToggleRadarUpdateState_Implementation(bool bUpdateEnabled) override;
	virtual void GetPlayerHUD_Implementation(UUserWidget*& HUD) override;

	// --- Input Functions ---
	virtual void SwitchInputContext_Implementation(TArray<UInputMappingContext*>& ContextsToEnable, TArray<UInputMappingContext*>& ContextsToDisable) override;
	virtual void ToggleInput_Implementation(bool bEnabled) override;

	// --- Getter Functions ---
	virtual void GetCurrency_Implementation(int32& Currency) override;
	virtual void GetProgressManager_Implementation(UActorComponent*& ProgressManager) override;
	virtual void GetSoulslikeController_Implementation(APlayerController*& Controller) override;
	virtual void GetPawnFromController_Implementation(APawn*& OutPawn) override;
	virtual void GetInventoryComponent_Implementation(UActorComponent*& Inventory) override;

	// --- Loot Functions ---
	virtual void AdjustCurrency_Implementation(int32 Delta) override;
	virtual void LootItemToInventory_Implementation(AActor* Item) override;

	// --- Other Functions ---
	virtual void StartRespawn_Implementation(double FadeDelay) override;
	virtual void BlendViewTarget_Implementation(AActor* TargetActor, double BlendTime, double BlendExp, bool bLockOutgoing) override;

	// ═══════════════════════════════════════════════════════════════════════
	// TEST/DEBUG FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	/** Spawn a test spell projectile with C++ generated VFX */
	UFUNCTION(Exec, BlueprintCallable, Category = "Debug")
	void SpawnTestSpell();
};
