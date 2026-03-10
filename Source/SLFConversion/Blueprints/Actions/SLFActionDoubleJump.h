// SLFActionDoubleJump.h
// Double jump — second jump while airborne

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionDoubleJump.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionDoubleJump : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionDoubleJump();

	/** Upward impulse for the second jump */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Double Jump")
	float DoubleJumpImpulse = 1100.0f;

	/** Stamina cost for double jump */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Double Jump")
	float StaminaCost = 15.0f;

	/** Montage to play during double jump (flip/spin) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Double Jump")
	TSoftObjectPtr<UAnimMontage> DoubleJumpMontage;

	virtual void ExecuteAction_Implementation() override;
	virtual bool CanExecuteAction_Implementation() override;
};
