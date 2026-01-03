// B_BaseCharacter.h
// C++ class for Blueprint B_BaseCharacter
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_BaseCharacter.json
// Parent: Character -> ACharacter
// Variables: 3 | Functions: 1 | Dispatchers: 3

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
#include "Interfaces/SLFGenericCharacterInterface.h"
#include "B_BaseCharacter.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_BaseCharacter_OnLocationLerpEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_BaseCharacter_OnRotationLerpEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_BaseCharacter_OnLocationRotationLerpEnd);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_BaseCharacter : public ACharacter, public ISLFGenericCharacterInterface
{
	GENERATED_BODY()

public:
	AB_BaseCharacter();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	double IK_Weight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	FRotator Cache_Rotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Caches")
	FVector Cache_Location;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (3)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_BaseCharacter_OnLocationLerpEnd OnLocationLerpEnd;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_BaseCharacter_OnRotationLerpEnd OnRotationLerpEnd;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_BaseCharacter_OnLocationRotationLerpEnd OnLocationRotationLerpEnd;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

};
