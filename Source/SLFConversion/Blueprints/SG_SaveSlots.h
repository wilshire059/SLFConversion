// SG_SaveSlots.h
// C++ class for Blueprint SG_SaveSlots
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/SG_SaveSlots.json
// Parent: SaveGame -> UObject
// Variables: 2 | Functions: 3 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SG_SaveSlots.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USG_SaveSlots : public USaveGame
{
	GENERATED_BODY()

public:
	USG_SaveSlots();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (2)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TArray<FString> Slots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FString LastSavedSlot;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SaveSlots")
	void AddSlot(const FString& SlotName);
	virtual void AddSlot_Implementation(const FString& SlotName);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SaveSlots")
	FString GetLastSaveSlot();
	virtual FString GetLastSaveSlot_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SG_SaveSlots")
	TArray<FString> GetAllSlots();
	virtual TArray<FString> GetAllSlots_Implementation();
};
