// B_Soulslike_NPC.h
// C++ class for Blueprint B_Soulslike_NPC
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Soulslike_NPC.json
// Parent: B_BaseCharacter_C -> AB_BaseCharacter
// Variables: 3 | Functions: 2 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/B_BaseCharacter.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "Interfaces/SLFNPCInterface.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "B_Soulslike_NPC.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_NPC : public AB_BaseCharacter, public ISLFNPCInterface, public ISLFInteractableInterface
{
	GENERATED_BODY()

public:
	AB_Soulslike_NPC();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool CanBeTraced;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPrimitiveComponent* LookAtComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving (Generate GUID for Saving)")
	FGuid NpcId;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_NPC")
	void GetLookAtLocation(FVector& OutLocation, FVector& OutLocation1);
	virtual void GetLookAtLocation_Implementation(FVector& OutLocation, FVector& OutLocation1);
};
