// ABP_SoulslikeNPC.h
// C++ AnimInstance for ABP_SoulslikeNPC
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ABP_SoulslikeNPC.json
// Variables: 14 | Functions: 1
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABP_SoulslikeNPC.generated.h"

// Forward declarations - avoid including Blueprint headers to prevent cascade loading
class UAnimMontage;
class ACharacter;
class UCharacterMovementComponent;
class UActorComponent;

UCLASS()
class SLFCONVERSION_API UABP_SoulslikeNPC : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABP_SoulslikeNPC();

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
	double GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Essential Movement Data")
	bool ShouldMove;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Essential Movement Data")
	bool IsFalling;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UActorComponent* EquipmentComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	AActor* SoulslikeNpc;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UActorComponent* ACAICombatManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	bool HasLookAtTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	FVector LookAtLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double DistanceToLookAtTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double MaxDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	double Direction;

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
