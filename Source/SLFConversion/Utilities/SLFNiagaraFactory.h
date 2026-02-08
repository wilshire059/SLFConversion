// SLFNiagaraFactory.h
// Utility class for creating Niagara systems programmatically in C++
//
// This demonstrates that C++ CAN create VFX from scratch, not just spawn existing assets.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "SLFNiagaraFactory.generated.h"

/**
 * Factory class for creating Niagara particle systems entirely in C++
 */
UCLASS()
class SLFCONVERSION_API USLFNiagaraFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Create a simple magic projectile effect from scratch
	 * This creates glowing particles that trail behind a projectile
	 *
	 * @param Color - Base color of the effect
	 * @param ParticleSize - Size of particles
	 * @param SpawnRate - Particles per second
	 * @return The created Niagara System (transient, not saved to disk)
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Niagara Factory")
	static UNiagaraSystem* CreateMagicProjectileEffect(
		FLinearColor Color = FLinearColor(0.5f, 0.2f, 1.0f, 1.0f),  // Purple default
		float ParticleSize = 15.0f,
		float SpawnRate = 50.0f);

	/**
	 * Create a hit/impact burst effect
	 * Spawns a burst of particles on impact
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Niagara Factory")
	static UNiagaraSystem* CreateImpactBurstEffect(
		FLinearColor Color = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f),  // Orange default
		float ParticleSize = 20.0f,
		int32 BurstCount = 30);

	/**
	 * Create a simple fire spell effect
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Niagara Factory")
	static UNiagaraSystem* CreateFireSpellEffect();

	/**
	 * Create a healing/holy spell effect
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Niagara Factory")
	static UNiagaraSystem* CreateHealingSpellEffect();

	/**
	 * Apply a generated effect to a Niagara Component
	 */
	UFUNCTION(BlueprintCallable, Category = "SLF|Niagara Factory")
	static void ApplyGeneratedEffect(UNiagaraComponent* Component, UNiagaraSystem* System);
};
