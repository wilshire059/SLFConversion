// W_CharacterSelection.h
// C++ Widget class for W_CharacterSelection
//
// Source: BlueprintDNA/WidgetBlueprint/W_CharacterSelection.json
// Parent: UW_Navigable_InputReader

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFPrimaryDataAssets.h"

#include "W_CharacterSelection.generated.h"

// Forward declarations
class UW_CharacterSelectionCard;
class UScrollBox;

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
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// The base class to scan for in the AssetRegistry (set in Blueprint editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TSubclassOf<UObject> PrimaryClassAsset;

	// Populated by InitializeAndStoreClassAssets - all character info data assets
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<UPrimaryDataAsset*> BaseCharacterAssets;

	// Card widgets created for each character
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	TArray<UW_CharacterSelectionCard*> CharacterCardEntries;

	// Current selected card index
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	int32 NavigationIndex;

	// ═══════════════════════════════════════════════════════════════════════
	// BIND WIDGETS (UMG Designer References)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UScrollBox* ClassesScrollBox;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelection_OnClassCardClicked OnClassCardClicked;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelection_OnCharacterSelectionClosed OnCharacterSelectionClosed;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Scan AssetRegistry for all PDA_BaseCharacterInfo assets
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelection")
	void InitializeAndStoreClassAssets();
	virtual void InitializeAndStoreClassAssets_Implementation();

	// Navigation events
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
	// Create card widgets and populate ClassesScrollBox
	void CreateCardWidgets();

	// Update card selection visuals
	void UpdateCardSelection();

	// Card delegate handlers
	UFUNCTION()
	void OnCardClickedHandler(UPrimaryDataAsset* ClassAsset);

	UFUNCTION()
	void OnCardSelectedHandler(UW_CharacterSelectionCard* Card);
};
