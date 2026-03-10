// SLFCaveMarkerEmitter.h
// Custom marker emitter for cave dungeons — emits decoration markers
// (stalactites, stalagmites, mushrooms, lights, vines, etc.) after
// Cell Flow layout + standard markers are placed.

#pragma once

#include "CoreMinimal.h"
#include "Frameworks/ThemeEngine/Markers/DungeonMarkerEmitter.h"
#include "SLFCaveMarkerEmitter.generated.h"

UCLASS()
class SLFCONVERSION_API USLFCaveMarkerEmitter : public UDungeonMarkerEmitter
{
	GENERATED_BODY()

public:
	USLFCaveMarkerEmitter();

	// Per-room decoration counts
	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 StalactitesPerRoom = 8;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 StalagmitesPerRoom = 5;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 MushroomClustersPerRoom = 3;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 TorchesPerRoom = 4;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 HangingVinesPerRoom = 6;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 WallGrowthPerRoom = 4;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 FloorDebrisPerRoom = 8;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	int32 CeilingMossPerRoom = 5;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	float WaterPuddleChance = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	float CaveSmokeChance = 0.5f;

	// Grid size used by Cell Flow (needed to convert cell coords to world space)
	UPROPERTY(EditAnywhere, Category = "Cave Markers")
	FVector GridSize = FVector(400, 400, 300);

	virtual void EmitMarkers_Implementation(UDungeonBuilder* Builder, UDungeonModel* Model,
		UDungeonConfig* Config, UDungeonQuery* Query) override;
};
