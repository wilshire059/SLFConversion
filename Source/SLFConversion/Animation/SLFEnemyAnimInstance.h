// SLFEnemyAnimInstance.h
// C++ base class for ABP_SoulslikeEnemy
//
// Provides all animation-relevant properties that the AnimGraph needs.
// The AnimGraph (state machines, blends) remains in Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLFEnemyAnimInstance.generated.h"

// Forward declarations
class UCharacterMovementComponent;
class UAICombatManagerComponent;
class UEquipmentManagerComponent;
class UDataAsset;
class UPDA_PoiseBreakAnimData;
class ASLFSoulslikeEnemy;

/**
 * C++ AnimInstance for enemy characters.
 * Provides cached component references and animation state properties.
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USLFEnemyAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	void CacheReferences();
	void UpdateAnimationProperties(float DeltaSeconds);

public:
	// ═══════════════════════════════════════════════════════════════════
	// CACHED REFERENCES
	// Names MUST match original Blueprint variable names exactly!
	// ═══════════════════════════════════════════════════════════════════

	/** Reference to owning character - matches BP "SoulslikeCharacter" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	AActor* SoulslikeCharacter;

	/** Reference to owning enemy - matches BP "SoulslikeEnemy" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	ASLFSoulslikeEnemy* SoulslikeEnemy;

	/** Cached movement component - matches BP "MovementComponent" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	UCharacterMovementComponent* MovementComponent;

	/** Cached AI Combat Manager component - matches BP "AC AI Combat Manager" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References", meta = (DisplayName = "AC AI Combat Manager"))
	UAICombatManagerComponent* AC_AI_CombatManager;

	/** Cached Equipment component - matches BP "EquipmentComponent" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	UEquipmentManagerComponent* EquipmentComponent;

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

	/** Locomotion type enum - matches BP "LocomotionType" */
	UPROPERTY(BlueprintReadWrite, Category = "Animation|Movement")
	uint8 LocomotionType;

	// ═══════════════════════════════════════════════════════════════════
	// COMBAT PROPERTIES (from AICombatManager)
	// Names match original Blueprint variable names exactly
	// ═══════════════════════════════════════════════════════════════════

	/** Whether poise is broken - named to match BP "PoiseBroken" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat", meta = (DisplayName = "PoiseBroken"))
	bool PoiseBroken;

	/** Hit location for reactions - named to match BP "Hit Location" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat", meta = (DisplayName = "Hit Location"))
	FVector HitLocation;

	/** Poise break animation asset */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Combat", meta = (DisplayName = "Poise Break Asset"))
	UPDA_PoiseBreakAnimData* PoiseBreakAsset;

	// ═══════════════════════════════════════════════════════════════════
	// PHYSICS/RAGDOLL
	// ═══════════════════════════════════════════════════════════════════

	/** Physics blend weight for ragdoll */
	UPROPERTY(BlueprintReadWrite, Category = "Animation|Physics")
	float PhysicsWeight;
};
