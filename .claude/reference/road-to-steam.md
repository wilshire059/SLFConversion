# Ashborne — Road to Steam Roadmap

Generated: 2026-03-11

## Current State Summary

**Engine/Systems: ~80% complete** — Combat, AI, inventory, equipment, save/load, stat system, traversal (mantle/slide/double jump/grapple/stealth), fast travel, world map, 30+ UI widgets, dungeon system, NPC framework. All 306+ Blueprints migrated to C++.

**Content: ~30% complete** — 8x8km open world (Project Titan), 3 procedural dungeons, 2 working enemies (Guard, Sentinel), 358 player animations, equipment/weapon framework with 8 weapon types.

**Custom Enemy Pipeline: COMPLETE** — Gemini concept → Meshy AI mesh → Blender rig → ARP cross-skeleton retarget (28 humanoid sources validated) → 16-layer forensic transforms → UE5 commandlet → playable enemy. Can generate new enemies in hours.

**Forensics: CLEAN** — 16 forensic layers, multi-source animation mixing from 5+ Elden Ring enemies, no FromSoft assets ship.

---

## Phase 1: Playable Loop (Make the world feel alive)

**Goal**: Walk through the world, fight enemies, rest at bonfires, die, respawn.

### 1.1 PCG Foliage Swap
- Buy 1 fantasy tree pack + 1 ground cover pack from Fab (~$40)
- Write PCG graph with zone-based rules (dead trees in Ashfields, glowing flora in Rift)
- Remove Titan's original foliage in same pass
- **Impact**: Makes Titan unrecognizable as a stock map

### 1.2 Zone Atmosphere
- PostProcess volumes per zone (7 zones)
- ExponentialHeightFog + SkyAtmosphere settings per zone
- Ashfields: orange haze. Sanctum: ethereal blue. Undercroft: green toxic
- **Impact**: Free, 1-2 hours, completely changes mood

### 1.3 Place Resting Points
- 15-18 bonfires across Titan zones
- Use existing SLFRestingPointBase (sit/stand montages already working)
- Place at zone transitions, before boss arenas, inside dungeons
- Can do via commandlet extension (SetupWorldCommandlet)

### 1.4 Place Enemy Spawns
- Use existing SLFEnemySpawnPoint with respawn-on-rest
- Place Guard + Sentinel across zones with difficulty scaling
- Patrol paths for roaming enemies
- Weighted spawn tables per zone

### 1.5 Generate 4-6 More Enemy Types
- **Use the custom enemy pipeline** — this is your superpower
- **Art direction: Post-apocalyptic sci-fi** (Borderlands/Fallout, NOT fantasy)
- Full 58-enemy plan: `.claude/reference/enemy-faction-plan.md`
- Priority enemies for first playable loop:
  - Ashfields: Withered Wanderer (c4370, mutant with bone-club arm)
  - Ashfields: Feral Hound (c3180, irradiated dog, quadruped pipeline)
  - Ashfields: Mutant Brute (c3970, elite, wrecking-ball fist)
  - Ironworks: Covenant Grunt (c5830, scrap-armored soldier with pipe wrench)
  - Rift: Phase Hunter (c4290, semi-transparent stalker)
  - Undercroft: Hollow Test Subject (c3000, blade prosthetic)
- Weapons are makeshift: rebar clubs, buzz-saws, shock batons, pipe guns, energy weapons
- Each enemy: Gemini concept → Meshy mesh → ARP retarget → UE5 commandlet
- **Time estimate**: 2-4 hours per regular enemy, 1-2 days per boss

### 1.6 Main Menu
- Title screen with New Game / Continue / Settings / Quit
- Settings: resolution, graphics preset, audio volume, controls
- Use existing Lvl_TitanMenu.umap
- W_MainMenu widget + W_Settings widget

---

## Phase 2: Bosses (The reason people buy Soulslike games)

**Goal**: 5 memorable boss fights with unique patterns, phases, and arenas.

### 2.1 Boss Arena Design
- Buy 1-2 modular castle/ruin kits from Fab (~$40-60)
- Place at 5 locations across Titan zones
- Each arena: fog gate entrance, open combat space, visual theme
- Use existing SLFBossEncounter framework (phase system, fog gates, immunity objectives)

### 2.2 Boss Implementation (Priority Order)
| # | Boss | Zone | Concept | Animation Sources |
|---|------|------|---------|-------------------|
| 1 | Broodmother | Ashfields | Aggressive, multi-phase | c3000 (Exile) + c4350 (Lordsworn Knight) |
| 2 | Forgemaster | Ironworks | Heavy, slow, devastating | c3400 (Grave Warden) + c3010 (Banished Knight) |
| 3 | Convergent | Rift | Fast, teleporting | c4290 (Bloodhound) + c3300 (Nox Swordstress) |
| 4 | Hierophant | Sanctum | Magic-heavy, summons | c2500 (Crucible Knight) + c5830 (Messmer) |
| 5 | Primordial | Core | Final boss, 3 phases | Mix all sources |

### 2.3 Per Boss Checklist
- [ ] Concept art (Gemini)
- [ ] Mesh (Meshy AI — larger/more imposing than regular enemies)
- [ ] Animation set (20+ anims from mixed sources via ARP pipeline)
- [ ] Phase transitions (health thresholds, visual/audio cues)
- [ ] Unique attack patterns (3-4 per phase)
- [ ] Boss loot table (unique weapon drop)
- [ ] Arena design + fog gate placement
- [ ] Music/audio (even placeholder)
- [ ] PIE test: full fight from fog gate to death to victory

---

## Phase 3: Content Depth

**Goal**: Enough variety that the game feels complete.

### 3.1 Weapons (15+ total, currently 8)
- 7+ more weapons with unique movesets
- Each weapon: animation set from different source enemies
- Stat tables, scaling (STR/DEX/INT/FTH/ARC)
- 5 boss weapons (unique drops from Phase 2 bosses)
- Use existing AC_EquipmentManager + weapon data asset framework

### 3.2 Armor Sets (10-15 sets)
- Models from Meshy AI or Fab marketplace
- Per-set: head, chest, gloves, legs
- Poise, defense stats, weight
- Visual variety per zone (ash-covered, crystalline, ornate, rusted)

### 3.3 Landmark Replacement on Titan
- Replace Titan's modern buildings with marketplace castle/ruin kits
- Commandlet-driven placement (extend SetupWorldCommandlet)
- Material instance swap on terrain per zone
- PCG road/path network connecting bonfires → bosses → dungeons

### 3.4 Quest System (5-8 quests)
- Implement quest state machine on AC_ProgressManager
- Dialog tree system for NPCs (Vara, Kael, + 3-4 new NPCs)
- Serin (Rift researcher), Dren (retired soldier), The Whisper (mysterious guide)
- Quest journal UI widget
- 2-3 side quest chains with meaningful rewards

### 3.5 Dungeon Theming
- Replace sci-fi DA_StarterPackTheme with zone-specific themes
- Buy/create dark fantasy dungeon tile sets
- Ashfields dungeon: collapsed mines. Ironworks: foundry halls. Rift: crystalline caves.

### 3.6 Audio/Music
- Zone ambient tracks (buy licensed or commission, ~$100-200)
- Boss themes (5 unique tracks)
- SFX pass: weapon impacts, footsteps, UI sounds
- Integrate with existing orchestral music infrastructure

---

## Phase 4: Polish & Ship

**Goal**: A complete, stable product ready for Steam.

### 4.1 Character Creator (Simple, Built In-House)
- FSLFCharacterAppearance struct (face/body morph targets, colors, hair index)
- Buy 1 base body mesh with morph targets + 1 hair pack (~$50)
- Slider UI widget with 3D preview
- Integrates with existing modular armor system (armor hides body sections)
- Save/load via AC_SaveLoadManager
- ~4 days total work, no marketplace plugin needed

### 4.2 Full Playthrough QA
- 20+ hour playthrough minimum
- Balance pass: damage curves, health scaling, stamina costs
- Difficulty tuning: enemy placement density, boss pattern timing
- Progression pacing: are bonfires spaced well? Is loot rewarding?

### 4.3 Performance Optimization
- LOD validation across all meshes
- Level streaming optimization (515 landscape proxies need tuning)
- Memory profiling under sustained play
- Frame time analysis in combat (weapon traces, particles, AI)

### 4.4 Steam Integration
- Steamworks SDK integration
- Achievements (10-15: boss kills, zone discovery, quest completion)
- Steam Cloud saves
- Rich presence (zone display, boss fight status)

### 4.5 Store Page & Marketing
- Steam capsule art (Gemini can help concept, commission final)
- 1-2 minute trailer (PIE capture + video editing)
- Store description, screenshots, system requirements
- Coming Soon page (build wishlist before launch)

---

## Not Needed for v1.0 (Post-Launch)

- Multiplayer / co-op / PvP / invasions
- Full crafting system (framework exists, not critical for launch)
- New Game+ / harder difficulty modes
- Additional zones / DLC content
- Mod support
- Controller remapping (Enhanced Input already integrated)

---

## Budget Estimate (Marketplace Assets)

| Item | Cost |
|------|------|
| Fantasy tree + ground cover pack | ~$40 |
| Modular castle/ruin kit (x2) | ~$80 |
| Dark fantasy dungeon tileset | ~$40 |
| Base body mesh + morph targets | ~$30 |
| Hair pack (10-15 styles) | ~$20 |
| Licensed ambient music (per zone) | ~$150 |
| **Total** | **~$360** |

Everything else is generated (Meshy AI enemies, Gemini concepts) or built in C++ with existing systems.

---

## Key Leverage Points

1. **Custom enemy pipeline** — new enemy in hours, not weeks. Use this aggressively.
2. **C++ migration complete** — no Blueprint spaghetti, fast iteration.
3. **ARP multi-source retarget** — 750+ humanoid animations across 10 validated source enemies. Every new enemy gets a unique moveset.
4. **Project Titan** — 8x8km world already placed. Transform it, don't replace it.
5. **Commandlet automation** — SetupWorldCommandlet can place bonfires, NPCs, spawns programmatically. Iterate fast.

---

## Immediate Next Actions (This Week)

1. **Buy foliage packs** → write PCG graph → transform Titan's look
2. **Generate 2 new enemies** via pipeline (Ashfields soldier + Ironworks brute)
3. **Place 5 bonfires** in first zone (Ashfields) via commandlet
4. **Place enemy spawns** around those bonfires
5. **Walk through Ashfields** — rest, fight, die, respawn. Validate the loop.

Once the loop feels right in one zone, replicate across all 7.

---

## World Population & Enemy Density Planning

### Elden Ring Reference Data

| Metric | Value | Source |
|--------|-------|--------|
| Total map area (visible) | ~79 km² | Community measurements |
| Playable overworld | ~40 km² | Excluding water/cliffs |
| Tightly playable area | ~15-20 km² | Actual traversed terrain |
| Unique enemy types | ~126 base, ~300+ with variants | Fextralife wiki |
| Total bosses | 238 (all types) | Fandom wiki |
| Major legacy dungeons | 6 | Stormveil, Raya Lucaria, Volcano Manor, Leyndell, Haligtree, Farum Azula |
| Minor dungeons | 51 | Catacombs, caves, mines, hero's graves |
| Regions | 8 major | Limgrave, Liurnia, Caelid, Altus, Mountaintops, etc. |

### Elden Ring Enemy Placement Patterns

Based on community analysis and gameplay observation:

**Overworld Density:**
- Elden Ring places enemies in **clusters of 2-6** with space between encounters
- Rarely 6-7 at once unless player aggros multiple groups
- Average encounter spacing: **50-100m** along paths, **150-300m** in open fields
- Estimated **2,500-3,500 total enemy instances** across the overworld
- That's roughly **60-90 enemies per km²** of tightly playable area

**By Region (estimated from community maps):**
| Region | Approx Area | Est. Enemies | Density | Notes |
|--------|-------------|--------------|---------|-------|
| Limgrave + Weeping Peninsula | ~4 km² | 500-700 | ~150/km² | Starter area, highest density |
| Liurnia of the Lakes | ~4 km² | 400-500 | ~110/km² | Water reduces walkable area |
| Caelid | ~3 km² | 300-400 | ~115/km² | Fewer but much harder |
| Altus Plateau + Capital Outskirts | ~3 km² | 350-450 | ~130/km² | Mid-game escalation |
| Mountaintops + Consecrated | ~2 km² | 200-300 | ~125/km² | Late game, spread out |
| Underground (Siofra, Ainsel, Deeproot) | ~3 km² | 250-350 | ~100/km² | Atmospheric, sparser |

**Dungeon Density:**
| Dungeon Type | Count | Avg Enemies | Avg Length |
|--------------|-------|-------------|------------|
| Catacombs | 19 | 15-25 | 5-10 min |
| Caves | 15 | 10-20 | 3-8 min |
| Tunnels/Mines | 8 | 12-18 | 5-8 min |
| Hero's Graves | 6 | 8-15 + traps | 10-20 min |
| Legacy Dungeons | 6 | 80-200 | 1-3 hours |

**Pack Compositions (what makes it feel alive):**
- **Solo sentinels**: 1 tough enemy guarding a path/item (Tree Sentinel, field bosses)
- **Patrol pairs**: 2 enemies walking a route, break off to pursue
- **Camp clusters**: 4-8 enemies around a campfire/ruin, mix of melee + ranged
- **Ambush groups**: 2-4 hidden enemies triggered by proximity
- **Mounted patrols**: 1-2 fast enemies on horseback covering large areas
- **Environmental hazards**: Enemies + terrain (cliffs, poison swamps, tight bridges)
- **Wildlife scatter**: Passive/neutral animals + occasional aggressive beast

### Ashborne Density Plan (8x8 km = 64 km²)

Our map is **3-4x larger** than Elden Ring's playable overworld. We should NOT try to fill it uniformly — Elden Ring itself has vast empty stretches that serve as breathing room.

**Target: ~30% of map is "encounter space"** (~20 km² active, ~44 km² wilderness/travel)

**Total Enemy Budget:**

| Category | Count | Notes |
|----------|-------|-------|
| Overworld enemies | 1,500-2,000 | Placed via commandlet, respawn on rest |
| Dungeon enemies | 300-500 | 3 procedural dungeons × 100-150 each |
| Boss encounters | 12-15 | 5 main + 7-10 field/mini-bosses |
| **Total instances** | **~2,000-2,500** | Similar feel to ER at our scale |

**Per Zone (7 zones):**
| Zone | Area | Enemies | Density | Packs | Character |
|------|------|---------|---------|-------|-----------|
| Ashfields (starter) | ~12 km² | 350-400 | ~30/km² | 60-70 | Dense camps, guided paths |
| Ironworks | ~8 km² | 250-300 | ~35/km² | 45-55 | Industrial clusters, tight corridors |
| The Rift | ~10 km² | 200-250 | ~22/km² | 35-45 | Sparse, fast enemies, ambushes |
| Sanctum | ~8 km² | 250-300 | ~35/km² | 45-55 | Orderly patrols, magic support |
| Undercroft | ~6 km² | 200-250 | ~38/km² | 35-45 | Dense, low visibility |
| The Core | ~4 km² | 150-200 | ~45/km² | 25-35 | Elite, every encounter matters |
| Wilderness/travel | ~16 km² | 100-150 | ~8/km² | Scattered | Wildlife, rare wanderers |

**Pack Templates for Commandlet:**
```
PackTemplate_Solo:       1 enemy (field boss or elite sentinel)
PackTemplate_Pair:       2 enemies (patrol route, staggered aggro)
PackTemplate_SmallCamp:  3-4 enemies (1 ranged + 2-3 melee around campfire)
PackTemplate_LargeCamp:  5-8 enemies (2 ranged + 3-5 melee + 1 elite, near POI)
PackTemplate_Ambush:     2-3 enemies (hidden, trigger on proximity)
PackTemplate_Roaming:    1-2 enemies (patrol path 200-500m)
```

**Enemy Types Needed (minimum for v1.0):**
| # | Type | Zone | Animation Source | Role |
|---|------|------|-----------------|------|
| 1 | Guard (existing) | All | c3100 direct | Basic melee |
| 2 | Sentinel (existing) | All | c3100 forensic | Heavy melee |
| 3 | Ashfield Soldier | Ashfields | c4310 (Lordsworn Soldier) | Light/fast melee |
| 4 | Ironworks Brute | Ironworks | c3400 (Grave Warden) | Slow/heavy |
| 5 | Rift Stalker | Rift | c4290 (Bloodhound Knight) | Fast/dodge |
| 6 | Sanctum Knight | Sanctum | c3010 (Banished Knight) | Armored/defensive |
| 7 | Undercroft Dancer | Undercroft | c3300 (Nox Swordstress) | Dual wield |
| 8 | Core Vanguard | Core | c5830+c5840 mix | Elite hybrid |
| — | **5 Bosses** | Per zone | Mixed multi-source | Unique movesets |

**What Makes the World Feel Alive (beyond enemies):**
- **Ambient wildlife**: Non-hostile animals (deer, birds, insects) in wilderness areas
- **Environmental storytelling**: Corpses, broken weapons, burned camps, collapsed buildings
- **NPC encounters**: Friendly/neutral NPCs at bonfires, crossroads, ruins
- **Dynamic lighting**: Time-of-day cycle, torch-lit camps visible from distance
- **Sound design**: Distant combat sounds, wind, creature calls, camp chatter
- **Verticality**: Enemies on ledges, ruins, towers — visible before engagement
- **Reward placement**: Items near enemy camps reward clearing, guide exploration

**Commandlet Placement Strategy:**
1. Define zone boundaries as box volumes
2. Place bonfires first (15-18 total, ~500-800m apart)
3. Place enemy packs along paths between bonfires (50-100m spacing)
4. Place field bosses at zone transition chokepoints
5. Scatter ambush groups near loot/item locations
6. Add roaming patrols on major roads
7. Wildlife in empty spaces between encounter zones
