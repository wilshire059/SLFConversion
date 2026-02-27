// SetupTitanWorldCommandlet.h
// Commandlet to set up the Project Titan map as our game world.
// Opens TitanMain, places zone triggers, spawn points, resting points, NPCs.
//
// Usage: -run=SetupTitanWorld [-nosave] [-zonesonly] [-spawnsonly]

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GameplayTagContainer.h"
#include "SLFGameTypes.h"
#include "SetupTitanWorldCommandlet.generated.h"

UCLASS()
class USetupTitanWorldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	USetupTitanWorldCommandlet();
	virtual int32 Main(const FString& Params) override;

private:
	// ── Zone Setup ──
	/** Place zone trigger volumes at region boundaries */
	void SetupZoneTriggers(UWorld* World);

	/** Place resting points (2-3 per zone + hub) */
	void SetupRestingPoints(UWorld* World);

	/** Place enemy spawn points for each zone */
	void SetupEnemySpawnPoints(UWorld* World);

	/** Place boss encounter arenas */
	void SetupBossEncounters(UWorld* World);

	/** Place NPCs in the hub */
	void SetupHubNPCs(UWorld* World);

	/** Configure SkyManager to drive Titan's existing lighting actors */
	void SetupSkyManagerForTitan(UWorld* World);

	// ── Helpers ──
	bool SaveLevel(UWorld* World);

	/** Spawn an actor at a world location */
	AActor* SpawnActorAtLocation(UWorld* World, UClass* ActorClass, FVector Location, FRotator Rotation = FRotator::ZeroRotator);

	/** Get terrain height at XY position */
	float GetTerrainHeight(UWorld* World, float X, float Y);

	/** Zone region definitions — approximate XY bounds on the 8x8km Titan map */
	struct FZoneRegion
	{
		FGameplayTag ZoneTag;
		FString DisplayName;
		FVector Center;
		float Radius;
		FVector BossArenaLocation;
		TArray<FVector> RestingPointLocations;
		TArray<FVector> SpawnPointLocations;
	};

	TArray<FZoneRegion> ZoneRegions;
	void InitializeZoneRegions();
};
