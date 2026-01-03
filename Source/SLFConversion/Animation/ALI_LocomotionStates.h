// ALI_LocomotionStates.h
// C++ AnimInstance for ALI_LocomotionStates
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ALI_LocomotionStates.json
// Variables: 0 | Functions: 5
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"

#include "ALI_LocomotionStates.generated.h"

// Forward declarations
class UAnimMontage;
class ACharacter;


UCLASS()
class SLFCONVERSION_API UALI_LocomotionStates : public UAnimInstance
{
	GENERATED_BODY()

public:
	UALI_LocomotionStates();

	// Called every frame to update animation
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the animation instance is initialized
	virtual void NativeInitializeAnimation() override;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLES (0)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (5)
	// Migrated from EventGraph
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category = "ALI_LocomotionStates")
	void LL_Idle();
	UFUNCTION(BlueprintCallable, Category = "ALI_LocomotionStates")
	void LL_Cycle();
	UFUNCTION(BlueprintCallable, Category = "ALI_LocomotionStates")
	void LL_CrouchIdle();
	UFUNCTION(BlueprintCallable, Category = "ALI_LocomotionStates")
	void LL_CrouchCycle();
	UFUNCTION(BlueprintCallable, Category = "ALI_LocomotionStates")
	void LL_RestingIdle();

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;
};
