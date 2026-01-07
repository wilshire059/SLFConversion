// SG_SoulslikeFramework.h
// C++ class for Blueprint SG_SoulslikeFramework
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/SG_SoulslikeFramework.json
// Parent: SaveGame -> UObject
// Variables: 1 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SLFGameTypes.h"
#include "SG_SoulslikeFramework.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USG_SoulslikeFramework : public USaveGame
{
	GENERATED_BODY()

public:
	USG_SoulslikeFramework();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FSLFSaveGameInfo SavedData;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SoulslikeFramework")
	void SetSavedData(const FSLFSaveGameInfo& InSavedData);
	virtual void SetSavedData_Implementation(const FSLFSaveGameInfo& InSavedData);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SoulslikeFramework")
	FSLFSaveGameInfo GetSavedData();
	virtual FSLFSaveGameInfo GetSavedData_Implementation();
};
