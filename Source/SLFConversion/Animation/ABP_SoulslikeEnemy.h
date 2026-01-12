// ABP_SoulslikeEnemy.h
// C++ AnimInstance for ABP_SoulslikeEnemy
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ABP_SoulslikeEnemy.json
// Variables: 14 | Functions: 1
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SLFEnums.h"
#include "ABP_SoulslikeEnemy.generated.h"

// Forward declarations - avoid including Blueprint headers to prevent cascade loading
class UAnimMontage;
class ACharacter;
class UCharacterMovementComponent;
class UActorComponent;
class UPrimaryDataAsset;
class UPDA_PoiseBreakAnimData;

UCLASS()
class SLFCONVERSION_API UABP_SoulslikeEnemy : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABP_SoulslikeEnemy();

	// Called every frame to update animation
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the animation instance is initialized
	virtual void NativeInitializeAnimation() override;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLES (14)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// Note: Use base types (AActor*, UActorComponent*) to avoid cascade loading
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "References")
	AActor* SoulslikeCharacter;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "References")
	UCharacterMovementComponent* MovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EssentialMovementData")
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Essential Movement Data")
	float GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Essential Movement Data")
	bool IsFalling;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UActorComponent* EquipmentComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	ESLFOverlayState LocomotionType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	float PhysicsWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AActor* SoulslikeEnemy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UActorComponent* ACAICombatManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector HitLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	float Direction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool PoiseBroken;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UPDA_PoiseBreakAnimData* PoiseBreakAsset;

	// ═══════════════════════════════════════════════════════════════════════
	// ADDITIONAL ANIMATION VARIABLES (from AnimGraph analysis)
	// ═══════════════════════════════════════════════════════════════════════

	// IK weight for physics-based animation blending
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "IK")
	float IkWeight;

	// Current hit normal for hit reactions
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	FVector CurrentHitNormal;

	// Note: AnimGraph function removed - conflicts with UE's internal AnimGraph function name

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Cached component references
	UPROPERTY()
	UActorComponent* CachedCombatManager;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;

	// Helper to set Blueprint variables by name (supports spaces in FName)
	// Used for variables like "AC AI Combat Manager" and "Hit Location"
	template<typename T>
	void SetBlueprintVariable(const FName& VarName, const T& Value);

	// Specialized helper for object properties
	void SetBlueprintObjectVariable(const FName& VarName, UObject* Value);
	void SetBlueprintVectorVariable(const FName& VarName, const FVector& Value);
};
