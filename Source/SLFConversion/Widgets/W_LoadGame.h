// W_LoadGame.h
// C++ Widget class for W_LoadGame
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_LoadGame.json
// Parent: UW_Navigable_InputReader
// Variables: 2 | Functions: 2 | Dispatchers: 1

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


#include "W_LoadGame.generated.h"

// Forward declarations for widget types
class UW_LoadGame_Entry;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_LoadGame_OnLoadGameClosed);

UCLASS()
class SLFCONVERSION_API UW_LoadGame : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_LoadGame(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_LoadGame_Entry*> LoadSlotEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_LoadGame_OnLoadGameClosed OnLoadGameClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void InitializeSaveSlots();
	virtual void InitializeSaveSlots_Implementation();


	// Event Handlers (6 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateDown();
	virtual void EventNavigateDown_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventNavigateUp();
	virtual void EventNavigateUp_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventOnSaveSlotSelected(UW_LoadGame_Entry* Card);
	virtual void EventOnSaveSlotSelected_Implementation(UW_LoadGame_Entry* Card);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LoadGame")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

protected:
	// Cache references
	void CacheWidgetReferences();
};
