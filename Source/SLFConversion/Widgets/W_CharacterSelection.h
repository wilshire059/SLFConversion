// W_CharacterSelection.h
// C++ Widget class for W_CharacterSelection
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_CharacterSelection.json
// Parent: UW_Navigable_InputReader
// Variables: 4 | Functions: 2 | Dispatchers: 2

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


#include "W_CharacterSelection.generated.h"

// Forward declarations for widget types
class UW_CharacterSelectionCard;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_CharacterSelection_OnClassCardClicked, UPrimaryDataAsset*, ClassAsset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_CharacterSelection_OnCharacterSelectionClosed);

UCLASS()
class SLFCONVERSION_API UW_CharacterSelection : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_CharacterSelection(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSubclassOf<UObject> PrimaryClassAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UPrimaryDataAsset*> BaseCharacterAssets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_CharacterSelectionCard*> CharacterCardEntries;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelection_OnClassCardClicked OnClassCardClicked;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelection_OnCharacterSelectionClosed OnCharacterSelectionClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void InitializeAndStoreClassAssets();
	virtual void InitializeAndStoreClassAssets_Implementation();


	// Event Handlers (7 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventNavigateCancel();
	virtual void EventNavigateCancel_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventNavigateLeft();
	virtual void EventNavigateLeft_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventNavigateOk();
	virtual void EventNavigateOk_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventNavigateRight();
	virtual void EventNavigateRight_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventOnCardPressed(UPDA_BaseCharacterInfo* ClassAsset);
	virtual void EventOnCardPressed_Implementation(UPDA_BaseCharacterInfo* ClassAsset);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventOnCardSelected(UW_CharacterSelectionCard* Card);
	virtual void EventOnCardSelected_Implementation(UW_CharacterSelectionCard* Card);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

protected:
	// Cache references
	void CacheWidgetReferences();
};
