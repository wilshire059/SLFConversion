// SLFActionGrapple.h
// Grapple hook action — traversal to grapple points + grapple attack

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionGrapple.generated.h"

class ASLFGrapplePoint;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionGrapple : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionGrapple();

	/** Max range to detect grapple points */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float MaxGrappleRange = 3000.0f;

	/** Flight speed toward grapple point (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float GrappleSpeed = 2000.0f;

	/** Cooldown in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	float GrappleCooldown = 8.0f;

	/** Grapple attack damage multiplier (vs base melee) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Combat")
	float GrappleAttackMultiplier = 1.5f;

	/** Boss damage reduction for grapple attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Combat")
	float BossDamageReduction = 0.25f;

	/** Grapple launch montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	TSoftObjectPtr<UAnimMontage> GrappleLaunchMontage;

	/** Grapple land montage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	TSoftObjectPtr<UAnimMontage> GrappleLandMontage;

	/** Grapple attack montage (melee slam) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
	TSoftObjectPtr<UAnimMontage> GrappleAttackMontage;

	/** Currently grappling */
	UPROPERTY(BlueprintReadOnly, Category = "Grapple")
	bool bIsGrappling = false;

	/** On cooldown */
	UPROPERTY(BlueprintReadOnly, Category = "Grapple")
	bool bOnCooldown = false;

	/** Current target grapple point */
	UPROPERTY(BlueprintReadOnly, Category = "Grapple")
	ASLFGrapplePoint* TargetPoint = nullptr;

	/** Find the best grapple point in player's view */
	UFUNCTION(BlueprintCallable, Category = "Grapple")
	ASLFGrapplePoint* FindBestGrapplePoint() const;

	virtual void ExecuteAction_Implementation() override;
	virtual bool CanExecuteAction_Implementation() override;

private:
	FTimerHandle CooldownHandle;
	FTimerHandle FlightHandle;

	void UpdateGrappleFlight();
	void OnGrappleArrival();
	void ClearCooldown();
};
