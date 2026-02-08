// W_Settings_PlayerCard.h
// C++ Widget class for W_Settings_PlayerCard
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Settings_PlayerCard.json
// Parent: UUserWidget
// Variables: 1 | Functions: 0 | Dispatchers: 0

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


#include "W_Settings_PlayerCard.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Settings_PlayerCard : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Settings_PlayerCard(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	int32 PlayerLevel;

	// ═══════════════════════════════════════════════════════════════════════
	// WIDGET BINDINGS (from WidgetTree - IsVariable=true)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	class UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	class UTextBlock* PlayTimeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	class UTextBlock* ZoneNameText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Widgets")
	class UTextBlock* PlayerClassText;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// Event Handlers (2 events)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_PlayerCard")
	void EventOnLevelUpdated(int32 NewLevel);
	virtual void EventOnLevelUpdated_Implementation(int32 NewLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_PlayerCard")
	void EventOnPlayTimeUpdated(FTimespan NewTime);
	virtual void EventOnPlayTimeUpdated_Implementation(FTimespan NewTime);

protected:
	// Cache references
	void CacheWidgetReferences();
};
