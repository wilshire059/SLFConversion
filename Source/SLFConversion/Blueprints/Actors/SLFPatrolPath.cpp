// SLFPatrolPath.cpp
#include "SLFPatrolPath.h"
#include "Components/SplineComponent.h"

ASLFPatrolPath::ASLFPatrolPath()
{
	PrimaryActorTick.bCanEverTick = false;



	UE_LOG(LogTemp, Log, TEXT("[PatrolPath] Created"));
}

void ASLFPatrolPath::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[PatrolPath] BeginPlay - Points: %d, Looping: %s"),
		PatrolPoints.Num(), bIsLooping ? TEXT("Yes") : TEXT("No"));
}

void ASLFPatrolPath::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Sync spline points with patrol points
	if (PathSpline && PatrolPoints.Num() > 0)
	{
		if (PathSpline) PathSpline->ClearSplinePoints();
		for (int32 i = 0; i < PatrolPoints.Num(); i++)
		{
			if (PathSpline) PathSpline->AddSplinePoint(PatrolPoints[i].Location, ESplineCoordinateSpace::World);
		}
		if (PathSpline) PathSpline->SetClosedLoop(bIsLooping);
	}
}

FVector ASLFPatrolPath::GetPatrolPoint(int32 Index) const
{
	if (PatrolPoints.IsValidIndex(Index))
	{
		return PatrolPoints[Index].Location;
	}
	return GetActorLocation();
}

int32 ASLFPatrolPath::GetNextPointIndex(int32 CurrentIndex) const
{
	if (PatrolPoints.Num() == 0)
	{
		return 0;
	}

	int32 NextIndex;
	if (bReverse)
	{
		NextIndex = CurrentIndex - 1;
		if (NextIndex < 0)
		{
			NextIndex = bIsLooping ? PatrolPoints.Num() - 1 : 0;
		}
	}
	else
	{
		NextIndex = CurrentIndex + 1;
		if (NextIndex >= PatrolPoints.Num())
		{
			NextIndex = bIsLooping ? 0 : PatrolPoints.Num() - 1;
		}
	}
	return NextIndex;
}

float ASLFPatrolPath::GetWaitTimeAtPoint(int32 Index) const
{
	if (PatrolPoints.IsValidIndex(Index))
	{
		return PatrolPoints[Index].WaitTime;
	}
	return 0.0f;
}
