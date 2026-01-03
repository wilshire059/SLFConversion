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

// Forward declarations - avoid including Blueprint headers to prevent cascade loading
class UAnimMontage;
class ACharacter;
class UCharacterMovementComponent;
class UActorComponent;

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

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLES (10)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// Note: Use base types (AActor*, UActorComponent*) to avoid cascade loading
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AActor* SoulslikeBoss;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UCharacterMovementComponent* MovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UActorComponent* ACAICombatManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool IsFalling;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double Direction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double PhysicsWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector HitLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool PoiseBroken;

	// Note: AnimGraph function removed - conflicts with UE's internal AnimGraph function name

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;
};
