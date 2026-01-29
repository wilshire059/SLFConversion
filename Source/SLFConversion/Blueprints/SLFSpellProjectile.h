// SLFSpellProjectile.h
// Spell projectile with blackhole visual effect - dark core with purple swirling particles
//
// This demonstrates creating VFX entirely in C++ without editor assets.

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFProjectileBase.h"
#include "SLFSpellProjectile.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UPointLightComponent;

UENUM(BlueprintType)
enum class ESLFSpellElement : uint8
{
	Fire		UMETA(DisplayName = "Fire"),
	Ice			UMETA(DisplayName = "Ice"),
	Lightning	UMETA(DisplayName = "Lightning"),
	Holy		UMETA(DisplayName = "Holy"),
	Dark		UMETA(DisplayName = "Dark"),
	Arcane		UMETA(DisplayName = "Arcane")
};

/**
 * Spell projectile with blackhole visual effect
 * Creates a dark core sphere with swirling purple particles and trail
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSpellProjectile : public ASLFProjectileBase
{
	GENERATED_BODY()

public:
	ASLFSpellProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ═══════════════════════════════════════════════════════════════════
	// BLACKHOLE COMPONENTS
	// ═══════════════════════════════════════════════════════════════════

	/** Dark sphere core mesh - the "blackhole" center */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CoreMesh;

	/** Outer glow/corona mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GlowMesh;

	/** Point light for purple glow */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPointLightComponent* CoreLight;

	// ═══════════════════════════════════════════════════════════════════
	// SPELL CONFIGURATION
	// ═══════════════════════════════════════════════════════════════════

	/** Element type determines color and behavior */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	ESLFSpellElement SpellElement;

	/** FP cost to cast this spell */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FPCost;

	/** Intelligence scaling (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float IntelligenceScaling;

	/** Faith scaling (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell")
	float FaithScaling;

	// ═══════════════════════════════════════════════════════════════════
	// BLACKHOLE VFX SETTINGS
	// ═══════════════════════════════════════════════════════════════════

	/** Core sphere radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	float CoreRadius;

	/** Outer glow radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	float GlowRadius;

	/** Core color (dark purple/black) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	FLinearColor CoreColor;

	/** Glow/trail color (purple) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	FLinearColor GlowColor;

	/** Emissive intensity for glow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	float EmissiveIntensity;

	/** Core rotation speed (degrees per second) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	float CoreRotationSpeed;

	/** Glow pulse speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	float PulseSpeed;

	/** Glow pulse intensity range */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Blackhole")
	float PulseIntensity;

	// ═══════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════

	/** Setup the blackhole visual effect */
	UFUNCTION(BlueprintCallable, Category = "Spell")
	void SetupBlackholeEffect();

	/** Get color for spell element */
	UFUNCTION(BlueprintPure, Category = "Spell")
	static FLinearColor GetElementColor(ESLFSpellElement Element);

protected:
	/** Dynamic material for core */
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* CoreMaterial;

	/** Dynamic material for glow */
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* GlowMaterial;

	/** Accumulated time for animations */
	float AccumulatedTime;

	/** Update pulsing glow effect */
	void UpdatePulseEffect(float DeltaTime);
};
