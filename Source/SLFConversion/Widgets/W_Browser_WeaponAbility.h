// W_Browser_WeaponAbility.h
// C++ Widget class for W_Browser_WeaponAbility
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Browser_WeaponAbility.json
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


#include "W_Browser_WeaponAbility.generated.h"

// Forward declarations for widget types
class UW_Browser_WeaponAbility_Tooltip;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FW_Browser_WeaponAbility_OnSelected, UW_Browser_WeaponAbility*, Entry, bool, Selected, bool, DoubleClick);

UCLASS()
class SLFCONVERSION_API UW_Browser_WeaponAbility : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Browser_WeaponAbility(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool Selected;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_Browser_WeaponAbility_Tooltip* Tooltip;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* Asset;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_Browser_WeaponAbility_OnSelected OnSelected;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_WeaponAbility")
	UWidget* GetToolTipWidget();
	virtual UWidget* GetToolTipWidget_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_WeaponAbility")
	FEventReply OnMouseDoubleClick_0(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	virtual FEventReply OnMouseDoubleClick_0_Implementation(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);


	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Browser_WeaponAbility")
	void EventClear();
	virtual void EventClear_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
