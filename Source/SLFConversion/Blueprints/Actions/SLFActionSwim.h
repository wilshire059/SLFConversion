// SLFActionSwim.h
// Swimming action — activates when player enters a water volume, switches movement mode

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionSwim.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionSwim : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionSwim();

	/** Swim idle (treading water) montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	TSoftObjectPtr<UAnimMontage> SwimIdleMontage;

	/** Forward swim montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	TSoftObjectPtr<UAnimMontage> SwimForwardMontage;

	/** Left swim montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	TSoftObjectPtr<UAnimMontage> SwimLeftMontage;

	/** Right swim montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	TSoftObjectPtr<UAnimMontage> SwimRightMontage;

	/** Movement speed while swimming (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swimming")
	float SwimSpeed = 300.0f;

	/** Whether the player is currently in water */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swimming")
	bool bIsInWater = false;

	/** Whether actively swim-moving (vs treading) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Swimming")
	bool bIsSwimming = false;

	/** Enter water — called when overlapping a water volume */
	UFUNCTION(BlueprintCallable, Category = "Swimming")
	void EnterWater();

	/** Exit water — called when leaving a water volume */
	UFUNCTION(BlueprintCallable, Category = "Swimming")
	void ExitWater();

	virtual void ExecuteAction_Implementation() override;
	virtual bool CanExecuteAction_Implementation() override;

	/** Called each frame while swimming to update montage based on input direction */
	void UpdateSwimAnimation(const FVector2D& MovementInput);

private:
	/** Currently playing swim montage (to avoid restarting same one) */
	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveSwimMontage;

	void PlaySwimMontage(UAnimMontage* Montage);
};
