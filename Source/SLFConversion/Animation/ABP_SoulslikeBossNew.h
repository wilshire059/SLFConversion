// ABP_SoulslikeBossNew.h
// C++ AnimInstance for ABP_SoulslikeBossNew
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ABP_SoulslikeBossNew.json
// Variables: 10 | Functions: 1
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABP_SoulslikeBossNew.generated.h"

// Forward declarations
class UAnimMontage;
class ACharacter;
class UCharacterMovementComponent;
class UActorComponent;
class UAC_AI_CombatManager;

UCLASS()
class SLFCONVERSION_API UABP_SoulslikeBossNew : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABP_SoulslikeBossNew();

	// Called every frame to update animation
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the animation instance is initialized
	virtual void NativeInitializeAnimation() override;

	// Called when the animation instance is uninitialized (before destruction)
	virtual void NativeUninitializeAnimation() override;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLES (10)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// Note: Use base types (AActor*, UActorComponent*) to avoid cascade loading
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AActor* SoulslikeBoss;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UCharacterMovementComponent* MovementComponent;
	// NOTE: DisplayName MUST match Blueprint variable name exactly (with spaces)
	// The AnimGraph reads "AC AI Combat Manager" not "ACAICombatManager"
	// CRITICAL: Must use UAC_AI_CombatManager* because AnimGraph property bindings were compiled
	// expecting this type to access PoiseBreakAsset
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default", meta = (DisplayName = "AC AI Combat Manager"))
	UAC_AI_CombatManager* ACAICombatManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	float GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool IsFalling;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	float Direction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	float PhysicsWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector HitLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool PoiseBroken;

	// Additional variables for AnimGraph
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IK")
	float IkWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	FVector CurrentHitNormal;

	// Note: AnimGraph function removed - conflicts with UE's internal AnimGraph function name

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Cached component references
	UPROPERTY()
	UAC_AI_CombatManager* CachedCombatManager;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;

	// Reflection helpers for Blueprint variables with spaces
	void SetBlueprintObjectVariable(const FName& VarName, UObject* Value);
	void SetBlueprintVectorVariable(const FName& VarName, const FVector& Value);
};
