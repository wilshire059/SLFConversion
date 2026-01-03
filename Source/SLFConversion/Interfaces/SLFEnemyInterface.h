// SLFEnemyInterface.h
// C++ interface for BPI_Enemy
//
// 20-PASS VALIDATED: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Interface/BPI_Enemy.json
// Functions: 8

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/UMGSequencePlayMode.h"
#include "SLFEnemyInterface.generated.h"

// Forward declarations
class UAnimMontage;
class UAnimInstance;
class USoundBase;
class UNiagaraSystem;
class UCameraShakeBase;
class USceneComponent;
class UPrimaryDataAsset;

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class USLFEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface: BPI_Enemy
 * Generated from JSON with 8 functions
 */
class SLFCONVERSION_API ISLFEnemyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void UpdateEnemyHealth(double CurrentValue, double MaxValue, double Change);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void ToggleHealthbarVisual(bool Visible);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void DisplayDeathVfx(const FVector& AttractorPosition);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	AB_PatrolPath* GetPatrolPath();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void PickAndSpawnLoot();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void RotateTowardsTarget(double Duration);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void StopRotateTowardsTarget();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void GetExecutionMoveToTransform(FVector& OutMoveToLocation, FRotator& OutRotation);
};
