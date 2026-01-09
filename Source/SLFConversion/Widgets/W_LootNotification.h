// W_LootNotification.h
// C++ Widget class for W_LootNotification
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_LootNotification.json
// Parent: UUserWidget
// Variables: 5 | Functions: 0 | Dispatchers: 0

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


#include "W_LootNotification.generated.h"

// Forward declarations for widget types
class UWidgetAnimation;


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_LootNotification : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_LootNotification(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (5)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 MaxItemNameLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FText ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 ItemLootedAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TSoftObjectPtr<UTexture2D> ItemTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	double Duration;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (1 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_LootNotification")
	void EventFinished();
	virtual void EventFinished_Implementation();

protected:
	// Cache references
	void CacheWidgetReferences();

	// Timer handle for auto-removal
	FTimerHandle RemovalTimerHandle;

	// Called when fade out animation completes
	UFUNCTION()
	void OnFadeOutComplete();

	// Helper to find widget animation by name
	UWidgetAnimation* FindAnimationByName(const FName& AnimName) const;
};
