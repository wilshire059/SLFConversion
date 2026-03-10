// SLFGrapplePoint.h
// World grapple anchor actor — player can grapple to these points

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SLFGrapplePoint.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFGrapplePoint : public AActor
{
	GENERATED_BODY()

public:
	ASLFGrapplePoint();

	/** Visual mesh (hook/ring/anchor) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* AnchorMesh;

	/** Detection sphere — player can target this point when within range */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DetectionSphere;

	/** Landing point offset from anchor (where player lands after grapple) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
	FVector LandingOffset = FVector(0.0f, 0.0f, -100.0f);

	/** Maximum range from which player can target this point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
	float GrappleRange = 3000.0f;

	/** Whether this point is currently usable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
	bool bIsActive = true;

	/** Get the world-space landing location */
	UFUNCTION(BlueprintCallable, Category = "Grapple Point")
	FVector GetLandingLocation() const;

	/** Check if an actor is within grapple range */
	UFUNCTION(BlueprintCallable, Category = "Grapple Point")
	bool IsInRange(AActor* Actor) const;

protected:
	virtual void BeginPlay() override;
};
