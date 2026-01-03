// W_Utility_Component.h
// C++ Widget class for W_Utility_Component
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Utility_Component.json
// Parent: UUserWidget
// Variables: 2 | Functions: 3 | Dispatchers: 2

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


#include "W_Utility_Component.generated.h"

// Forward declarations for widget types
class UW_Utility_AssetTooltip;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_Utility_Component_OnDoubleClick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FW_Utility_Component_OnClicked, UActorComponent*, Component);

UCLASS()
class SLFCONVERSION_API UW_Utility_Component : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Utility_Component(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_Utility_AssetTooltip* Tooltip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UActorComponent* Component;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Utility_Component_OnDoubleClick OnDoubleClick;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Utility_Component_OnClicked OnClicked;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Utility_Component")
	UWidget* GetToolTipWidget();
	virtual UWidget* GetToolTipWidget_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Utility_Component")
	FEventReply OnMouseDoubleClick_0(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FEventReply OnMouseDoubleClick_0_Implementation(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

protected:
	// Cache references
	void CacheWidgetReferences();
};
