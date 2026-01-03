// W_Debug_HUD.h
// C++ Widget class for W_Debug_HUD
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Debug_HUD.json
// Parent: UUserWidget
// Variables: 2 | Functions: 1 | Dispatchers: 0

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


#include "W_Debug_HUD.generated.h"

// Forward declarations for widget types
class UW_DebugWindow;

// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Debug_HUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Debug_HUD(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TMap<FGameplayTag, FGameplayTag> Components;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<UW_DebugWindow*> Windows;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Debug_HUD")
	UW_DebugWindow* FindWindowForComponentWithTag(const FGameplayTag& Tag);
	virtual UW_DebugWindow* FindWindowForComponentWithTag_Implementation(const FGameplayTag& Tag);


	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Debug_HUD")
	void EventInitializeText(const FKey& InKey);
	virtual void EventInitializeText_Implementation(const FKey& InKey);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Debug_HUD")
	void EventOnComponentClicked(FGameplayTag Tag);
	virtual void EventOnComponentClicked_Implementation(FGameplayTag Tag);

protected:
	// Cache references
	void CacheWidgetReferences();
};
