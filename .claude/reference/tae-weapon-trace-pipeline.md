# TAE-Driven Weapon Trace & Telegraph Pipeline

## Overview

End-to-end pipeline for extracting Elden Ring TAE (TimeAct Event) hitbox timing data, applying forensic time transforms, and placing frame-precise weapon traces + telegraph VFX on enemy attack montages in UE5.

**Problem solved**: Multi-hit attacks (whirlwinds, combos) only hit once because each montage had a single weapon trace covering 30-60% of the animation. TAE data provides per-swing hitbox windows.

---

## Pipeline Components

### Step 1: Python TAE Extraction (`extract_tae_hitboxes.py`)
**Location**: `C:\scripts\elden_ring_tools\extract_tae_hitboxes.py`

Extracts hitbox windows from Elden Ring ANIBND TAE data and applies forensic time transforms.

**Input**: `--name withered_wanderer`
**Output**: `test_meshes/withered_wanderer/tae_hitboxes.json`

**Logic**:
1. Scans `test_meshes/{name}/fbx/` for FBX filenames to determine anim_id -> source chr_id mapping
2. For duplicate anim_ids from different sources, keeps last alphabetically (matches UE5 import order)
3. Loads each source chr's ANIBND via soulstruct `Binder` + `TAEParser`
4. Extracts attack events: `AttackBehavior` (type 1), `ThrowAttackBehavior` (type 304), `BulletBehavior` (type 2)
5. Deduplicates by (start_time, end_time) to remove duplicate params
6. Applies forensic time transforms matching `mesh_animation_pipeline.py`:
   - Time warp: `T + 0.15 * sin(2*pi*T) / (2*pi)`
   - Speed variation: deterministic per-anim from seed 77742 (range 0.88-1.12)
7. Filters sentinel end_times (> 120s) from duration calculation

**JSON format**:
```json
{
  "animations": {
    "a000_003000": {
      "source_chr": "c5830",
      "forensic_duration": 3.23,
      "hitbox_windows": [
        { "start": 0.771, "end": 0.868 }
      ]
    }
  }
}
```

### Step 2: C++ Multi-Window Weapon Traces (`AddMultipleWeaponTracesToMontage`)
**File**: `SLFAutomationLibrary.h/.cpp`

- `FWeaponTraceWindow` struct: StartTime, EndTime, DamageMultiplier
- Creates one independent `USLFAnimNotifyStateWeaponTrace` per window
- Applies UE5 timing widening: start -0.1s, end +0.15s (Bug #18: TAE windows too tight for variable framerate)
- Also places `USLFAnimNotifyStateTelegraph` (furnace/ribbon VFX) 0.4s before each hitbox window
- Clears existing traces + telegraphs first (idempotent)

### Step 3: Commandlet Integration (`SetupBatchEnemyCommandlet`)
- `AddWeaponTracesFromTAE()` reads `tae_hitboxes.json` from source dir
- Extracts anim_id from montage's backing AnimSequence via regex
- Falls back to heuristic single-window placement if no JSON exists
- Processes both Attack01-08 and HeavyAttack montages

---

## Telegraph VFX System

### `USLFAnimNotifyStateTelegraph`
**Files**: `Animation/SLFAnimNotifyStateTelegraph.h/.cpp`

Spawns Niagara VFX on weapon/hand bones before each hitbox window to telegraph incoming attacks.

- Attaches to `weapon_r` bone (fallback: `R_Hand`, `hand_r`, `lowerarm_r`)
- Second attachment on `hand_r` for dual-point glow
- VFX priority: `NS_RibbonTrail` > `NS_FireParticles` > `NS_BossBuffHands`
- Default color: hot orange (1.0, 0.4, 0.05)
- Lead time: 0.4s before each hitbox window

---

## Weapon Trace Improvements

### Bone Fallback System
Sockets (`weapon_start`, `weapon_end`) often don't exist on custom enemy meshes at runtime. The weapon trace now resolves bones first:
1. Try socket name
2. Try socket name as bone name
3. Try fallback bones: `R_Hand`, `hand_r`, `weapon_r`, `Hand_R`, `RightHand`, `R_Sword`
4. Last resort: trace 100cm in front of enemy actor center

### Swing-Arc Tracing
Tracks hand bone position between frames. When the hand moves significantly (indicating a swing), the trace goes from **previous hand position -> current weapon tip** instead of always pointing forward. This catches:
- Horizontal slashes (L->R sweeps)
- Overhead slams
- Upcuts
- Cross-body swings

### Direction Resolution
Uses bone-to-bone direction (`DirectionBoneName = lowerarm_r` -> through hand toward blade) when available, falls back to actor forward vector.

---

## Elden Ring Animation ID Convention (Corrected)

```
a000_000xxx = idle/stance
a000_001xxx = transitions/turns
a000_002000 = walk forward
a000_002001 = walk backward
a000_002002 = sidestep left
a000_002003 = sidestep right
a000_002100 = RUN FORWARD (c3100 Crucible Knight)
a000_002030 = run forward (infantry enemies: c3000, c3300, c5830)
a000_002300 = walk variant (NOT sprint as previously assumed)
a000_003xxx = attacks (light)
a000_004xxx = heavy attacks (ThrowAttackBehavior type 304)
a000_005xxx = dodges/evasion
a000_006xxx = damage reactions
a000_007xxx = guard/block
a000_009xxx = poise break
a000_011xxx = death
```

**Key correction**: `a000_002100` is the c3100 run animation, not backstep. `a000_002002/002003` are sidesteps, not run/sprint.

---

## Locomotion Montage Assignment

The commandlet now picks Walk and Run by ID pattern rather than array index:
- Walk: `a000_002000` (preferred) or first locomotion anim
- Run: `a000_002100` (c3100 run), `a000_002030` (infantry run), or second locomotion anim

### Sprint Speed Fix
The AI controller's `MoveToLocation()` was activating the path follower component even without NavMesh, producing ~65cm/s velocity that overrode `AddMovementInput`. Fix: only call `MoveToLocation` if NavMesh exists at the pawn's position, explicitly `StopMovement()` otherwise.

Three movement tiers:
- Sprint (>500cm): RunSpeed (650cm/s)
- Jog (300-500cm): 60% lerp between walk and run
- Walk (<300cm): WalkSpeed (350cm/s)

Run montage activates at GroundSpeed > 300cm/s.

---

## Withered Wanderer State

### Attack Montages (TAE-driven)
| Montage | Anim ID | Source | Windows |
|---------|---------|--------|---------|
| Attack01 | a000_003000 | c5830 | 1 |
| Attack02 | a000_003001 | c5830 | 1 |
| Attack03 | a000_003002 | c5830 | 1 |
| Attack04 | a000_003003 | c3970 | 1 |
| Attack05 | a000_003004 | c3970 | 5 (whirlwind) |
| Attack06 | a000_003005 | c3970 | 1 |
| Attack07 | a000_003006 | c3970 | 1 |
| HeavyAttack | a000_004100 | c3100 | 1 (ThrowAttackBehavior) |

Attack08 (a000_003007, BulletBehavior ranged) exists as montage but excluded from AI abilities (7-attack cap).

### Locomotion
- Walk: a000_002000 (c3100 walk forward)
- Run: a000_002100 (c3100 run forward)

### Body VFX
- Furnace effect (NS_Furnace, was NS_Lightning)

### AI Config
- Attack MaxDistance: 250cm (normal), 350cm (gap closers + heavy)
- RVO avoidance enabled
- Obstacle slide on AddMovementInput fallback

---

## Files Created/Modified

### New Files
| File | Purpose |
|------|---------|
| `C:\scripts\elden_ring_tools\extract_tae_hitboxes.py` | TAE extraction + forensic transforms |
| `Source/SLFConversion/Animation/SLFAnimNotifyStateTelegraph.h` | Telegraph VFX ANS header |
| `Source/SLFConversion/Animation/SLFAnimNotifyStateTelegraph.cpp` | Telegraph VFX ANS implementation |

### Modified Files
| File | Changes |
|------|---------|
| `SLFAutomationLibrary.h` | `FWeaponTraceWindow`, `AddMultipleWeaponTracesToMontage()` |
| `SLFAutomationLibrary.cpp` | Multi-window weapon trace + telegraph placement |
| `SetupBatchEnemyCommandlet.h` | `AddWeaponTracesFromTAE()` declaration |
| `SetupBatchEnemyCommandlet.cpp` | TAE JSON reading, locomotion ID matching, JSON includes |
| `SLFAnimNotifyStateWeaponTrace.h` | `PreviousStartPos` for swing-arc tracking |
| `SLFAnimNotifyStateWeaponTrace.cpp` | Bone fallback, swing-arc tracing, Warning-level hit logging |
| `SLFEnemyGeneric.cpp` | Furnace body VFX, attack distance tuning, RVO avoidance, 7-attack cap |
| `SLFAIStateMachineComponent.cpp` | NavMesh-aware movement, 3-tier speed, obstacle slide |
| `SLFPIETestRunner.cpp` | (reverted god mode addition) |
