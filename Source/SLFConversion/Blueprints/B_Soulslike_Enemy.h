// B_Soulslike_Enemy.h
// C++ class for Blueprint B_Soulslike_Enemy
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Soulslike_Enemy.json
// Parent: B_BaseCharacter_C -> AB_BaseCharacter
// Variables: 1 | Functions: 2 | Dispatchers: 1

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
#include "Interfaces/SLFEnemyInterface.h"
#include "Interfaces/SLFExecutableInterface.h"
#include "B_Soulslike_Enemy.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_Soulslike_Enemy_OnAttackEnd);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Soulslike_Enemy : public AB_BaseCharacter, public ISLFEnemyInterface, public ISLFExecutableInterface
{
	GENERATED_BODY()

public:
	AB_Soulslike_Enemy();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saving (Generate GUID for Saving)")
	FGuid EnemyId;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (1)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_Soulslike_Enemy_OnAttackEnd OnAttackEnd;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (2)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_Soulslike_Enemy")
	void CheckSense(UAIPerceptionComponent* AiPerceptionComponent, AActor* Actor, ESLFAISenses Sense, bool& OutReturnValue, FAIStimulus& OutStimulus);
	virtual void CheckSense_Implementation(UAIPerceptionComponent* AiPerceptionComponent, AActor* Actor, ESLFAISenses Sense, bool& OutReturnValue, FAIStimulus& OutStimulus);

	// Called by AI to perform currently selected ability
	// This is a BlueprintImplementableEvent because the actual logic will remain in Blueprint
	// until B_Soulslike_Enemy EventGraph is fully migrated
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "B_Soulslike_Enemy|AI")
	void PerformAbility();
};
