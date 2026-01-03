// W_Interaction.h
// C++ Widget class for W_Interaction
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Interaction.json
// Parent: UW_Navigable_InputReader
// Variables: 2 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_Navigable_InputReader.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericWindow.h"
#include "MediaPlayer.h"


#include "W_Interaction.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Interaction : public UW_Navigable_InputReader
{
	GENERATED_BODY()

public:
	UW_Interaction(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FVector2D InputIconSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UInputMappingContext* InputMapping_0;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (5 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Interaction")
	void EventHide();
	virtual void EventHide_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Interaction")
	void EventOnInteractableOverlap(const FText& InteractionText);
	virtual void EventOnInteractableOverlap_Implementation(const FText& InteractionText);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Interaction")
	void EventOnItemOverlap(FSLFItemInfo ItemInfo);
	virtual void EventOnItemOverlap_Implementation(FSLFItemInfo ItemInfo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Interaction")
	void EventOnVisibilityChanged(uint8 InVisibility);
	virtual void EventOnVisibilityChanged_Implementation(uint8 InVisibility);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Interaction")
	void EventShow();
	virtual void EventShow_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();
};
