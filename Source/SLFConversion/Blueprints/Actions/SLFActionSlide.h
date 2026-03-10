// SLFActionSlide.h
// Slide action — sprint + crouch = slide

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionSlide.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionSlide : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionSlide();

	/** Slide duration in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float SlideDuration = 0.8f;

	/** Distance covered during slide (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float SlideDistance = 400.0f;

	/** Cooldown between slides */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float SlideCooldown = 1.5f;

	/** Slide montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	TSoftObjectPtr<UAnimMontage> SlideMontage;

	/** Whether currently sliding */
	UPROPERTY(BlueprintReadOnly, Category = "Slide")
	bool bIsSliding = false;

	/** Whether slide is on cooldown */
	UPROPERTY(BlueprintReadOnly, Category = "Slide")
	bool bOnCooldown = false;

	virtual void ExecuteAction_Implementation() override;
	virtual bool CanExecuteAction_Implementation() override;

private:
	FTimerHandle SlideEndHandle;
	FTimerHandle CooldownHandle;

	void EndSlide();
	void ClearCooldown();
};
