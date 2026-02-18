# SLFConversion Project - Claude Code Context

---

## Context Compaction Recovery

If your context was just compacted:
1. Read `C:/scripts/SLFConversion/migration_tracker.md` for current status
2. Check the "Priority Queue" for the current item
3. Use `/slf-migration [BLUEPRINT_NAME]` to extract and implement logic
4. Complete 20-pass validation, update tracker before ending session

---

## CRITICAL MIGRATION RULES (NON-NEGOTIABLE)

1. **ALL Blueprint logic MUST be in C++** - NO stubs, NO TODOs, NO "implement later"
2. **Use `/slf-migration` skill** - Extracts actual Blueprint graph logic
3. **Migrate dependencies first** - Check migration_tracker.md for order
4. **Preserve ALL defaults** - Mesh assignments, skeleton, component settings
5. **AnimGraphs are PRESERVED** - Only clear EventGraph in AnimBPs
6. **SURGICAL MIGRATION ONLY** - Never do full Content/ restore
7. **BLUEPRINT SCS OWNS COMPONENTS** - C++ only caches references (see Component Ownership)
8. **USE C++ AUTOMATION, NOT PYTHON** - See "Never Use Unreal Python API" below
9. **NEVER use reflection** for Blueprint struct properties - Always migrate to C++ properties

---

## NEVER Use Unreal Python API (CRITICAL)

**NEVER use the Unreal Python API for ANYTHING. It is too limited and almost always fails.**

The Python API has read-only properties, missing functions, version-breaking changes, and unreliable behavior. Every time we've tried it, it has failed:
- `SocketName`, `Sockets`, `reference_skeleton`, `bone_tree` - all read-only or protected
- `generated_class` doesn't work in commandlets
- `AnimationLibrary.add_blend_space_sample` missing in UE5.7
- `skeleton.bone_tree[i].get_editor_property("name")` throws "property is protected"
- `SkeletalMeshComponent.skeletal_mesh` deprecated without warning

### What To Do Instead
**ALWAYS extend `SLFAutomationLibrary` in C++.** If a C++ function doesn't exist for what you need, ADD ONE:
1. Add function to `SLFAutomationLibrary.h/.cpp`
2. Register console command in `SLFConversion.cpp` or `SLFPIETestRunner.cpp`
3. Invoke via `-ExecCmds="SLF.MyCommand arg1 arg2"`

```bash
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -ExecCmds="SLF.Reparent /Game/BP/B_Test /Script/SLFConversion.CppClass" ^
  -unattended -nosplash
```

### No Exceptions
Do NOT use Python even as a "thin wrapper". The Python API is too unreliable. Use C++ console commands for everything: reparenting, socket creation, validation, screenshot capture, log export, input simulation.

---

## C++ Automation Library

`SLFAutomationLibrary.h` provides functions for:
- **Reparenting**: `ReparentBlueprint()`
- **Cleanup**: `ClearEventGraphs()`, `ClearAllBlueprintLogic()`, `RemoveVariable()`
- **Data Extraction/Apply**: `ExtractBaseStats()`, `ApplyBaseStatsFromCache()`
- **AnimBP**: `SetupBossAnimGraph()`, `ConfigureBlendSpace1DSamples()`, `BatchRetargetAnimations()`
- **Validation**: `ValidateBlueprintMigration()`, `DiagnoseAnimBP()`
- **Data Assets**: `ApplyWeaponStatusEffects()`, `ApplyArmorStatChanges()`
- **Testing**: `SLFTestManager` (spawn, screenshot, log, validate)

Console commands registered in `SLFConversion.cpp`:
- `SLF.Reparent`, `SLF.ClearLogic`, `SLF.Save`
- `SLF.Test.*` (Actions, Movement, SaveLoad, NPCDialog, SpawnEnemy, Screenshot)
- `SLF.Sim*` (SimKey, SimAttack, SimDodge, SimMove, SimCrouch)

---

## Component Ownership Strategy

**Blueprint SCS owns all components. C++ only caches references at runtime.**

```cpp
// WRONG - Blueprint SCS already has "InteractionCollision"
InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));

// RIGHT - Cache in BeginPlay
void AB_Interactable::BeginPlay()
{
    Super::BeginPlay();
    TArray<USphereComponent*> SphereComponents;
    GetComponents<USphereComponent>(SphereComponents);
    for (USphereComponent* SC : SphereComponents)
    {
        if (SC && SC->GetName().Contains(TEXT("Interaction")))
        {
            InteractionCollision = SC;
            break;
        }
    }
}
```

**Use CreateDefaultSubobject when:** C++ creates NEW components not in Blueprint SCS, or pure C++ actors.

---

## Surgical Migration

**NEVER do a full Content/ restore. Copy ONLY the specific assets being migrated.**

```bash
# RIGHT - surgical
cp "C:/scripts/bp_only/Content/.../DA_ExampleArmor.uasset" "C:/scripts/SLFConversion/Content/.../"

# WRONG - causes regressions
Remove-Item 'C:\scripts\SLFConversion\Content\*' -Recurse
```

### Pattern
1. Make C++ changes
2. Extract data from backup (cache mesh/icon/stat data BEFORE restore)
3. Copy ONLY affected assets from backup
4. Reparent ONLY those assets
5. Apply cached data
6. Test the specific feature

---

## Multi-Phase Migration Order

Assets must be processed in this specific order to avoid cascade loading crashes:

```
Phase 0A: Clear Priority AnimNotifyStates (ANS_InputBuffer, ANS_RegisterAttackSequence, etc.)
Phase 0A2: Save immediately
Phase 0B: Clear AnimBlueprints (EventGraph only, keep variables)
Phase 0B2: Save immediately
Phase 0C: Clear NPC Characters
Phase 1+: Process all other Blueprints normally
```

**Why:** AnimNotifyStates call C++ functions with "?" suffix pins. C++ can't use "?". By clearing FIRST, later loads get the cleared versions.

---

## Quick Status (Update Before Ending Session)

- **Completed**: AC_InteractionManager
- **Current**: AC_CombatManager
- **Next**: AC_InputBuffer
- **Blocked**: B_Soulslike_Character (waiting on remaining AC_* components)
- **Tracker**: `C:/scripts/SLFConversion/migration_tracker.md`

| Metric | Value |
|--------|-------|
| Blueprints Reparented | 306 |
| AnimBPs Migrated | 3 |
| Widgets Migrated | 100+ |
| Components Migrated | 20+ |

---

## Animation Migration Pipeline

End-to-end pipeline for importing Elden Ring character animations into UE5.

**5 Phases:**
1. **Extract** (Blender headless) - ANIBND/CHRBND -> FBX + TAE JSON
2. **Import** (UE5 commandlet) - FBX -> USkeleton + UAnimSequence + AnimNotifies
3. **Setup** (C++ Automation) - AnimBP, BlendSpace, Montages, AI Abilities via `SLFAutomationLibrary`
4. **Reskin** (Blender headless) - Re-skin custom mesh to target skeleton (auto-weights + FLVER transfer)
5. **Test** (PIE) - Visual validation with screenshots, AnimNotify monitoring, log export

**Scripts:** `C:\scripts\elden_ring_tools\` (extract_animations.py, build_proper_blend.py, batch_import_anims.py, reskin_pipeline.py, validate_reskin.py)

**Process Docs:**
- `.claude/reference/animation-pipeline.md` - Full 5-phase pipeline, coordinate space rules, .blend builder
- `memory/elden_ring_mesh_pipeline.md` - Mesh import details, BONE_CoB chain rule, Blender API
- `memory/elden_ring_tools.md` - Script reference and character IDs

**BONE_CoB Chain Rule:** NEVER chain Blender-space bone locals with CoB appended. Chain in GAME SPACE first, then convert per-bone world transforms to Blender space. See `animation-pipeline.md` for details.

**Blender Review:** `build_proper_blend.py` builds .blend files with HKX armature + FLVER mesh + animations for visual review. Output: `C:\scripts\elden_ring_tools\output\c3100_proper\`

**Critical: ALL UE5 setup/validation uses C++ automation (SLFAutomationLibrary), NEVER Python API.**

---

## Visual Validation Requirements

- **ALL animation work** requires screenshot comparison (before/after)
- **ALL gameplay changes** require PIE screenshot with HUD visible
- Use Play-Test Loop system (`.claude/skills/play-test-loop/SKILL.md`)
- SLFTestManager can spawn enemies, capture screenshots, export filtered logs

---

## Play-Test Loop

Automated PIE testing system for iterative validation:

1. Build C++ -> verify compile
2. Launch PIE via console or `-ExecCmds`
3. Spawn test actors with `SLF.Test.SpawnEnemy`
4. Simulate player actions (SLF.SimAttack, SLF.SimDodge)
5. Capture screenshots at key moments
6. Export filtered logs for analysis
7. Validate results against expected state

**C++ Implementation:** `Source/SLFConversion/Testing/SLFTestManager.h/cpp`
**Existing tests:** `Source/SLFConversion/Testing/SLFPIETestRunner.h/cpp`
**Skill:** `.claude/skills/play-test-loop/SKILL.md`

---

## Backups

| Backup | Location | Purpose |
|--------|----------|---------|
| `bp_only` | `C:/scripts/bp_only/` | **PRIMARY** - Original Blueprint-only project |
| `animbp_native_complete` | `backups/animbp_native_complete/` | AnimBP after C++ native migration |

**Master Index:** `C:/scripts/SLFConversion/backups/BACKUPS.md`

**NEVER overwrite entire Content/ from bp_only.** Use surgical restores only.

---

## Critical Files

| File | Purpose |
|------|---------|
| `run_migration.py` | Multi-phase reparenting script |
| `migration_cache/` | Cached icons, niagara, stats (survives restores) |
| `Exports/BlueprintDNA_v2/*.json` | Source of truth for Blueprint logic |
| `Source/SLFConversion/` | C++ implementation |
| `C:\scripts\bp_only\` | Clean backup content |
| `SLFAutomationLibrary.h/cpp` | C++ automation functions |
| `SLFConversion.cpp` | Console command registration |

---

## Non-Negotiable Rules

### NEVER:
- Leave logic in Blueprint "because it's complex"
- Write stub implementations with `// TODO`
- Use reflection for Blueprint struct properties
- Use Unreal Python API for asset manipulation
- Make assumptions without verifying from JSON exports
- Do full Content/ restore (use surgical migration)

### ALWAYS:
- Read the JSON export BEFORE writing any code
- Complete ALL 20 passes for EVERY migrated item
- Verify branch logic (TRUE/FALSE) matches exactly
- Extend SLFAutomationLibrary for new automation needs
- Use headless mode (never ask user to open editor)

---

## Critical Bug Watch

### #1: Inverted Logic
```cpp
// WRONG                              // RIGHT (matches Blueprint)
if (bBufferOpen) { ProcessNow(); }    if (bBufferOpen) { QueueForLater(); }
                                      else { ProcessNow(); }
```

### #2: Name Mismatch
```cpp
bool IsSprinting;   // CORRECT - Blueprint uses "IsSprinting"
bool bIsSprinting;  // WRONG - breaks callers
```

### #3: Signature Mismatch
```cpp
void AddItem(UDataAsset* Item, int32 Count, bool bTriggerLootUi); // CORRECT
void AddItem(UDataAsset* Item, int32 Amount);                      // WRONG
```

---

## The Mission

Migrate ALL SoulslikeFramework Blueprint logic to native C++. An item is DONE when:
1. ALL logic is in C++ (no Blueprint execution nodes)
2. All 20 validation passes complete (see `/slf-migration` skill)
3. C++ compiles, Blueprint callers compile, PIE test passes
4. Tracker updated with notes

---

## Headless Execution

**ALWAYS use headless mode - never ask user to open editor!**
- `UnrealEditor-Cmd.exe` with `-run=pythonscript` or `-ExecCmds`
- Build via `Build.bat`
- Tests via console commands

---

## Reference Documents

| Document | Content |
|----------|---------|
| `.claude/reference/known-issues.md` | All 11 known issues with solutions |
| `.claude/reference/migration-workflow.md` | Detailed 4-step migration workflow |
| `.claude/reference/animation-pipeline.md` | 5-phase pipeline overview + coordinate space rules |
| `.claude/reference/animation-import-runbook.md` | **Step-by-step import commands with sample output** |
| `.claude/skills/slf-migration/SKILL.md` | 20-pass validation protocol |
| `.claude/skills/animBP-fix/SKILL.md` | AnimBP troubleshooting |
| `.claude/skills/play-test-loop/SKILL.md` | Automated PIE testing |
