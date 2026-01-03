// W_CharacterSelectionCard.h
// C++ Widget class for W_CharacterSelectionCard
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_CharacterSelectionCard.json
// Parent: UUserWidget
// Variables: 2 | Functions: 2 | Dispatchers: 2

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_CharacterSelectionCard.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


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
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* CharacterClassAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelectionCard_OnCardSelected OnCardSelected;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_CharacterSelectionCard_OnCardClicked OnCardClicked;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelectionCard")
	void SetCardSelected(bool InSelected);
	virtual void SetCardSelected_Implementation(bool InSelected);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelectionCard")
	void InitializeStatEntries(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& InputPin);
	virtual void InitializeStatEntries_Implementation(const TMap<FGameplayTag, TSubclassOf<UB_Stat>>& InputPin);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_CharacterSelectionCard")
	void EventOnCardPressed();
	virtual void EventOnCardPressed_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
