// W_Utility_Asset.h
// C++ Widget class for W_Utility_Asset
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Utility_Asset.json
// Parent: UUserWidget
// Variables: 3 | Functions: 3 | Dispatchers: 1

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


#include "W_Utility_Asset.generated.h"

// Forward declarations for widget types
class UW_Utility_AssetTooltip;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Utility_Asset_OnDoubleClick);

UCLASS()
class SLFCONVERSION_API UW_Utility_Asset : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Utility_Asset(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_Utility_AssetTooltip* Tooltip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* Asset;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Utility_Asset_OnDoubleClick OnDoubleClick;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Utility_Asset")
	UWidget* GetToolTipWidget();
	virtual UWidget* GetToolTipWidget_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Utility_Asset")
	FEventReply OnMouseDoubleClick_0(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FEventReply OnMouseDoubleClick_0_Implementation(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Utility_Asset")
	void EventClear();
	virtual void EventClear_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
