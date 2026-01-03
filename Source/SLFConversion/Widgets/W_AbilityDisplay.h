// W_AbilityDisplay.h
// C++ Widget class for W_AbilityDisplay
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_AbilityDisplay.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 1

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


#include "W_AbilityDisplay.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FW_AbilityDisplay_OnAbilityInfoValidated);

UCLASS()
class SLFCONVERSION_API UW_AbilityDisplay : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_AbilityDisplay(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* ActiveAbility;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FW_AbilityDisplay_OnAbilityInfoValidated OnAbilityInfoValidated;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_AbilityDisplay")
	void EventOnAbilityInfoReceived(UPDA_WeaponAbility* Ability);
	virtual void EventOnAbilityInfoReceived_Implementation(UPDA_WeaponAbility* Ability);

protected:
	// Cache references
	void CacheWidgetReferences();
};
