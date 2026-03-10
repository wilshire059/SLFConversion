// SLFCaveMazeTypes.h
// Data structures for design-driven dungeon maze generation.
// Phase C layout data: rooms, corridors, wall segments, loaded from JSON.

#pragma once

#include "CoreMinimal.h"
#include "SLFCaveMazeTypes.generated.h"

UENUM(BlueprintType)
enum class ESLFRoomDesignation : uint8
{
	Entrance,
	BossArena,
	Treasure,
	TrapRoom,
	AmbushRoom,
	PuzzleRoom,
	ShortcutHub,
	Corridor,
	Junction,
	HiddenRoom
};

USTRUCT(BlueprintType)
struct FSLFWallSegment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector Start = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FVector End = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float Thickness = 200.0f;

	UPROPERTY(EditAnywhere)
	float HeightOverride = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bNaturalRock = true;

	UPROPERTY(EditAnywhere)
	FString Tag;
};

USTRUCT(BlueprintType)
struct FSLFRoomDesign
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 GroupId = -1;

	UPROPERTY(EditAnywhere)
	ESLFRoomDesignation Designation = ESLFRoomDesignation::Corridor;

	UPROPERTY(EditAnywhere)
	FVector Center = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float Radius = 500.0f;

	UPROPERTY(EditAnywhere)
	TArray<int32> ConnectedRoomIds;
};

USTRUCT(BlueprintType)
struct FSLFCorridorDesign
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 FromRoomId = -1;

	UPROPERTY(EditAnywhere)
	int32 ToRoomId = -1;

	UPROPERTY(EditAnywhere)
	float Width = 400.0f;

	UPROPERTY(EditAnywhere)
	TArray<FVector> Waypoints;

	UPROPERTY(EditAnywhere)
	bool bIsShortcut = false;
};

USTRUCT(BlueprintType)
struct FSLFCaveLayout
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FSLFRoomDesign> Rooms;

	UPROPERTY(EditAnywhere)
	TArray<FSLFCorridorDesign> Corridors;

	UPROPERTY(EditAnywhere)
	TArray<FSLFWallSegment> Walls;

	UPROPERTY(EditAnywhere)
	FVector DungeonOrigin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	FVector DungeonExtent = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	int32 DungeonIndex = 0;
};
