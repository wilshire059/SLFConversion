// SetupTitanWorldCommandlet.cpp
// Opens the Project Titan TitanMain map and places SLF gameplay actors:
// zone triggers, resting points, enemy spawn points, boss encounters, NPCs.
//
// Titan's existing lighting (directional light, sky atmosphere, fog, post-process)
// is preserved. SkyManager is configured to drive Titan's level-placed lights
// instead of spawning its own.
//
// Usage: -run=SetupTitanWorld [-nosave] [-zonesonly] [-spawnsonly]

#include "SetupTitanWorldCommandlet.h"

#if WITH_EDITOR
#include "SLFAutomationLibrary.h"
#include "Blueprints/Actors/SLFZoneTrigger.h"
#include "Blueprints/Actors/SLFEnemySpawnPoint.h"
#include "Blueprints/Actors/SLFBossEncounter.h"
#include "Blueprints/Actors/SLFSkyManager.h"
#include "Blueprints/SLFRestingPointBase.h"
#include "Blueprints/SLFNPCVara.h"
#include "Blueprints/SLFNPCKael.h"
#include "UObject/SavePackage.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetCompilingManager.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Components/BoxComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "EditorLevelUtils.h"
#include "FileHelpers.h"
#include "EngineUtils.h"
#include "GameplayTagsManager.h"
#include "CollisionQueryParams.h"
#endif // WITH_EDITOR

USetupTitanWorldCommandlet::USetupTitanWorldCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 USetupTitanWorldCommandlet::Main(const FString& Params)
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("=== SetupTitanWorld Commandlet START ==="));
	UE_LOG(LogTemp, Warning, TEXT("Params: [%s]"), *Params);

	const bool bNoSave = Params.Contains(TEXT("-nosave"));
	const bool bZonesOnly = Params.Contains(TEXT("-zonesonly"));
	const bool bSpawnsOnly = Params.Contains(TEXT("-spawnsonly"));
	const bool bNPCsOnly = Params.Contains(TEXT("-npcsonly"));

	// ── Initialize zone region data ──
	InitializeZoneRegions();
	UE_LOG(LogTemp, Warning, TEXT("Initialized %d zone regions"), ZoneRegions.Num());

	// ── Load TitanMain map ──
	const FString MapPath = TEXT("/Game/Maps/TitanMain");
	UE_LOG(LogTemp, Warning, TEXT("Loading map: %s"), *MapPath);

	// Scan asset registry
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.SearchAllAssets(true);

	// Try to load the map package
	FString MapPackagePath = MapPath;
	UPackage* MapPackage = LoadPackage(nullptr, *MapPackagePath, LOAD_None);
	if (!MapPackage)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load map package: %s"), *MapPackagePath);
		UE_LOG(LogTemp, Error, TEXT("Make sure Content/Titan/ symlink exists and points to ProjectTitan content"));
		return 1;
	}

	UWorld* World = UWorld::FindWorldInPackage(MapPackage);
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No world found in package: %s"), *MapPackagePath);
		return 1;
	}

	// Initialize the world for editing
	World->WorldType = EWorldType::Editor;
	World->AddToRoot();
	World->InitWorld();
	World->LoadSecondaryLevels(true, nullptr);

	UE_LOG(LogTemp, Warning, TEXT("World loaded: %s (Levels: %d)"), *World->GetName(), World->GetNumLevels());

	// ── Place gameplay actors ──
	int32 TotalActorsPlaced = 0;

	if (!bSpawnsOnly && !bNPCsOnly)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- Setting up Zone Triggers ---"));
		SetupZoneTriggers(World);
	}

	if (!bZonesOnly && !bNPCsOnly)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- Setting up Resting Points ---"));
		SetupRestingPoints(World);

		UE_LOG(LogTemp, Warning, TEXT("--- Setting up Enemy Spawn Points ---"));
		SetupEnemySpawnPoints(World);

		UE_LOG(LogTemp, Warning, TEXT("--- Setting up Boss Encounters ---"));
		SetupBossEncounters(World);
	}

	if (!bZonesOnly && !bSpawnsOnly)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- Setting up Hub NPCs ---"));
		SetupHubNPCs(World);
	}

	// ── Configure SkyManager to use Titan's lighting ──
	UE_LOG(LogTemp, Warning, TEXT("--- Configuring SkyManager for Titan lighting ---"));
	SetupSkyManagerForTitan(World);

	// ── Place PlayerStart at hub ──
	UE_LOG(LogTemp, Warning, TEXT("--- Placing PlayerStart at Hub ---"));
	// Hub center — player starts here
	FVector HubLocation = FVector(0.0, 0.0, 0.0);
	float HubHeight = GetTerrainHeight(World, HubLocation.X, HubLocation.Y);
	if (HubHeight > -100000.0f)
	{
		HubLocation.Z = HubHeight + 100.0f;
	}
	else
	{
		HubLocation.Z = 500.0f; // Fallback
	}

	APlayerStart* PlayerStart = Cast<APlayerStart>(
		SpawnActorAtLocation(World, APlayerStart::StaticClass(), HubLocation));
	if (PlayerStart)
	{
		UE_LOG(LogTemp, Warning, TEXT("  PlayerStart placed at (%.0f, %.0f, %.0f)"),
			HubLocation.X, HubLocation.Y, HubLocation.Z);
	}

	// ── Save ──
	if (!bNoSave)
	{
		if (SaveLevel(World))
		{
			UE_LOG(LogTemp, Warning, TEXT("Map saved successfully"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save map"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skipping save (-nosave)"));
	}

	World->RemoveFromRoot();

	UE_LOG(LogTemp, Warning, TEXT("=== SetupTitanWorld Commandlet COMPLETE ==="));
	return 0;

#else
	UE_LOG(LogTemp, Error, TEXT("SetupTitanWorld requires WITH_EDITOR"));
	return 1;
#endif
}

#if WITH_EDITOR

// ═══════════════════════════════════════════════════════════════════════════════
// Zone Region Initialization
// ═══════════════════════════════════════════════════════════════════════════════
// Project Titan is 8x8 km = 80,000 x 80,000 UE units.
// Map roughly centered at origin. Coordinates are approximate —
// will need tuning once we can see the terrain in-editor.
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::InitializeZoneRegions()
{
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

	// Hub — central safe area (map center)
	{
		FZoneRegion Hub;
		Hub.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Hub"));
		Hub.DisplayName = TEXT("The Refuge");
		Hub.Center = FVector(0.0, 0.0, 0.0);
		Hub.Radius = 5000.0;
		Hub.BossArenaLocation = FVector::ZeroVector; // No boss in hub
		Hub.RestingPointLocations = {
			FVector(0.0, 200.0, 0.0),      // Hub center resting point
		};
		Hub.SpawnPointLocations = {}; // No enemies in hub
		ZoneRegions.Add(Hub);
	}

	// Zone 1: The Ashfields — south, open wasteland (easiest)
	{
		FZoneRegion Zone;
		Zone.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Ashfields"));
		Zone.DisplayName = TEXT("The Ashfields");
		Zone.Center = FVector(0.0, -25000.0, 0.0);
		Zone.Radius = 12000.0;
		Zone.BossArenaLocation = FVector(3000.0, -32000.0, 0.0);
		Zone.RestingPointLocations = {
			FVector(-2000.0, -16000.0, 0.0),   // Zone entrance
			FVector(1000.0, -24000.0, 0.0),     // Midpoint
			FVector(2000.0, -30000.0, 0.0),     // Pre-boss
		};
		Zone.SpawnPointLocations = {
			FVector(-3000.0, -18000.0, 0.0),
			FVector(2000.0, -20000.0, 0.0),
			FVector(-1000.0, -22000.0, 0.0),
			FVector(4000.0, -24000.0, 0.0),
			FVector(0.0, -26000.0, 0.0),
			FVector(-2000.0, -28000.0, 0.0),
		};
		ZoneRegions.Add(Zone);
	}

	// Zone 2: Ironworks — east, industrial ruins
	{
		FZoneRegion Zone;
		Zone.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Ironworks"));
		Zone.DisplayName = TEXT("The Ironworks");
		Zone.Center = FVector(25000.0, 0.0, 0.0);
		Zone.Radius = 12000.0;
		Zone.BossArenaLocation = FVector(34000.0, 2000.0, 0.0);
		Zone.RestingPointLocations = {
			FVector(16000.0, -1000.0, 0.0),
			FVector(24000.0, 1000.0, 0.0),
			FVector(31000.0, 0.0, 0.0),
		};
		Zone.SpawnPointLocations = {
			FVector(18000.0, -2000.0, 0.0),
			FVector(20000.0, 2000.0, 0.0),
			FVector(22000.0, -1000.0, 0.0),
			FVector(26000.0, 3000.0, 0.0),
			FVector(28000.0, -2000.0, 0.0),
			FVector(30000.0, 1000.0, 0.0),
		};
		ZoneRegions.Add(Zone);
	}

	// Zone 3: The Rift — north, corrupted terrain
	{
		FZoneRegion Zone;
		Zone.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Rift"));
		Zone.DisplayName = TEXT("The Rift");
		Zone.Center = FVector(0.0, 25000.0, 0.0);
		Zone.Radius = 12000.0;
		Zone.BossArenaLocation = FVector(-2000.0, 34000.0, 0.0);
		Zone.RestingPointLocations = {
			FVector(1000.0, 16000.0, 0.0),
			FVector(-1000.0, 24000.0, 0.0),
			FVector(0.0, 31000.0, 0.0),
		};
		Zone.SpawnPointLocations = {
			FVector(3000.0, 18000.0, 0.0),
			FVector(-2000.0, 20000.0, 0.0),
			FVector(1000.0, 22000.0, 0.0),
			FVector(-3000.0, 26000.0, 0.0),
			FVector(2000.0, 28000.0, 0.0),
			FVector(0.0, 30000.0, 0.0),
		};
		ZoneRegions.Add(Zone);
	}

	// Zone 4: Sanctum of Ash — west, cathedral/fortress
	{
		FZoneRegion Zone;
		Zone.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Sanctum"));
		Zone.DisplayName = TEXT("Sanctum of Ash");
		Zone.Center = FVector(-25000.0, 0.0, 0.0);
		Zone.Radius = 12000.0;
		Zone.BossArenaLocation = FVector(-34000.0, -1000.0, 0.0);
		Zone.RestingPointLocations = {
			FVector(-16000.0, 1000.0, 0.0),
			FVector(-24000.0, -1000.0, 0.0),
			FVector(-31000.0, 0.0, 0.0),
		};
		Zone.SpawnPointLocations = {
			FVector(-18000.0, 2000.0, 0.0),
			FVector(-20000.0, -2000.0, 0.0),
			FVector(-22000.0, 1000.0, 0.0),
			FVector(-26000.0, -3000.0, 0.0),
			FVector(-28000.0, 2000.0, 0.0),
			FVector(-30000.0, -1000.0, 0.0),
		};
		ZoneRegions.Add(Zone);
	}

	// Zone 5: The Undercroft — northeast underground/caves
	{
		FZoneRegion Zone;
		Zone.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Undercroft"));
		Zone.DisplayName = TEXT("The Undercroft");
		Zone.Center = FVector(18000.0, 18000.0, -2000.0); // Underground
		Zone.Radius = 10000.0;
		Zone.BossArenaLocation = FVector(22000.0, 24000.0, -3000.0);
		Zone.RestingPointLocations = {
			FVector(14000.0, 14000.0, -1000.0),
			FVector(18000.0, 20000.0, -2000.0),
			FVector(20000.0, 22000.0, -2500.0),
		};
		Zone.SpawnPointLocations = {
			FVector(15000.0, 15000.0, -1500.0),
			FVector(17000.0, 17000.0, -1800.0),
			FVector(19000.0, 19000.0, -2000.0),
			FVector(20000.0, 21000.0, -2200.0),
			FVector(21000.0, 23000.0, -2500.0),
		};
		ZoneRegions.Add(Zone);
	}

	// Zone 6: The Core — map center, final area (slightly offset from hub)
	{
		FZoneRegion Zone;
		Zone.ZoneTag = TagManager.RequestGameplayTag(FName("Zone.Core"));
		Zone.DisplayName = TEXT("The Core");
		Zone.Center = FVector(0.0, 0.0, -5000.0); // Deep underground, below hub
		Zone.Radius = 8000.0;
		Zone.BossArenaLocation = FVector(0.0, 0.0, -6000.0);
		Zone.RestingPointLocations = {
			FVector(0.0, -3000.0, -4000.0),   // Entrance
			FVector(0.0, 0.0, -5500.0),        // Pre-boss
		};
		Zone.SpawnPointLocations = {
			FVector(2000.0, -2000.0, -4500.0),
			FVector(-2000.0, -1000.0, -4800.0),
			FVector(1000.0, 1000.0, -5000.0),
			FVector(-1000.0, 2000.0, -5200.0),
		};
		ZoneRegions.Add(Zone);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// Zone Triggers
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::SetupZoneTriggers(UWorld* World)
{
	int32 Count = 0;
	for (const FZoneRegion& Region : ZoneRegions)
	{
		// Place a zone trigger at the center of each region
		FVector Location = Region.Center;
		float Height = GetTerrainHeight(World, Location.X, Location.Y);
		if (Height > -100000.0f)
		{
			Location.Z = Height;
		}

		ASLFZoneTrigger* Trigger = Cast<ASLFZoneTrigger>(
			SpawnActorAtLocation(World, ASLFZoneTrigger::StaticClass(), Location));

		if (Trigger)
		{
			Trigger->ZoneTag = Region.ZoneTag;
			Trigger->TransitionText = FText::FromString(FString::Printf(TEXT("Entering %s"), *Region.DisplayName));
			Trigger->bShowZoneNameOnEntry = true;

			// Scale the trigger box to cover the zone radius
			UBoxComponent* Box = Cast<UBoxComponent>(Trigger->GetComponentByClass(UBoxComponent::StaticClass()));
			if (Box)
			{
				Box->SetBoxExtent(FVector(Region.Radius, Region.Radius, 5000.0f));
			}

			Trigger->SetFolderPath(FName(TEXT("Zones")));
			Count++;
			UE_LOG(LogTemp, Warning, TEXT("  Zone trigger: %s at (%.0f, %.0f, %.0f) radius=%.0f"),
				*Region.DisplayName, Location.X, Location.Y, Location.Z, Region.Radius);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("  Placed %d zone triggers"), Count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Resting Points
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::SetupRestingPoints(UWorld* World)
{
	int32 Count = 0;
	for (const FZoneRegion& Region : ZoneRegions)
	{
		for (int32 i = 0; i < Region.RestingPointLocations.Num(); i++)
		{
			FVector Location = Region.RestingPointLocations[i];
			float Height = GetTerrainHeight(World, Location.X, Location.Y);
			if (Height > -100000.0f)
			{
				Location.Z = Height + 10.0f; // Slightly above ground
			}

			ASLFRestingPointBase* RestPoint = Cast<ASLFRestingPointBase>(
				SpawnActorAtLocation(World, ASLFRestingPointBase::StaticClass(), Location));

			if (RestPoint)
			{
				// Name based on zone + index
				FString PointName = FString::Printf(TEXT("%s - Rest %d"), *Region.DisplayName, i + 1);
				RestPoint->LocationName = FText::FromString(PointName);
				RestPoint->SetFolderPath(FName(TEXT("RestingPoints")));
				Count++;
				UE_LOG(LogTemp, Warning, TEXT("  Resting point: %s at (%.0f, %.0f, %.0f)"),
					*PointName, Location.X, Location.Y, Location.Z);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("  Placed %d resting points"), Count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Enemy Spawn Points
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::SetupEnemySpawnPoints(UWorld* World)
{
	int32 Count = 0;
	for (const FZoneRegion& Region : ZoneRegions)
	{
		// Skip hub and core (no regular spawns in hub, core has special handling)
		if (Region.DisplayName == TEXT("The Refuge"))
		{
			continue;
		}

		for (int32 i = 0; i < Region.SpawnPointLocations.Num(); i++)
		{
			FVector Location = Region.SpawnPointLocations[i];
			float Height = GetTerrainHeight(World, Location.X, Location.Y);
			if (Height > -100000.0f)
			{
				Location.Z = Height + 10.0f;
			}

			ASLFEnemySpawnPoint* SpawnPoint = Cast<ASLFEnemySpawnPoint>(
				SpawnActorAtLocation(World, ASLFEnemySpawnPoint::StaticClass(), Location));

			if (SpawnPoint)
			{
				SpawnPoint->ZoneTag = Region.ZoneTag;
				SpawnPoint->SpawnRadius = 200.0f;
				SpawnPoint->bRespawnOnRest = true;
				SpawnPoint->bSpawnOnBeginPlay = true;

				// EnemyClass left null — will be set when enemy classes are created
				// StatScale and CurrencyReward vary by zone
				if (Region.DisplayName == TEXT("The Ashfields"))
				{
					SpawnPoint->StatScale = 1.0f;
					SpawnPoint->CurrencyReward = 50;
				}
				else if (Region.DisplayName == TEXT("The Ironworks"))
				{
					SpawnPoint->StatScale = 1.5f;
					SpawnPoint->CurrencyReward = 100;
				}
				else if (Region.DisplayName == TEXT("The Rift"))
				{
					SpawnPoint->StatScale = 2.0f;
					SpawnPoint->CurrencyReward = 150;
				}
				else if (Region.DisplayName == TEXT("Sanctum of Ash"))
				{
					SpawnPoint->StatScale = 2.5f;
					SpawnPoint->CurrencyReward = 200;
				}
				else if (Region.DisplayName == TEXT("The Undercroft"))
				{
					SpawnPoint->StatScale = 3.0f;
					SpawnPoint->CurrencyReward = 300;
				}
				else if (Region.DisplayName == TEXT("The Core"))
				{
					SpawnPoint->StatScale = 4.0f;
					SpawnPoint->CurrencyReward = 500;
				}

				SpawnPoint->SetFolderPath(FName(*FString::Printf(TEXT("Spawns/%s"), *Region.DisplayName)));
				Count++;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("  Placed %d enemy spawn points"), Count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Boss Encounters
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::SetupBossEncounters(UWorld* World)
{
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	int32 Count = 0;

	for (const FZoneRegion& Region : ZoneRegions)
	{
		// Skip hub (no boss) and core (final boss, handled separately if needed)
		if (Region.BossArenaLocation.IsNearlyZero())
		{
			continue;
		}

		FVector Location = Region.BossArenaLocation;
		float Height = GetTerrainHeight(World, Location.X, Location.Y);
		if (Height > -100000.0f)
		{
			Location.Z = Height + 10.0f;
		}

		ASLFBossEncounter* BossEncounter = Cast<ASLFBossEncounter>(
			SpawnActorAtLocation(World, ASLFBossEncounter::StaticClass(), Location));

		if (BossEncounter)
		{
			BossEncounter->ZoneTag = Region.ZoneTag;
			BossEncounter->ArenaCenter = Location;
			BossEncounter->ArenaRadius = 3000.0f;

			// Configure boss based on zone
			FSLFBossConfig& Config = BossEncounter->BossConfig;

			if (Region.DisplayName == TEXT("The Ashfields"))
			{
				Config.BossName = FText::FromString(TEXT("The Broodmother"));
				Config.CurrencyReward = 5000;
				Config.NumPhases = 2;
				Config.PhaseThresholds = { 0.5f };
				Config.DefeatProgressTag = TagManager.RequestGameplayTag(FName("Boss.Broodmother.Defeated"));
			}
			else if (Region.DisplayName == TEXT("The Ironworks"))
			{
				Config.BossName = FText::FromString(TEXT("The Forgemaster"));
				Config.CurrencyReward = 10000;
				Config.NumPhases = 2;
				Config.PhaseThresholds = { 0.5f };
				Config.DefeatProgressTag = TagManager.RequestGameplayTag(FName("Boss.Forgemaster.Defeated"));
			}
			else if (Region.DisplayName == TEXT("The Rift"))
			{
				Config.BossName = FText::FromString(TEXT("The Convergent"));
				Config.CurrencyReward = 15000;
				Config.NumPhases = 3;
				Config.PhaseThresholds = { 0.6f, 0.3f };
				Config.DefeatProgressTag = TagManager.RequestGameplayTag(FName("Boss.Convergent.Defeated"));
			}
			else if (Region.DisplayName == TEXT("Sanctum of Ash"))
			{
				Config.BossName = FText::FromString(TEXT("The Ashen Hierophant"));
				Config.CurrencyReward = 20000;
				Config.NumPhases = 3;
				Config.PhaseThresholds = { 0.6f, 0.3f };
				Config.DefeatProgressTag = TagManager.RequestGameplayTag(FName("Boss.Hierophant.Defeated"));
			}
			else if (Region.DisplayName == TEXT("The Undercroft"))
			{
				Config.BossName = FText::FromString(TEXT("The Primordial"));
				Config.CurrencyReward = 25000;
				Config.NumPhases = 3;
				Config.PhaseThresholds = { 0.7f, 0.4f };
				Config.DefeatProgressTag = TagManager.RequestGameplayTag(FName("Boss.Primordial.Defeated"));
			}
			else if (Region.DisplayName == TEXT("The Core"))
			{
				Config.BossName = FText::FromString(TEXT("The Convergence"));
				Config.CurrencyReward = 50000;
				Config.NumPhases = 3;
				Config.PhaseThresholds = { 0.7f, 0.35f };
				Config.DefeatProgressTag = TagManager.RequestGameplayTag(FName("Boss.Convergence.Defeated"));
			}

			BossEncounter->SetFolderPath(FName(TEXT("Bosses")));
			Count++;
			UE_LOG(LogTemp, Warning, TEXT("  Boss encounter: %s at (%.0f, %.0f, %.0f)"),
				*Config.BossName.ToString(), Location.X, Location.Y, Location.Z);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("  Placed %d boss encounters"), Count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Hub NPCs
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::SetupHubNPCs(UWorld* World)
{
	// Hub center is at origin — place NPCs nearby
	FVector HubCenter = FVector(0.0, 0.0, 0.0);
	float HubHeight = GetTerrainHeight(World, HubCenter.X, HubCenter.Y);
	if (HubHeight < -100000.0f) HubHeight = 500.0f;

	// Vara — wandering guide/merchant, near hub resting point
	{
		FVector VaraLocation = FVector(300.0, 400.0, HubHeight + 10.0f);
		ASLFNPCVara* Vara = Cast<ASLFNPCVara>(
			SpawnActorAtLocation(World, ASLFNPCVara::StaticClass(), VaraLocation,
				FRotator(0.0f, -90.0f, 0.0f))); // Face toward center

		if (Vara)
		{
			Vara->SetFolderPath(FName(TEXT("NPCs")));
			UE_LOG(LogTemp, Warning, TEXT("  NPC Vara placed at (%.0f, %.0f, %.0f)"),
				VaraLocation.X, VaraLocation.Y, VaraLocation.Z);
		}
	}

	// Kael — blacksmith, opposite side of hub
	{
		FVector KaelLocation = FVector(-400.0, 300.0, HubHeight + 10.0f);
		ASLFNPCKael* Kael = Cast<ASLFNPCKael>(
			SpawnActorAtLocation(World, ASLFNPCKael::StaticClass(), KaelLocation,
				FRotator(0.0f, 45.0f, 0.0f))); // Face toward center

		if (Kael)
		{
			Kael->SetFolderPath(FName(TEXT("NPCs")));
			UE_LOG(LogTemp, Warning, TEXT("  NPC Kael placed at (%.0f, %.0f, %.0f)"),
				KaelLocation.X, KaelLocation.Y, KaelLocation.Z);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// SkyManager — find Titan's lighting actors and wire SkyManager to drive them
// ═══════════════════════════════════════════════════════════════════════════════

void USetupTitanWorldCommandlet::SetupSkyManagerForTitan(UWorld* World)
{
	// Find Titan's existing lighting actors
	ADirectionalLight* TitanSun = nullptr;
	ASkyLight* TitanSkyLight = nullptr;
	AExponentialHeightFog* TitanFog = nullptr;
	APostProcessVolume* TitanPPV = nullptr;

	for (TActorIterator<ADirectionalLight> It(World); It; ++It)
	{
		if (!TitanSun)
		{
			TitanSun = *It;
			UE_LOG(LogTemp, Warning, TEXT("  Found Titan directional light: %s"), *TitanSun->GetName());
		}
	}

	for (TActorIterator<ASkyLight> It(World); It; ++It)
	{
		if (!TitanSkyLight)
		{
			TitanSkyLight = *It;
			UE_LOG(LogTemp, Warning, TEXT("  Found Titan sky light: %s"), *TitanSkyLight->GetName());
		}
	}

	for (TActorIterator<AExponentialHeightFog> It(World); It; ++It)
	{
		if (!TitanFog)
		{
			TitanFog = *It;
			UE_LOG(LogTemp, Warning, TEXT("  Found Titan fog: %s"), *TitanFog->GetName());
		}
	}

	for (TActorIterator<APostProcessVolume> It(World); It; ++It)
	{
		if (!TitanPPV)
		{
			TitanPPV = *It;
			UE_LOG(LogTemp, Warning, TEXT("  Found Titan post-process volume: %s"), *TitanPPV->GetName());
		}
	}

	// Spawn SkyManager and configure it to use Titan's actors
	FVector SkyManagerLocation = FVector(0.0, 0.0, 10000.0);
	ASLFSkyManager* SkyManager = Cast<ASLFSkyManager>(
		SpawnActorAtLocation(World, ASLFSkyManager::StaticClass(), SkyManagerLocation));

	if (SkyManager)
	{
		// Point SkyManager at Titan's directional light
		if (TitanSun)
		{
			SkyManager->SunLight = TitanSun;
			UE_LOG(LogTemp, Warning, TEXT("  SkyManager->SunLight = %s"), *TitanSun->GetName());
		}

		// Disable SkyManager's own lighting components — Titan's are better configured
		// SkyManager will drive Titan's actors via the SunLight/SkyAtmosphereActor references
		if (SkyManager->SunLightComponent)
		{
			SkyManager->SunLightComponent->SetVisibility(false);
			SkyManager->SunLightComponent->SetActive(false);
		}
		if (SkyManager->MoonLightComponent)
		{
			SkyManager->MoonLightComponent->SetVisibility(false);
			SkyManager->MoonLightComponent->SetActive(false);
		}
		if (SkyManager->SkyLightComp)
		{
			SkyManager->SkyLightComp->SetVisibility(false);
			SkyManager->SkyLightComp->SetActive(false);
		}
		if (SkyManager->SkyAtmosphereComp)
		{
			SkyManager->SkyAtmosphereComp->SetVisibility(false);
			SkyManager->SkyAtmosphereComp->SetActive(false);
		}
		if (SkyManager->VolumetricCloudComp)
		{
			SkyManager->VolumetricCloudComp->SetVisibility(false);
			SkyManager->VolumetricCloudComp->SetActive(false);
		}
		if (SkyManager->HeightFogComp)
		{
			SkyManager->HeightFogComp->SetVisibility(false);
			SkyManager->HeightFogComp->SetActive(false);
		}
		if (SkyManager->SkySphereComp)
		{
			SkyManager->SkySphereComp->SetVisibility(false);
			SkyManager->SkySphereComp->SetActive(false);
		}

		SkyManager->SetFolderPath(FName(TEXT("World")));
		UE_LOG(LogTemp, Warning, TEXT("  SkyManager placed — Titan lighting will be driven by SLF day/night cycle"));
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════════

AActor* USetupTitanWorldCommandlet::SpawnActorAtLocation(
	UWorld* World, UClass* ActorClass, FVector Location, FRotator Rotation)
{
	if (!World || !ActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams);
	if (!NewActor)
	{
		UE_LOG(LogTemp, Error, TEXT("  Failed to spawn %s at (%.0f, %.0f, %.0f)"),
			*ActorClass->GetName(), Location.X, Location.Y, Location.Z);
	}
	return NewActor;
}

float USetupTitanWorldCommandlet::GetTerrainHeight(UWorld* World, float X, float Y)
{
	if (!World)
	{
		return -200000.0f;
	}

	// Trace downward from high above to find terrain surface
	FVector Start(X, Y, 50000.0f);
	FVector End(X, Y, -50000.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;

	if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, QueryParams))
	{
		return HitResult.ImpactPoint.Z;
	}

	return -200000.0f; // No terrain found
}

bool USetupTitanWorldCommandlet::SaveLevel(UWorld* World)
{
	if (!World)
	{
		return false;
	}

	UPackage* Package = World->GetOutermost();
	if (!Package)
	{
		return false;
	}

	// Mark dirty
	Package->MarkPackageDirty();

	FString PackageFilename;
	if (FPackageName::DoesPackageExist(Package->GetName(), &PackageFilename))
	{
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		FSavePackageResultStruct Result = UPackage::Save(Package, World, *PackageFilename, SaveArgs);
		if (Result.Result == ESavePackageResult::Success)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Saved: %s"), *PackageFilename);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("  Save failed for: %s"), *PackageFilename);
			return false;
		}
	}
	else
	{
		// New package — save to Titan maps location
		FString NewPath = FPaths::ProjectContentDir() / TEXT("Titan/Maps/TitanMain_SLF.umap");
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Standalone;
		FSavePackageResultStruct Result = UPackage::Save(Package, World, *NewPath, SaveArgs);
		UE_LOG(LogTemp, Warning, TEXT("  Saved new map: %s (Result: %d)"), *NewPath, (int32)Result.Result);
		return Result.Result == ESavePackageResult::Success;
	}
}

#endif // WITH_EDITOR
