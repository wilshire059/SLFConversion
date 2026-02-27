---
name: dungeon-building
description: |
  Build procedural dungeons using Dungeon Architect plugin + SetupOpenWorldCommandlet.
  Covers DA theme selection, grid config tuning, level streaming integration, and custom theme creation.
  Triggers on "dungeon", "dungeon architect", "DA", "procedural dungeon", "dungeon theme", "level generation". (project)
---

# Dungeon Building Pipeline

## PURPOSE

Generate procedural dungeon levels using Dungeon Architect (DA) v3.4.1 plugin integrated into
`SetupOpenWorldCommandlet`. Dungeons are saved as separate `.umap` files and streamed in at runtime
via `ASLFLevelStreamManager` when the player walks near an overworld entrance structure.

## QUICK REFERENCE

```bash
# 1. Delete old maps (avoids partial-load crash — MEMORY #21)
rm -f C:/scripts/SLFConversion/Content/Maps/L_Dungeon_*.umap C:/scripts/SLFConversion/Content/Maps/L_OpenWorld.umap

# 2. Build
"C:/Program Files/Epic Games/UE_5.7/Engine/Build/BatchFiles/Build.bat" SLFConversionEditor Win64 Development "C:/scripts/SLFConversion/SLFConversion.uproject"

# 3. Generate open world + dungeons
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
  "C:/scripts/SLFConversion/SLFConversion.uproject" \
  -run=SetupOpenWorld -unattended -nosplash -stdout -UTF8Output -FullStdOutLogOutput -NoSound

# 4. Clear save files (player spawns at old location otherwise)
rm -f C:/scripts/SLFConversion/Saved/SaveGames/*.sav

# 5. Open editor, load L_OpenWorld, PIE — walk near dungeon entrance tower to teleport in
```

## PREREQUISITES

| Requirement | Details |
|------------|---------|
| **Dungeon Architect** | v3.4.1+ installed via Epic Launcher → Library → Vault → Install to Engine |
| **Plugin location** | `C:/Program Files/Epic Games/UE_5.7/Engine/Plugins/Marketplace/DungeonAcfe38245a8e7V18/` |
| **Module dependency** | `DungeonArchitectRuntime` in `SLFConversion.Build.cs` (editor deps block) |
| **uproject plugin** | `{"Name": "DungeonArchitect", "Enabled": true}` in `SLFConversion.uproject` |
| **Wasteland Environment** | Optional — for overworld dressing. Content at `/Game/Wasteland/` |

## ARCHITECTURE

```
SetupOpenWorldCommandlet
  ├── Phase 1: CreateLandscape() — Perlin noise heightmap
  ├── Phase 2: ScatterWastelandMeshes() — overworld dressing + entrance towers
  ├── Phase 4: PopulateOverworldActors() — PlayerStart, lights, SkyManager, LevelStreamManagers
  ├── SaveLevel(L_OpenWorld)
  └── Phase 3: GenerateDungeons()
       ├── CreateDungeonLevel(0, L_Dungeon_01) — DA grid builder
       ├── CreateDungeonLevel(1, L_Dungeon_02)
       └── CreateDungeonLevel(2, L_Dungeon_03)

Runtime (PIE):
  Player walks near entrance tower
  → ASLFLevelStreamManager detects proximity (tick-based, 800 unit radius)
  → Screen fades to black
  → ULevelStreamingDynamic::LoadLevelInstance() streams dungeon at offset (0, Y, -5000)
  → Player teleported to dungeon spawn point
  → Screen fades in

Dungeon exit:
  → Teleport back to overworld entrance + Z offset
  → Unload dungeon level
```

## DA INTEGRATION (C++ API)

### Key Classes

| Class | Header | Purpose |
|-------|--------|---------|
| `ADungeon` | `Core/Dungeon.h` | Main dungeon actor — owns builder, config, themes |
| `UDungeonBuilder` | `Core/DungeonBuilder.h` | Abstract base builder |
| `UGridDungeonBuilder` | `Builders/Grid/GridDungeonBuilder.h` | Grid-based room/corridor generator |
| `UDungeonConfig` | `Core/DungeonConfig.h` | Base config (Seed, Instanced, MaxBuildTimePerFrameMs) |
| `UGridDungeonConfig` | `Builders/Grid/GridDungeonConfig.h` | Grid config (NumCells, GridCellSize, RoomAreaThreshold, etc.) |
| `UDungeonThemeAsset` | `Frameworks/ThemeEngine/DungeonThemeAsset.h` | Visual theme — maps markers to meshes |

### Creating a Dungeon in C++ (Commandlet Pattern)

```cpp
#include "Core/Dungeon.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonBuilder.h"
#include "Builders/Grid/GridDungeonBuilder.h"
#include "Builders/Grid/GridDungeonConfig.h"
#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"

// 1. Load a theme
UDungeonThemeAsset* Theme = LoadObject<UDungeonThemeAsset>(nullptr,
    TEXT("/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme"));

// 2. Spawn ADungeon actor
ADungeon* DungeonActor = World->SpawnActor<ADungeon>(Origin, FRotator::ZeroRotator);

// 3. Set builder class + create instance (creates Config, Model, Query)
DungeonActor->BuilderClass = UGridDungeonBuilder::StaticClass();
DungeonActor->CreateBuilderInstance();

// 4. Configure grid settings
UGridDungeonConfig* GridConfig = Cast<UGridDungeonConfig>(DungeonActor->GetConfig());
GridConfig->Seed = 42;
GridConfig->NumCells = 80;                    // Room density (more = bigger dungeon)
GridConfig->GridCellSize = FVector(400, 400, 300); // Cell size in UU (4m x 4m x 3m)
GridConfig->MinCellSize = 2;
GridConfig->MaxCellSize = 5;
GridConfig->RoomAreaThreshold = 5;            // Lower = more rooms
GridConfig->RoomAspectDelta = 0.3f;           // 0=square, 1=elongated
GridConfig->SpanningTreeLoopProbability = 0.15f; // 0=linear, 1=many loops
GridConfig->MaxBuildTimePerFrameMs = 0;       // 0 = build all in one frame (commandlet)
GridConfig->Instanced = false;                // Non-instanced for save

// 5. Assign theme
DungeonActor->Themes.Empty();
DungeonActor->Themes.Add(Theme);

// 6. Build
DungeonActor->BuildDungeon();

// 7. Verify
if (DungeonActor->GetBuilder()->HasBuildSucceeded()) { /* success */ }
```

### Grid Config Parameters

| Parameter | Type | Default | Effect |
|-----------|------|---------|--------|
| `Seed` | int32 | — | Random seed — different seed = different layout |
| `NumCells` | int32 | 100 | Total cells generated. ~80=small, ~150=large |
| `GridCellSize` | FVector | (400,400,200) | Physical size per grid cell. Match your theme meshes |
| `MinCellSize` | int32 | 2 | Min cell dimensions (in grid units) |
| `MaxCellSize` | int32 | 5 | Max cell dimensions |
| `RoomAreaThreshold` | int32 | 8 | Cells larger than this become rooms (lower = more rooms) |
| `RoomAspectDelta` | float | 0.3 | Room squareness (0=square, 1=stretched) |
| `SpanningTreeLoopProbability` | float | 0.15 | Loop frequency (0=linear, 1=many loops) |
| `DoorProximitySteps` | float | — | Remove nearby duplicate doors |
| `NormalMean` / `NormalStd` | float | — | Cell size distribution |
| `LaneWidth` | int32 | — | Corridor connection width |
| `InitialRoomRadius` | float | — | Advanced: cell scatter radius (10-15) |
| `bFastCellDistribution` | bool | false | Use DungeonWidth/Length instead of scatter |

### Current Dungeon Config (3 dungeons)

| Dungeon | Seed | NumCells | Location | Level |
|---------|------|----------|----------|-------|
| 1 | 42 | 80 | (0, 0, -5000) | L_Dungeon_01 |
| 2 | 12387 | 110 | (0, 20000, -5000) | L_Dungeon_02 |
| 3 | 24732 | 140 | (0, 40000, -5000) | L_Dungeon_03 |

## THEMES

### Pre-Built Themes (DA Plugin Content)

| Theme | Path | Style |
|-------|------|-------|
| **D_StarterPackTheme** (current) | `/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleSciFi/DungeonTheme/D_StarterPackTheme` | Sci-fi walls, glass fences, stairs |
| D_MyTheme1 | `/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleShapes/Dungeons/D_MyTheme1` | Basic geometric shapes |
| D_MyTheme_Night | `/DungeonArchitect/Showcase/Samples/Themes/Legacy/SimpleShapes/Dungeons/D_MyTheme_Night` | Night variant |
| T_Theme_Founddry | `/DungeonArchitect/Showcase/Samples/Themes/Foundry/T_Theme_Founddry` | Industrial foundry |
| T_GridFlow_Prehistoric | `/DungeonArchitect/Showcase/Samples/Features/GridFlow/DA_GridFlow/Themes/T_GridFlow_Prehistoric` | Prehistoric stone |

### Using Different Themes Per Dungeon

To give each dungeon a unique look, assign different themes:

```cpp
// In CreateDungeonLevel(), select theme by dungeon index
const TArray<FString> ThemePerDungeon = {
    TEXT("/DungeonArchitect/.../D_StarterPackTheme"),   // Dungeon 1: sci-fi
    TEXT("/DungeonArchitect/.../T_Theme_Founddry"),     // Dungeon 2: foundry
    TEXT("/DungeonArchitect/.../T_GridFlow_Prehistoric"),// Dungeon 3: prehistoric
};
UDungeonThemeAsset* Theme = LoadObject<UDungeonThemeAsset>(nullptr, *ThemePerDungeon[DungeonIndex]);
```

### Creating Custom Themes (Editor Workflow)

DA themes are created visually in the editor (not C++):

1. **Content Browser** → Right-click → Dungeon Architect → Dungeon Theme
2. Opens a **node graph editor**
3. DA emits **markers** during generation: `Wall`, `Floor`, `Pillar`, `Door`, `Stair`, `Ground`, `Fence`, etc.
4. For each marker, add a **Mesh Node** and assign your static mesh + material
5. Use **Selection Rules** to add variety (random mesh selection, probability)
6. Use **Transform Rules** for random rotation/offset
7. Save the theme asset, then reference its path in the commandlet

**Marker names** for Grid builder:
`Wall`, `WallSeparator`, `Fence`, `Floor`, `Ceiling`, `Door`, `Stair`, `Ground`, `WallHalf`, `WallEnd`

### ClusterThemes (Mixed Themes Within One Dungeon)

```cpp
// ADungeon::ClusterThemes allows region-based theme variation
FClusterThemeInfo ClusterInfo;
ClusterInfo.ThemeAsset = LoadObject<UDungeonThemeAsset>(nullptr, TEXT("..."));
ClusterInfo.ClusterThemeName = TEXT("DeepSection");
DungeonActor->ClusterThemes.Add(ClusterInfo);

// Also enable on config:
GridConfig->bEnableClusteredTheming = true;
```

## LEVEL STREAMING (ASLFLevelStreamManager)

### Entrance Detection
- **Primary**: Tick-based proximity check (4x/sec, 800 unit trigger radius)
- **Secondary**: Box overlap trigger (800x800x400 extent)
- Both methods guarded by `bTransitionInProgress` flag

### Transition Flow
1. Cache player character reference
2. Fade screen to black (`StartCameraFade`)
3. `ULevelStreamingDynamic::LoadLevelInstance()` with dungeon level name + world offset
4. `OnDungeonLevelLoaded` callback → teleport player to `DungeonWorldOffset + DungeonSpawnOffset`
5. Fade screen back in

### Key Properties (set in commandlet)

```cpp
// Per LevelStreamManager actor:
LSM->DungeonLevelName = TEXT("/Game/Maps/L_Dungeon_01");
LSM->DungeonDisplayName = TEXT("Dungeon 1");
LSM->DungeonWorldOffset = FVector(0, 0, -5000);      // Underground offset
LSM->DungeonSpawnOffset = FVector(0, 0, 200);         // Spawn inside dungeon
LSM->OverworldReturnLocation = EntranceLocation;      // Where to teleport back
LSM->FadeDuration = 0.5f;
LSM->bIsDungeonExit = false;                          // true for exit triggers
```

## TROUBLESHOOTING

### DA Theme Not Found
```
Theme not found at: /DungeonArchitect/...
```
**Cause**: Plugin content mount path doesn't match expected path.
**Fix**: The commandlet falls back to asset registry search (`GetAssetsByClass(UDungeonThemeAsset)`). Check the log for discovered theme paths and update the hardcoded paths.

### BuildDungeon() Fails Silently
**Cause**: No theme assigned, or theme has empty compiled graph.
**Fix**: Ensure `DungeonActor->Themes` has at least one valid theme before calling `BuildDungeon()`. Pre-built DA themes have compiled graphs; custom themes must be saved from the editor (which compiles the graph).

### Player Falls Through Dungeon Floor
**Cause**: DA theme meshes might not have collision, or dungeon is at wrong Z offset.
**Fix**: Ensure `DungeonWorldOffset.Z` matches `DungeonSpawnOffset.Z` range. The fallback generator uses Cube meshes (have collision) instead of Plane meshes (no collision).

### Dungeon Entrance Not Triggering
**Cause**: Player not close enough, or `bTransitionInProgress` stuck true.
**Fix**: Walk directly into the entrance tower structure. The tick-based proximity detector triggers at 800 units (8 meters). Check log for `[LevelStreamMgr]` messages.

### "Partially Loaded" Package Error
**Cause**: Old .umap file exists on disk when commandlet tries to create same package.
**Fix**: Delete old `.umap` files before running commandlet (MEMORY #21):
```bash
rm -f C:/scripts/SLFConversion/Content/Maps/L_Dungeon_*.umap
```

### Player Spawns at Old Location
**Cause**: Save file overrides PlayerStart position.
**Fix**: Delete save files: `rm -f C:/scripts/SLFConversion/Saved/SaveGames/*.sav`

## FILE REFERENCE

### C++ Source Files

| File | Purpose |
|------|---------|
| `Source/SLFConversion/SetupOpenWorldCommandlet.h` | Commandlet header — phases, helpers, FlatArea struct |
| `Source/SLFConversion/SetupOpenWorldCommandlet.cpp` | Main commandlet — landscape, dressing, DA dungeons, actors |
| `Source/SLFConversion/Blueprints/Actors/SLFLevelStreamManager.h` | Level stream manager — trigger, teleport, fade properties |
| `Source/SLFConversion/Blueprints/Actors/SLFLevelStreamManager.cpp` | Runtime streaming logic — proximity detect, fade, teleport |
| `Source/SLFConversion/SLFConversion.Build.cs` | Module deps — includes DungeonArchitectRuntime |

### Generated Assets

| Asset | Path |
|-------|------|
| Open World | `/Game/Maps/L_OpenWorld.umap` |
| Dungeon 1 | `/Game/Maps/L_Dungeon_01.umap` |
| Dungeon 2 | `/Game/Maps/L_Dungeon_02.umap` |
| Dungeon 3 | `/Game/Maps/L_Dungeon_03.umap` |

### DA Plugin Paths

| Item | Path |
|------|------|
| Plugin root | `C:/Program Files/Epic Games/UE_5.7/Engine/Plugins/Marketplace/DungeonAcfe38245a8e7V18/` |
| Runtime headers | `.../Source/DungeonArchitectRuntime/Public/` |
| Core headers | `.../Public/Core/Dungeon.h`, `DungeonConfig.h`, `DungeonBuilder.h` |
| Grid headers | `.../Public/Builders/Grid/GridDungeonBuilder.h`, `GridDungeonConfig.h` |
| Theme headers | `.../Public/Frameworks/ThemeEngine/DungeonThemeAsset.h` |
| Pre-built themes | `.../Content/Showcase/Samples/Themes/` |

### Other DA Builder Types (Future)

| Builder | Class | Use Case |
|---------|-------|----------|
| Grid | `UGridDungeonBuilder` | Classic room + corridor dungeons (current) |
| GridFlow | — | Flow-graph driven grid layouts |
| SimpleCity | — | City block generation |
| CellFlow | — | Cell-based pattern matching |
| SnapGridFlow | — | Module-based snap-together dungeons |
| Canvas | — | 2D dungeon rendering on canvas |
