# Game Design Document — Working Title: "Ashborne"

## Overview
- **Genre**: Soulslike Action RPG
- **Engine**: Unreal Engine 5.7
- **Setting**: Post-apocalyptic sci-fi (Borderlands/Fallout aesthetic, NOT fantasy)
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

## Art Direction: Post-Apocalyptic Sci-Fi
Borderlands meets Fallout meets Soulslike combat. Makeshift weapons, scrap armor, irradiated mutants, rogue machines, wasteland cults. NO medieval fantasy — the Convergence was a tech disaster.
- **Materials**: Rusted steel, duct tape, welded scrap, exposed wiring, hazmat fabric, cracked concrete
- **Silhouettes**: Scrap armor, gas masks, welding goggles, jury-rigged exoskeletons
- **Full enemy roster**: See `.claude/reference/enemy-faction-plan.md` (58 enemies, 5 factions)

## Weapons (25 total — Post-Apocalyptic)
| Category | Weapons | Scaling |
|----------|---------|---------|
| Improvised Melee | Rebar Club, Pipe Wrench, Sledgehammer | STR |
| Bladed Melee | Machete, Buzz-Saw Blade, Shiv, Cleaver | DEX or STR/DEX |
| Tech Melee | Shock Baton, Plasma Cutter, Chain Whip, Cattle Prod | DEX/INT |
| Ballistic Ranged | Pipe Pistol, Nail Gun, Scrap Shotgun, Crossbow | DEX |
| Energy Ranged | Convergence Pistol, Arc Caster, Void Launcher | INT |
| Shields/Off-hand | Car Door Shield, Riot Shield, Manhole Cover, Stop Sign | STR/END |
| Boss Weapons | Broodmother's Claw, Forgemaster's Hammer, Convergence Blade | Unique |

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

## Player Movement & Traversal
| Action | Input | Description | Stamina | Cooldown |
|--------|-------|-------------|---------|----------|
| Slide | Sprint + Crouch | Fast ground slide, 400cm over 0.8s, can go under obstacles | — | 1.5s |
| Double Jump | Jump while airborne | Second jump with upward impulse | 15 | Reset on land |
| Vault | Auto (jump into obstacle <100cm) | Quick hop over low obstacles | — | — |
| Hurdle | Auto (obstacle 100-150cm) | Athletic hurdle over medium obstacles | — | — |
| Mantle | Auto (obstacle 150-250cm) | Climb up onto ledge | — | — |
| Climb | Auto (obstacle 250-350cm) | Full climb up tall wall | — | — |
| Grapple | Grapple input (aim at point) | Pull toward grapple point, 3000cm range | — | 8s |
| Grapple Attack | Attack during grapple | Melee slam at destination, 1.5x damage (0.25x vs bosses) | — | Shared with grapple |

## Landing Reactions
| Fall Distance | Reaction | Effect |
|---------------|----------|--------|
| < 300cm | Light landing | Brief recovery animation |
| 300-600cm | Heavy landing | Longer recovery, slight stagger |
| 600-1200cm | Roll landing | Automatic combat roll on impact |
| > 1200cm | Stumble landing | Long recovery + fall damage (0.1 HP/cm above threshold) |

## Stealth System
- **Activation**: Crouch + no enemy currently targeting the player
- **Detection**: Crouching halves enemy detection radius (1500cm → 750cm)
- **Stealth Backstab**: 3x damage multiplier (vs normal backstab damage)
- **Breaking Stealth**: Standing up, being detected, attacking (non-backstab)

## Combat Actions
| Action | Trigger | Damage | Notes |
|--------|---------|--------|-------|
| Guard Counter | Heavy attack within 0.3s of blocking | 1.8x, 2.0x poise | Must successfully block first |
| Jump Attack | Attack while airborne | High poise damage | Long recovery, good for engaging from above |
| Stealth Backstab | Backstab from stealth | 3x damage | Must be crouching + undetected + behind enemy |

## Resting Points
- Sit-down/stand-up animations when interacting
- Menu opens after sit-down completes
- Stand-up animation plays on exit before releasing player
- 15-18 total (2-3 per zone)

## Dungeon Features
| Feature | Description |
|---------|-------------|
| Tier-Colored Doors | Blue (Normal), Purple (Elite), Orange (Legendary) entrance doors |
| Boss Room Barriers | Fog gate seals on entry, unseals on boss defeat |
| Shortcuts | 1-2 one-way gates per dungeon (open from one side only) |
| Trap Markers | Pressure plates, arrow launchers, swinging blades (framework) |
| Puzzle Rooms | 1 per dungeon before boss — Lever/Pressure/Sequence/ElementMatch types |
| Boss Immunity Phases | Destructible objectives that must be cleared to remove boss invulnerability |
| No Fast Travel | World map shows "Fast travel unavailable" when inside dungeon |

## Crafting System (Framework)
| Zone | Material | Use |
|------|----------|-----|
| Ashfields | Cinder Bloom | Fire consumables/coatings |
| Ironworks | Slag Crystal | Weapon upgrade material |
| Rift | Void Petal | Magic consumables |
| Sanctum | Sacred Ember | Holy coatings/cures |
| Undercroft | Hollow Root | Poison/cure materials |
| Core | Convergence Shard | Endgame upgrades |

## World Map
- Full-screen overlay with zoom (scroll wheel) and pan (click-drag)
- Zone circles with color-coding and labels
- Resting point markers (click to fast travel)
- Fast travel blocked inside dungeons

## Production Phases
### Phase 1: Foundation
- Download + integrate Project Titan
- Zone streaming system
- Hub safe zone (resting point + NPCs)
- Zone 1 (Ashfields) playable: 2-3 enemies, 1 boss, 1 dungeon

### Phase 1.5: Movement & Animation (COMPLETE - Feb 2026)
- GameAnimationSample integration (358 animation files, 11 montages)
- Advanced traversal: vault/hurdle/mantle/climb with height detection
- Slide action with crouch transition
- Height-aware landing reactions (4 tiers)
- Resting point sit-down/stand-up animations
- Stealth system (crouch + undetected = stealth, 3x backstab damage)
- Guard counter and jump attack actions
- Double jump
- Grapple hook (traversal + combat attack)
- Dungeon gameplay: tiered doors, shortcuts, traps, puzzles, boss immunity
- World map zone overlay + fast travel system
- Crafting system framework

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
