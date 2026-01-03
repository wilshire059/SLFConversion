// SLFProjectileBase.h
// C++ base class for B_BaseProjectile
//
// ═══════════════════════════════════════════════════════════════════════════════
// MIGRATION SUMMARY - B_BaseProjectile
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         10/10 migrated
// Functions:         1/1 migrated (UserConstructionScript)
// Event Dispatchers: 0/0
// Graphs:            2 (logic in functions)
// ═══════════════════════════════════════════════════════════════════════════════
//
// Original Blueprint: /Game/SoulslikeFramework/Blueprints/_WorldActors/B_BaseProjectile
//
// PURPOSE: Base projectile actor - ranged attacks with homing, damage, effects
// CHILDREN: B_Projectile_ThrowingKnife, B_Projectile_Boss_Fireball, etc.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFProjectileBase.generated.h"

// Forward declarations
class UNiagaraSystem;

/**
 * Base projectile actor - ranged attack with collision, damage, and effects
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	ASLFProjectileBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// VARIABLES: 10/10 migrated
	// ═══════════════════════════════════════════════════════════════════

	// --- Targeting (2) ---

	/** [1/10] Tag that identifies valid targets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Targeting")
	FName TargetTag;

	/** [2/10] Stat that negates damage (for resistance calculation) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Targeting")
	FGameplayTag NegationStat;

	// --- Damage (2) ---

	/** [3/10] Minimum damage on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Damage")
	float MinDamage;

	/** [4/10] Maximum damage on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Damage")
	float MaxDamage;

	// --- Homing (2) ---

	/** [5/10] Whether projectile homes in on target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Homing")
	bool bIsHoming;

	/** [6/10] Duration of homing behavior */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Homing")
	float HomingDuration;

	/** [6.1/10] Homing strength (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Homing")
	float HomingStrength = 0.0f;

	// --- Movement ---

	/** Projectile speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Movement")
	float ProjectileSpeed = 2000.0f;

	// --- Lifetime (2) ---

	/** [7/10] Projectile lifespan in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Lifetime")
	float Lifespan;

	/** [8/10] Delay before destroying after hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Lifetime")
	float DestroyDelay;

	// --- Effects (2) ---

	/** [9/10] VFX on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Effects")
	UNiagaraSystem* HitEffect;

	/** [10/10] Status effects to apply on hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Effects")
	TArray<FGameplayTag> StatusEffects;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS: 1/1 migrated + additional helpers
	// ═══════════════════════════════════════════════════════════════════

	/** [1/1] Setup projectile (called from construction) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void SetupProjectile();
	virtual void SetupProjectile_Implementation();

	// --- Lifecycle ---

	/** Called when projectile hits something */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void OnProjectileHit(AActor* HitActor, const FHitResult& HitResult);
	virtual void OnProjectileHit_Implementation(AActor* HitActor, const FHitResult& HitResult);

	/** Launch projectile in direction */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void Launch(FVector Direction, float Speed);
	virtual void Launch_Implementation(FVector Direction, float Speed);

	/** Set homing target */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	void SetHomingTarget(AActor* Target);
	virtual void SetHomingTarget_Implementation(AActor* Target);

	// --- Damage Calculation ---

	/** Calculate damage to apply */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Projectile")
	float CalculateDamage();
	virtual float CalculateDamage_Implementation();

protected:
	/** Current homing target */
	UPROPERTY(BlueprintReadWrite, Category = "Projectile|Runtime")
	AActor* HomingTarget;

	/** Time remaining for homing */
	UPROPERTY(BlueprintReadWrite, Category = "Projectile|Runtime")
	float RemainingHomingTime;
};
