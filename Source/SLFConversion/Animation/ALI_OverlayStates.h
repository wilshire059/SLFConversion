// ALI_OverlayStates.h
// C++ AnimInstance for ALI_OverlayStates
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ALI_OverlayStates.json
// Variables: 0 | Functions: 8
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"

#include "ALI_OverlayStates.generated.h"

// Forward declarations
class UAnimMontage;
class ACharacter;


UCLASS()
class SLFCONVERSION_API UALI_OverlayStates : public UAnimInstance
{
	GENERATED_BODY()

public:
	UALI_OverlayStates();

	// Called every frame to update animation
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the animation instance is initialized
	virtual void NativeInitializeAnimation() override;

	// ═══════════════════════════════════════════════════════════════════════
	// ANIMATION VARIABLES (0)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// HELPER FUNCTIONS (8)
	// Migrated from EventGraph
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_OneHanded_Left();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_Shield_Left();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_Shield_Block_Left();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_TwoHanded_Left();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_OneHanded_Right();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_Shield_Right();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_Shield_Block_Right();
	UFUNCTION(BlueprintCallable, Category = "ALI_OverlayStates")
	void LL_TwoHanded_Right();

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;
};
