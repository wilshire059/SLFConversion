# Testing Guide — Phase 1.5: Movement, Animation & Dungeon Gameplay

All features added in Phase 1.5 (Feb 2026). Build must succeed before testing.

## Prerequisites

```bash
# Build
"C:/Program Files/Epic Games/UE_5.7/Engine/Build/BatchFiles/Build.bat" \
  SLFConversionEditor Win64 Development "C:/scripts/SLFConversion/SLFConversion.uproject"
```

Verify `Result: Succeeded` before proceeding.

---

## 1. Resting Point Sit-Down / Stand-Up

**Files**: `SLFRestingPointBase.h/cpp`
**Montages**: `AM_Rest_SitDown`, `AM_Rest_StandUp`

### Test Steps
1. Open PIE, find a resting point (bonfire) in the level
2. Walk up to it and press Interact (E)
3. **First visit**: Discover animation plays, point activates
4. **Second visit**: Player should play sit-down animation before the menu opens
5. Exit the resting point menu
6. Player should play stand-up animation before being released to move

### Expected Behavior
- Sit-down montage plays (~10.8s) then player teleports to SittingZone position
- Resting point menu appears after sit-down completes
- On exit, stand-up montage plays, then player regains control
- If montages don't load (asset not found), system falls back to instant positioning (no crash)

### What to Look For
- `[RestingPoint] Playing SitDownMontage` in log
- `[RestingPoint] SitDown montage ended, positioning actor` in log
- `[RestingPoint] Playing StandUpMontage` in log
- `[RestingPoint] StandUp complete, releasing player` in log

---

## 2. Slide Action

**Files**: `SLFActionSlide.h/cpp`
**Montage**: `AM_Slide`

### Test Steps
1. Sprint forward (hold Shift or sprint button)
2. While sprinting, press Crouch (C or Ctrl)
3. Player should enter a fast slide

### Expected Behavior
- Slide covers ~400cm over 0.8s
- Player crouches during slide
- Ground friction drops to 0 during slide (smooth movement)
- After slide ends, player uncrouches and friction restores
- 1.5s cooldown before next slide
- Cannot slide while standing still or walking slowly (needs >100 velocity)

### What to Look For
- `[ActionSlide] Starting slide` in log
- `[ActionSlide] Slide ended, starting cooldown` in log
- Player visibly slides forward with crouch posture
- Slide montage plays (if AM_Slide loaded)

---

## 3. Traversal System (Vault / Hurdle / Mantle / Climb)

**Files**: `SLFActionMantle.h/cpp`
**Montages**: `AM_Vault`, `AM_Hurdle`, `AM_Mantle`, `AM_Climb`

### Test Steps
1. Find or place obstacles of varying heights:
   - Low wall (~80cm) for Vault
   - Medium wall (~120cm) for Hurdle
   - Tall wall (~200cm) for Mantle
   - Very tall wall (~300cm) for Climb
2. Run toward each obstacle
3. The system should auto-detect the ledge and play the correct animation

### Expected Behavior
| Obstacle Height | Action | Montage |
|----------------|--------|---------|
| 50-100cm | Vault | AM_Vault |
| 100-150cm | Hurdle | AM_Hurdle |
| 150-250cm | Mantle | AM_Mantle |
| 250-350cm | Climb | AM_Climb |

- Forward trace detects wall face
- Downward trace finds ledge top
- Capsule sweep verifies room to stand
- Player teleports to ledge position after montage
- Movement mode set to Walking after traversal

### What to Look For
- `[ActionMantle] Vault to (X, Y, Z), height=XXcm` in log
- `[ActionMantle] Hurdle to ...` etc.
- If no ledge detected, nothing happens (no error)
- If no room to stand on top, traversal is rejected

---

## 4. Landing Reactions

**Files**: `SLFSoulslikeCharacter.h/cpp`
**Montages**: `AM_Landing_Light`, `AM_Landing_Heavy`, `AM_Landing_Roll`, `AM_Landing_Stumble`

### Test Steps
1. Find elevation differences:
   - Short drop (~2m / 200cm) — should be no reaction or light
   - Medium drop (~4m / 400cm) — heavy landing
   - Tall drop (~8m / 800cm) — roll landing
   - Very tall drop (~15m / 1500cm) — stumble + damage
2. Walk off each edge and observe landing
3. Check health after stumble landing (should take damage)

### Expected Behavior
| Fall Distance | Reaction | Damage |
|--------------|----------|--------|
| < 300cm | Light | None |
| 300-600cm | Heavy | None |
| 600-1200cm | Roll | None |
| > 1200cm | Stumble | 0.1 HP per cm above 1200 |

- Fall tracking starts when `IsFalling()` becomes true
- `FallStartZ` recorded at start of fall
- `Landed()` calculates distance and selects montage
- `bTrackingFall` resets after landing

### What to Look For
- `[SoulslikeCharacter] Landing - FallDistance=XXXcm, Type=Light/Heavy/Roll/Stumble` in log
- Fall damage applied only for stumble (>1200cm)
- Double jump resets on landing (`bHasUsedDoubleJump = false`)

---

## 5. Double Jump

**Files**: `SLFActionDoubleJump.h/cpp`, `SLFSoulslikeCharacter.h/cpp`

### Test Steps
1. Jump (Space)
2. While airborne, press Jump again
3. Should get a second upward impulse
4. Land — should be able to double jump again

### Expected Behavior
- First jump: normal ACharacter jump
- Second jump while airborne: upward impulse, flip/spin montage
- `bHasUsedDoubleJump` prevents triple jump
- Landing resets the flag
- Stamina cost: 15 (same as dodge)

---

## 6. Stealth System

**Files**: `SLFSoulslikeCharacter.h/cpp`, `SLFAIStateMachineComponent.cpp`, `SLFActionBackstab.cpp`

### Test Steps

#### A. Stealth Detection
1. Spawn an enemy (`SLF.Test.SpawnEnemy` or place in level)
2. Stand far away from enemy (>1500cm)
3. Crouch (C key)
4. Walk toward enemy while crouched
5. Note the distance at which the enemy detects you

#### B. Stealth vs Non-Stealth Detection Range
1. **Standing**: Enemy detects at ~1500cm (default `DetectionRadius`)
2. **Crouching**: Enemy detects at ~750cm (halved by `CrouchDetectionMultiplier = 0.5`)

#### C. Stealth Backstab
1. Crouch behind an enemy that hasn't detected you
2. `bInStealth` should be true (visible in Blueprint debugger or check log)
3. Get behind the enemy and press Attack
4. Backstab should trigger with 3x damage multiplier

### Expected Behavior
- `bInStealth = true` when: `bIsCrouched == true` AND no enemy's `CurrentTarget == this`
- `bInStealth = false` when: standing, or any enemy has targeted this player
- Stealth backstab sets `CollisionManager::DamageMultiplier = 3.0` before montage plays

### What to Look For
- `[ActionBackstab] STEALTH BACKSTAB! Damage multiplier set to 3.0x` in log (stealth backstab)
- `[ActionBackstab] *** BACKSTAB ACTION ***` without stealth message (normal backstab)
- Enemy detection range halved when crouching (compare standing vs crouching approach distance)

### Properties (Tunable in Editor)
| Property | Default | Location |
|----------|---------|----------|
| `CrouchDetectionMultiplier` | 0.5 | SLFSoulslikeCharacter |
| `StealthBackstabDamageMultiplier` | 3.0 | SLFSoulslikeCharacter |
| `DetectionRadius` | 1500.0 | SLFAIStateMachineComponent (FSLFAIConfig) |

---

## 7. Guard Counter

**Files**: `SLFActionGuardCounter.h/cpp`

### Test Steps
1. Equip a shield or hold Guard (right mouse / LT)
2. Block an enemy attack successfully
3. Within 0.3s of the block, press Heavy Attack
4. Guard counter should trigger

### Expected Behavior
- 1.8x damage multiplier
- 2.0x poise damage multiplier
- High stagger chance on enemy
- Requires successful block first (timing window)

---

## 8. Grapple Hook

**Files**: `SLFActionGrapple.h/cpp`, `SLFGrapplePoint.h/cpp`

### Test Steps
1. Place `ASLFGrapplePoint` actors in the level at elevated positions
2. Aim at a grapple point within 3000cm range
3. Press Grapple input
4. Player should be pulled toward the point

#### Grapple Attack
1. While grappling toward an enemy, press Attack
2. Should perform melee slam at destination (1.5x damage)
3. Against bosses: 0.25x damage (anti-cheese)
4. 8s cooldown (shared between traversal and attack grapple)

---

## 9. Dungeon Door System

**Files**: `SLFBossDoor.h/cpp`

### Test Steps
1. Place `ASLFBossDoor` actors with different `ESLFDungeonTier` values
2. Verify door frame color matches tier:
   - Normal = Blue
   - Elite = Purple
   - Legendary = Orange
3. Set `bIsDungeonEntrance = true` on one door, provide `DungeonLevelPath`
4. Interact with entrance door — should fade + stream dungeon level

### Boss Room Barrier
1. Place a second `ASLFBossDoor` inside dungeon before boss room
2. When player enters boss trigger, call `SealDoor()` — fog gate appears
3. On boss defeat, call `UnsealDoor()` — gate clears

---

## 10. Dungeon Fast Travel Restriction

**Files**: `SLFPlayerController.h/cpp`, `W_WorldMap.h/cpp`

### Test Steps
1. Enter a dungeon (interact with dungeon entrance door)
2. `bInDungeon` should be set to true on the PlayerController
3. Open World Map (M key)
4. Try to fast travel — should show "Fast travel unavailable"
5. Exit dungeon
6. `bInDungeon` should be false
7. World map fast travel should work again

### What to Look For
- `[SLFPlayerController] ExecuteFastTravel blocked - inside dungeon` in log
- `W_WorldMap::CheckFastTravelBlocked()` returns true when in dungeon

---

## 11. Dungeon Gameplay Elements

### Shortcut Gates (`SLFShortcutGate`)
1. Place in dungeon connecting non-adjacent rooms
2. Interact from one side — gate opens permanently
3. From the other side — interaction is blocked (one-way)

### Trap Markers (`SLFTrapBase`)
1. Place in corridors/narrow rooms
2. Base class — specific trap logic is future work
3. Verify mesh + collision renders correctly

### Puzzle Markers (`SLFPuzzleMarker`)
1. Place before boss room
2. `EPuzzleType`: Lever, Pressure, Sequence, ElementMatch
3. Linked `ASLFBossDoor` with `bRequiresPuzzle = true`
4. Solving puzzle calls `UnsealDoor()` on the boss gate

### Boss Phase Objectives (`SLFBossPhaseObjective`)
1. Place destructible objectives in boss arena
2. When all objectives destroyed, boss immunity drops
3. Framework — specific boss mechanics come later

---

## 12. World Map Zone Overlay

**Files**: `W_WorldMap.h/cpp`

### Test Steps
1. Open World Map (M key)
2. Verify zone circles appear with correct colors
3. Zone names displayed as labels
4. Zoom in/out with scroll wheel — circles reposition correctly
5. Pan with click-drag — circles follow
6. Click resting point marker — fast travel initiates (if not in dungeon)

---

## 13. Crafting System (Framework)

**Files**: `SLFCraftingManager.h/cpp`

### Test Steps
1. Verify component can be added to player
2. Recipe struct (`FSLFCraftingRecipe`) stores inputs/output correctly
3. Zone-specific materials defined in design doc
4. Full crafting UI is future work — framework only

---

## Quick Validation Checklist

```
[ ] Build succeeds (SLFConversionEditor Win64 Development)
[ ] PIE launches without crash
[ ] Resting point: sit-down animation plays on interact
[ ] Resting point: stand-up animation plays on exit
[ ] Slide: triggers when sprinting + crouch, covers ~400cm
[ ] Vault: auto-triggers on low obstacles (<100cm)
[ ] Mantle: auto-triggers on tall obstacles (150-250cm)
[ ] Landing: light landing on short fall (<300cm)
[ ] Landing: stumble + damage on long fall (>1200cm)
[ ] Double jump: second jump while airborne
[ ] Stealth: bInStealth=true when crouching + undetected
[ ] Stealth: enemy detection range halved when crouching
[ ] Stealth backstab: 3x damage in log
[ ] Guard counter: triggers after block + heavy attack
[ ] Dungeon door: color matches tier (blue/purple/orange)
[ ] Fast travel: blocked inside dungeon
[ ] World map: zone overlay circles visible
```

---

## Log Filtering

To find relevant log output during PIE testing:

```bash
# Filter for specific systems
grep "[ActionSlide]" Saved/Logs/SLFConversion.log
grep "[ActionMantle]" Saved/Logs/SLFConversion.log
grep "[ActionBackstab].*STEALTH" Saved/Logs/SLFConversion.log
grep "[SoulslikeCharacter].*Landing" Saved/Logs/SLFConversion.log
grep "[RestingPoint]" Saved/Logs/SLFConversion.log
grep "[AIStateMachine].*Detected" Saved/Logs/SLFConversion.log
```

Or use the SLF test infrastructure:
```
SLF.Test.ExportLog ActionSlide    # Exports filtered log
SLF.Test.ExportLog ActionMantle
SLF.Test.ExportLog RestingPoint
```
