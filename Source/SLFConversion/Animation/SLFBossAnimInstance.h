// SLFBossAnimInstance.h
// C++ base class for ABP_SoulslikeBossNew
//
// Provides all animation-relevant properties that the AnimGraph needs.
// The AnimGraph (state machines, blends) remains in Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLFBossAnimInstance.generated.h"

// Forward declarations
class UCharacterMovementComponent;
class UAICombatManagerComponent;
class UDataAsset;
class ASLFSoulslikeBoss;

/**
 * C++ AnimInstance for boss characters.
 * Provides cached component references and animation state properties.
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USLFBossAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// Cache component references
	void CacheReferences();
	void UpdateAnimationProperties(float DeltaSeconds);

public:
	// ═══════════════════════════════════════════════════════════════════
	// CACHED REFERENCES (set in NativeInitializeAnimation)
	// Names MUST match original Blueprint variable names exactly!
	// ═══════════════════════════════════════════════════════════════════

	/** Reference to owning boss character - matches BP "SoulslikeBoss" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	ASLFSoulslikeBoss* SoulslikeBoss;

	/** Cached movement component - matches BP "MovementComponent" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	UCharacterMovementComponent* MovementComponent;

	/** Cached AI Combat Manager component - matches BP "AC AI Combat Manager" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References", meta = (DisplayName = "AC AI Combat Manager"))
	UAICombatManagerComponent* AC_AI_CombatManager;

	// ═══════════════════════════════════════════════════════════════════
	// MOVEMENT PROPERTIES (updated every frame)
	// Names match original Blueprint variable names exactly
	// ═══════════════════════════════════════════════════════════════════

	/** Current velocity vector */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	FVector Velocity;

	/** Ground speed (XY plane magnitude) */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	double GroundSpeed;

	/** Whether character is falling - named to match BP "IsFalling" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement", meta = (DisplayName = "IsFalling"))
	bool IsFalling;

	/** Movement direction relative to actor forward */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	float Direction;

	// ═══════════════════════════════════════════════════════════════════
	// COMBAT PROPERTIES (from AICombatManager)
	// Names match original Blueprint variable names exactly
	// ═══════════════════════════════════════════════════════════════════

	/** Whether poise is broken - named to match BP "PoiseBroken" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat", meta = (DisplayName = "PoiseBroken"))
	bool PoiseBroken;

	/** IK blend weight */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat")
	float IkWeight;

	/** Current hit normal vector */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat")
	FVector CurrentHitNormal;

	/** Hit location for reactions - named to match BP "Hit Location" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat", meta = (DisplayName = "Hit Location"))
	FVector HitLocation;

	/** Poise break animation asset */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat")
	UDataAsset* PoiseBreakAsset;

	// ═══════════════════════════════════════════════════════════════════
	// PHYSICS/RAGDOLL
	// ═══════════════════════════════════════════════════════════════════

	/** Physics blend weight for ragdoll */
	UPROPERTY(BlueprintReadWrite, Category = "Animation|Physics")
	float PhysicsWeight;
};
