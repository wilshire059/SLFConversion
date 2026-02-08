// W_CharacterSelectionCard.h
// C++ Widget class for W_CharacterSelectionCard
//
// Source: BlueprintDNA/WidgetBlueprint/W_CharacterSelectionCard.json
// Parent: UUserWidget

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLFPrimaryDataAssets.h"

#include "W_CharacterSelectionCard.generated.h"

// Forward declarations
class UTextBlock;
class UImage;
class UBorder;
class UButton;
class UVerticalBox;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_CharacterSelectionCard_OnCardSelected, UW_CharacterSelectionCard*, Card);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_CharacterSelectionCard_OnCardClicked, UPrimaryDataAsset*, ClassAsset);

UCLASS()
class SLFCONVERSION_API UW_CharacterSelectionCard : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_CharacterSelectionCard(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// The character class this card represents (set by parent before AddChild)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* CharacterClassAsset;

	// Whether this card is currently selected
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// BIND WIDGETS (UMG Designer References)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UBorder* SelectionBorder;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UTextBlock* ClassNameText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UImage* CharacterImage;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UButton* CardBtn;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	UVerticalBox* StatBox;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelectionCard_OnCardSelected OnCardSelected;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelectionCard_OnCardClicked OnCardClicked;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Toggle selection state and highlight border
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelectionCard")
	void SetCardSelected(bool InSelected);
	virtual void SetCardSelected_Implementation(bool InSelected);

	// Initialize stat display entries
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelectionCard")
	void InitializeStatEntries(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& InputPin);
	virtual void InitializeStatEntries_Implementation(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& InputPin);

	// Called when card is confirmed (broadcasts OnCardClicked)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelectionCard")
	void EventOnCardPressed();
	virtual void EventOnCardPressed_Implementation();

protected:
	UFUNCTION()
	void OnCardBtnClicked();

	// Display character name from CharacterClassAsset
	void PopulateCardDisplay();
};
