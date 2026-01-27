// B_Chaos_ForceField.h
// C++ class for Blueprint B_Chaos_ForceField - Chaos force field for destruction effects
//
// 20-PASS VALIDATION: 2026-01-17
// Source: BlueprintDNA_v2/Blueprint/B_Chaos_ForceField.json
// Parent: FieldSystemActor (NOT UObject!)
// Variables: 3 | Functions: 3 (EnableChaosDestroy, DisableChaosDestroy, Tick)

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemActor.h"
#include "Field/FieldSystemObjects.h"
#include "Interfaces/SLFDestructibleHelperInterface.h"
#include "B_Chaos_ForceField.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class USphereComponent;
class URadialForceComponent;

/**
 * Force field actor for Chaos physics destruction effects.
 * This actor is attached as a child actor component to B_Soulslike_Character.
 * Tick gets enabled/disabled through animation notifies via the BPI_DestructibleHelper interface.
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_Chaos_ForceField : public AFieldSystemActor, public ISLFDestructibleHelperInterface
{
	GENERATED_BODY()

public:
	AB_Chaos_ForceField();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (3)
	// ═══════════════════════════════════════════════════════════════════════

	/** Operator field input for combining field effects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TObjectPtr<UOperatorField> OperatorFIeld_Input;  // Note: Typo matches Blueprint

	/** Field volume static mesh for visualization */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UStaticMeshComponent> FieldVolume;

	/** Skeletal mesh component (unused, kept for compatibility) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	TObjectPtr<USkeletalMeshComponent> skel;

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS (Cached from Blueprint SCS at runtime)
	// NOTE: Named with "Cached" prefix to avoid collision with Blueprint SCS component names
	// ═══════════════════════════════════════════════════════════════════════

	/** Radial falloff field for distance-based effect falloff */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URadialFalloff> CachedRadialFalloff;

	/** Sphere collision for effect bounds */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CachedSphere;

	/** Radial force component for physics impulse */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URadialForceComponent> CachedRadialForce;

	/** Radial vector field for directional effects */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URadialVector> CachedRadialVector;

	/** Metadata filter for targeting specific actors */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFieldSystemMetaDataFilter> CachedFieldSystemMetaDataFilter;

	// ═══════════════════════════════════════════════════════════════════════
	// INTERFACE FUNCTIONS (ISLFDestructibleHelperInterface)
	// Only provide _Implementation - interface already declares UFUNCTION
	// ═══════════════════════════════════════════════════════════════════════

	/** Enable chaos destruction - enables actor tick */
	virtual void EnableChaosDestroy_Implementation() override;

	/** Disable chaos destruction - disables actor tick */
	virtual void DisableChaosDestroy_Implementation() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
