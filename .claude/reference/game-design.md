# Game Design Document — Working Title: "Ashborne"

## Overview
- **Genre**: Soulslike Action RPG
- **Engine**: Unreal Engine 5.7
- **Setting**: Post-apocalyptic open world
- **Map**: Project Titan (8x8 km)
- **Progression**: Fully open, difficulty varies by area
- **Target Playtime**: 10-15 hours main, 20+ completionist

## World Lore: The Convergence
Ancient technology harnessing unknown energy catastrophically failed. The Convergence tore reality, reshaped landscape, mutated life, corrupted machines, opened rifts. Centuries later, civilization clings to Embers — remnants providing warmth and barrier against corruption. Player is Ember-touched — absorbs energy from fallen enemies. Goal: reach The Core and seal or claim its power.

## Factions
| Faction | Zone | Philosophy |
|---------|------|------------|
| The Withered | Ashfields | Mindless, corrupted by mutation |
| The Iron Covenant | Ironworks | Worship machines, tech salvation |
| The Veilborn | The Rift | Embrace Convergence, willingly transformed |
| The Ashen Order | Sanctum of Ash | Religious guard of forbidden knowledge |
| The Hollow | The Undercroft | Pre-Convergence experiments sealed underground |

## Zones (Difficulty Order)
| # | Zone | Theme | Difficulty | Boss |
|---|------|-------|------------|------|
| 1 | The Ashfields | Desolate wasteland | Easy (tutorial) | The Broodmother |
| 2 | Ironworks | Rusted factories | Medium | The Forgemaster |
| 3 | The Rift | Reality-warped | Medium-Hard | The Convergent |
| 4 | Sanctum of Ash | Gothic ruins | Hard | The Ashen Hierophant |
| 5 | The Undercroft | Ancient tunnels | Very Hard | The Primordial |
| 6 | The Core | All corruption | Final | The Convergence |
| HUB | Safe Haven | Neutral ground | Safe | None |

## Zone Connectivity
```
                [The Rift (3)]
                     |
[Ashfields (1)] --- HUB --- [Ironworks (2)]
                     |
           [Sanctum of Ash (4)]
                     |
             [The Undercroft (5)]
                     |
                [The Core (6)]
```

## Key NPCs
| NPC | Role | Location | Quest |
|-----|------|----------|-------|
| Vara | Guide/merchant | Hub, moves between zones | Main quest — Convergence truth |
| Kael | Blacksmith | Hub safe zone | Weapon upgrades — rare materials |
| Serin | Lore keeper | Found injured Zone 2 | Optional: history, rewards spells |
| Dren | Covenant deserter | Zone 2 entrance | Intel + shortcut |
| The Whisper | Mysterious entity | Boss arenas post-victory | Cryptic guidance |

## Enemy Roster (29 types)
### Per-Zone
| Zone | Regular 1 | Regular 2 | Regular 3 | Elite | Boss |
|------|-----------|-----------|-----------|-------|------|
| Ashfields | Feral Hound | Withered Wanderer | Bloated Spitter | Mutant Brute | The Broodmother |
| Ironworks | Sentry Drone | Iron Soldier | Spark Crawler | Heavy Mech | The Forgemaster |
| The Rift | Veil Wraith | Twisted Pilgrim | Rift Hound | Rift Knight | The Convergent |
| Sanctum | Ashen Acolyte | Zealot Archer | Censer Bearer | Ashen Champion | The Ashen Hierophant |
| Undercroft | Cave Lurker | Blight Crawler | Hollow Experiment | The Warden | The Primordial |

## Weapons (25 total)
| Category | Count | Scaling |
|----------|-------|---------|
| Straight Swords | 3 | STR/DEX |
| Greatswords | 2 | STR |
| Katanas | 2 | DEX |
| Daggers | 2 | DEX/ARC |
| Hammers/Maces | 2 | STR |
| Spears | 2 | STR/DEX |
| Shields | 3 | END/STR |
| Catalysts | 2 | INT/FTH |
| Bows | 2 | DEX |
| Boss Weapons | 5 | Unique |

## Currency & Leveling
- **Cinders** — dropped by enemies, lost on death, recoverable
- Level 1-15: Ashfields, 15-30: Ironworks, 30-50: Rift/Sanctum, 50-70: Undercroft, 70-80: Core
- 15-18 resting points total (2-3 per zone)

## Dungeons (DA-Generated)
| Zone | Dungeon | DA Cells |
|------|---------|----------|
| Ashfields | Collapsed Bunker | 60 |
| Ironworks | The Assembly Line | 100 |
| The Rift | Fractured Sanctum | 80 |
| Sanctum | Catacombs of Ash | 120 |
| Undercroft | The Hollow Labs | 150 |

## GameplayTag Hierarchy
```
Zone.Hub
Zone.Ashfields
Zone.Ironworks
Zone.Rift
Zone.Sanctum
Zone.Undercroft
Zone.Core
Faction.Withered
Faction.IronCovenant
Faction.Veilborn
Faction.AshenOrder
Faction.Hollow
Quest.Main.*
Quest.Side.*
Boss.Broodmother
Boss.Forgemaster
Boss.Convergent
Boss.Hierophant
Boss.Primordial
Boss.Convergence
```

## Production Phases
### Phase 1: Foundation
- Download + integrate Project Titan
- Zone streaming system
- Hub safe zone (resting point + NPCs)
- Zone 1 (Ashfields) playable: 2-3 enemies, 1 boss, 1 dungeon

### Phase 2: Core Loop
- 10 weapons across categories
- Zone 2 (Ironworks) playable
- NPC quests (Vara, Kael)
- VFX + audio integration

### Phase 3: Content Expansion
- Zones 3-4 playable
- All 5 bosses
- Full weapon/armor roster
- All NPCs with dialog
- All dungeon themes

### Phase 4: Final + Polish
- Zones 5-6 + final boss
- Side dungeons
- Balancing pass
- Title screen, credits, audio polish
