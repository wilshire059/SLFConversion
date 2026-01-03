# SoulslikeFramework Comprehensive Migration Plan

## Executive Summary

This document defines the complete migration order for converting the SoulslikeFramework from Blueprint to C++ using the **Shell Pattern**:
- C++ base class contains ALL variables and function implementations
- Blueprint inherits from C++ and becomes a "shell" with only settings/data references
- All asset paths preserved - no broken references

**Total Assets to Migrate:**
| Category | Count | Priority |
|----------|-------|----------|
| Enums | 39 | Layer 0 |
| Structs | 95 | Layer 0 |
| Interfaces | 18 | Layer 1 |
| Data Asset Classes | ~20 base | Layer 2 |
| Components | 21 | Layer 3 |
| Core Blueprints | ~15 base | Layer 4 |
| Character Blueprints | ~15 | Layer 5 |
| Game Framework | 5 | Layer 6 |
| Animation Blueprints | 8 | Layer 7 |
| Widgets | 122 | Layer 8 |

---

## Migration Layers

### Layer 0: Foundation Types (Enums & Structs)

**Dependencies:** None - these are the foundation everything else depends on

**Migration Strategy:**
1. Create C++ enum/struct with EXACT same name
2. Use CoreRedirects in DefaultEngine.ini to redirect Blueprint type to C++
3. Delete Blueprint UserDefinedStruct/Enum after redirect works

#### Enums (39 total)

| Enum | Purpose | Dependencies |
|------|---------|--------------|
| E_ValueType | Stat value type (Current/Max) | None |
| E_Direction | 8-directional movement | None |
| E_AI_States | AI behavior states | None |
| E_AI_StateHandle | AI state transitions | E_AI_States |
| E_AI_BossEncounterType | Boss encounter setup | None |
| E_AI_Senses | AI perception types | None |
| E_AI_StrafeLocations | AI strafe positions | None |
| E_ActionWeaponSlot | Weapon slot types | None |
| E_AttackPower | Damage type categories | None |
| E_CalculationType | Damage calculation modes | None |
| E_DebugWindowScaleType | Debug UI scaling | None |
| E_DotProductThreshold | Angle thresholds | None |
| E_EnclosureLevel | Area enclosure types | None |
| E_ExecutionType | Execution animation types | None |
| E_FadeTypes | Screen fade types | None |
| E_HitReactType | Hit reaction categories | None |
| E_InputAction | Input action mappings | None |
| E_InventoryAmountedActionType | Inventory actions | None |
| E_InventorySlotType | Inventory slot types | None |
| E_ItemCategory | Item categories | None |
| E_ItemSubCategory | Item subcategories | E_ItemCategory |
| E_KeyType | Key/door types | None |
| E_LadderClimbState | Ladder climb states | None |
| E_LightningMode | Lightning effect modes | None |
| E_MontageSection | Animation montage sections | None |
| E_MovementType | Movement mode types | None |
| E_NpcState | NPC behavior states | None |
| E_OverlayState | Character overlay states | None |
| E_Progress | Progress tracking types | None |
| E_RoomLighting | Room lighting modes | None |
| E_SaveBehavior | Save system behaviors | None |
| E_SettingCategory | Settings categories | None |
| E_SettingEntry | Settings entries | E_SettingCategory |
| E_StatCategory | Stat categories | None |
| E_StatScaling | Stat scaling types | None |
| E_TraceType | Combat trace types | None |
| E_VendorType | Vendor types | None |
| E_WeaponAbilityHandle | Weapon ability slots | None |
| E_WorldMeshStyle | World mesh styling | None |

**Recommended Migration Order (Enums):**
1. Foundation: E_ValueType, E_Direction, E_StatCategory, E_ItemCategory
2. Combat: E_AttackPower, E_HitReactType, E_TraceType, E_ExecutionType
3. AI: E_AI_States, E_AI_StateHandle, E_AI_Senses, E_AI_BossEncounterType
4. Character: E_OverlayState, E_MovementType, E_LadderClimbState
5. UI/System: E_FadeTypes, E_SettingCategory, E_SaveBehavior
6. Rest: All remaining enums

#### Structs (95 total - grouped by domain)

**Stat System Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FRegen | Stat regeneration data | None |
| FStatBehavior | Stat behavior config | FGameplayTag |
| FStatInfo | Complete stat definition | FRegen, FStatBehavior |
| FStatOverride | Stat override values | FRegen |
| FStatEntry | UI stat display entry | FGameplayTag |
| FStatChange | Stat modification | FGameplayTag |
| FStatChangePercent | Percentage stat change | FGameplayTag |
| FAffectedStat | Affected stat reference | FGameplayTag |
| FAffectedStats | Collection of affected stats | FAffectedStat |

**Combat System Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FDodgeMontages | Directional dodge animations | UAnimMontage |
| FExecutionInfo | Execution attack data | FExecutionAnimInfo |
| FExecutionAnimInfo | Execution animation data | UAnimMontage |
| FExecutionType | Execution type config | E_ExecutionType |
| FWeaponAttackPower | Weapon damage values | E_AttackPower |

**Item System Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FItemInfo | Item definition | E_ItemCategory, E_ItemSubCategory |
| FItemInfoCount | Item with quantity | FItemInfo |
| FItemCategory | Item category config | None |
| FInventoryCategory | Inventory category | E_ItemCategory |
| FLootItem | Loot drop definition | FItemInfo |
| FWeightedLoot | Weighted loot entry | FLootItem |

**Equipment System Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FEquipmentInfo | Equipment definition | FItemInfo |
| FEquipmentSlot | Equipment slot config | E_ActionWeaponSlot |
| FEquipmentSocketInfo | Socket attachment data | None |
| FEquipmentStat | Equipment stat bonus | FGameplayTag |
| FEquipmentWeaponStatInfo | Weapon stat info | FEquipmentStat |
| FCurrentEquipment | Active equipment data | FEquipmentSlot |

**AI System Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FAiAttackEntry | AI attack definition | UAnimMontage |
| FAiBossPhase | Boss phase config | None |
| FAiPatrolPathInfo | Patrol path data | None |
| FAiRuleDistance | Distance-based AI rule | None |
| FAiRuleStat | Stat-based AI rule | FGameplayTag |
| FAiSenseLocationInfo | AI perception data | None |
| FAiSenseTargetInfo | AI target info | None |
| FAiStrafeInfo | AI strafe behavior | E_AI_StrafeLocations |

**Save System Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FSaveData | Main save data container | Multiple |
| FSaveGameInfo | Save slot info | None |
| FClassSaveInfo | Class save data | None |
| FInventoryItemsSaveInfo | Inventory save data | FItemInfo |
| FEquipmentItemsSaveInfo | Equipment save data | FEquipmentInfo |
| FProgressSaveInfo | Progress save data | FProgress |
| FWorldSaveInfo | World state save data | None |
| FNpcSaveInfo | NPC state save data | None |
| FSpawnedActorSaveInfo | Spawned actor data | None |

**Animation Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FAnimationData | Animation config | UAnimMontage |
| FMontage | Montage reference | UAnimMontage |
| FCardinalData | Directional animation data | None |

**Status Effect Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FStatusEffectApplication | Status effect apply data | None |
| FStatusEffectRankInfo | Status effect rank config | None |
| FStatusEffectStatChanges | Stat changes from effects | FStatChange |
| FStatusEffectTick | Tick damage config | None |
| FStatusEffectVfxInfo | Visual effect config | None |
| FStatusEffectOneShotAndTick | Combined effect data | FStatusEffectTick |
| FStatusEffectFrostbiteExample | Frostbite-specific | FStatusEffectStatChanges |
| FStatusEffectPlagueExample | Plague-specific | FStatusEffectStatChanges |

**Miscellaneous Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FActionsData | Action configuration | FGameplayTag |
| FActorClass | Actor class reference | None |
| FClass | Generic class reference | None |
| FDayNightInfo | Day/night cycle data | None |
| FDialogEntry | Dialog system entry | None |
| FDialogProgress | Dialog progress tracking | None |
| FDialogRequirement | Dialog requirements | None |
| FDoorLockInfo | Door lock data | E_KeyType |
| FFlaskData | Flask item data | None |
| FLevelChangeData | Level transition data | None |
| FLoadingScreenTip | Loading screen text | None |
| FProgress | Progress tracking | E_Progress |
| FSkeletalMeshData | Mesh configuration | None |
| FSprintCost | Sprint stamina cost | None |
| FWorldMeshInfo | World mesh data | E_WorldMeshStyle |
| FCreditsEntry | Credits screen data | None |
| FKeyMappingCorrelation | Input mapping data | None |

**Primitive Wrapper Structs:**
| Struct | Purpose | Dependencies |
|--------|---------|--------------|
| FBool | Boolean wrapper | None |
| FFloat | Float wrapper | None |
| FInt | Integer wrapper | None |
| FString | String wrapper | None |
| FName | Name wrapper | None |
| FObject | Object wrapper | None |
| FVector | Vector wrapper | None |
| FRotator | Rotator wrapper | None |
| FGuid | GUID wrapper | None |
| FEnumByte | Enum byte wrapper | None |

---

### Layer 1: Interfaces (18 total)

**Dependencies:** Layer 0 (Enums, Structs)

**Migration Strategy:**
1. Create C++ UInterface with BlueprintType, BlueprintImplementable functions
2. Functions become pure virtual in IInterfaceName class
3. Blueprint implements interface via class settings

| Interface | Purpose | Used By |
|-----------|---------|---------|
| BPI_GenericCharacter | Base character interface | All characters |
| BPI_Player | Player-specific interface | B_Soulslike_Character |
| BPI_Enemy | Enemy-specific interface | B_Soulslike_Enemy, B_Soulslike_Boss |
| BPI_NPC | NPC interface | B_Soulslike_NPC |
| BPI_Controller | Controller interface | PC_SoulslikeFramework |
| BPI_AIC | AI Controller interface | AIC_SoulslikeFramework |
| BPI_Item | Item interface | B_Item, all item children |
| BPI_Interactable | Interactable interface | B_Interactable, doors, chests |
| BPI_Projectile | Projectile interface | B_BaseProjectile |
| BPI_Executable | Execution interface | Execution system |
| BPI_ExecutionIndicator | Execution UI | HUD widgets |
| BPI_DestructibleHelper | Destructible support | B_Destructible |
| BPI_BossDoor | Boss door interface | B_BossDoor |
| BPI_RestingPoint | Rest point interface | B_RestingPoint |
| BPI_EnemyHealthbar | Enemy HP bar interface | Enemy HUD widgets |
| BPI_GameInstance | Game instance interface | GI_SoulslikeFramework |
| BPI_MainMenu | Main menu interface | Menu widgets |
| BPI_StatEntry | Stat entry UI interface | Stat display widgets |

**Recommended Migration Order (Interfaces):**
1. Core: BPI_GenericCharacter, BPI_Player, BPI_Enemy, BPI_NPC
2. Systems: BPI_Item, BPI_Interactable, BPI_Projectile
3. Combat: BPI_Executable, BPI_ExecutionIndicator
4. Game: BPI_GameInstance, BPI_Controller, BPI_AIC
5. UI: BPI_MainMenu, BPI_StatEntry, BPI_EnemyHealthbar

---

### Layer 2: Data Asset Base Classes

**Dependencies:** Layer 0-1

**Migration Strategy:**
1. Create C++ UPrimaryDataAsset subclass
2. Define all UPROPERTY fields
3. Existing Data Assets inherit from C++ class automatically

| Data Asset Class | Purpose | Instances |
|------------------|---------|-----------|
| PDA_Action | Action data definition | 27+ actions |
| PDA_Buff | Buff data definition | Multiple buffs |
| PDA_StatusEffect | Status effect data | 7+ status effects |
| PDA_AI_Ability | AI ability data | AI attacks |
| PDA_BaseCharacterInfo | Character info | Character configs |
| PDA_CombatReactionAnimData | Hit reaction anims | Player/Enemy |
| PDA_CustomSettings | Game settings | Custom options |
| PDA_DayNight | Day/night config | Time system |
| PDA_DefaultMeshData | Default meshes | Character meshes |
| PDA_ExecutionAnimData | Execution anims | Execution system |
| PDA_Calculations | Damage calculations | Combat math |

---

### Layer 3: Components (21 total)

**Dependencies:** Layer 0-2

**Current Status:**
- [x] UInputBufferComponent - MIGRATED
- [~] UActionManagerComponent - C++ ready, needs dependent fixes
- [~] UBuffManagerComponent - C++ ready, needs testing
- [~] UStatManagerComponent - C++ ready, needs testing
- [ ] All other components - Not started

| Component | Purpose | Dependencies | Priority |
|-----------|---------|--------------|----------|
| AC_InputBuffer | Input queueing | None | DONE |
| AC_ActionManager | Action execution | AC_InputBuffer, AC_StatManager | HIGH |
| AC_BuffManager | Buff management | AC_StatManager | HIGH |
| AC_StatManager | Stat management | None | HIGH |
| AC_CombatManager | Combat logic | AC_ActionManager, AC_StatManager | HIGH |
| AC_InteractionManager | Interaction handling | None | MEDIUM |
| AC_EquipmentManager | Equipment slots | AC_StatManager, AC_InventoryManager | MEDIUM |
| AC_InventoryManager | Inventory system | None | MEDIUM |
| AC_StatusEffectManager | Status effects | AC_StatManager | MEDIUM |
| AC_CollisionManager | Combat collision | None | MEDIUM |
| AC_LadderManager | Ladder climbing | AC_ActionManager | LOW |
| AC_LootDropManager | Loot drops | AC_InventoryManager | LOW |
| AC_ProgressManager | Progress tracking | None | LOW |
| AC_SaveLoadManager | Save/load system | Multiple | LOW |
| AC_RadarManager | Radar/minimap | None | LOW |
| AC_RadarElement | Radar element | AC_RadarManager | LOW |
| AC_DebugCentral | Debug tools | None | LOW |
| AC_AI_BehaviorManager | AI behavior | E_AI_States | MEDIUM |
| AC_AI_Boss | Boss AI | AC_AI_BehaviorManager | MEDIUM |
| AC_AI_CombatManager | AI combat | AC_AI_BehaviorManager | MEDIUM |
| AC_AI_InteractionManager | AI interaction | None | LOW |

**Recommended Migration Order (Components):**
1. **Phase 1 - Core Combat (already in progress):**
   - AC_InputBuffer (DONE)
   - AC_StatManager
   - AC_ActionManager
   - AC_BuffManager

2. **Phase 2 - Combat Extension:**
   - AC_CombatManager
   - AC_StatusEffectManager
   - AC_CollisionManager

3. **Phase 3 - Inventory/Equipment:**
   - AC_InventoryManager
   - AC_EquipmentManager
   - AC_LootDropManager

4. **Phase 4 - AI:**
   - AC_AI_BehaviorManager
   - AC_AI_CombatManager
   - AC_AI_Boss
   - AC_AI_InteractionManager

5. **Phase 5 - Miscellaneous:**
   - AC_InteractionManager
   - AC_LadderManager
   - AC_ProgressManager
   - AC_SaveLoadManager
   - AC_RadarManager
   - AC_RadarElement
   - AC_DebugCentral

---

### Layer 4: Core Blueprints (Base Classes)

**Dependencies:** Layer 0-3

These are Blueprint classes that inherit from UObject or AActor and serve as base classes.

| Blueprint | Parent | Purpose | Children |
|-----------|--------|---------|----------|
| B_Stat | UObject | Stat instance | B_HP, B_Stamina, B_FP, 20+ stats |
| B_Action | UObject | Action logic | 27 action children |
| B_Buff | UObject | Buff instance | B_Buff_AttackPower, etc. |
| B_StatusEffect | UObject | Status effect | 7 status effect children |
| B_Item | AActor | World item | B_Item_Weapon, etc. |
| B_Interactable | AActor | Interactable object | B_Door, B_Container, etc. |
| B_BaseProjectile | AActor | Projectile base | B_Projectile_*, etc. |
| B_AbilityEffectBase | AActor | Ability effect | B_Chaos_ForceField, etc. |

**Inheritance Trees:**

```
B_Stat (UObject)
├── B_HP
├── B_Stamina
├── B_FP
├── B_Strength
├── B_Dexterity
├── B_Intelligence
├── B_Faith
├── B_Arcane
├── B_Mind
├── B_Vigor
├── B_Endurance
├── B_Poise
├── B_Weight
├── B_Stance
├── B_Discovery
├── B_IncantationPower
├── B_AP_Physical
├── B_AP_Fire
├── B_AP_Frost
├── B_AP_Holy
├── B_AP_Lightning
├── B_AP_Magic
├── B_DN_Physical
├── B_DN_Fire
├── B_DN_Frost
├── B_DN_Holy
├── B_DN_Lightning
├── B_DN_Magic
├── B_Resistance_Focus
├── B_Resistance_Immunity
├── B_Resistance_Robustness
└── B_Resistance_Vitality

B_Action (UObject)
├── B_Action_Backstab
├── B_Action_ComboHeavy
├── B_Action_ComboLight_L
├── B_Action_ComboLight_R
├── B_Action_Crouch
├── B_Action_Dodge
├── B_Action_DrinkFlask_HP
├── B_Action_DualWieldAttack
├── B_Action_Execute
├── B_Action_GuardCancel
├── B_Action_GuardEnd
├── B_Action_GuardStart
├── B_Action_Jump
├── B_Action_JumpAttack
├── B_Action_PickupItemMontage
├── B_Action_ScrollWheel_LeftHand
├── B_Action_ScrollWheel_RightHand
├── B_Action_ScrollWheel_Tools
├── B_Action_SprintAttack
├── B_Action_StartSprinting
├── B_Action_StopSprinting
├── B_Action_ThrowProjectile
├── B_Action_TwoHandedStance_L
├── B_Action_TwoHandedStance_R
├── B_Action_UseEquippedTool
└── B_Action_WeaponAbility

B_Item (AActor, implements BPI_Item)
├── B_Item_Weapon
│   ├── B_Item_Weapon_Greatsword
│   ├── B_Item_Weapon_Katana
│   ├── B_Item_Weapon_PoisonSword
│   ├── B_Item_Weapon_Shield
│   ├── B_Item_Weapon_SwordExample01
│   ├── B_Item_Weapon_SwordExample02
│   └── B_Item_Weapon_BossMace
├── B_Item_AI_Weapon
│   ├── B_Item_AI_Weapon_BossMace
│   ├── B_Item_AI_Weapon_Greatsword
│   └── B_Item_AI_Weapon_Sword
└── B_Item_Lantern

B_StatusEffect (UObject)
├── B_StatusEffect_Bleed
├── B_StatusEffect_Burn
├── B_StatusEffect_Corruption
├── B_StatusEffect_Frostbite
├── B_StatusEffect_Madness
├── B_StatusEffect_Plague
├── B_StatusEffect_Poison
├── B_StatusEffectArea
├── B_StatusEffectBuildup
└── B_StatusEffectOneShot
```

---

### Layer 5: Character Blueprints

**Dependencies:** Layer 0-4

| Blueprint | Parent | Purpose | Components |
|-----------|--------|---------|------------|
| B_BaseCharacter | ACharacter | Base character | AC_StatManager, AC_StatusEffectManager, AC_BuffManager |
| B_Soulslike_Character | B_BaseCharacter | Player | + AC_InputBuffer, AC_ActionManager, AC_CombatManager, etc. |
| B_Soulslike_Enemy | B_BaseCharacter | Basic enemy | + AI components |
| B_Soulslike_Enemy_Guard | B_Soulslike_Enemy | Guard enemy | Specific config |
| B_Soulslike_Enemy_Showcase | B_Soulslike_Enemy | Demo enemy | Specific config |
| B_Soulslike_Boss | ??? | Boss base | Boss AI |
| B_Soulslike_Boss_Malgareth | B_Soulslike_Boss | Specific boss | Boss config |
| B_Soulslike_NPC | B_BaseCharacter | NPC base | NPC systems |
| B_Soulslike_NPC_ShowcaseGuide | B_Soulslike_NPC | Guide NPC | Specific config |
| B_Soulslike_NPC_ShowcaseVendor | B_Soulslike_NPC | Vendor NPC | Vendor config |

**Character Inheritance Tree:**
```
ACharacter (Engine)
└── B_BaseCharacter (implements BPI_GenericCharacter)
    ├── B_Soulslike_Character (implements BPI_Player)
    ├── B_Soulslike_Enemy (implements BPI_Enemy)
    │   ├── B_Soulslike_Enemy_Guard
    │   └── B_Soulslike_Enemy_Showcase
    ├── B_Soulslike_Boss (implements BPI_Enemy)
    │   └── B_Soulslike_Boss_Malgareth
    └── B_Soulslike_NPC (implements BPI_NPC)
        ├── B_Soulslike_NPC_ShowcaseGuide
        └── B_Soulslike_NPC_ShowcaseVendor
```

---

### Layer 6: Game Framework (5 classes)

**Dependencies:** Layer 0-5

| Class | Parent | Purpose |
|-------|--------|---------|
| GI_SoulslikeFramework | UGameInstance | Game instance |
| GM_SoulslikeFramework | AGameModeBase | Game mode |
| GM_Menu_SoulslikeFramework | AGameModeBase | Menu game mode |
| PC_SoulslikeFramework | APlayerController | Player controller |
| PC_Menu_SoulslikeFramework | APlayerController | Menu controller |
| GS_SoulslikeFramework | AGameStateBase | Game state |
| PS_SoulslikeFramework | APlayerState | Player state |

---

### Layer 7: Animation Blueprints (8 total)

**Dependencies:** Layer 0-5 (Character classes)

| Animation Blueprint | Purpose | Character |
|--------------------|---------|-----------|
| ABP_SoulslikeCharacter_Additive | Player animations | B_Soulslike_Character |
| ABP_SoulslikeEnemy | Enemy animations | B_Soulslike_Enemy |
| ABP_SoulslikeBossNew | Boss animations | B_Soulslike_Boss |
| ABP_SoulslikeNPC | NPC animations | B_Soulslike_NPC |
| ABP_Manny_PostProcess | Post-process male | General |
| ABP_Quinn_PostProcess | Post-process female | General |
| ALI_LocomotionStates | Locomotion linked | All characters |
| ALI_OverlayStates | Overlay linked | All characters |

**Migration Strategy:** Animation Blueprints stay as Blueprint but can call C++ functions from character base classes.

---

### Layer 8: Widgets (122 total)

**Dependencies:** All previous layers

**Migration Strategy:**
- Widgets stay as Blueprint (visual editing essential)
- Create C++ base classes for complex widget logic
- Widget Blueprints inherit from C++ for shared functionality

**Widget Categories:**
- HUD Widgets (health bars, stamina, etc.)
- Menu Widgets (main menu, pause menu)
- Inventory Widgets (inventory UI)
- Dialog Widgets (NPC dialog)
- Settings Widgets (options menus)
- Debug Widgets (development tools)

---

## Recommended Migration Sequence

### Phase 1: Foundation (1-2 weeks of work)
1. All 39 Enums → C++
2. Core Structs (FStatInfo, FRegen, FItemInfo, etc.) → C++
3. Verify type redirects work

### Phase 2: Interfaces (1 week)
1. Core interfaces (BPI_GenericCharacter, BPI_Player, BPI_Enemy)
2. System interfaces (BPI_Item, BPI_Interactable)

### Phase 3: Components - Core Combat (current work)
1. AC_StatManager - Complete migration
2. AC_ActionManager - Fix dependent callers
3. AC_BuffManager - Complete testing
4. AC_CombatManager - New migration

### Phase 4: Components - Extended
1. AC_StatusEffectManager
2. AC_InventoryManager
3. AC_EquipmentManager
4. AC_CollisionManager

### Phase 5: Core Blueprints
1. B_Stat and all stat children
2. B_Action and all action children
3. B_Item and item children
4. B_StatusEffect and children

### Phase 6: Character Blueprints
1. B_BaseCharacter
2. B_Soulslike_Character (Player)
3. B_Soulslike_Enemy
4. B_Soulslike_Boss
5. B_Soulslike_NPC

### Phase 7: AI Components
1. AC_AI_BehaviorManager
2. AC_AI_CombatManager
3. AC_AI_Boss

### Phase 8: Game Framework
1. GI_SoulslikeFramework
2. GM_SoulslikeFramework
3. PC_SoulslikeFramework

### Phase 9: Remaining Systems
1. Save/Load system
2. Progress system
3. Interaction system
4. Ladder system

### Phase 10: Animation & Polish
1. Animation Blueprint integration
2. Widget C++ base classes (if needed)
3. Performance optimization

---

## Current Status

### Completed:
- [x] AC_InputBuffer → UInputBufferComponent

### In Progress:
- [~] AC_ActionManager → UActionManagerComponent (C++ ready, dependent fixes needed)
- [~] AC_BuffManager → UBuffManagerComponent (C++ ready)
- [~] AC_StatManager → USoulslikeStatComponent (C++ ready)

### Not Started:
- [ ] All other components
- [ ] All enums/structs (except those needed by completed components)
- [ ] All interfaces
- [ ] All data asset classes
- [ ] All core blueprints
- [ ] All character blueprints
- [ ] Game framework classes

---

## Critical Dependencies Map

```
                    ┌─────────────────┐
                    │   Layer 0       │
                    │ Enums & Structs │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │   Layer 1       │
                    │   Interfaces    │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │   Layer 2       │
                    │  Data Assets    │
                    └────────┬────────┘
                             │
         ┌───────────────────┼───────────────────┐
         │                   │                   │
┌────────▼────────┐ ┌────────▼────────┐ ┌────────▼────────┐
│ AC_StatManager  │ │ AC_InputBuffer  │ │ AC_Inventory    │
│    (Stats)      │ │   (Input)       │ │   (Items)       │
└────────┬────────┘ └────────┬────────┘ └────────┬────────┘
         │                   │                   │
         └───────────┬───────┴───────────────────┘
                     │
            ┌────────▼────────┐
            │ AC_ActionManager │
            │ AC_CombatManager │
            │ AC_BuffManager   │
            └────────┬────────┘
                     │
            ┌────────▼────────┐
            │ B_BaseCharacter │
            └────────┬────────┘
                     │
    ┌────────────────┼────────────────┐
    │                │                │
┌───▼───┐      ┌─────▼─────┐    ┌─────▼─────┐
│Player │      │  Enemy    │    │   NPC     │
└───────┘      └───────────┘    └───────────┘
```

---

## Files Reference

**JSON Exports:** `C:\scripts\SLFConversion_Migration\Backups\SoulsClean\Exports\BlueprintDNA\`

**C++ Source:** `C:\scripts\SLFConversion\Source\SLFConversion\`

**Migration Scripts:** `C:\scripts\SLFConversion\migrate_*.py`

**Backups:** `C:\scripts\backups\`

---

## Next Steps

1. **Immediate:** Fix AC_ActionManager dependent callers (B_Soulslike_Character)
2. **Short-term:** Complete Phase 3 (Core Combat Components)
3. **Medium-term:** Begin Phase 1 (Foundation Types) in parallel
4. **Long-term:** Follow migration sequence through all phases

---

*Document created: December 31, 2025*
*Last updated: December 31, 2025*
