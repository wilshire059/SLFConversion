// W_Browser_Animset_OwnerEntry.h
// C++ Widget class for W_Browser_Animset_OwnerEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/WidgetBlueprint/W_Browser_Animset_OwnerEntry.json
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


#include "W_Browser_Animset_OwnerEntry.generated.h"

// Forward declarations for widget types


// Forward declarations for Blueprint types


// Forward declarations for SaveGame types


// Event Dispatchers


UCLASS()
class SLFCONVERSION_API UW_Browser_Animset_OwnerEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UW_Browser_Animset_OwnerEntry(const FObjectInitializer& ObjectInitializer);

	// Widget lifecycle
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UPrimaryDataAsset* Item;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════


protected:
	// Cache references
	void CacheWidgetReferences();
};
