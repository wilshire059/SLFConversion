// W_ItemWheel_NextSlot.h
// C++ Widget class for W_ItemWheel_NextSlot
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_ItemWheel_NextSlot.json
// Parent: UUserWidget
// Variables: 3 | Functions: 1 | Dispatchers: 0

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


#include "W_ItemWheel_NextSlot.generated.h"

// Forward declarations for widget types
class UW_ItemWheelSlot;
class UImage;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_ItemWheel_NextSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_ItemWheel_NextSlot(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FVector2D SizingInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_ItemWheelSlot* ParentWheelSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* ActiveItem;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheel_NextSlot")
	void HandleChildItem(UPrimaryDataAsset* InItem);
	virtual void HandleChildItem_Implementation(UPrimaryDataAsset* InItem);


	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheel_NextSlot")
	void EventOnItemAmountUpdated(UPDA_Item* InItem, int32 NewCount);
	virtual void EventOnItemAmountUpdated_Implementation(UPDA_Item* InItem, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_ItemWheel_NextSlot")
	void EventPlayStanceFadeAnimation(bool Pressed);
	virtual void EventPlayStanceFadeAnimation_Implementation(bool Pressed);

protected:
	// Cache references
	void CacheWidgetReferences();

	// Cached UI widget references (prefixed to avoid Blueprint name conflicts)
	UPROPERTY(Transient)
	TObjectPtr<UImage> CachedItemIcon;
};
