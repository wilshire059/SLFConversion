// W_Browser_Animset.h
// C++ Widget class for W_Browser_Animset
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Browser_Animset.json
// Parent: UUserWidget
// Variables: 4 | Functions: 4 | Dispatchers: 1

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


#include "W_Browser_Animset.generated.h"

// Forward declarations for widget types
class UW_Browser_AnimsetTooltip;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FW_Browser_Animset_OnSelected, UW_Browser_Animset*, Entry, bool, Selected, bool, DoubleClick);

UCLASS()
class SLFCONVERSION_API UW_Browser_Animset : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Browser_Animset(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_Browser_AnimsetTooltip* Tooltip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* Asset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TArray<UPrimaryDataAsset*> AllItems;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Browser_Animset_OnSelected OnSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (4)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_Animset")
	UWidget* GetToolTipWidget();
	virtual UWidget* GetToolTipWidget_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_Animset")
	FEventReply OnMouseDoubleClick_0(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FEventReply OnMouseDoubleClick_0_Implementation(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_Animset")
	TArray<UPrimaryDataAsset*> GetUsedByItems();
	virtual TArray<UPrimaryDataAsset*> GetUsedByItems_Implementation();


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_Animset")
	void EventClear();
	virtual void EventClear_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
