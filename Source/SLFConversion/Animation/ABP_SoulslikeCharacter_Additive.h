// ABP_SoulslikeCharacter_Additive.h
// C++ AnimInstance for ABP_SoulslikeCharacter_Additive
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/AnimBlueprint/ABP_SoulslikeCharacter_Additive.json
// Variables: 23 | Functions: 17
//
// STRATEGY: EventGraph logic -> NativeUpdateAnimation()
//           AnimGraph nodes -> Custom FAnimNode_* classes (future)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "Components/AC_ActionManager.h"
#include "Components/AC_CombatManager.h"
#include "Components/AC_EquipmentManager.h"
#include "SLFPrimaryDataAssets.h"
#include "ABP_SoulslikeCharacter_Additive.generated.h"

// Forward declarations
class UAnimMontage;
class ACharacter;
class UAC_ActionManager;
class UAC_CombatManager;
class UAC_EquipmentManager;
class UAnimSequenceBase;
class UPDA_AnimData;

UCLASS()
class SLFCONVERSION_API UABP_SoulslikeCharacter_Additive : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABP_SoulslikeCharacter_Additive();

	// Called every frame to update animation
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Called when the animation instance is initialized
	virtual void NativeInitializeAnimation() override;

	// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•
	// ANIMATION VARIABLES (23)
	// These are updated in NativeUpdateAnimation and read by AnimGraph
	// â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•â•

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FVector WorldLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FRotator WorldRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FVector Acceleration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FVector Acceleration2D;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	bool bIsAccelerating;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FVector Velocity2D;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	double Direction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	double Speed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	UPDA_AnimData* AnimDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FGameplayTagContainer GrantedTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	bool bIsBlocking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	bool IsCrouched;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	bool bIsFalling;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	bool IsResting;
	// Overlay states - populated from EquipmentManager in NativeUpdateAnimation
	// Blueprint variables with same names are REMOVED during migration to avoid conflict
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	ESLFOverlayState LeftHandOverlayState = ESLFOverlayState::Unarmed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	ESLFOverlayState RightHandOverlayState = ESLFOverlayState::Unarmed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	ESLFOverlayState ActiveOverlayState = ESLFOverlayState::Unarmed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_EquipmentManager* EquipmentManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_CombatManager* CombatManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UAC_ActionManager* ActionManager;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	UAnimSequenceBase* ActiveGuardSequence;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	double IkWeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	FVector ActiveHitNormal;

	// NOTE: Getter functions removed - they conflict with Blueprint functions when reparenting
	// NativeUpdateAnimation() sets all variables directly, so getters are not needed

protected:
	// Cached owner reference
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ACharacter* OwnerCharacter;

	// Helper to get owner velocity
	FVector GetOwnerVelocity() const;

	// Helper to get owner rotation
	FRotator GetOwnerRotation() const;
};
