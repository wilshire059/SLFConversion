// SLFCaveMazeBuilder.h
// Design-driven dungeon maze construction.
// Loads layout JSON (rooms, corridors, walls), spawns wall geometry,
// overlay visualizations, and gameplay actors.

#pragma once

#include "CoreMinimal.h"
#include "SLFCaveMazeTypes.h"

class UWorld;
class ADungeon;

class SLFCONVERSION_API FSLFCaveMazeBuilder
{
public:
	// ── Phase B: Floor Capture ──
	// Hide ceiling geometry, capture orthographic top-down floor map.
	// Requires editor with RHI (not commandlet).
	static void CaptureCaveFloor(UWorld* World, int32 DungeonIndex, int32 Resolution = 4096);

	// ── Phase C: Layout Template Generation ──
	// Reads cell graph from dungeon model, writes template JSON with room data pre-filled.
	static void GenerateTemplateLayout(UWorld* World, int32 DungeonIndex);

	// ── Phase C: Cell Graph Export ──
	// Exports cell graph data (groups, leaves, connections) to JSON.
	static void ExportCellGraphToJson(ADungeon* Dungeon, int32 DungeonIndex, const FVector& DungeonOrigin, const FVector& GridSize);

	// ── Phase D: Overlay Visualization ──
	// Spawn colored actors (planes, spheres) on the cave floor to visualize planned layout.
	static void GenerateOverlay(UWorld* World, int32 DungeonIndex);

	// ── Phase E: Wall Construction ──
	// Build wall geometry from layout JSON. Ray-traces floor/ceiling for height.
	static void BuildCaveMaze(UWorld* World, int32 DungeonIndex);

	// ── Phase E: Gameplay Placement ──
	// Place gameplay actors (enemies, traps, lights, boss gate) based on room designations.
	static void PlaceCaveGameplay(UWorld* World, int32 DungeonIndex);

	// ── Cleanup ──
	// Destroy all actors tagged CaveMazeWall or LayoutOverlay.
	static void ClearCaveMaze(UWorld* World, int32 DungeonIndex);

private:
	// Load layout JSON from MapCapture/dungeon_NN_layout.json
	static bool LoadLayoutFromJson(int32 DungeonIndex, FSLFCaveLayout& OutLayout);

	// Find the ADungeon actor in the world
	static ADungeon* FindDungeonActor(UWorld* World);

	// Compute dungeon bounds from all actors in the world
	static FBox ComputeDungeonBounds(UWorld* World);

	// Ray trace floor/ceiling heights at a world XY position
	static float TraceFloorHeight(UWorld* World, FVector Position, float StartZ, float EndZ);
	static float TraceCeilingHeight(UWorld* World, FVector Position, float StartZ, float EndZ);

	// Spawn a single wall segment
	static AActor* SpawnWallSegment(UWorld* World, const FSLFWallSegment& Wall, float FloorZ, float CeilingZ);

	// Spawn overlay marker actor (flat plane or sphere)
	static AActor* SpawnOverlayMarker(UWorld* World, FVector Position, FLinearColor Color, float Size, bool bIsSphere = false);

	// Get designation color for overlay
	static FLinearColor GetDesignationColor(ESLFRoomDesignation Designation);

	// Get designation name string
	static FString GetDesignationName(ESLFRoomDesignation Designation);

	// Parse designation from string
	static ESLFRoomDesignation ParseDesignation(const FString& Name);
};
