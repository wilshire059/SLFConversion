// W_NPC_Window.h
// C++ Widget class for W_NPC_Window
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_NPC_Window.json
// Parent: UW_Navigable_InputReader
// Variables: 5 | Functions: 1 | Dispatchers: 3

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"
#include "Components/Button.h"

#include "W_NPC_Window.generated.h"

// Forward declarations for widget types
class UW_RestMenu_Button;

// Forward declarations for Blueprint types
class UAC_AI_InteractionManager;

// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_NPC_Window_OnNpcWindowActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_NPC_Window_OnNpcWindowClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_NPC_Window_OnNpcWindowTalkInitiated);

UCLASS()
class SLFCONVERSION_API UW_NPC_Window : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_NPC_Window(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_RestMenu_Button*> NpcMenuButtons;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UAC_AI_InteractionManager* ActiveDialogComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* VendorAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FText NpcName;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_NPC_Window_OnNpcWindowActive OnNpcWindowActive;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_NPC_Window_OnNpcWindowClosed OnNpcWindowClosed;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_NPC_Window_OnNpcWindowTalkInitiated OnNpcWindowTalkInitiated;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (7 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventCloseNpcMenu();
	virtual void EventCloseNpcMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventInitializeNpcWindow(const FText& InName, UPDA_Vendor* InVendorAsset, UAC_AI_InteractionManager* DialogComponent);
	virtual void EventInitializeNpcWindow_Implementation(const FText& InName, UPDA_Vendor* InVendorAsset, UAC_AI_InteractionManager* DialogComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_NPC_Window")
	void EventOnButtonSelected(UW_RestMenu_Button* Button);
	virtual void EventOnButtonSelected_Implementation(UW_RestMenu_Button* Button);

protected:
	// Cache references
	void CacheWidgetReferences();
};
