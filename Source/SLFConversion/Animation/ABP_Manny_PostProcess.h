// ABP_Manny_PostProcess.h
// C++ AnimInstance for ABP_Manny_PostProcess
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ABP_Manny_PostProcess.json
// Variables: 0 | Functions: 1
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"

#include "ABP_Manny_PostProcess.generated.h"

// Forward declarations
class UAnimMontage;
class ACharacter;


UCLASS()
class SLFCONVERSION_API UABP_Manny_PostProcess : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABP_Manny_PostProcess();

	// Called every frame to update animation
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the animation instance is initialized
	virtual void NativeInitializeAnimation() override;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLES (0)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// ═══════════════════════════════════════════════════════════════════════



	// AnimGraph function removed - conflicts with UE's internal AnimGraph function name

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;
};
