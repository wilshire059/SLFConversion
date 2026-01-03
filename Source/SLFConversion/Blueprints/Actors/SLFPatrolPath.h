// SLFPatrolPath.h
// C++ base for B_PatrolPath - AI patrol waypoint path
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFPatrolPath.generated.h"

USTRUCT(BlueprintType)
struct FSLFPatrolPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLookAround = false;
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPatrolPath : public AActor
{
	GENERATED_BODY()

public:
	ASLFPatrolPath();

	// ============================================================
	// MIGRATION SUMMARY: B_PatrolPath
	// Variables: 4 | Functions: 4 | Dispatchers: 0
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* PathSpline;

	// Patrol Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<FSLFPatrolPoint> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool bIsLooping = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	bool bReverse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	float DefaultMoveSpeed = 200.0f;

	// Functions
	UFUNCTION(BlueprintPure, Category = "Patrol")
	FVector GetPatrolPoint(int32 Index) const;

	UFUNCTION(BlueprintPure, Category = "Patrol")
	int32 GetNextPointIndex(int32 CurrentIndex) const;

	UFUNCTION(BlueprintPure, Category = "Patrol")
	int32 GetNumPatrolPoints() const { return PatrolPoints.Num(); }

	UFUNCTION(BlueprintPure, Category = "Patrol")
	float GetWaitTimeAtPoint(int32 Index) const;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
