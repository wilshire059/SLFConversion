// B_BaseProjectile.h
// C++ class for Blueprint B_BaseProjectile
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Updated 2026-01-28: Added ProjectileMovementComponent, NiagaraComponent, SphereComponent
// Source: BlueprintDNA/Blueprint/B_BaseProjectile.json
// Parent: Actor -> AActor
// Variables: 10 | Functions: 1 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
#include "Interfaces/SLFProjectileInterface.h"
#include "B_BaseProjectile.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UProjectileMovementComponent;
class UNiagaraComponent;
class USphereComponent;
class USceneComponent;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_BaseProjectile : public AActor, public ISLFProjectileInterface
{
	GENERATED_BODY()

public:
	AB_BaseProjectile();

	virtual void BeginPlay() override;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE IMPLEMENTATION (ISLFProjectileInterface)
	// ═══════════════════════════════════════════════════════════════════════

	virtual void InitializeProjectile_Implementation(AActor* TargetActor) override;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (cached from Blueprint SCS - see CLAUDE.md pattern)
	// ═══════════════════════════════════════════════════════════════════════

	/** Root component - cached from SCS */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	USceneComponent* DefaultSceneRoot;

	/** Projectile movement - cached from SCS */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	/** Niagara effect for trail/impact VFX - cached from SCS */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* Effect;

	/** Collision trigger sphere - cached from SCS */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	USphereComponent* Trigger;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (10)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damaging")
	FName TargetTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damaging")
	FGameplayTag NegationStat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damaging")
	double MinDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damaging")
	double MaxDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool IsHoming;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double HomingDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	double Lifespan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UNiagaraSystem* HitEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Damaging")
	TMap<FGameplayTag, UPrimaryDataAsset*> StatusEffects;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double DestroyDelay;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (1)
	// ═══════════════════════════════════════════════════════════════════════

protected:
	/** Cache component references from Blueprint SCS */
	void CacheComponentReferences();

	/** Handle trigger overlap for damage and effects */
	UFUNCTION()
	void OnTriggerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
