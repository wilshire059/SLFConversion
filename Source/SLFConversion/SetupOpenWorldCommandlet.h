// SetupOpenWorldCommandlet.h
// Commandlet to generate an open world level with procedural dungeons
// Uses Landscape for terrain, Wasteland Environment meshes for dressing,
// and Dungeon Architect for procedural dungeon generation.
//
// Usage: -run=SetupOpenWorld [-nodungeons] [-nosave]

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "SetupOpenWorldCommandlet.generated.h"

UCLASS()
class USetupOpenWorldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	USetupOpenWorldCommandlet();
	virtual int32 Main(const FString& Params) override;

private:
	// ── Phase 1: Landscape ──
	bool CreateLandscape(UWorld* World);
	TArray<uint16> GenerateHeightmap(int32 SizeX, int32 SizeY);
	void CarveFlats();

	// ── Phase 2: Wasteland Dressing ──
	bool ScatterWastelandMeshes(UWorld* World);
	void ScatterCategory(UWorld* World, const TArray<FString>& MeshPaths, const FString& CategoryName,
		int32 Count, FVector RegionMin, FVector RegionMax, float MinSpacing, float ScaleMin, float ScaleMax,
		bool bMirrorBackface = false);
	void BuildDungeonEntrance(UWorld* World, FVector Location, int32 DungeonIndex);

	// ── Phase 3: Dungeon Generation ──
	bool GenerateDungeons(UWorld* OverWorld);
	bool CreateDungeonLevel(int32 DungeonIndex, const FString& LevelName, FVector OverworldEntrance);

	// ── Phase 4: Overworld Actors ──
	bool PopulateOverworldActors(UWorld* World);

	// ── Helpers ──
	bool SaveLevel(UWorld* World, const FString& PackageName);
	UStaticMesh* FindWastelandMesh(const FString& NamePrefix, int32 Index);
	TArray<FString> DiscoverWastelandAssets(const FString& NamePrefixFilter);
	TArray<FString> DiscoverWastelandByPrefixes(const TArray<FString>& Prefixes);
	float SampleTerrainHeight(UWorld* World, float X, float Y);

	// Flat area locations (world space) for gameplay features
	struct FlatArea
	{
		FVector Center;
		float Radius;
		FString Purpose; // "spawn", "resting", "dungeon_entrance", "patrol"
	};
	TArray<FlatArea> FlatAreas;
};
