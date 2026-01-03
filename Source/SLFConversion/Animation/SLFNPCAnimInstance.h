// SLFNPCAnimInstance.h
// C++ base class for ABP_SoulslikeNPC
//
// Provides all animation-relevant properties that the AnimGraph needs.
// The AnimGraph (state machines, blends) remains in Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLFNPCAnimInstance.generated.h"

// Forward declarations
class UCharacterMovementComponent;
class UAICombatManagerComponent;
class UEquipmentManagerComponent;
class ASLFSoulslikeNPC;

/**
 * C++ AnimInstance for NPC characters.
 * Provides cached component references and animation state properties.
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFNPCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USLFNPCAnimInstance();

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

	/** Reference to owning NPC - matches BP "SoulslikeNpc" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|References")
	ASLFSoulslikeNPC* SoulslikeNpc;

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

	/** Whether NPC should move - matches BP "ShouldMove" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	bool ShouldMove;

	/** Whether character is falling - named to match BP "IsFalling" */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement", meta = (DisplayName = "IsFalling"))
	bool IsFalling;

	/** Movement direction relative to actor forward */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	float Direction;

	// ═══════════════════════════════════════════════════════════════════
	// LOOK-AT PROPERTIES
	// Names match original Blueprint variable names exactly
	// ═══════════════════════════════════════════════════════════════════

	/** Whether NPC has a look-at target */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|LookAt")
	bool HasLookAtTarget;

	/** Location to look at */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|LookAt")
	FVector LookAtLocation;

	/** Distance to look-at target */
	UPROPERTY(BlueprintReadOnly, Category = "Animation|LookAt")
	double DistanceToLookAtTarget;

	/** Maximum look-at distance */
	UPROPERTY(BlueprintReadWrite, Category = "Animation|LookAt")
	float MaxDistance;
};
