# SoulslikeFramework Blueprint Migration Checklist

**Last Updated:** 2026-01-06
**Total Blueprints:** ~312 reparented (ABP_SoulslikeBossNew, B_Door_Demo added)
**Migration Result:** 0 errors, 6108 warnings (non-critical)
**Migration Script:** `run_migration.py`

---

## Legend

- [x] **Migrated** - Reparented to C++ class, EventGraph cleared
- [~] **Partial** - Reparented only (EventGraph preserved for external callers)
- [ ] **Not Migrated** - Still Blueprint-only
- [!] **Excluded** - Intentionally skipped (crashes, special handling needed)

---

## 1. ENGINE BASE CLASSES (Top of Hierarchy)

These are UE5 engine classes that our Blueprints inherit from.

### Game Framework
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | GM_Menu_SoulslikeFramework | GameModeBase | GM_Menu_SoulslikeFramework |
| [ ] | GM_SoulslikeFramework | GameModeBase | (Keep Blueprint logic) |
| [x] | GI_SoulslikeFramework | GameInstance | SLFGameInstance |
| [x] | GS_SoulslikeFramework | GameStateBase | GS_SoulslikeFramework |
| [x] | PS_SoulslikeFramework | PlayerState | PS_SoulslikeFramework |
| [x] | PC_SoulslikeFramework | PlayerController | SLFPlayerController |
| [x] | PC_Menu_SoulslikeFramework | PlayerController | PC_Menu_SoulslikeFramework |
| [x] | AIC_SoulslikeFramework | AIController | SLFAIController |

### Save System
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | SG_SoulslikeFramework | SaveGame | SG_SoulslikeFramework |
| [x] | SG_SaveSlots | SaveGame | SG_SaveSlots |

---

## 2. ACTOR COMPONENTS (21)

All inherit from `ActorComponent`.

### Manager Components (17)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | AC_StatManager | ActorComponent | StatManagerComponent |
| [x] | AC_InputBuffer | ActorComponent | InputBufferComponent |
| [x] | AC_BuffManager | ActorComponent | BuffManagerComponent |
| [x] | AC_StatusEffectManager | ActorComponent | StatusEffectManagerComponent |
| [x] | AC_InventoryManager | ActorComponent | InventoryManagerComponent |
| [x] | AC_CollisionManager | ActorComponent | CollisionManagerComponent |
| [x] | AC_RadarManager | ActorComponent | RadarManagerComponent |
| [x] | AC_RadarElement | ActorComponent | RadarElementComponent |
| [x] | AC_DebugCentral | ActorComponent | DebugCentralComponent |
| [x] | AC_ProgressManager | ActorComponent | ProgressManagerComponent |
| [x] | AC_SaveLoadManager | ActorComponent | SaveLoadManagerComponent |
| [x] | AC_LadderManager | ActorComponent | LadderManagerComponent |
| [x] | AC_LootDropManager | ActorComponent | LootDropManagerComponent |
| [x] | AC_ActionManager | ActorComponent | AC_ActionManager (C++ implementation complete) |
| [x] | AC_CombatManager | ActorComponent | AC_CombatManager (C++ implementation complete) |
| [x] | AC_EquipmentManager | ActorComponent | AC_EquipmentManager (C++ implementation complete) |
| [x] | AC_InteractionManager | ActorComponent | AC_InteractionManager (C++ implementation complete) |
| [N/A] | AC_TargetManager | ActorComponent | (Doesn't exist in content) |

### AI Components (4)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | AC_AI_Boss | ActorComponent | AIBossComponent |
| [x] | AC_AI_BehaviorManager | ActorComponent | AIBehaviorManagerComponent |
| [x] | AC_AI_InteractionManager | ActorComponent | AIInteractionManagerComponent |
| [x] | AC_AI_CombatManager | ActorComponent | AC_AI_CombatManager (Already in MIGRATION_MAP) |

---

## 3. CHARACTERS (10)

### Character Hierarchy
```
Character (Engine)
└── B_BaseCharacter
    ├── B_Soulslike_Character (Player)
    ├── B_Soulslike_Enemy
    │   ├── B_Soulslike_Enemy_Guard
    │   └── B_Soulslike_Enemy_Showcase
    ├── B_Soulslike_Boss
    │   └── B_Soulslike_Boss_Malgareth
    └── B_Soulslike_NPC
        ├── B_Soulslike_NPC_ShowcaseGuide
        └── B_Soulslike_NPC_ShowcaseVendor
```

| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [~] | B_BaseCharacter | Character | SLFBaseCharacter (Keep vars) |
| [x] | B_Soulslike_Character | B_BaseCharacter_C | SLFSoulslikeCharacter |
| [x] | B_Soulslike_Enemy | B_BaseCharacter_C | SLFSoulslikeEnemy |
| [x] | B_Soulslike_Enemy_Guard | B_Soulslike_Enemy_C | SLFEnemyGuard |
| [x] | B_Soulslike_Enemy_Showcase | B_Soulslike_Enemy_C | SLFEnemyShowcase |
| [x] | B_Soulslike_Boss | B_BaseCharacter_C | SLFSoulslikeBoss |
| [x] | B_Soulslike_Boss_Malgareth | B_Soulslike_Boss_C | SLFBossMalgareth |
| [x] | B_Soulslike_NPC | B_BaseCharacter_C | SLFSoulslikeNPC |
| [x] | B_Soulslike_NPC_ShowcaseGuide | B_Soulslike_NPC_C | SLFNPCShowcaseGuide |
| [x] | B_Soulslike_NPC_ShowcaseVendor | B_Soulslike_NPC_C | SLFNPCShowcaseVendor |

---

## 4. ANIMATION BLUEPRINTS (4)

| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | ABP_SoulslikeCharacter_Additive | AnimInstance | (EventGraph cleared) |
| [x] | ABP_SoulslikeNPC | AnimInstance | (EventGraph cleared) |
| [x] | ABP_SoulslikeEnemy | AnimInstance | (EventGraph cleared) |
| [x] | ABP_SoulslikeBossNew | AnimInstance | (EventGraph cleared - C++ SLFBossAnimInstance has vars) |

---

## 5. ANIMATION NOTIFIES (25)

### AnimNotifyStates (12)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | ANS_InputBuffer | AnimNotifyState | SLFAnimNotifyStateInputBuffer |
| [x] | ANS_RegisterAttackSequence | AnimNotifyState | SLFAnimNotifyStateInputBuffer |
| [x] | ANS_InvincibilityFrame | AnimNotifyState | SLFAnimNotifyStateInvincibility |
| [x] | ANS_HyperArmor | AnimNotifyState | SLFAnimNotifyStateHyperArmor |
| [x] | ANS_WeaponTrace | AnimNotifyState | SLFAnimNotifyStateWeaponTrace |
| [x] | ANS_Trail | AnimNotifyState | SLFAnimNotifyStateTrail |
| [x] | ANS_FistTrace | AnimNotifyState | SLFAnimNotifyStateFistTrace |
| [x] | ANS_AI_FistTrace | AnimNotifyState | SLFAnimNotifyStateFistTrace |
| [x] | ANS_AI_RotateTowardsTarget | AnimNotifyState | SLFAnimNotifyStateAIRotateToTarget |
| [x] | ANS_AI_Trail | AnimNotifyState | SLFAnimNotifyStateTrail |
| [x] | ANS_AI_WeaponTrace | AnimNotifyState | SLFAnimNotifyStateWeaponTrace |
| [x] | ANS_ToggleChaosField | AnimNotifyState | SLFAnimNotifyStateTrail |

### AnimNotifies (13)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | AN_AdjustStat | AnimNotify | SLFAnimNotifyAdjustStat |
| [x] | AN_AI_SpawnProjectile | AnimNotify | SLFAnimNotifySpawnProjectile |
| [x] | AN_AoeDamage | AnimNotify | SLFAnimNotifyAoeDamage |
| [x] | AN_CameraShake | AnimNotify | SLFAnimNotifyCameraShake |
| [x] | AN_FootstepTrace | AnimNotify | SLFAnimNotifyFootstepTrace |
| [x] | AN_InterruptMontage | AnimNotify | SLFAnimNotifyInterruptMontage |
| [x] | AN_LaunchField | AnimNotify | SLFAnimNotifySpawnProjectile |
| [x] | AN_PlayCameraSequence | AnimNotify | SLFAnimNotifyCameraShake |
| [x] | AN_SetAiState | AnimNotify | SLFAnimNotifySetAIState |
| [x] | AN_SetMovementMode | AnimNotify | SLFAnimNotifySetMovementMode |
| [x] | AN_SpawnProjectile | AnimNotify | SLFAnimNotifySpawnProjectile |
| [x] | AN_TryGuard | AnimNotify | SLFAnimNotifyTryGuard |
| [x] | AN_WorldCameraShake | AnimNotify | SLFAnimNotifyCameraShake |

---

## 6. WORLD ACTORS

### Items (Base Classes)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Item | Actor | SLFItemBase |
| [~] | B_PickupItem | Actor | SLFPickupItemBase (Keep vars) |
| [x] | B_Item_Weapon | B_Item_C | SLFWeaponBase |
| [x] | B_Item_AI_Weapon | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Lantern | B_Item_C | SLFItemBase |

### Player Weapons (7)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Item_Weapon_BossMace | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Weapon_Greatsword | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Weapon_Katana | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Weapon_PoisonSword | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Weapon_Shield | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Weapon_SwordExample01 | B_Item_Weapon_C | SLFWeaponBase |
| [x] | B_Item_Weapon_SwordExample02 | B_Item_Weapon_C | SLFWeaponBase |

### AI Weapons (3)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Item_AI_Weapon_BossMace | B_Item_AI_Weapon_C | SLFWeaponBase |
| [x] | B_Item_AI_Weapon_Greatsword | B_Item_AI_Weapon_C | SLFWeaponBase |
| [x] | B_Item_AI_Weapon_Sword | B_Item_AI_Weapon_C | SLFWeaponBase |

### Interactables (8)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [~] | B_Interactable | Actor | SLFInteractableBase (Keep vars) |
| [~] | B_Door | B_Interactable_C | SLFDoorBase (Keep vars) |
| [~] | B_Ladder | B_Interactable_C | SLFLadderBase (Keep vars) |
| [~] | B_RestingPoint | B_Interactable_C | SLFRestingPointBase (Keep vars) |
| [x] | B_Container | B_Interactable_C | SLFContainer |
| [x] | B_BossDoor | B_Interactable_C | SLFInteractableBase |
| [x] | B_DeathCurrency | B_Interactable_C | SLFInteractableBase |
| [x] | B_Door_Demo | B_Door_C | SLFDoorDemo |

### Projectiles (3)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_BaseProjectile | Actor | SLFProjectileBase |
| [x] | B_Projectile_Boss_Fireball | B_BaseProjectile_C | SLFProjectileBase |
| [x] | B_Projectile_ThrowingKnife | B_BaseProjectile_C | SLFProjectileBase |

### Level Design (4)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_DeathTrigger | Actor | SLFInteractableBase |
| [x] | B_Destructible | Actor | SLFInteractableBase |
| [x] | B_LocationActor | Actor | SLFInteractableBase |
| [x] | B_Torch | Actor | SLFInteractableBase |

### Misc World Actors (3)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Chaos_ForceField | FieldSystemActor | SLFInteractableBase |
| [x] | B_SequenceActor | Actor | SLFInteractableBase |
| [x] | B_PatrolPath | Actor | SLFInteractableBase |

---

## 7. AI SYSTEM

### AI Controller
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | AIC_SoulslikeFramework | AIController | SLFAIController |

### BT Tasks (13)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | BTT_ClearKey | BTTask_BlueprintBase | BTT_ClearKey |
| [x] | BTT_GetCurrentLocation | BTTask_BlueprintBase | BTT_GetCurrentLocation |
| [x] | BTT_GetRandomPoint | BTTask_BlueprintBase | BTT_GetRandomPoint |
| [x] | BTT_GetRandomPointNearStartPosition | BTTask_BlueprintBase | BTT_GetRandomPointNearStartPosition |
| [x] | BTT_GetStrafePointAroundTarget | BTTask_BlueprintBase | BTT_GetStrafePointAroundTarget |
| [x] | BTT_PatrolPath | BTTask_BlueprintBase | BTT_PatrolPath |
| [x] | BTT_SetKey | BTTask_BlueprintBase | BTT_SetKey |
| [x] | BTT_SetMovementMode | BTTask_BlueprintBase | BTT_SetMovementMode |
| [x] | BTT_SimpleMoveTo | BTTask_BlueprintBase | BTT_SimpleMoveTo |
| [x] | BTT_SwitchState | BTTask_BlueprintBase | BTT_SwitchState |
| [x] | BTT_SwitchToPreviousState | BTTask_BlueprintBase | BTT_SwitchToPreviousState |
| [x] | BTT_ToggleFocus | BTTask_BlueprintBase | BTT_ToggleFocus |
| [x] | BTT_TryExecuteAbility | BTTask_BlueprintBase | BTT_TryExecuteAbility |

### BT Services (5)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | BTS_ChaseBounds | BTService_BlueprintBase | BTS_ChaseBounds |
| [x] | BTS_DistanceCheck | BTService_BlueprintBase | BTS_DistanceCheck |
| [x] | BTS_IsTargetDead | BTService_BlueprintBase | BTS_IsTargetDead |
| [x] | BTS_SetMovementModeBasedOnDistance | BTService_BlueprintBase | BTS_SetMovementModeBasedOnDistance |
| [x] | BTS_TryGetAbility | BTService_BlueprintBase | BTS_TryGetAbility |

### Behavior Trees & Blackboards
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [N/A] | BB_Standard | Blackboard | (Asset - AI data definition) |
| [N/A] | BT_Enemy | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_Combat | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_Idle | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_Investigating | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_OutOfBounds | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_PatrolPath | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_PoiseBroken | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_RandomRoam | BehaviorTree | (Asset - AI behavior tree) |
| [N/A] | BT_Uninterruptable | BehaviorTree | (Asset - AI behavior tree) |

---

## 8. DATA ASSETS

### Primary Data Asset Classes
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | PDA_Action | PrimaryDataAsset | PDA_ActionBase |
| [x] | PDA_Item | PrimaryDataAsset | PDA_Item |
| [x] | PDA_Dialog | PrimaryDataAsset | PDA_Dialog |
| [x] | PDA_DefaultMeshData | PrimaryDataAsset | PDA_DefaultMeshData |
| [x] | PDA_AI_Ability | PrimaryDataAsset | PDA_AI_Ability |
| [x] | PDA_Buff | PrimaryDataAsset | PDA_Buff |
| [x] | PDA_Calculations | PrimaryDataAsset | PDA_Calculations |
| [x] | PDA_AnimData | PrimaryDataAsset | PDA_AnimData |
| [x] | PDA_CombatReactionAnimData | PrimaryDataAsset | PDA_CombatReactionAnimData |
| [x] | PDA_CombatReactionAnimData_Player | PrimaryDataAsset | PDA_CombatReactionAnimData_Player |
| [x] | PDA_LadderAnimData | PrimaryDataAsset | PDA_LadderAnimData |
| [x] | PDA_PoiseBreakAnimData | PrimaryDataAsset | PDA_PoiseBreakAnimData |
| [x] | PDA_WeaponAnimset | PrimaryDataAsset | PDA_WeaponAnimset |
| [x] | PDA_WeaponAbility | PrimaryDataAsset | PDA_WeaponAbility |
| [x] | PDA_StatusEffect | PrimaryDataAsset | PDA_StatusEffect |
| [x] | PDA_Vendor | PrimaryDataAsset | PDA_Vendor |
| [x] | PDA_MainMenuData | PrimaryDataAsset | PDA_MainMenuData |
| [x] | PDA_BaseCharacterInfo | PrimaryDataAsset | PDA_BaseCharacterInfo |
| [x] | PDA_Credits | PrimaryDataAsset | PDA_Credits |
| [x] | PDA_CustomSettings | PrimaryDataAsset | PDA_CustomSettings |
| [x] | PDA_DayNight | PrimaryDataAsset | PDA_DayNight |
| [x] | PDA_LoadingScreens | PrimaryDataAsset | PDA_LoadingScreens |
| [x] | PDA_MovementSpeedData | PrimaryDataAsset | PDA_MovementSpeedData |

### Stat System (33)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Stat | Object | SLFStatBase |
| [x] | B_HP | B_Stat_C | SLFStatHP |
| [x] | B_FP | B_Stat_C | SLFStatFP |
| [x] | B_Stamina | B_Stat_C | SLFStatStamina |
| [x] | B_Vigor | B_Stat_C | SLFStatVigor |
| [x] | B_Mind | B_Stat_C | SLFStatMind |
| [x] | B_Endurance | B_Stat_C | SLFStatEndurance |
| [x] | B_Strength | B_Stat_C | SLFStatStrength |
| [x] | B_Dexterity | B_Stat_C | SLFStatDexterity |
| [x] | B_Intelligence | B_Stat_C | SLFStatIntelligence |
| [x] | B_Faith | B_Stat_C | SLFStatFaith |
| [x] | B_Arcane | B_Stat_C | SLFStatArcane |
| [x] | B_Weight | B_Stat_C | SLFStatWeight |
| [x] | B_Poise | B_Stat_C | SLFStatPoise |
| [x] | B_Stance | B_Stat_C | SLFStatStance |
| [x] | B_IncantationPower | B_Stat_C | SLFStatIncantationPower |
| [x] | B_Discovery | B_Stat_C | SLFStatBase |
| [x] | B_Resistance_Focus | B_Stat_C | SLFResistanceFocus |
| [x] | B_Resistance_Immunity | B_Stat_C | SLFResistanceImmunity |
| [x] | B_Resistance_Robustness | B_Stat_C | SLFResistanceRobustness |
| [x] | B_Resistance_Vitality | B_Stat_C | SLFResistanceVitality |
| [x] | B_AP_Physical | B_Stat_C | SLFAttackPowerPhysical |
| [x] | B_AP_Fire | B_Stat_C | SLFAttackPowerFire |
| [x] | B_AP_Frost | B_Stat_C | SLFAttackPowerFrost |
| [x] | B_AP_Lightning | B_Stat_C | SLFAttackPowerLightning |
| [x] | B_AP_Magic | B_Stat_C | SLFAttackPowerMagic |
| [x] | B_AP_Holy | B_Stat_C | SLFAttackPowerHoly |
| [x] | B_DN_Physical | B_Stat_C | SLFDamageNegationPhysical |
| [x] | B_DN_Fire | B_Stat_C | SLFDamageNegationFire |
| [x] | B_DN_Frost | B_Stat_C | SLFDamageNegationFrost |
| [x] | B_DN_Lightning | B_Stat_C | SLFDamageNegationLightning |
| [x] | B_DN_Magic | B_Stat_C | SLFDamageNegationMagic |
| [x] | B_DN_Holy | B_Stat_C | SLFDamageNegationHoly |

### Status Effects (11)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_StatusEffect | Object | SLFStatusEffectBase |
| [x] | B_StatusEffect_Bleed | B_StatusEffect_C | SLFStatusEffectBleed |
| [x] | B_StatusEffect_Burn | B_StatusEffect_C | SLFStatusEffectBurn |
| [x] | B_StatusEffect_Corruption | B_StatusEffect_C | SLFStatusEffectCorruption |
| [x] | B_StatusEffect_Frostbite | B_StatusEffect_C | SLFStatusEffectFrostbite |
| [x] | B_StatusEffect_Madness | B_StatusEffect_C | SLFStatusEffectMadness |
| [x] | B_StatusEffect_Plague | B_StatusEffect_C | SLFStatusEffectPlague |
| [x] | B_StatusEffect_Poison | B_StatusEffect_C | SLFStatusEffectPoison |
| [x] | B_StatusEffectArea | Object | SLFStatusEffectArea |
| [x] | B_StatusEffectOneShot | Object | SLFStatusEffectOneShot |
| [x] | B_StatusEffectBuildup | B_Stat_C | SLFStatusEffectBuildup |

### Buffs (3)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Buff | Object | SLFBuffBase |
| [x] | B_Buff_AttackPower | B_Buff_C | SLFBuffAttackPower |
| [x] | DA_Buff_AttackPower | PDA_Buff_C | Data instance - inherits C++ via PDA_Buff |

### Action System (29 Logic + 31 Data)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_Action | Object | SLFActionBase |
| [x] | B_Action_Backstab | B_Action_C | SLFActionBackstab |
| [x] | B_Action_ComboHeavy | B_Action_C | SLFActionComboHeavy |
| [x] | B_Action_ComboLight_L | B_Action_C | SLFActionComboLightL |
| [x] | B_Action_ComboLight_R | B_Action_C | SLFActionComboLightR |
| [x] | B_Action_Crouch | B_Action_C | SLFActionCrouch |
| [x] | B_Action_Dodge | B_Action_C | SLFActionDodge |
| [x] | B_Action_DrinkFlask_HP | B_Action_C | SLFActionDrinkFlaskHP |
| [x] | B_Action_DualWieldAttack | B_Action_C | SLFActionDualWieldAttack |
| [x] | B_Action_Execute | B_Action_C | SLFActionExecute |
| [x] | B_Action_GuardStart | B_Action_C | SLFActionGuardStart |
| [x] | B_Action_GuardEnd | B_Action_C | SLFActionGuardEnd |
| [x] | B_Action_GuardCancel | B_Action_C | SLFActionGuardCancel |
| [x] | B_Action_Jump | B_Action_C | SLFActionJump |
| [x] | B_Action_JumpAttack | B_Action_C | SLFActionJumpAttack |
| [x] | B_Action_PickupItemMontage | B_Action_C | SLFActionPickupItemMontage |
| [x] | B_Action_ScrollWheel_LeftHand | B_Action_C | SLFActionScrollWheelLeftHand |
| [x] | B_Action_ScrollWheel_RightHand | B_Action_C | SLFActionScrollWheelRightHand |
| [x] | B_Action_ScrollWheel_Tools | B_Action_C | SLFActionScrollWheelTools |
| [x] | B_Action_SprintAttack | B_Action_C | SLFActionSprintAttack |
| [x] | B_Action_StartSprinting | B_Action_C | SLFActionStartSprinting |
| [x] | B_Action_StopSprinting | B_Action_C | SLFActionStopSprinting |
| [x] | B_Action_ThrowProjectile | B_Action_C | SLFActionThrowProjectile |
| [x] | B_Action_TwoHandedStance_L | B_Action_C | SLFActionTwoHandedStanceL |
| [x] | B_Action_TwoHandedStance_R | B_Action_C | SLFActionTwoHandedStanceR |
| [x] | B_Action_UseEquippedTool | B_Action_C | SLFActionUseEquippedTool |
| [x] | B_Action_WeaponAbility | B_Action_C | SLFActionWeaponAbility |

### Action Data Assets (26) - Instances of PDA_Action (inherit from UPDA_Action)
| Status | Blueprint | Notes |
|--------|-----------|-------|
| [x] | DA_Action_Backstab | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_ComboHeavy | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_ComboLight_L | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_ComboLight_R | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_Crouch | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_Dodge | Data instance - DodgeMontages migrated |
| [x] | DA_Action_DrinkFlask_HP | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_DualWieldAttack | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_Execute | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_GuardCancel | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_GuardEnd | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_GuardStart | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_Jump | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_JumpAttack | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_PickupItemMontage | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_Projectile | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_ScrollWheel_LeftHand | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_ScrollWheel_RightHand | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_ScrollWheel_Tools | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_SprintAttack | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_StartSprinting | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_StopSprinting | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_TwoHandedStance_L | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_TwoHandedStance_R | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_UseEquippedTool | Data instance - inherits C++ via PDA_Action |
| [x] | DA_Action_WeaponAbility | Data instance - inherits C++ via PDA_Action |

### Item Data Assets (21) - Instances of PDA_Item (inherit from UPDA_Item)
| Status | Blueprint | Notes |
|--------|-----------|-------|
| [x] | DA_Apple | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_BossMace | Data instance - inherits C++ via PDA_Item |
| [x] | DA_CrimsonEverbloom | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_Cube | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_ExampleArmor | Data instance - inherits C++ via PDA_Item |
| [x] | DA_ExampleArmor02 | Data instance - inherits C++ via PDA_Item |
| [x] | DA_ExampleBracers | Data instance - inherits C++ via PDA_Item |
| [x] | DA_ExampleGreaves | Data instance - inherits C++ via PDA_Item |
| [x] | DA_ExampleHat | Data instance - inherits C++ via PDA_Item |
| [x] | DA_ExampleHelmet | Data instance - inherits C++ via PDA_Item |
| [x] | DA_ExampleTalisman | Data instance - inherits C++ via PDA_Item |
| [x] | DA_Greatsword | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_HealthFlask | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_Katana | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_Lantern | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_PoisonSword | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_PrisonKey | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_Shield01 | Data instance - inherits C++ via PDA_Item |
| [x] | DA_Sword01 | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_Sword02 | Data instance - WorldNiagaraSystem migrated |
| [x] | DA_ThrowingKnife | Data instance - WorldNiagaraSystem migrated |

### Ability Effects (1)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_AbilityEffectBase | Object | SLFAbilityEffectBase |

---

## 9. INTERFACES (18)

| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | BPI_AIC | Interface | SLFAICInterface |
| [x] | BPI_BossDoor | Interface | SLFBossDoorInterface |
| [x] | BPI_Controller | Interface | SLFControllerInterface |
| [x] | BPI_DestructibleHelper | Interface | SLFDestructibleHelperInterface |
| [x] | BPI_Enemy | Interface | SLFEnemyInterface |
| [x] | BPI_EnemyHealthbar | Interface | SLFEnemyHealthbarInterface |
| [x] | BPI_Executable | Interface | SLFExecutableInterface |
| [x] | BPI_ExecutionIndicator | Interface | SLFExecutionIndicatorInterface |
| [x] | BPI_GameInstance | Interface | SLFGameInstanceInterface |
| [x] | BPI_GenericCharacter | Interface | SLFGenericCharacterInterface |
| [x] | BPI_Interactable | Interface | SLFInteractableInterface |
| [x] | BPI_Item | Interface | SLFItemInterface |
| [x] | BPI_MainMenu | Interface | SLFMainMenuInterface |
| [x] | BPI_NPC | Interface | SLFNPCInterface |
| [x] | BPI_Player | Interface | SLFPlayerInterface |
| [x] | BPI_Projectile | Interface | SLFProjectileInterface |
| [x] | BPI_RestingPoint | Interface | SLFRestingPointInterface |
| [x] | BPI_StatEntry | Interface | SLFStatEntryInterface |

---

## 10. WIDGETS (122+)

### Core/Base Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Navigable | UserWidget | W_Navigable |
| [x] | W_Navigable_InputReader | UserWidget | W_Navigable_InputReader |
| [x] | W_GenericButton | UserWidget | W_GenericButton |
| [x] | W_GenericError | UserWidget | W_GenericError |

### HUD Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_HUD | UserWidget | W_HUD |
| [x] | W_AbilityDisplay | UserWidget | W_AbilityDisplay |
| [~] | W_BigScreenMessage | UserWidget | W_BigScreenMessage (EventGraph preserved) |
| [x] | W_Boss_Healthbar | UserWidget | W_Boss_Healthbar |
| [x] | W_Buff | UserWidget | W_Buff |
| [x] | W_CurrencyContainer | UserWidget | W_CurrencyContainer |
| [~] | W_Dialog | UserWidget | W_Dialog (EventGraph preserved) |
| [x] | W_FirstLootNotification | UserWidget | W_FirstLootNotification |
| [~] | W_Interaction | UserWidget | W_Interaction (EventGraph preserved) |
| [~] | W_InteractionError | UserWidget | W_InteractionError (EventGraph preserved) |
| [x] | W_ItemWheelSlot | UserWidget | W_ItemWheelSlot |
| [x] | W_ItemWheel_NextSlot | UserWidget | W_ItemWheel_NextSlot |
| [x] | W_LoadingScreen | UserWidget | W_LoadingScreen |
| [x] | W_LootNotification | UserWidget | W_LootNotification |
| [x] | W_Resources | UserWidget | W_Resources |
| [x] | W_SkipCinematic | UserWidget | W_SkipCinematic |
| [x] | W_StatusEffectBar | UserWidget | W_StatusEffectBar |
| [x] | W_StatusEffectNotification | UserWidget | W_StatusEffectNotification |

### Equipment Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Equipment | UserWidget | W_Equipment |
| [x] | W_EquipmentSlot | UserWidget | W_EquipmentSlot |
| [x] | W_Equipment_Item_AttackPower | UserWidget | W_Equipment_Item_AttackPower |
| [x] | W_Equipment_Item_DamageNegation | UserWidget | W_Equipment_Item_DamageNegation |
| [x] | W_Equipment_Item_ItemEffect | UserWidget | W_Equipment_Item_ItemEffect |
| [x] | W_Equipment_Item_OnUseEffect | UserWidget | W_Equipment_Item_OnUseEffect |
| [x] | W_Equipment_Item_RequiredStats | UserWidget | W_Equipment_Item_RequiredStats |
| [x] | W_Equipment_Item_Resistance | UserWidget | W_Equipment_Item_Resistance |
| [x] | W_Equipment_Item_StatScaling | UserWidget | W_Equipment_Item_StatScaling |
| [x] | W_Equipment_Item_StatsGranted | UserWidget | W_Equipment_Item_StatsGranted |
| [x] | W_ItemInfoEntry | UserWidget | W_ItemInfoEntry |
| [x] | W_ItemInfoEntry_RequiredStats | UserWidget | W_ItemInfoEntry_RequiredStats |
| [x] | W_ItemInfoEntry_StatScaling | UserWidget | W_ItemInfoEntry_StatScaling |

### Inventory Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Inventory | UserWidget | W_Inventory |
| [x] | W_InventoryAction | UserWidget | W_InventoryAction |
| [x] | W_InventoryActionAmount | UserWidget | W_InventoryActionAmount |
| [x] | W_InventorySlot | UserWidget | W_InventorySlot |
| [x] | W_Inventory_ActionButton | UserWidget | W_Inventory_ActionButton |
| [x] | W_Inventory_CategoryEntry | UserWidget | W_Inventory_CategoryEntry |

### Main Menu Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_MainMenu | UserWidget | W_MainMenu |
| [x] | W_MainMenu_Button | UserWidget | W_MainMenu_Button |
| [x] | W_CharacterSelection | UserWidget | W_CharacterSelection |
| [x] | W_CharacterSelectionCard | UserWidget | W_CharacterSelectionCard |
| [x] | W_CharacterSelection_StatEntry | UserWidget | W_CharacterSelection_StatEntry |
| [x] | W_CreditEntry | UserWidget | W_CreditEntry |
| [x] | W_CreditEntry_Extra | UserWidget | W_CreditEntry_Extra |
| [x] | W_CreditEntry_Sub | UserWidget | W_CreditEntry_Sub |
| [x] | W_CreditEntry_Sub_NameEntry | UserWidget | W_CreditEntry_Sub_NameEntry |
| [x] | W_Credits | UserWidget | W_Credits |
| [x] | W_LoadGame | UserWidget | W_LoadGame |
| [x] | W_LoadGame_Entry | UserWidget | W_LoadGame_Entry |

### Rest Menu Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_RestMenu | UserWidget | W_RestMenu |
| [x] | W_RestMenu_Button | UserWidget | W_RestMenu_Button |
| [x] | W_RestMenu_TimeEntry | UserWidget | W_RestMenu_TimeEntry |
| [x] | W_TimePass | UserWidget | W_TimePass |
| [x] | W_LevelUp | UserWidget | W_LevelUp |
| [x] | W_LevelEntry | UserWidget | W_LevelEntry |
| [x] | W_LevelUpCost | UserWidget | W_LevelUpCost |
| [x] | W_LevelCurrencyBlock_LevelUp | UserWidget | W_LevelCurrencyBlock_LevelUp |
| [x] | W_CurrencyEntry | UserWidget | W_CurrencyEntry |
| [x] | W_StatBlock_LevelUp | UserWidget | W_StatBlock_LevelUp |
| [x] | W_StatEntry_LevelUp | UserWidget | W_StatEntry_LevelUp |

### Settings Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Settings | UserWidget | W_Settings |
| [x] | W_Settings_CategoryEntry | UserWidget | W_Settings_CategoryEntry |
| [x] | W_Settings_CenteredText | UserWidget | W_Settings_CenteredText |
| [x] | W_Settings_ControlEntry | UserWidget | W_Settings_ControlEntry |
| [x] | W_Settings_ControlsDisplay | UserWidget | W_Settings_ControlsDisplay |
| [x] | W_Settings_Entry | UserWidget | W_Settings_Entry |
| [x] | W_Settings_InputKeySelector | UserWidget | W_Settings_InputKeySelector |
| [x] | W_Settings_KeyMapping | UserWidget | W_Settings_KeyMapping |
| [x] | W_Settings_KeyMapping_Category | UserWidget | W_Settings_KeyMapping_Category |
| [x] | W_Settings_KeyMapping_Entry | UserWidget | W_Settings_KeyMapping_Entry |
| [x] | W_Settings_PlayerCard | UserWidget | W_Settings_PlayerCard |
| [x] | W_Settings_QuitConfirmation | UserWidget | W_Settings_QuitConfirmation |

### Game Menu Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_GameMenu | UserWidget | W_GameMenu |
| [x] | W_GameMenu_Button | UserWidget | W_GameMenu_Button |

### Crafting Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Crafting | UserWidget | W_Crafting |
| [x] | W_CraftingAction | UserWidget | W_CraftingAction |
| [x] | W_CraftingEntry | UserWidget | W_CraftingEntry |
| [x] | W_CraftingEntrySimple | UserWidget | W_CraftingEntrySimple |

### Stats Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_StatBlock | UserWidget | W_StatBlock |
| [x] | W_StatEntry | UserWidget | W_StatEntry |
| [x] | W_StatEntry_StatName | UserWidget | W_StatEntry_StatName |
| [x] | W_StatEntry_Status | UserWidget | W_StatEntry_Status |
| [x] | W_CategoryEntry | UserWidget | W_CategoryEntry |

### Status Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Status | UserWidget | W_Status |
| [x] | W_Status_LevelCurrencyBlock | UserWidget | W_Status_LevelCurrencyBlock |
| [x] | W_Status_StatBlock | UserWidget | W_Status_StatBlock |

### Radar Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_Radar | UserWidget | W_Radar |
| [x] | W_Radar_Cardinal | UserWidget | W_Radar_Cardinal |
| [x] | W_Radar_TrackedElement | UserWidget | W_Radar_TrackedElement |

### NPC/Vendor Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_NPC_Window | UserWidget | W_NPC_Window |
| [x] | W_NPC_Window_Vendor | UserWidget | W_NPC_Window_Vendor |
| [x] | W_VendorAction | UserWidget | W_VendorAction |
| [x] | W_VendorSlot | UserWidget | W_VendorSlot |

### World Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_EnemyHealthbar | UserWidget | W_EnemyHealthbar |
| [x] | W_TargetExecutionIndicator | UserWidget | W_TargetExecutionIndicator |
| [x] | W_TargetLock | UserWidget | W_TargetLock |

### Utility Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_BrowserFilterEntry | UserWidget | W_BrowserFilterEntry |
| [x] | W_Browser_Action | UserWidget | W_Browser_Action |
| [x] | W_Browser_Action_Tooltip | UserWidget | W_Browser_Action_Tooltip |
| [x] | W_Browser_Animset | UserWidget | W_Browser_Animset |
| [x] | W_Browser_AnimsetTooltip | UserWidget | W_Browser_AnimsetTooltip |
| [x] | W_Browser_Animset_OwnerEntry | UserWidget | W_Browser_Animset_OwnerEntry |
| [x] | W_Browser_Categories | UserWidget | W_Browser_Categories |
| [x] | W_Browser_CategoryEntry | UserWidget | W_Browser_CategoryEntry |
| [x] | W_Browser_Item | UserWidget | W_Browser_Item |
| [x] | W_Browser_StatusEffect | UserWidget | W_Browser_StatusEffect |
| [x] | W_Browser_StatusEffect_Tooltip | UserWidget | W_Browser_StatusEffect_Tooltip |
| [x] | W_Browser_Tooltip | UserWidget | W_Browser_Tooltip |
| [x] | W_Browser_WeaponAbility | UserWidget | W_Browser_WeaponAbility |
| [x] | W_Browser_WeaponAbility_Tooltip | UserWidget | W_Browser_WeaponAbility_Tooltip |
| [x] | W_Error | UserWidget | W_Error |
| [x] | W_Utility_Asset | UserWidget | W_Utility_Asset |
| [x] | W_Utility_AssetTooltip | UserWidget | W_Utility_AssetTooltip |
| [x] | W_Utility_Component | UserWidget | W_Utility_Component |

### Debug Widgets
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | W_DebugWindow | UserWidget | W_DebugWindow |
| [x] | W_Debug_ComponentSlot | UserWidget | W_Debug_ComponentSlot |
| [x] | W_Debug_ComponentTooltip | UserWidget | W_Debug_ComponentTooltip |
| [x] | W_Debug_HUD | UserWidget | W_Debug_HUD |

---

## 11. MISC BLUEPRINTS

### Blueprint Function Libraries
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | BFL_Helper | BlueprintFunctionLibrary | BFL_Helper |
| [N/A] | BML_HelperMacros | BlueprintMacroLibrary | (Asset - BFL_Helper provides C++ equivalents) |
| [N/A] | BML_StructConversion | BlueprintMacroLibrary | (Asset - BFL_Helper provides C++ equivalents) |

### Camera Shakes (6)
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [N/A] | CS_Boss_Roar | LegacyCameraShake | (Asset - no code logic) |
| [N/A] | CS_Guard | LegacyCameraShake | (Asset - no code logic) |
| [N/A] | CS_JumpHit | LegacyCameraShake | (Asset - no code logic) |
| [N/A] | CS_Seq_Handheld | LegacyCameraShake | (Asset - no code logic) |
| [N/A] | CS_StrongHit | LegacyCameraShake | (Asset - no code logic) |
| [N/A] | CS_SubtleHit | LegacyCameraShake | (Asset - no code logic) |

### Environment
| Status | Blueprint | Original Parent | C++ Parent |
|--------|-----------|-----------------|------------|
| [x] | B_SkyManager | Actor | SLFSkyManager |

---

## MIGRATION SUMMARY

| Category | Total | Migrated | Partial | N/A (Assets) | Excluded |
|----------|-------|----------|---------|--------------|----------|
| Game Framework | 8 | 7 | 0 | 1 | 0 |
| Components | 21 | 17 | 4 | 0 | 0 |
| Characters | 10 | 9 | 1 | 0 | 0 |
| AnimBPs | 4 | 4 | 0 | 0 | 0 |
| AnimNotifies | 25 | 25 | 0 | 0 | 0 |
| World Actors | 26 | 23 | 4 | 0 | 0 |
| AI System | 28 | 18 | 0 | 10 | 0 |
| Data Assets (Classes) | 60+ | 60+ | 0 | 0 | 0 |
| Data Asset Instances | 47+ | 47+ | 0 | 0 | 0 |
| Interfaces | 18 | 18 | 0 | 0 | 0 |
| Widgets | 122 | 118 | 4 | 0 | 0 |
| Misc (Macros/Shakes) | 9 | 1 | 0 | 8 | 0 |
| **TOTAL** | **~380** | **~347** | **~13** | **~20** | **0** |

---

## NOTES

### Why Some Blueprints Are Not Migrated

1. **GM_SoulslikeFramework**: Keep Blueprint logic intentionally (game mode setup)
2. **Behavior Trees**: No C++ equivalent needed (data-driven)
3. **Data Asset Instances (DA_*)**: These are data, not logic - no migration needed
4. **Interfaces**: Blueprint interfaces work fine alongside C++ interfaces
5. **Camera Shakes**: Simple data, no logic to migrate
6. **Macro Libraries**: Blueprint-only construct

### Why Some Are Partial (~)

1. **KEEP_VARS_MAP items**: Variables must be preserved for AnimBP PropertyAccess
2. **REPARENT_ONLY_MAP items**: EventGraph has custom events called by external Blueprints

### Previously Excluded Items (Now Migrated)

1. **ABP_SoulslikeBossNew**: EventGraph cleared - C++ SLFBossAnimInstance has matching UPROPERTY variables
2. **B_Door_Demo**: Reparented to SLFDoorDemo - inherits from B_Door which is in KEEP_VARS_MAP
