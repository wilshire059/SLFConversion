# Procedural Level Pipeline

Data-driven dungeon generation system using CellFlow + voxel carving with per-biome PBR materials.

---

## Architecture Overview

```
FDungeonBiomeConfig (C++ struct — per-biome settings)
        │
        ▼
AutoBuildDungeonCommandlet -Biome=lava -Seed=42
        │
        ├── CellFlow Asset (Voronoi layout + hub-and-spoke paths)
        ├── Theme Asset (visual meshes, lights)
        ├── Biome Material (MIC with PBR textures from disk)
        └── Output Map (L_LavaCavern, L_IceGrotto, etc.)
              │
              └── ADungeon + voxel carving + beacon lights + NavMesh
```

**One biome preset per level. One commandlet invocation per level.**

---

## Quick Start — Generate a Level

```bash
# Generate a single biome level
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=AutoBuildDungeon -Biome=lava -Seed=42 -unattended -nosplash

# Generate all 5 biomes
generate_all_biomes.bat
```

Available biomes: `stone`, `lava`, `ice`, `fungal`, `abyss`

---

## 1. Biome Config — FDungeonBiomeConfig

All tunable parameters live in `AutoBuildDungeonCommandlet.h`:

```cpp
struct FDungeonBiomeConfig
{
    FString BiomeName;           // Display name ("Lava Cavern")

    // Canvas (Voronoi layout)
    FIntPoint WorldSize;         // Grid dimensions (80x80 for lava)
    int32 NumPoints;             // Voronoi seed count (1000 for lava)
    int32 NumRelaxIterations;    // Lloyd relaxation (12 default)
    float EdgeConnectionThreshold;
    float MinGroupArea;
    float ClusterBlobbiness;
    FVector GridSize;            // Cell size in cm (400,400,400)

    // Entrance / Hub / Spokes
    int32 EntrancePathSize;      // Nodes in entrance tunnel
    int32 HubMinSize, HubMaxSize;
    int32 SpokeMinSize, SpokeMaxSize;

    // Starfish distance enforcement
    float MinHubBossDistance;     // Min hub-to-boss distance
    float MinHubTerminalDistance; // Min hub-to-terminal distance
    int32 MaxSeedRetries;        // Seed retry limit (25)

    // Voxel noise
    float VoxelSize, WallThickness, NoiseAmplitude;
    int32 NoiseOctaves;
    float NoiseFloorScale, NoiseCeilingScale;
    bool bDomainWarp;
    float DomainWarpStrength, CeilingExtraHeight;

    // Lighting (biome-colored)
    FLinearColor PathLightColor, TorchColor, AmbientColor;
    FLinearColor SkyLightColor, DirLightColor;
    float PathLightIntensity, TorchIntensity, AmbientIntensity;
    float SkyLightIntensity, DirLightIntensity;

    // Materials
    FString VoxelMaterialPath;     // Fallback (MI_Rock_03)
    FString FallbackMaterialPath;

    // Biome PBR Material (from downloaded texture sets)
    FString BiomeTextureDiskPath;  // Disk folder with PBR textures
    FString BiomeMaterialPath;     // UE asset path for generated MIC

    // Output paths
    FString FlowAssetPath, ThemeAssetPath, MapPath;
};
```

---

## 2. Biome Presets — Current Configuration

Presets are defined in `GetBiomePreset()` in `AutoBuildDungeonCommandlet.cpp`.

| Biome | WorldSize | NumPts | Spokes | Material | Map |
|-------|-----------|--------|--------|----------|-----|
| **Stone** (default) | 120x120 | 1500 | 15-25 | MI_Rock_03 | L_CaveTest |
| **Lava** | 80x80 | 1000 | 8-15 | M_Lava (lava rock cliff) | L_LavaCavern |
| **Ice** | 100x100 | 1300 | 10-20 | M_Ice (snow) | L_IceGrotto |
| **Fungal** | 140x140 | 2000 | 15-25 | M_Fungal (mossy ground) | L_FungalDepths |
| **Abyss** | 60x60 | 800 | 6-12 | M_Abyss (slate) | L_Abyss |

### Scaling Rules

Smaller canvases need proportionally shorter spokes and lower distance thresholds:
- Lava (80x80): spokes 8-15, distances 8000/5000
- Abyss (60x60): spokes 6-12, distances 5000/3000, edge threshold 0.08

---

## 3. PBR Material Pipeline

### How Biome Materials Work

Biome materials are **MaterialInstanceConstant** (MIC) assets parented to `MI_Rock_03`. The MIC overrides texture parameters with imported PBR textures. This avoids shader compilation issues in commandlet mode (no RHI).

**Parameter mapping** (MI_Rock_03 uses short names):
| Parameter | Maps To |
|-----------|---------|
| `BC` | BaseColor texture |
| `N` | Normal map |
| `ORM` | Roughness (packed ORM channel) |

### Texture Set Requirements

Download 2K texture sets from [Fab](https://www.fab.com/) as **Texture Set** format. Place in a folder on disk.

**Supported naming conventions:**
- Quixel/Fab: `*_BaseColor.jpg`, `*_Normal.jpg`, `*_Roughness.jpg`
- Substance: `*_basecolor.png`, `*_normal.png`, `*_roughness.png`
- Both JPG and PNG supported

**Required textures:** BaseColor (required), Normal (optional), Roughness (optional)
**Optional:** Metallic (set if parent material has a Metallic param)

### Adding a New Biome Material

1. **Download textures** from Fab to a local folder:
   ```
   C:/Users/james/Downloads/cave_materials/extracted/my_new_texture_2k/
   ├── My_Texture_2K_BaseColor.jpg
   ├── My_Texture_2K_Normal.jpg
   └── My_Texture_2K_Roughness.jpg
   ```

2. **Add to biome preset** in `AutoBuildDungeonCommandlet.cpp` → `GetBiomePreset()`:
   ```cpp
   Cfg.BiomeTextureDiskPath = TEXT("C:/Users/james/Downloads/cave_materials/extracted/my_new_texture_2k");
   Cfg.BiomeMaterialPath = TEXT("/Game/Dungeons/Materials/M_MyBiome");
   ```

3. **Run commandlet** — textures are imported and MIC is created automatically:
   ```bash
   UnrealEditor-Cmd.exe SLFConversion.uproject -run=AutoBuildDungeon -Biome=mybiome -Seed=42 -unattended -nosplash
   ```

### Generated Assets

For each biome with textures, the pipeline creates:
```
Content/Dungeons/Materials/
├── M_Lava.uasset              (MaterialInstanceConstant)
├── T_Lava_BaseColor.uasset    (Texture2D)
├── T_Lava_Normal.uasset       (Texture2D)
└── T_Lava_Roughness.uasset    (Texture2D)
```

### Why MIC, Not UMaterial

`UMaterial` requires shader compilation via the rendering backend (RHI). Commandlets run without RHI (`-nosplash`), so raw materials have no compiled shaders → checkered texture in editor. `MaterialInstanceConstant` inherits compiled shaders from its parent → renders immediately.

---

## 4. Adding a New Biome Preset

### Step 1: Add the preset in `GetBiomePreset()`

```cpp
else if (BiomeName.Equals(TEXT("crystal"), ESearchCase::IgnoreCase))
{
    Cfg.BiomeName = TEXT("Crystal Caverns");
    Cfg.WorldSize = FIntPoint(90, 90);
    Cfg.NumPoints = 1100;
    Cfg.SpokeMinSize = 10;
    Cfg.SpokeMaxSize = 18;
    Cfg.NoiseAmplitude = 180.0f;
    Cfg.WallThickness = 130.0f;
    Cfg.MinHubBossDistance = 9000.0f;
    Cfg.MinHubTerminalDistance = 6000.0f;

    // Lighting
    Cfg.PathLightColor = FLinearColor(0.5f, 0.8f, 1.0f);
    Cfg.TorchColor = FLinearColor(0.3f, 0.6f, 1.0f);
    // ... other light colors ...

    // PBR textures from disk
    Cfg.BiomeTextureDiskPath = TEXT("C:/path/to/crystal_textures_2k");
    Cfg.BiomeMaterialPath = TEXT("/Game/Dungeons/Materials/M_Crystal");

    // Output
    Cfg.FlowAssetPath = TEXT("/Game/Dungeons/CellFlow/CF_CrystalCaverns");
    Cfg.ThemeAssetPath = TEXT("/Game/Dungeons/Themes/DT_CrystalCaverns");
    Cfg.MapPath = TEXT("/Game/Maps/L_CrystalCaverns");
}
```

### Step 2: Build and generate

```bash
Build.bat SLFConversionEditor Win64 Development "C:/scripts/SLFConversion/SLFConversion.uproject"
UnrealEditor-Cmd.exe SLFConversion.uproject -run=AutoBuildDungeon -Biome=crystal -Seed=42 -unattended -nosplash
```

### Step 3: Add to `generate_all_biomes.bat`

```batch
echo [6/6] Crystal Caverns...
%UE% %PROJ% -run=AutoBuildDungeon -Biome=crystal -Seed=42 -unattended -nosplash
```

---

## 5. Changing Textures for an Existing Biome

To swap lava's textures with a different set:

1. Download new textures to a folder on disk
2. Update `BiomeTextureDiskPath` in the lava preset:
   ```cpp
   Cfg.BiomeTextureDiskPath = TEXT("C:/path/to/new_lava_rock_2k");
   ```
3. Delete old texture/material assets:
   ```bash
   rm -f C:/scripts/SLFConversion/Content/Dungeons/Materials/M_Lava.uasset
   rm -f C:/scripts/SLFConversion/Content/Dungeons/Materials/T_Lava_*.uasset
   ```
4. Rebuild and regenerate:
   ```bash
   Build.bat SLFConversionEditor Win64 Development "C:/scripts/SLFConversion/SLFConversion.uproject"
   UnrealEditor-Cmd.exe SLFConversion.uproject -run=AutoBuildDungeon -Biome=lava -Seed=42 -unattended -nosplash
   ```

---

## 6. Commandlet Arguments

| Argument | Description | Default | Example |
|----------|-------------|---------|---------|
| `-Biome=` | Biome preset name | `stone` | `-Biome=lava` |
| `-Seed=` | Random seed for layout | `42` | `-Seed=777` |
| `-OutMap=` | Override output map path | From preset | `-OutMap=/Game/Maps/L_Custom` |

### Full Command Line

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=AutoBuildDungeon ^
  -Biome=lava ^
  -Seed=42 ^
  -unattended -nosplash
```

### Generating Multiple Seeds for Comparison

```bash
for %%S in (42 100 256 777 999) do (
    UnrealEditor-Cmd.exe SLFConversion.uproject ^
      -run=AutoBuildDungeon -Biome=lava -OutMap=/Game/Maps/L_Lava_%%S -Seed=%%S -unattended -nosplash
)
```

---

## 7. Hub-and-Spoke Topology

Each level uses a starfish layout:

```
                    ┌─── Corridor (dead end)
                    │
Entrance ──── Hub ──┼─── Boss Arena (large, flat floor)
                    │
                    ├─── Treasure Room (small, gold beacon)
                    │
                    ├─── Trap Room (medium, red beacon)
                    │
                    └─── Mini-Boss Arena (medium, blue beacon)
```

- **Entrance tunnel**: Linear path from spawn to hub
- **Hub**: Central room with high ceiling, white beacon
- **Spokes**: 5 branches radiating outward to terminal arenas
- **Starfish enforcement**: Seeds rejected if any terminal is too close to hub (best-of-N fallback)
- **Door carving volumes**: Placed at path boundaries for chokepoints
- **Two-pass build**: Pass 1 generates layout + door volumes, Pass 2 rebuilds with volumes

---

## 8. Texture Sources

Current downloaded textures at `C:/Users/james/Downloads/cave_materials/extracted/`:

| Biome | Folder | Source |
|-------|--------|--------|
| Lava | `lava_icelandic_rock_cliff_smokagcp_2k/` | Fab — Quixel JPGs |
| Ice | `snow_qbarr0_2k/` | Fab — Quixel JPGs |
| Fungal | `mossy_rocky_ground_vcqhcig_2k/` | Fab — Quixel JPGs |
| Abyss | `slate/` | Fab — Substance PNGs |

**Backup options available:**
- `trampled_snow_vcqnfdk_2k/` — alternate ice
- `lava_icelandic_sand_with_pebbles_tbjibefn_2k/` — alternate lava

---

## 9. Key Files

| File | Purpose |
|------|---------|
| `AutoBuildDungeonCommandlet.h` | `FDungeonBiomeConfig` struct, commandlet class |
| `AutoBuildDungeonCommandlet.cpp` | `GetBiomePreset()`, `CreateBiomeMaterial()`, level generation |
| `Dungeon/SLFProceduralCaveManager.h/cpp` | Runtime cave manager (PIE builds) |
| `generate_all_biomes.bat` | Batch script to generate all 5 biome levels |
| `.claude/reference/procedural-level-pipeline.md` | This document |

---

## 10. Troubleshooting

### "CellFlow failed to generate" for small canvases
Reduce spoke lengths (`SpokeMinSize`/`SpokeMaxSize`) proportionally to canvas size. Increase `NumPoints` and `EdgeConnectionThreshold`.

### Checkered/default material on voxel walls
- Ensure `BiomeTextureDiskPath` points to a folder with BaseColor/Normal/Roughness images
- Material must be created BEFORE `BuildDungeon()` (set as `VMS.Material`)
- Use `MaterialInstanceConstant` (MIC), not raw `UMaterial` — commandlets have no RHI for shader compilation

### Texture shows as 0x0 in commandlet log
Normal — `GetSizeX()` returns 0 in commandlet mode (no GPU resources). The source data IS saved. Textures render correctly when loaded in editor.

### All terminals too close to hub
Increase `MinHubBossDistance`/`MinHubTerminalDistance`, or increase canvas `WorldSize`. The commandlet tries 25 seeds and uses the best layout.
