---
name: bunker-dungeon
description: Generate sci-fi bunker/spaceship interior dungeons using Dungeon Architect CellFlow builder with Foundry theme. Fully enclosed rooms with walls, ceilings, floors, doors, stairs. Supports material overrides for visual variety. Triggers on "bunker", "dungeon", "spaceship interior", "foundry", "sci-fi dungeon", "generate dungeon". (project)
---

# Bunker Dungeon Generation — CellFlow + Foundry Theme

## PURPOSE

Generate fully enclosed sci-fi dungeon interiors (bunkers, spaceship corridors, research facilities) using Dungeon Architect's CellFlow builder with the Foundry theme pack. Produces connected rooms with proper walls, ceilings, floors, doors, stairs, and lighting.

## QUICK START

```bash
# Generate a bunker dungeon
UnrealEditor-Cmd.exe SLFConversion.uproject \
  -run=AutoBuildDungeon -Biome=bunker -Seed=42 -unattended -nosplash

# Different seed = different layout
-run=AutoBuildDungeon -Biome=bunker -Seed=123

# Custom output path
-run=AutoBuildDungeon -Biome=bunker -Seed=42 -OutMap=/Game/Maps/L_MyBunker
```

Output: `/Game/Maps/L_Bunker` (10MB+ map with full dungeon geometry)

## HOW IT WORKS

### Architecture
1. **CellFlow_Graph_Foundry** — DA's pre-built flow graph defines room topology (branching paths, corridors, elevation changes)
2. **T_Theme_Founddry** — Maps CellFlow markers to Foundry sci-fi meshes (walls, floors, ceilings, doors, stairs, pillars, lights)
3. **CF_MarkerSettings_Foundry** — Marker configuration for CellFlow grid generation
4. **No voxel carving** (`bCarveVoxels=false`) — theme meshes provide all geometry

### Key Config (from DA demo Map_DA_Theme_Foundry)
```cpp
GridSize = FVector(800, 800, 200);   // Wide rooms, low ceiling (bunker feel)
bCarveVoxels = false;                 // Theme-only mode
MaxRetries = 50;                      // More seed attempts for valid layout
```

### Grid Size Guide
| Style | GridSize | Feel |
|-------|----------|------|
| Tight corridors | (600, 600, 150) | Cramped submarine |
| Standard bunker | (800, 800, 200) | DA demo default |
| Open facility | (1000, 1000, 300) | Research complex |
| Hangar/warehouse | (1200, 1200, 400) | Tall industrial space |

## DA SHOWCASE THEMES (Available in Plugin)

### CellFlow-Ready (fully enclosed, our approach)

| Theme | Assets | Style | Best For |
|-------|--------|-------|----------|
| **Foundry** | CellFlow_Graph_Foundry + T_Theme_Founddry + CF_MarkerSettings_Foundry | Sci-fi metallic corridors, stairs, doors, neon lights | Bunkers, space stations, facilities |
| **OilRig** | Grid_CellFlowGraph + Theme_OilRig_CellFlow + OilRigMarkerSettings | Industrial platforms, catwalks, cranes | Industrial zones, refineries, docks |

### Grid Builder (rooms + corridors, open ceiling)

| Theme | Assets | Style |
|-------|--------|-------|
| **Candy** | D_Candy_v2 | Stylized interior (rooms, doors, furniture) |
| **CityOfBrass** | Theme folder (Palace, Garden, Outskirt variants) | Arabian palace, outdoor gardens |
| **StarterPack** | D_StarterPackTheme | Basic sci-fi (legacy, simpler than Foundry) |

### Voxel Cave (organic, our existing biomes)

| Biome | Style | Command |
|-------|-------|---------|
| Stone | Default rocky cave | `-Biome=stone` |
| Lava | Volcanic, red/orange | `-Biome=lava` |
| Ice | Frozen grotto, blue | `-Biome=ice` |
| Fungal | Bio-luminescent, purple/green | `-Biome=fungal` |
| Abyss | Dark void, minimal light | `-Biome=abyss` |

### Game Starter Maps (pre-built, playable)

| Map | Path | Type |
|-----|------|------|
| Foundry FPS | `StarterModes/FoundryGame_Starter_FPS` | First-person sci-fi |
| Foundry TPS | `StarterModes/FoundryGame_Starter_TPS` | Third-person sci-fi |
| Ice Cave FPS | `StarterModes/IceCaveGame_Starter_FPS` | Voxel ice cave |
| Mountain Cave TPS | `StarterModes/MountainCaveGame_Starter_TPS` | Voxel mountain |

## FOUNDRY ASSET INVENTORY

### Structural Meshes (`AssetPacks/Foundry/Meshes/`)
- **Walls**: Scifi2_WallA (flat, corner in/out, pipe variants), WallB, WallE (3 variants + half + prop)
- **Floors**: Scifi2_Ground, Ground_TrimA
- **Ceilings**: Scifi2_Ceiling (flat, edge trim, corner in/out)
- **Doors**: ScifiDoorA_02
- **Stairs**: Scifi2_StairA + StairA_Guard (railing)
- **Pillars**: Scifi2_PillarA/B/C/C2 + corner variants

### Props (`AssetPacks/Foundry/Meshes/Props/`)
- **Lighting**: Scifi2_Ceiling_Light (3 variants), BP_Tubelight (white/blue/orange)
- **Ceiling**: Ceiling2x_Prop_A/B, CeilingFan, Scifi2_Ceiling2x_Frame
- **Storage**: Scifi2_Crate_A (single + clusters), Prop_ContainerA/B
- **Industrial**: Greeble_Pipe, PipeCage, PipeCluster, Pipe_Vertical, GirderA
- **Ground**: Scifi2_Prop_GroundPit (1x1, 2x1), LargePipeCluster variants
- **Barriers**: Scifi2_Fence, Scifi2_WallPanelB
- **Platforms**: BP_Platform_2x2A/B

### Materials (`AssetPacks/Foundry/Materials/`)
- **Metal surfaces**: Metal_Black, Metal_Dark, Metal_Grey, Metal_Ground, Metal_Ceiling
- **SciFi2**: DoorA, PillarB/C/C2, Stair materials
- **Lights**: Plasma_Blue, Plasma_Green, Plasma_Red, Plasma_Crane
- **Props**: Container/Crane/Vent materials
- **Master**: M_DA_Substance_Master (base PBR material)

## MATERIAL OVERRIDE SYSTEM (Planned)

After dungeon builds, iterate spawned meshes and swap materials by mesh name:
```cpp
// Detect mesh type from name:
// "Scifi2_Wall*"    → apply wall material
// "Scifi2_Ground*"  → apply floor material
// "Scifi2_Ceiling*" → apply ceiling material
// "ScifiDoor*"      → apply door material

// Biome material variants:
// -Style=rusty    → weathered industrial textures
// -Style=clean    → sterile white lab panels
// -Style=alien    → organic/bio textures on angular geometry
// -Style=cryo     → frost/ice overlay materials
// -Style=emergency → dark base + red emissive strips
```

## MARKETPLACE INTEGRATION

Marketplace assets drop in at three levels:

1. **Materials only** (easiest): Buy a material pack, point material override paths at them. All geometry stays the same.

2. **Props** (medium): Duplicate T_Theme_Founddry, add marketplace meshes to marker nodes. DA spawns them automatically during generation. Or add to post-build gameplay placement.

3. **Full theme** (advanced): Create a new theme asset mapping all markers to marketplace meshes. Requires DA theme editor (editor UI, not commandlet).

## DUNGEON INSPECTION

Dump all properties from any DA demo map:
```bash
MSYS_NO_PATHCONV=1 UnrealEditor-Cmd.exe SLFConversion.uproject \
  -run=AutoBuildDungeon \
  -DumpMap=/DungeonArchitect/Showcase/Samples/Themes/Foundry/Map_DA_Theme_Foundry \
  -unattended -nosplash
# Output: C:/scripts/SLFConversion/dungeon_dump.json
```

## LIGHTING PRESETS

| Mood | PathLight | Ambient | SkyLight |
|------|-----------|---------|----------|
| Standard | (0.8, 0.9, 1.0) 25k | (0.4, 0.5, 0.7) 10k | (0.5, 0.6, 0.8) 10 |
| Emergency | (1.0, 0.2, 0.1) 30k | (0.3, 0.05, 0.05) 5k | (0.2, 0.05, 0.05) 3 |
| Cryo | (0.5, 0.7, 1.0) 20k | (0.4, 0.6, 0.9) 8k | (0.6, 0.7, 1.0) 12 |
| Bio-hazard | (0.2, 1.0, 0.3) 22k | (0.1, 0.4, 0.15) 6k | (0.15, 0.3, 0.1) 5 |
| Furnace | (1.0, 0.5, 0.1) 28k | (0.5, 0.2, 0.05) 8k | (0.6, 0.3, 0.1) 6 |

## KEY FILES

| File | Purpose |
|------|---------|
| `AutoBuildDungeonCommandlet.h/cpp` | Biome presets, CellFlow generation, level building |
| `SLFProceduralCaveManager.h/cpp` | Runtime dungeon manager (CellFlow + voxel) |
| `SLFCaveMazeBuilder.h/cpp` | Post-build gameplay placement (enemies, traps, bosses) |
| `SLFLevelStreamManager.h/cpp` | Level streaming integration |
| `.claude/reference/tae-weapon-trace-pipeline.md` | Related: enemy weapon traces for dungeon combat |

## INTEGRATION WITH GAME SYSTEMS

The generated dungeon maps integrate with existing systems:
- **Level streaming**: `ASLFLevelStreamManager` proximity trigger → load dungeon
- **Boss doors**: `ASLFBossDoor` (blue/purple/orange tiers)
- **Shortcut gates**: `ASLFShortcutGate` (one-way unlocks)
- **Traps**: `ASLFTrapBase` (configurable damage/timing)
- **Puzzles**: `ASLFPuzzleMarker` (interaction points)
- **Enemy spawning**: Post-build placement via room designations
- **Boss immunity**: `ASLFBossPhaseObjective` (destroy-to-remove-shield)
