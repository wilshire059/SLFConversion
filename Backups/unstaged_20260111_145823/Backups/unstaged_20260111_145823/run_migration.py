# run_migration.py
# Simplified migration with better error handling
import unreal

# Full migration map with all Blueprints
MIGRATION_MAP = {
    # Components (17)
    "AC_StatManager": "/Script/SLFConversion.StatManagerComponent",
    "AC_InputBuffer": "/Script/SLFConversion.InputBufferComponent",
    "AC_BuffManager": "/Script/SLFConversion.BuffManagerComponent",
    "AC_StatusEffectManager": "/Script/SLFConversion.StatusEffectManagerComponent",
    "AC_InventoryManager": "/Script/SLFConversion.InventoryManagerComponent",
    "AC_CollisionManager": "/Script/SLFConversion.CollisionManagerComponent",
    "AC_RadarManager": "/Script/SLFConversion.RadarManagerComponent",
    "AC_RadarElement": "/Script/SLFConversion.RadarElementComponent",
    "AC_DebugCentral": "/Script/SLFConversion.DebugCentralComponent",
    "AC_ProgressManager": "/Script/SLFConversion.ProgressManagerComponent",
    "AC_SaveLoadManager": "/Script/SLFConversion.SaveLoadManagerComponent",
    "AC_LadderManager": "/Script/SLFConversion.LadderManagerComponent",
    "AC_LootDropManager": "/Script/SLFConversion.LootDropManagerComponent",
    "AC_AI_Boss": "/Script/SLFConversion.AIBossComponent",
    "AC_AI_BehaviorManager": "/Script/SLFConversion.AIBehaviorManagerComponent",
    "AC_AI_InteractionManager": "/Script/SLFConversion.AIInteractionManagerComponent",
    "AC_AI_CombatManager": "/Script/SLFConversion.AC_AI_CombatManager",
    "AC_AI_SenseManager": "/Script/SLFConversion.AC_AI_SenseManager",
    "AC_AI_TargetSortManager": "/Script/SLFConversion.AC_AI_TargetSortManager",
    # REMOVED from MIGRATION_MAP - These are in KEEP_VARS_MAP (preserve vars for AnimBP)
    "AC_ActionManager": "/Script/SLFConversion.AC_ActionManager",  # C++ implementation complete - migrate fully
    "AC_CombatManager": "/Script/SLFConversion.AC_CombatManager",  # C++ implementation complete - migrate fully
    "AC_EquipmentManager": "/Script/SLFConversion.AC_EquipmentManager",  # C++ implementation complete - migrate fully
    "AC_InteractionManager": "/Script/SLFConversion.AC_InteractionManager",  # C++ implementation complete - migrate fully
    # "AC_TargetManager" - doesn't exist in content
    # Game Framework (5)
    # "GM_SoulslikeFramework": "/Script/SLFConversion.GM_SoulslikeFramework",  # Keep Blueprint logic
    "GM_Menu_SoulslikeFramework": "/Script/SLFConversion.GM_Menu_SoulslikeFramework",
    "GI_SoulslikeFramework": "/Script/SLFConversion.SLFGameInstance",  # Clear EventGraph - GetAllSlots signature changed
    "PC_SoulslikeFramework": "/Script/SLFConversion.SLFPlayerController",  # Clear EventGraph - C++ handles HUD/input
    "PC_Menu_SoulslikeFramework": "/Script/SLFConversion.PC_Menu_SoulslikeFramework",
    "GS_SoulslikeFramework": "/Script/SLFConversion.GS_SoulslikeFramework",
    "PS_SoulslikeFramework": "/Script/SLFConversion.PS_SoulslikeFramework",
    # AI Controller
    "AIC_SoulslikeFramework": "/Script/SLFConversion.SLFAIController",
    # Characters (10) - SOME SKIPPED due to AnimBP dependencies
    # # "B_BaseCharacter": "/Script/SLFConversion.SLFBaseCharacter",  # Keep EventGraph  # In KEEP_VARS_MAP
    # # "B_Soulslike_Character": "/Script/SLFConversion.SLFSoulslikeCharacter",  # Keep EventGraph  # In KEEP_VARS_MAP
    "B_Soulslike_Enemy": "/Script/SLFConversion.SLFSoulslikeEnemy",
    "B_Soulslike_Boss": "/Script/SLFConversion.SLFSoulslikeBoss",
    "B_Soulslike_Boss_Malgareth": "/Script/SLFConversion.SLFBossMalgareth",
    # NPC and Enemy subclasses
    "B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "B_Soulslike_Enemy_Guard": "/Script/SLFConversion.SLFEnemyGuard",
    "B_Soulslike_Enemy_Showcase": "/Script/SLFConversion.SLFEnemyShowcase",
    "B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",
    # Base Classes (12)
    "B_StatusEffect": "/Script/SLFConversion.SLFStatusEffectBase",
    "B_Stat": "/Script/SLFConversion.SLFStatBase",
    "B_Buff": "/Script/SLFConversion.SLFBuffBase",
    # Primary Data Assets - DO NOT REPARENT PDA_Action!
    # Reparenting loses Blueprint variable values in child data assets (DA_Action_*)
    "PDA_Action": "/Script/SLFConversion.PDA_ActionBase",  # Now safe to reparent
    # EXPERIMENT: Reparent PDA_Item to add C++ WorldNiagaraSystem property
    "PDA_Item": "/Script/SLFConversion.PDA_Item",
    # PDA_AI_Ability - migrated with Montage, Score, Cooldown, AiRules, SetupData()
    "PDA_AI_Ability": "/Script/SLFConversion.PDA_AI_Ability",
    # PDA_Buff - migrated with BuffTag, BuffIcon, Class, Duration, RankMultiplierCurve
    "PDA_Buff": "/Script/SLFConversion.PDA_Buff",
    # PDA_Calculations - migrated with CalculationType, Calculate()
    "PDA_Calculations": "/Script/SLFConversion.PDA_Calculations",
    # PDA_Dialog - migrated with Requirement, DefaultDialogTable, GetDialogTableBasedOnProgress()
    "PDA_Dialog": "/Script/SLFConversion.PDA_Dialog",
    # PDA_WeaponAbility - migrated with Icon, Name, AffectedStat, Cost, Montage, AdditionalEffectClass, SetupData()
    "PDA_WeaponAbility": "/Script/SLFConversion.PDA_WeaponAbility",
    # PDA_Vendor - migrated with Items, DefaultSlotCount, DefaultSlotsPerRow, CachedItems, OnItemStockUpdated, ReduceItemStock()
    "PDA_Vendor": "/Script/SLFConversion.PDA_Vendor",
    # PDA_MainMenuData - migrated with LoadingScreenAsset, AdditionalWaitMin/Max, bRemoveOnFinish, DefaultLevelToLoad
    "PDA_MainMenuData": "/Script/SLFConversion.PDA_MainMenuData",
    "PDA_StatusEffect": "/Script/SLFConversion.PDA_StatusEffect",
    "PDA_Credits": "/Script/SLFConversion.PDA_Credits",
    "PDA_LoadingScreens": "/Script/SLFConversion.PDA_LoadingScreens",
    "PDA_DayNight": "/Script/SLFConversion.PDA_DayNight",
    "PDA_AnimData": "/Script/SLFConversion.PDA_AnimData",
    "PDA_CombatReactionAnimData": "/Script/SLFConversion.PDA_CombatReactionAnimData",
    "PDA_CombatReactionAnimData_Player": "/Script/SLFConversion.PDA_CombatReactionAnimData_Player",
    "PDA_LadderAnimData": "/Script/SLFConversion.PDA_LadderAnimData",
    "PDA_PoiseBreakAnimData": "/Script/SLFConversion.PDA_PoiseBreakAnimData",
    "PDA_WeaponAnimset": "/Script/SLFConversion.PDA_WeaponAnimset",
    "PDA_BaseCharacterInfo": "/Script/SLFConversion.PDA_BaseCharacterInfo",
    "PDA_CustomSettings": "/Script/SLFConversion.PDA_CustomSettings",
    "PDA_MovementSpeedData": "/Script/SLFConversion.PDA_MovementSpeedData",
    "PDA_LadderAnimData": "/Script/SLFConversion.PDA_LadderAnimData",
    "PDA_ExecutionAnimData": "/Script/SLFConversion.PDA_ExecutionAnimData",
    "B_Action": "/Script/SLFConversion.SLFActionBase",
    "B_Item": "/Script/SLFConversion.SLFItemBase",
    "B_Item_Weapon": "/Script/SLFConversion.SLFWeaponBase",
    # B_Interactable and B_Door: Now in KEEP_VARS_MAP to preserve SCS (DefaultSceneRoot needed by child Blueprints)
    "B_Container": "/Script/SLFConversion.SLFContainer",
    "B_BaseProjectile": "/Script/SLFConversion.SLFProjectileBase",
    "B_SkyManager": "/Script/SLFConversion.SLFSkyManager",
    "B_AbilityEffectBase": "/Script/SLFConversion.SLFAbilityEffectBase",
    # SaveGame classes
    "SG_SoulslikeFramework": "/Script/SLFConversion.SG_SoulslikeFramework",
    "SG_SaveSlots": "/Script/SLFConversion.SG_SaveSlots",
    # Stats (19)
    "B_HP": "/Script/SLFConversion.SLFStatHP",
    "B_FP": "/Script/SLFConversion.SLFStatFP",
    "B_Stamina": "/Script/SLFConversion.SLFStatStamina",
    "B_Poise": "/Script/SLFConversion.SLFStatPoise",
    "B_Vigor": "/Script/SLFConversion.SLFStatVigor",
    "B_Mind": "/Script/SLFConversion.SLFStatMind",
    "B_Endurance": "/Script/SLFConversion.SLFStatEndurance",
    "B_Strength": "/Script/SLFConversion.SLFStatStrength",
    "B_Dexterity": "/Script/SLFConversion.SLFStatDexterity",
    "B_Intelligence": "/Script/SLFConversion.SLFStatIntelligence",
    "B_Faith": "/Script/SLFConversion.SLFStatFaith",
    "B_Arcane": "/Script/SLFConversion.SLFStatArcane",
    "B_Weight": "/Script/SLFConversion.SLFStatWeight",
    "B_Stance": "/Script/SLFConversion.SLFStatStance",
    "B_IncantationPower": "/Script/SLFConversion.SLFStatIncantationPower",
    "B_Resistance_Focus": "/Script/SLFConversion.SLFResistanceFocus",
    "B_Resistance_Immunity": "/Script/SLFConversion.SLFResistanceImmunity",
    "B_Resistance_Robustness": "/Script/SLFConversion.SLFResistanceRobustness",
    "B_Resistance_Vitality": "/Script/SLFConversion.SLFResistanceVitality",
    # Attack Power (6) - Use full class names
    "B_AP_Physical": "/Script/SLFConversion.SLFAttackPowerPhysical",
    "B_AP_Fire": "/Script/SLFConversion.SLFAttackPowerFire",
    "B_AP_Frost": "/Script/SLFConversion.SLFAttackPowerFrost",
    "B_AP_Lightning": "/Script/SLFConversion.SLFAttackPowerLightning",
    "B_AP_Magic": "/Script/SLFConversion.SLFAttackPowerMagic",
    "B_AP_Holy": "/Script/SLFConversion.SLFAttackPowerHoly",
    # Damage Negation (6) - Use full class names
    "B_DN_Physical": "/Script/SLFConversion.SLFDamageNegationPhysical",
    "B_DN_Fire": "/Script/SLFConversion.SLFDamageNegationFire",
    "B_DN_Frost": "/Script/SLFConversion.SLFDamageNegationFrost",
    "B_DN_Lightning": "/Script/SLFConversion.SLFDamageNegationLightning",
    "B_DN_Magic": "/Script/SLFConversion.SLFDamageNegationMagic",
    "B_DN_Holy": "/Script/SLFConversion.SLFDamageNegationHoly",
    # Additional Actions (actual Blueprints that exist)
    "B_Action_Execute": "/Script/SLFConversion.SLFActionExecute",
    "B_Action_GuardStart": "/Script/SLFConversion.SLFActionGuardStart",
    "B_Action_GuardEnd": "/Script/SLFConversion.SLFActionGuardEnd",
    "B_Action_GuardCancel": "/Script/SLFConversion.SLFActionGuardCancel",
    "B_Action_StartSprinting": "/Script/SLFConversion.SLFActionStartSprinting",
    "B_Action_StopSprinting": "/Script/SLFConversion.SLFActionStopSprinting",
    "B_Action_SprintAttack": "/Script/SLFConversion.SLFActionSprintAttack",
    "B_Action_ThrowProjectile": "/Script/SLFConversion.SLFActionThrowProjectile",
    "B_Action_TwoHandedStance_L": "/Script/SLFConversion.SLFActionTwoHandedStanceL",
    "B_Action_TwoHandedStance_R": "/Script/SLFConversion.SLFActionTwoHandedStanceR",
    "B_Action_UseEquippedTool": "/Script/SLFConversion.SLFActionUseEquippedTool",
    "B_Action_WeaponAbility": "/Script/SLFConversion.SLFActionWeaponAbility",
    "B_Action_PickupItemMontage": "/Script/SLFConversion.SLFActionPickupItemMontage",
    "B_Action_ScrollWheel_LeftHand": "/Script/SLFConversion.SLFActionScrollWheelLeftHand",
    "B_Action_ScrollWheel_RightHand": "/Script/SLFConversion.SLFActionScrollWheelRightHand",
    "B_Action_ScrollWheel_Tools": "/Script/SLFConversion.SLFActionScrollWheelTools",
    # Additional Interactables
    "B_BossDoor": "/Script/SLFConversion.SLFInteractableBase",
    "B_DeathCurrency": "/Script/SLFConversion.SLFInteractableBase",
    "B_Door_Demo": "/Script/SLFConversion.SLFDoorDemo",  # Inherits B_Door (KEEP_VARS) - reparent may fail but logic is cleared
    # Additional Items
    "B_Item_Lantern": "/Script/SLFConversion.SLFItemBase",
    "B_Item_AI_Weapon": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_AI_Weapon_BossMace": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_AI_Weapon_Greatsword": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_AI_Weapon_Sword": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_BossMace": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_Greatsword": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_Katana": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_PoisonSword": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_SwordExample01": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_SwordExample02": "/Script/SLFConversion.SLFWeaponBase",
    # Additional World Actors
    "B_Chaos_ForceField": "/Script/SLFConversion.SLFInteractableBase",
    "B_SequenceActor": "/Script/SLFConversion.SLFSequenceActor",
    # Blueprint Function Library
    "BFL_Helper": "/Script/SLFConversion.BFL_Helper",
    "B_DeathTrigger": "/Script/SLFConversion.SLFInteractableBase",
    "B_Destructible": "/Script/SLFConversion.SLFInteractableBase",
    "B_LocationActor": "/Script/SLFConversion.SLFInteractableBase",
    "B_Torch": "/Script/SLFConversion.SLFInteractableBase",
    "B_Projectile_Boss_Fireball": "/Script/SLFConversion.SLFProjectileBase",
    "B_Projectile_ThrowingKnife": "/Script/SLFConversion.SLFProjectileBase",
    # AI Tasks (actual ones that exist)
    # BT Tasks - Use actual class names (no U prefix in script references)
    "BTT_ClearKey": "/Script/SLFConversion.BTT_ClearKey",
    "BTT_GetCurrentLocation": "/Script/SLFConversion.BTT_GetCurrentLocation",
    "BTT_GetRandomPoint": "/Script/SLFConversion.BTT_GetRandomPoint",
    "BTT_GetRandomPointNearStartPosition": "/Script/SLFConversion.BTT_GetRandomPointNearStartPosition",
    "BTT_GetStrafePointAroundTarget": "/Script/SLFConversion.BTT_GetStrafePointAroundTarget",
    "BTT_PatrolPath": "/Script/SLFConversion.BTT_PatrolPath",
    "BTT_SetKey": "/Script/SLFConversion.BTT_SetKey",
    "BTT_SetMovementMode": "/Script/SLFConversion.BTT_SetMovementMode",
    "BTT_SimpleMoveTo": "/Script/SLFConversion.BTT_SimpleMoveTo",
    "BTT_SwitchState": "/Script/SLFConversion.BTT_SwitchState",
    "BTT_SwitchToPreviousState": "/Script/SLFConversion.BTT_SwitchToPreviousState",
    "BTT_ToggleFocus": "/Script/SLFConversion.BTT_ToggleFocus",
    "BTT_TryExecuteAbility": "/Script/SLFConversion.BTT_TryExecuteAbility",
    # AI Services - Use actual class names (no U prefix in script references)
    "BTS_ChaseBounds": "/Script/SLFConversion.BTS_ChaseBounds",
    "BTS_DistanceCheck": "/Script/SLFConversion.BTS_DistanceCheck",
    "BTS_IsTargetDead": "/Script/SLFConversion.BTS_IsTargetDead",
    "BTS_SetMovementModeBasedOnDistance": "/Script/SLFConversion.BTS_SetMovementModeBasedOnDistance",
    "BTS_TryGetAbility": "/Script/SLFConversion.BTS_TryGetAbility",
    # Animation Notifies - Use actual class names
    "AN_AdjustStat": "/Script/SLFConversion.SLFAnimNotifyAdjustStat",
    "AN_AI_SpawnProjectile": "/Script/SLFConversion.SLFAnimNotifySpawnProjectile",
    "AN_AoeDamage": "/Script/SLFConversion.SLFAnimNotifyAoeDamage",
    "AN_InterruptMontage": "/Script/SLFConversion.SLFAnimNotifyInterruptMontage",
    "AN_LaunchField": "/Script/SLFConversion.SLFAnimNotifySpawnProjectile",  # Reuses SpawnProjectile
    "AN_PlayCameraSequence": "/Script/SLFConversion.SLFAnimNotifyCameraShake",  # Reuses CameraShake
    "AN_SetAiState": "/Script/SLFConversion.SLFAnimNotifySetAIState",
    "AN_SetMovementMode": "/Script/SLFConversion.SLFAnimNotifySetMovementMode",
    "AN_TryGuard": "/Script/SLFConversion.SLFAnimNotifyTryGuard",
    "AN_WorldCameraShake": "/Script/SLFConversion.SLFAnimNotifyCameraShake",
    # Animation Notify States - Use actual class names
    "ANS_AI_FistTrace": "/Script/SLFConversion.SLFAnimNotifyStateFistTrace",
    "ANS_AI_RotateTowardsTarget": "/Script/SLFConversion.SLFAnimNotifyStateAIRotateToTarget",
    "ANS_AI_Trail": "/Script/SLFConversion.SLFAnimNotifyStateTrail",
    "ANS_AI_WeaponTrace": "/Script/SLFConversion.SLFAnimNotifyStateWeaponTrace",
    "ANS_FistTrace": "/Script/SLFConversion.SLFAnimNotifyStateFistTrace",
    "ANS_RegisterAttackSequence": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    # Misc
    "B_PatrolPath": "/Script/SLFConversion.SLFInteractableBase",
    "B_Discovery": "/Script/SLFConversion.SLFStatBase",
    # Status Effects (11)
    "B_StatusEffect_Bleed": "/Script/SLFConversion.SLFStatusEffectBleed",
    "B_StatusEffect_Burn": "/Script/SLFConversion.SLFStatusEffectBurn",
    "B_StatusEffect_Corruption": "/Script/SLFConversion.SLFStatusEffectCorruption",
    "B_StatusEffect_Frostbite": "/Script/SLFConversion.SLFStatusEffectFrostbite",
    "B_StatusEffect_Madness": "/Script/SLFConversion.SLFStatusEffectMadness",
    "B_StatusEffect_Plague": "/Script/SLFConversion.SLFStatusEffectPlague",
    "B_StatusEffect_Poison": "/Script/SLFConversion.SLFStatusEffectPoison",
    "B_StatusEffectArea": "/Script/SLFConversion.SLFStatusEffectArea",
    "B_StatusEffectBuildup": "/Script/SLFConversion.SLFStatusEffectBuildup",
    "B_StatusEffectOneShot": "/Script/SLFConversion.SLFStatusEffectOneShot",
    # Buffs
    "B_Buff_AttackPower": "/Script/SLFConversion.SLFBuffAttackPower",
    # Actions (29)
    "B_Action_Backstab": "/Script/SLFConversion.SLFActionBackstab",
    "B_Action_ComboHeavy": "/Script/SLFConversion.SLFActionComboHeavy",
    "B_Action_ComboLight_L": "/Script/SLFConversion.SLFActionComboLightL",
    "B_Action_ComboLight_R": "/Script/SLFConversion.SLFActionComboLightR",
    "B_Action_Crouch": "/Script/SLFConversion.SLFActionCrouch",
    "B_Action_Dodge": "/Script/SLFConversion.SLFActionDodge",
    "B_Action_DrinkFlask_HP": "/Script/SLFConversion.SLFActionDrinkFlaskHP",
    "B_Action_DualWieldAttack": "/Script/SLFConversion.SLFActionDualWieldAttack",
    "B_Action_Guard": "/Script/SLFConversion.SLFActionGuard",
    "B_Action_HeavyAttack": "/Script/SLFConversion.SLFActionHeavyAttack",
    "B_Action_Incantation": "/Script/SLFConversion.SLFActionIncantation",
    "B_Action_Jump": "/Script/SLFConversion.SLFActionJump",
    "B_Action_LightAttack": "/Script/SLFConversion.SLFActionLightAttack",
    "B_Action_Open": "/Script/SLFConversion.SLFActionOpen",
    "B_Action_Riposte": "/Script/SLFConversion.SLFActionRiposte",
    "B_Action_Run": "/Script/SLFConversion.SLFActionRun",
    "B_Action_SitDown": "/Script/SLFConversion.SLFActionSitDown",
    "B_Action_Sorcery": "/Script/SLFConversion.SLFActionSorcery",
    "B_Action_SorceryPose": "/Script/SLFConversion.SLFActionSorceryPose",
    "B_Action_StandUp": "/Script/SLFConversion.SLFActionStandUp",
    "B_Action_Talk": "/Script/SLFConversion.SLFActionTalk",
    "B_Action_TwoHandLeft": "/Script/SLFConversion.SLFActionTwoHandLeft",
    "B_Action_TwoHandRight": "/Script/SLFConversion.SLFActionTwoHandRight",
    "B_Action_UseItem": "/Script/SLFConversion.SLFActionUseItem",
    "B_Action_Walk": "/Script/SLFConversion.SLFActionWalk",
    "B_Action_RunningHeavyAttack": "/Script/SLFConversion.SLFActionRunningHeavyAttack",
    "B_Action_ChargingHeavyAttack": "/Script/SLFConversion.SLFActionChargingHeavyAttack",
    "B_Action_JumpAttack": "/Script/SLFConversion.SLFActionJumpAttack",
    "B_Action_Roll": "/Script/SLFConversion.SLFActionRoll",
    # Items (21)
    "B_Item_Armor": "/Script/SLFConversion.SLFItemArmor",
    "B_Item_Armor_Chest": "/Script/SLFConversion.SLFItemArmorChest",
    "B_Item_Armor_Gloves": "/Script/SLFConversion.SLFItemArmorGloves",
    "B_Item_Armor_Helmet": "/Script/SLFConversion.SLFItemArmorHelmet",
    "B_Item_Armor_Legs": "/Script/SLFConversion.SLFItemArmorLegs",
    "B_Item_Consumable": "/Script/SLFConversion.SLFItemConsumable",
    "B_Item_KeyItem": "/Script/SLFConversion.SLFItemKeyItem",
    "B_Item_Material": "/Script/SLFConversion.SLFItemMaterial",
    "B_Item_Spell": "/Script/SLFConversion.SLFItemSpell",
    "B_Item_Talisman": "/Script/SLFConversion.SLFItemTalisman",
    "B_Item_Weapon": "/Script/SLFConversion.SLFItemWeapon",
    "B_Item_Weapon_Axe": "/Script/SLFConversion.SLFItemWeaponAxe",
    "B_Item_Weapon_Bow": "/Script/SLFConversion.SLFItemWeaponBow",
    "B_Item_Weapon_Catalyst": "/Script/SLFConversion.SLFItemWeaponCatalyst",
    "B_Item_Weapon_Fist": "/Script/SLFConversion.SLFItemWeaponFist",
    "B_Item_Weapon_Halberd": "/Script/SLFConversion.SLFItemWeaponHalberd",
    "B_Item_Weapon_Hammer": "/Script/SLFConversion.SLFItemWeaponHammer",
    "B_Item_Weapon_Shield": "/Script/SLFConversion.SLFWeaponBase",
    "B_Item_Weapon_Spear": "/Script/SLFConversion.SLFItemWeaponSpear",
    "B_Item_Weapon_Staff": "/Script/SLFConversion.SLFItemWeaponStaff",
    "B_Item_Weapon_Sword": "/Script/SLFConversion.SLFItemWeaponSword",
    # Interactables (5)
    "B_LootDrop": "/Script/SLFConversion.SLFLootDrop",
    "B_TalkButton": "/Script/SLFConversion.SLFTalkButton",
    "B_Switch": "/Script/SLFConversion.SLFSwitch",
    "B_HiddenWall": "/Script/SLFConversion.SLFHiddenWall",
    "B_DisappearingPlatform": "/Script/SLFConversion.SLFDisappearingPlatform",
    # Projectiles (3)
    "B_Projectile_Arrow": "/Script/SLFConversion.SLFProjectileArrow",
    "B_Projectile_Magic": "/Script/SLFConversion.SLFProjectileMagic",
    "B_Projectile_Homing": "/Script/SLFConversion.SLFProjectileHoming",
    # Ability Effects (5)
    "B_AbilityEffect_Heal": "/Script/SLFConversion.SLFAbilityEffectHeal",
    "B_AbilityEffect_Buff": "/Script/SLFConversion.SLFAbilityEffectBuff",
    "B_AbilityEffect_Damage": "/Script/SLFConversion.SLFAbilityEffectDamage",
    "B_AbilityEffect_Projectile": "/Script/SLFConversion.SLFAbilityEffectProjectile",
    "B_AbilityEffect_Summon": "/Script/SLFConversion.SLFAbilityEffectSummon",
    # AI Tasks - TODO: These classes don't exist yet, commenting out
    # "BTT_Idle": "/Script/SLFConversion.SLFBTTIdle",
    # "BTT_MoveToLocation": "/Script/SLFConversion.SLFBTTMoveToLocation",
    # "BTT_MoveToTarget": "/Script/SLFConversion.SLFBTTMoveToTarget",
    # "BTT_Attack": "/Script/SLFConversion.SLFBTTAttack",
    # "BTT_CastAbility": "/Script/SLFConversion.SLFBTTCastAbility",
    # "BTT_Flee": "/Script/SLFConversion.SLFBTTFlee",
    # "BTT_Strafe": "/Script/SLFConversion.SLFBTTStrafe",
    # "BTT_Wait": "/Script/SLFConversion.SLFBTTWait",
    # "BTT_Patrol": "/Script/SLFConversion.SLFBTTPatrol",
    # "BTT_FindCover": "/Script/SLFConversion.SLFBTTFindCover",
    # "BTT_InvestigateLocation": "/Script/SLFConversion.SLFBTTInvestigateLocation",
    # "BTT_RotateToTarget": "/Script/SLFConversion.SLFBTTRotateToTarget",
    # "BTT_SetBehaviorState": "/Script/SLFConversion.SLFBTTSetBehaviorState",
    # "BTT_SelectAttack": "/Script/SLFConversion.SLFBTTSelectAttack",
    # "BTT_PerformAction": "/Script/SLFConversion.SLFBTTPerformAction",
    # "BTT_Summon": "/Script/SLFConversion.SLFBTTSummon",
    # "BTT_Taunt": "/Script/SLFConversion.SLFBTTTaunt",
    # "BTT_Guard": "/Script/SLFConversion.SLFBTTGuard",
    # AI Services - TODO: These classes don't exist yet, commenting out
    # "BTS_UpdateCombatState": "/Script/SLFConversion.SLFBTSUpdateCombatState",
    # "BTS_UpdateTarget": "/Script/SLFConversion.SLFBTSUpdateTarget",
    # "BTS_UpdateAggro": "/Script/SLFConversion.SLFBTSUpdateAggro",
    # "BTS_UpdateBehavior": "/Script/SLFConversion.SLFBTSUpdateBehavior",
    # "BTS_ManageCooldowns": "/Script/SLFConversion.SLFBTSManageCooldowns",
    # "BTS_CheckSenses": "/Script/SLFConversion.SLFBTSCheckSenses",
    # "BTS_UpdateDistance": "/Script/SLFConversion.SLFBTSUpdateDistance",
    # "BTS_EvaluateThreats": "/Script/SLFConversion.SLFBTSEvaluateThreats",
    # "BTS_UpdatePhase": "/Script/SLFConversion.SLFBTSUpdatePhase",
    # Animation Notifies - Use correct class names (SLFAnimNotify* not SLFAN_*)
    "AN_FootstepTrace": "/Script/SLFConversion.SLFAnimNotifyFootstepTrace",
    "AN_WeaponSwing": "/Script/SLFConversion.SLFAnimNotifyStateWeaponTrace",  # Reuse state version
    "AN_WeaponTrace": "/Script/SLFConversion.SLFAnimNotifyStateWeaponTrace",
    "AN_TriggerAbilityEffect": "/Script/SLFConversion.SLFAnimNotifySpawnProjectile",
    "AN_SpawnProjectile": "/Script/SLFConversion.SLFAnimNotifySpawnProjectile",
    "AN_PlaySound": "/Script/SLFConversion.SLFAnimNotifyCameraShake",  # Placeholder
    "AN_PlayParticle": "/Script/SLFConversion.SLFAnimNotifyCameraShake",  # Placeholder
    "AN_CameraShake": "/Script/SLFConversion.SLFAnimNotifyCameraShake",
    "AN_SetInvulnerable": "/Script/SLFConversion.SLFAnimNotifyStateInvincibility",
    "AN_ConsumeStamina": "/Script/SLFConversion.SLFAnimNotifyAdjustStat",
    "AN_ApplyBuff": "/Script/SLFConversion.SLFAnimNotifyAdjustStat",
    "AN_ResetCombo": "/Script/SLFConversion.SLFAnimNotifyAdjustStat",
    # Anim Notify States - Use correct class names (SLFAnimNotifyState* not SLFANS_*)
    "ANS_WeaponTrail": "/Script/SLFConversion.SLFAnimNotifyStateTrail",
    "ANS_SuperArmor": "/Script/SLFConversion.SLFAnimNotifyStateHyperArmor",
    "ANS_Invulnerable": "/Script/SLFConversion.SLFAnimNotifyStateInvincibility",
    "ANS_CanCombo": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_RotateToTarget": "/Script/SLFConversion.SLFAnimNotifyStateAIRotateToTarget",
    "ANS_ApplyRootMotion": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",  # Placeholder
    "ANS_CanDodgeCancel": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_TimedHitWindow": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_MeleeWindow": "/Script/SLFConversion.SLFAnimNotifyStateWeaponTrace",
    "ANS_BlockWindow": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_ParryWindow": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_ToggleChaosField": "/Script/SLFConversion.SLFAnimNotifyStateTrail",  # Placeholder
    "ANS_FootIKDisable": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    # Widgets (122)
    "W_AbilityDisplay": "/Script/SLFConversion.W_AbilityDisplay",
    # "W_BigScreenMessage": "/Script/SLFConversion.W_BigScreenMessage",  # REPARENT_ONLY: W_HUD calls custom events
    "W_Boss_Healthbar": "/Script/SLFConversion.W_Boss_Healthbar",  # Clear EventGraph - C++ has BlueprintNativeEvent functions
    "W_BrowserFilterEntry": "/Script/SLFConversion.W_BrowserFilterEntry",
    "W_Browser_Action": "/Script/SLFConversion.W_Browser_Action",
    "W_Browser_Action_Tooltip": "/Script/SLFConversion.W_Browser_Action_Tooltip",
    "W_Browser_Animset": "/Script/SLFConversion.W_Browser_Animset",
    "W_Browser_AnimsetTooltip": "/Script/SLFConversion.W_Browser_AnimsetTooltip",
    "W_Browser_Animset_OwnerEntry": "/Script/SLFConversion.W_Browser_Animset_OwnerEntry",
    "W_Browser_Categories": "/Script/SLFConversion.W_Browser_Categories",
    "W_Browser_CategoryEntry": "/Script/SLFConversion.W_Browser_CategoryEntry",
    "W_Browser_Item": "/Script/SLFConversion.W_Browser_Item",
    "W_Browser_StatusEffect": "/Script/SLFConversion.W_Browser_StatusEffect",
    "W_Browser_StatusEffect_Tooltip": "/Script/SLFConversion.W_Browser_StatusEffect_Tooltip",
    "W_Browser_Tooltip": "/Script/SLFConversion.W_Browser_Tooltip",
    "W_Browser_WeaponAbility": "/Script/SLFConversion.W_Browser_WeaponAbility",
    "W_Browser_WeaponAbility_Tooltip": "/Script/SLFConversion.W_Browser_WeaponAbility_Tooltip",
    "W_Buff": "/Script/SLFConversion.W_Buff",
    "W_CategoryEntry": "/Script/SLFConversion.W_CategoryEntry",
    "W_CharacterSelection": "/Script/SLFConversion.W_CharacterSelection",
    "W_CharacterSelectionCard": "/Script/SLFConversion.W_CharacterSelectionCard",
    "W_CharacterSelection_StatEntry": "/Script/SLFConversion.W_CharacterSelection_StatEntry",
    "W_Crafting": "/Script/SLFConversion.W_Crafting",
    "W_CraftingAction": "/Script/SLFConversion.W_CraftingAction",
    "W_CraftingEntry": "/Script/SLFConversion.W_CraftingEntry",
    "W_CraftingEntrySimple": "/Script/SLFConversion.W_CraftingEntrySimple",
    "W_CreditEntry": "/Script/SLFConversion.W_CreditEntry",
    "W_CreditEntry_Extra": "/Script/SLFConversion.W_CreditEntry_Extra",
    "W_CreditEntry_Sub": "/Script/SLFConversion.W_CreditEntry_Sub",
    "W_CreditEntry_Sub_NameEntry": "/Script/SLFConversion.W_CreditEntry_Sub_NameEntry",
    "W_Credits": "/Script/SLFConversion.W_Credits",
    "W_CurrencyContainer": "/Script/SLFConversion.W_CurrencyContainer",
    "W_CurrencyEntry": "/Script/SLFConversion.W_CurrencyEntry",
    "W_DebugWindow": "/Script/SLFConversion.W_DebugWindow",
    "W_Debug_ComponentSlot": "/Script/SLFConversion.W_Debug_ComponentSlot",
    "W_Debug_ComponentTooltip": "/Script/SLFConversion.W_Debug_ComponentTooltip",
    "W_Debug_HUD": "/Script/SLFConversion.W_Debug_HUD",
    # "W_Dialog": "/Script/SLFConversion.W_Dialog",  # REPARENT_ONLY: W_HUD calls custom events
    "W_EnemyHealthbar": "/Script/SLFConversion.W_EnemyHealthbar",
    "W_Equipment": "/Script/SLFConversion.W_Equipment",
    "W_EquipmentSlot": "/Script/SLFConversion.W_EquipmentSlot",
    "W_Equipment_Item_AttackPower": "/Script/SLFConversion.W_Equipment_Item_AttackPower",
    "W_Equipment_Item_DamageNegation": "/Script/SLFConversion.W_Equipment_Item_DamageNegation",
    "W_Equipment_Item_ItemEffect": "/Script/SLFConversion.W_Equipment_Item_ItemEffect",
    "W_Equipment_Item_OnUseEffect": "/Script/SLFConversion.W_Equipment_Item_OnUseEffect",
    "W_Equipment_Item_RequiredStats": "/Script/SLFConversion.W_Equipment_Item_RequiredStats",
    "W_Equipment_Item_Resistance": "/Script/SLFConversion.W_Equipment_Item_Resistance",
    "W_Equipment_Item_StatScaling": "/Script/SLFConversion.W_Equipment_Item_StatScaling",
    "W_Equipment_Item_StatsGranted": "/Script/SLFConversion.W_Equipment_Item_StatsGranted",
    "W_Error": "/Script/SLFConversion.W_Error",
    "W_FirstLootNotification": "/Script/SLFConversion.W_FirstLootNotification",
    "W_GameMenu": "/Script/SLFConversion.W_GameMenu",  # Full C++ implementation (moved from REPARENT_ONLY)
    "W_GameMenu_Button": "/Script/SLFConversion.W_GameMenu_Button",
    "W_GenericButton": "/Script/SLFConversion.W_GenericButton",
    "W_GenericError": "/Script/SLFConversion.W_GenericError",
    "W_HUD": "/Script/SLFConversion.W_HUD",  # Clear EventGraph - C++ has BlueprintNativeEvent functions
    # "W_Interaction": "/Script/SLFConversion.W_Interaction",  # REPARENT_ONLY: W_HUD calls custom events
    # "W_InteractionError": "/Script/SLFConversion.W_InteractionError",  # REPARENT_ONLY: W_HUD calls custom events
    "W_Inventory": "/Script/SLFConversion.W_Inventory",
    "W_InventoryAction": "/Script/SLFConversion.W_InventoryAction",
    "W_InventoryActionAmount": "/Script/SLFConversion.W_InventoryActionAmount",
    "W_InventorySlot": "/Script/SLFConversion.W_InventorySlot",
    "W_Inventory_ActionButton": "/Script/SLFConversion.W_Inventory_ActionButton",
    "W_Inventory_CategoryEntry": "/Script/SLFConversion.W_Inventory_CategoryEntry",
    "W_ItemInfoEntry": "/Script/SLFConversion.W_ItemInfoEntry",
    "W_ItemInfoEntry_RequiredStats": "/Script/SLFConversion.W_ItemInfoEntry_RequiredStats",
    "W_ItemInfoEntry_StatScaling": "/Script/SLFConversion.W_ItemInfoEntry_StatScaling",
    "W_ItemWheelSlot": "/Script/SLFConversion.W_ItemWheelSlot",
    "W_ItemWheel_NextSlot": "/Script/SLFConversion.W_ItemWheel_NextSlot",
    "W_LevelCurrencyBlock_LevelUp": "/Script/SLFConversion.W_LevelCurrencyBlock_LevelUp",
    "W_LevelEntry": "/Script/SLFConversion.W_LevelEntry",
    "W_LevelUp": "/Script/SLFConversion.W_LevelUp",
    "W_LevelUpCost": "/Script/SLFConversion.W_LevelUpCost",
    "W_LoadGame": "/Script/SLFConversion.W_LoadGame",
    "W_LoadGame_Entry": "/Script/SLFConversion.W_LoadGame_Entry",
    "W_LoadingScreen": "/Script/SLFConversion.W_LoadingScreen",
    "W_LootNotification": "/Script/SLFConversion.W_LootNotification",
    "W_MainMenu": "/Script/SLFConversion.W_MainMenu",
    "W_MainMenu_Button": "/Script/SLFConversion.W_MainMenu_Button",
    "W_Navigable": "/Script/SLFConversion.W_Navigable",
    "W_Navigable_InputReader": "/Script/SLFConversion.W_Navigable_InputReader",
    "W_NPC_Window": "/Script/SLFConversion.W_NPC_Window",  # Clear EventGraph - C++ has BlueprintNativeEvent functions
    "W_NPC_Window_Vendor": "/Script/SLFConversion.W_NPC_Window_Vendor",
    "W_Radar": "/Script/SLFConversion.W_Radar",
    "W_Radar_Cardinal": "/Script/SLFConversion.W_Radar_Cardinal",
    "W_Radar_TrackedElement": "/Script/SLFConversion.W_Radar_TrackedElement",
    "W_Resources": "/Script/SLFConversion.W_Resources",
    "W_RestMenu": "/Script/SLFConversion.W_RestMenu",  # Clear EventGraph - C++ has BlueprintNativeEvent functions
    "W_RestMenu_Button": "/Script/SLFConversion.W_RestMenu_Button",
    "W_RestMenu_TimeEntry": "/Script/SLFConversion.W_RestMenu_TimeEntry",
    "W_Settings": "/Script/SLFConversion.W_Settings",
    "W_Settings_CategoryEntry": "/Script/SLFConversion.W_Settings_CategoryEntry",
    "W_Settings_CenteredText": "/Script/SLFConversion.W_Settings_CenteredText",
    "W_Settings_ControlEntry": "/Script/SLFConversion.W_Settings_ControlEntry",
    "W_Settings_ControlsDisplay": "/Script/SLFConversion.W_Settings_ControlsDisplay",
    "W_Settings_Entry": "/Script/SLFConversion.W_Settings_Entry",
    "W_Settings_InputKeySelector": "/Script/SLFConversion.W_Settings_InputKeySelector",
    "W_Settings_KeyMapping": "/Script/SLFConversion.W_Settings_KeyMapping",
    "W_Settings_KeyMapping_Category": "/Script/SLFConversion.W_Settings_KeyMapping_Category",
    "W_Settings_KeyMapping_Entry": "/Script/SLFConversion.W_Settings_KeyMapping_Entry",
    "W_Settings_PlayerCard": "/Script/SLFConversion.W_Settings_PlayerCard",
    "W_Settings_QuitConfirmation": "/Script/SLFConversion.W_Settings_QuitConfirmation",
    "W_SkipCinematic": "/Script/SLFConversion.W_SkipCinematic",
    "W_StatBlock": "/Script/SLFConversion.W_StatBlock",
    "W_StatBlock_LevelUp": "/Script/SLFConversion.W_StatBlock_LevelUp",
    "W_StatEntry": "/Script/SLFConversion.W_StatEntry",
    "W_StatEntry_LevelUp": "/Script/SLFConversion.W_StatEntry_LevelUp",
    "W_StatEntry_StatName": "/Script/SLFConversion.W_StatEntry_StatName",
    "W_StatEntry_Status": "/Script/SLFConversion.W_StatEntry_Status",
    "W_Status": "/Script/SLFConversion.W_Status",
    "W_StatusEffectBar": "/Script/SLFConversion.W_StatusEffectBar",
    "W_StatusEffectNotification": "/Script/SLFConversion.W_StatusEffectNotification",
    "W_Status_LevelCurrencyBlock": "/Script/SLFConversion.W_Status_LevelCurrencyBlock",
    "W_Status_StatBlock": "/Script/SLFConversion.W_Status_StatBlock",
    "W_TargetExecutionIndicator": "/Script/SLFConversion.W_TargetExecutionIndicator",
    "W_TargetLock": "/Script/SLFConversion.W_TargetLock",
    "W_TimePass": "/Script/SLFConversion.W_TimePass",
    "W_Utility_Asset": "/Script/SLFConversion.W_Utility_Asset",
    "W_Utility_AssetTooltip": "/Script/SLFConversion.W_Utility_AssetTooltip",
    "W_Utility_Component": "/Script/SLFConversion.W_Utility_Component",
    "W_VendorAction": "/Script/SLFConversion.W_VendorAction",
    "W_VendorSlot": "/Script/SLFConversion.W_VendorSlot",
}

# Keep vars map - components referenced by AnimBPs (must match parent classes)
KEEP_VARS_MAP = {
    # "AC_ActionManager" moved to MIGRATION_MAP - C++ implementation complete
    # "AC_CombatManager" moved to MIGRATION_MAP - C++ implementation complete
    # "AC_EquipmentManager" moved to MIGRATION_MAP - C++ implementation complete
    # "AC_InteractionManager" moved to MIGRATION_MAP - C++ implementation complete
    # "AC_TargetManager" - doesn't exist in content
    # "AC_AI_CombatManager" already in MIGRATION_MAP
    # "AC_AI_SenseManager" already in MIGRATION_MAP
    # "AC_AI_TargetSortManager" already in MIGRATION_MAP
    # Character Blueprints - B_BaseCharacter is critical parent of all characters
    "B_BaseCharacter": "/Script/SLFConversion.SLFBaseCharacter",
    # Interactables - preserve SCS components (DefaultSceneRoot, World Niagara, mesh components)
    # B_Interactable's DefaultSceneRoot is needed as parent for child Blueprint SCS components
    "B_Interactable": "/Script/SLFConversion.SLFInteractableBase",
    "B_Door": "/Script/SLFConversion.SLFDoorBase",
    "B_Ladder": "/Script/SLFConversion.SLFLadderBase",
    "B_RestingPoint": "/Script/SLFConversion.SLFRestingPointBase",
    "B_PickupItem": "/Script/SLFConversion.SLFPickupItemBase",
}

# Interface map
INTERFACE_MAP = {
    "BPI_Character": "/Script/SLFConversion.SLFCharacterInterface",
    "BPI_Combat": "/Script/SLFConversion.SLFCombatInterface",
    "BPI_Interactable": "/Script/SLFConversion.SLFInteractableInterface",
    "BPI_Action": "/Script/SLFConversion.SLFActionInterface",
    "BPI_Buff": "/Script/SLFConversion.SLFBuffInterface",
    "BPI_Equipment": "/Script/SLFConversion.SLFEquipmentInterface",
    "BPI_Stat": "/Script/SLFConversion.SLFStatInterface",
    "BPI_Inventory": "/Script/SLFConversion.SLFInventoryInterface",
    "BPI_AI": "/Script/SLFConversion.SLFAIInterface",
    "BPI_Dialogue": "/Script/SLFConversion.SLFDialogueInterface",
    "BPI_Item": "/Script/SLFConversion.SLFItemInterface",
    "BPI_MainMenu": "/Script/SLFConversion.SLFMainMenuInterface",
    "BPI_NPC": "/Script/SLFConversion.SLFNPCInterface",
    "BPI_Player": "/Script/SLFConversion.SLFPlayerInterface",
    "BPI_Projectile": "/Script/SLFConversion.SLFProjectileInterface",
    "BPI_RestingPoint": "/Script/SLFConversion.SLFRestingPointInterface",
    "BPI_StatEntry": "/Script/SLFConversion.SLFStatEntryInterface",
    "BPI_Controller": "/Script/SLFConversion.BPI_Controller",
    "BPI_AIC": "/Script/SLFConversion.BPI_AIC",
    "BPI_GenericCharacter": "/Script/SLFConversion.BPI_GenericCharacter",
    "BPI_Enemy": "/Script/SLFConversion.BPI_Enemy",
    "BPI_GameInstance": "/Script/SLFConversion.BPI_GameInstance",
    "BPI_BossDoor": "/Script/SLFConversion.BPI_BossDoor",
    "BPI_DestructibleHelper": "/Script/SLFConversion.BPI_DestructibleHelper",
    "BPI_Executable": "/Script/SLFConversion.BPI_Executable",
    "BPI_ExecutionIndicator": "/Script/SLFConversion.BPI_ExecutionIndicator",
    "BPI_EnemyHealthbar": "/Script/SLFConversion.BPI_EnemyHealthbar",
}

# Data Assets - add to migration to clear logic
DATA_ASSET_MAP = {
    "PDA_Dialog": "/Script/SLFConversion.PDA_Dialog",
    "PDA_DefaultMeshData": "/Script/SLFConversion.PDA_DefaultMeshData",
}

# AnimBlueprints - special handling: clear EventGraph ONLY
# DO NOT reparent - causes "duplicate AnimGraph function" crash
ANIM_BP_MAP = {
    # Clear EventGraph only (keep variables for AnimGraph)
    # Reparenting to C++ AnimInstance causes crashes - keep original parent
    # The EventGraph contains getter functions that use Property Access to
    # read from owner and SET local variables, overwriting C++ values.
    # By clearing the EventGraph, C++ can set these variables directly.
    "ABP_SoulslikeNPC": None,  # No reparent, just clear EventGraph
    "ABP_SoulslikeEnemy": None,  # No reparent, just clear EventGraph
    "ABP_SoulslikeBossNew": None,  # No reparent, just clear EventGraph - C++ SLFBossAnimInstance has variables
    # ABP_SoulslikeCharacter_Additive - Clear EventGraph but DON'T reparent
    # AnimGraph uses 'Blend Poses by Enum' nodes with Blueprint enum E_OverlayState
    # EnumRedirects don't work for AnimGraph nodes, so we keep the Blueprint class
    # Character C++ code (SLFSoulslikeCharacter::Tick) sets BP variables via reflection
    "ABP_SoulslikeCharacter_Additive": None,  # No reparent - AnimGraph uses Blueprint enum
    "ABP_Manny_PostProcess": "/Script/SLFConversion.ABP_Manny_PostProcess",  # Post-process AnimBP
    "ABP_Quinn_PostProcess": "/Script/SLFConversion.ABP_Quinn_PostProcess",  # Post-process AnimBP
}

# AnimBlueprints that SHOULD be reparented (player character's AnimBP)
# EnumRedirects in DefaultEngine.ini handle E_OverlayState -> ESLFOverlayState mapping

ANIM_BP_REPARENT_MAP = {

    "ABP_SoulslikeCharacter_Additive": None,  # No reparent - AnimGraph uses Blueprint enum
}

# CRITICAL: AnimNotifyStates that call functions with "?" suffix pins
# These MUST be cleared FIRST before anything else loads them
# Otherwise cascade loading will trigger pin mismatch errors
PRIORITY_ANIM_NOTIFY_STATES = {
    # These call ToggleBuffer with "Buffer Open?" pin
    "ANS_InputBuffer": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    "ANS_RegisterAttackSequence": "/Script/SLFConversion.SLFAnimNotifyStateInputBuffer",
    # These call SetInvincibility with "Is Invincible?" pin
    "ANS_InvincibilityFrame": "/Script/SLFConversion.SLFAnimNotifyStateInvincibility",
    "ANS_HyperArmor": "/Script/SLFConversion.SLFAnimNotifyStateHyperArmor",
}

# NPC Characters that depend on ABP_SoulslikeNPC
# These must be processed AFTER AnimBPs are cleared
NPC_CHARACTER_MAP = {
    "B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",
}

# CRITICAL: Character Blueprints that call functions with special characters
# These MUST have their EventGraphs cleared to remove calls to:
# - IsEnemyDead? (? suffix)
# - Event Lockon (space in name)
# - AllowTargetSwap? (? suffix)
# NOTE: B_BaseCharacter is NOT here - it's in KEEP_VARS_MAP because its
# variables (components) are now defined in the C++ parent class
PRIORITY_CHARACTER_MAP = {
    # B_Soulslike_Character - NOW MIGRATED with full C++ implementation
    "B_Soulslike_Character": "/Script/SLFConversion.SLFSoulslikeCharacter",
    "B_Soulslike_Enemy": "/Script/SLFConversion.SLFSoulslikeEnemy",
    "B_Soulslike_Boss": "/Script/SLFConversion.SLFSoulslikeBoss",
}

# REPARENT ONLY: Blueprints that should inherit C++ parent but KEEP their EventGraph
# Use case: External callers (Level Blueprints) call custom events with space in names
# e.g., "Event ToggleCinematicMode" - C++ cannot expose functions with spaces
# Also: W_HUD calls child widgets via custom events like "Event ShowMessage"
REPARENT_ONLY_MAP = {
    # "W_HUD": "/Script/SLFConversion.W_HUD",  # MOVED to MIGRATION_MAP (EventGraph refs old B_Stat_C)
    # Child widgets that W_HUD calls via custom Blueprint events:
    "W_Interaction": "/Script/SLFConversion.W_Interaction",  # "Event OnItemOverlap", "Event Hide", "Event OnInteractableOverlap"
    "W_InteractionError": "/Script/SLFConversion.W_InteractionError",  # "Event ShowMessage"
    "W_BigScreenMessage": "/Script/SLFConversion.W_BigScreenMessage",  # "Event ShowMessage"
    # "W_Boss_Healthbar": "/Script/SLFConversion.W_Boss_Healthbar",  # MOVED to MIGRATION_MAP (EventGraph refs old B_Stat_C)
    # "W_RestMenu": "/Script/SLFConversion.W_RestMenu",  # MOVED to MIGRATION_MAP
    "W_Dialog": "/Script/SLFConversion.W_Dialog",  # "Event InitializeDialog", "Event CloseDialog"
    # "W_NPC_Window": "/Script/SLFConversion.W_NPC_Window",  # MOVED to MIGRATION_MAP
    # Menu widgets:
    # W_GameMenu - MOVED to MIGRATION_MAP (full C++ implementation complete)
    # Player Controller - reparent to C++ for input handling, keep Blueprint logic
}

# Path overrides - CORRECT paths from actual filesystem discovery
PATH_OVERRIDES = {
    # Components
    "AC_ActionManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
    "AC_AI_BehaviorManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_BehaviorManager",
    "AC_AI_Boss": "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss",
    "AC_AI_CombatManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
    "AC_AI_InteractionManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
    "AC_BuffManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_BuffManager",
    "AC_CollisionManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_CollisionManager",
    "AC_CombatManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
    "AC_DebugCentral": "/Game/SoulslikeFramework/Blueprints/Components/AC_DebugCentral",
    "AC_EquipmentManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
    "AC_InputBuffer": "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer",
    "AC_InteractionManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_InteractionManager",
    "AC_InventoryManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
    "AC_LadderManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_LadderManager",
    "AC_LootDropManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager",
    "AC_ProgressManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_ProgressManager",
    "AC_RadarElement": "/Game/SoulslikeFramework/Blueprints/Components/AC_RadarElement",
    "AC_RadarManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_RadarManager",
    "AC_SaveLoadManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_SaveLoadManager",
    "AC_StatManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
    "AC_StatusEffectManager": "/Game/SoulslikeFramework/Blueprints/Components/AC_StatusEffectManager",
    # Game Framework
    # "GI_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework",  # Keep Blueprint logic
    "GM_Menu_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/MainMenu/GM_Menu_SoulslikeFramework",
    # "GM_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework",  # Keep Blueprint logic
    "GS_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GS_SoulslikeFramework",
    "PC_Menu_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/MainMenu/PC_Menu_SoulslikeFramework",
    "PC_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework",  # Reparent only - C++ handles input
    "PS_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/PS_SoulslikeFramework",
    "AIC_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework",
    # SaveGames
    "SG_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Saving/SG_SoulslikeFramework",
    "SG_SaveSlots": "/Game/SoulslikeFramework/Blueprints/Saving/SG_SaveSlots",
    # Characters
    "B_BaseCharacter": "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "B_Soulslike_Character": "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "B_Soulslike_Boss": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "B_Soulslike_Boss_Malgareth": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    "B_Soulslike_Enemy": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "B_Soulslike_Enemy_Guard": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "B_Soulslike_Enemy_Showcase": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
    "B_Soulslike_NPC": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "B_Soulslike_NPC_ShowcaseGuide": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
    # Stats - CORRECT PATHS
    "B_Stat": "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "B_HP": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
    "B_FP": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP",
    "B_Stamina": "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina",
    "B_Vigor": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "B_Mind": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
    "B_Endurance": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",
    "B_Strength": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "B_Dexterity": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    "B_Intelligence": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
    "B_Faith": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
    "B_Arcane": "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",
    "B_Weight": "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight",
    "B_Poise": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Poise",
    "B_Stance": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Stance",
    "B_IncantationPower": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_IncantationPower",
    # Defense stats
    "B_Resistance_Focus": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus",
    "B_Resistance_Immunity": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity",
    "B_Resistance_Robustness": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness",
    "B_Resistance_Vitality": "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality",
    "B_DN_Fire": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire",
    "B_DN_Frost": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost",
    "B_DN_Holy": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy",
    "B_DN_Lightning": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning",
    "B_DN_Magic": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic",
    "B_DN_Physical": "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
    # Attack power stats
    "B_AP_Fire": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
    "B_AP_Frost": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost",
    "B_AP_Holy": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy",
    "B_AP_Lightning": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning",
    "B_AP_Magic": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic",
    "B_AP_Physical": "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical",
    # Status Effects
    "B_StatusEffect": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "B_StatusEffect_Bleed": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Bleed",
    "B_StatusEffect_Burn": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Burn",
    "B_StatusEffect_Corruption": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Corruption",
    "B_StatusEffect_Frostbite": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Frostbite",
    "B_StatusEffect_Madness": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Madness",
    "B_StatusEffect_Plague": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Plague",
    "B_StatusEffect_Poison": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Poison",
    "B_StatusEffectArea": "/Game/SoulslikeFramework/Data/StatusEffects/B_StatusEffectArea",
    "B_StatusEffectOneShot": "/Game/SoulslikeFramework/Data/StatusEffects/B_StatusEffectOneShot",
    "B_StatusEffectBuildup": "/Game/SoulslikeFramework/Data/Stats/_Backend/B_StatusEffectBuildup",
    # Buffs
    "B_Buff": "/Game/SoulslikeFramework/Data/Buffs/Logic/B_Buff",
    "B_Buff_AttackPower": "/Game/SoulslikeFramework/Data/Buffs/Logic/B_Buff_AttackPower",
    # Primary Data Assets - PDA_Action excluded (reparenting loses ActionClass data in child DA_* assets)
    "PDA_Action": "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action",
    # EXPERIMENT: PDA_Item reparent
    "PDA_Item": "/Game/SoulslikeFramework/Data/PDA_Item",
    "PDA_AI_Ability": "/Game/SoulslikeFramework/Data/AI_Abilities/PDA_AI_Ability",
    "PDA_Buff": "/Game/SoulslikeFramework/Data/Buffs/PDA_Buff",
    "PDA_Calculations": "/Game/SoulslikeFramework/Data/Calculators/PDA_Calculations",
    "PDA_Dialog": "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
    "PDA_WeaponAbility": "/Game/SoulslikeFramework/Data/WeaponAbilities/PDA_WeaponAbility",
    "PDA_Vendor": "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor",
    "PDA_MainMenuData": "/Game/SoulslikeFramework/Data/MainMenu/PDA_MainMenuData",
    "PDA_StatusEffect": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/PDA_StatusEffect",
    "PDA_Credits": "/Game/SoulslikeFramework/Data/PDA_Credits",
    "PDA_LoadingScreens": "/Game/SoulslikeFramework/Data/PDA_LoadingScreens",
    "PDA_DayNight": "/Game/SoulslikeFramework/Data/PDA_DayNight",
    "PDA_AnimData": "/Game/SoulslikeFramework/Data/_AnimationData/PDA_AnimData",
    "PDA_CombatReactionAnimData": "/Game/SoulslikeFramework/Data/_AnimationData/PDA_CombatReactionAnimData",
    "PDA_CombatReactionAnimData_Player": "/Game/SoulslikeFramework/Data/_AnimationData/PDA_CombatReactionAnimData_Player",
    "PDA_LadderAnimData": "/Game/SoulslikeFramework/Data/_AnimationData/PDA_LadderAnimData",
    "PDA_PoiseBreakAnimData": "/Game/SoulslikeFramework/Data/_AnimationData/PDA_PoiseBreakAnimData",
    "PDA_WeaponAnimset": "/Game/SoulslikeFramework/Data/WeaponAnimsets/PDA_WeaponAnimset",
    "PDA_BaseCharacterInfo": "/Game/SoulslikeFramework/Data/PDA_BaseCharacterInfo",
    "PDA_CustomSettings": "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
    "PDA_MovementSpeedData": "/Game/SoulslikeFramework/Data/PDA_MovementSpeedData",
    "PDA_LadderAnimData": "/Game/SoulslikeFramework/Data/_AnimationData/PDA_LadderAnimData",
    "PDA_ExecutionAnimData": "/Game/SoulslikeFramework/Data/_AnimationData/Executions/PDA_ExecutionAnimData",
    # Actions
    "B_Action": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action",
    "B_Action_Backstab": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Backstab",
    "B_Action_ComboHeavy": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboHeavy",
    "B_Action_ComboLight_L": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboLight_L",
    "B_Action_ComboLight_R": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboLight_R",
    "B_Action_Crouch": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Crouch",
    "B_Action_Dodge": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
    "B_Action_DrinkFlask_HP": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_DrinkFlask_HP",
    "B_Action_DualWieldAttack": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_DualWieldAttack",
    "B_Action_Jump": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Jump",
    "B_Action_JumpAttack": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_JumpAttack",
    # Interactables
    "B_Interactable": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "B_Ladder": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder",
    "B_Door": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
    "B_Container": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    "B_RestingPoint": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    # Items
    "B_Item": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_Item",
    "B_Item_Weapon": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "B_Item_Weapon_Shield": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    # Projectiles
    "B_BaseProjectile": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_BaseProjectile",
    # Ability Effects
    "B_AbilityEffectBase": "/Game/SoulslikeFramework/Data/WeaponAbilities/AbilityEffects/B_AbilityEffectBase",
    "B_SkyManager": "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager",
    # Animation Notifies - CORRECT PATH
    "AN_FootstepTrace": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_FootstepTrace",
    "AN_CameraShake": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_CameraShake",
    "AN_SpawnProjectile": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_SpawnProjectile",
    "AN_AdjustStat": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_AdjustStat",
    "AN_AI_SpawnProjectile": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_AI_SpawnProjectile",
    "AN_AoeDamage": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_AoeDamage",
    "AN_InterruptMontage": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_InterruptMontage",
    "AN_LaunchField": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_LaunchField",
    "AN_PlayCameraSequence": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_PlayCameraSequence",
    "AN_SetAiState": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_SetAiState",
    "AN_SetMovementMode": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_SetMovementMode",
    "AN_TryGuard": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_TryGuard",
    "AN_WorldCameraShake": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_WorldCameraShake",
    "ANS_WeaponTrace": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_WeaponTrace",
    "ANS_ToggleChaosField": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField",
    "ANS_HyperArmor": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_HyperArmor",
    "ANS_InvincibilityFrame": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InvincibilityFrame",
    "ANS_InputBuffer": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer",
    "ANS_Trail": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_Trail",
    "ANS_AI_FistTrace": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_AI_FistTrace",
    "ANS_AI_RotateTowardsTarget": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_AI_RotateTowardsTarget",
    "ANS_AI_Trail": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_AI_Trail",
    "ANS_AI_WeaponTrace": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_AI_WeaponTrace",
    "ANS_FistTrace": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_FistTrace",
    "ANS_RegisterAttackSequence": "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_RegisterAttackSequence",
    # AnimBlueprints
    # NOTE: ABP_SoulslikeCharacter_Additive EventGraph is CLEARED and reparented to C++
    # Additional Actions
    "B_Action_Execute": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Execute",
    "B_Action_GuardStart": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_GuardStart",
    "B_Action_GuardEnd": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_GuardEnd",
    "B_Action_GuardCancel": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_GuardCancel",
    "B_Action_StartSprinting": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_StartSprinting",
    "B_Action_StopSprinting": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_StopSprinting",
    "B_Action_SprintAttack": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_SprintAttack",
    "B_Action_ThrowProjectile": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ThrowProjectile",
    "B_Action_TwoHandedStance_L": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_TwoHandedStance_L",
    "B_Action_TwoHandedStance_R": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_TwoHandedStance_R",
    "B_Action_UseEquippedTool": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_UseEquippedTool",
    "B_Action_WeaponAbility": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_WeaponAbility",
    "B_Action_PickupItemMontage": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_PickupItemMontage",
    "B_Action_ScrollWheel_LeftHand": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ScrollWheel_LeftHand",
    "B_Action_ScrollWheel_RightHand": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ScrollWheel_RightHand",
    "B_Action_ScrollWheel_Tools": "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ScrollWheel_Tools",
    # Additional Interactables
    "B_BossDoor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
    "B_DeathCurrency": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_DeathCurrency",
    "B_Door_Demo": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo",
    # Additional Items
    "B_PickupItem": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem",
    "B_Item_Lantern": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Usables/B_Item_Lantern",
    "B_Item_AI_Weapon": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
    "B_Item_AI_Weapon_BossMace": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
    "B_Item_AI_Weapon_Greatsword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "B_Item_AI_Weapon_Sword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "B_Item_Weapon_BossMace": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
    "B_Item_Weapon_Greatsword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "B_Item_Weapon_Katana": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "B_Item_Weapon_PoisonSword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "B_Item_Weapon_SwordExample01": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "B_Item_Weapon_SwordExample02": "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    # Additional World Actors
    "B_Chaos_ForceField": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
    "B_SequenceActor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor",
    "BFL_Helper": "/Game/SoulslikeFramework/Blueprints/BFL_Helper",
    "B_DeathTrigger": "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_DeathTrigger",
    "B_Destructible": "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible",
    "B_LocationActor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_LocationActor",
    "B_Torch": "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Torch",
    "ABP_Manny_PostProcess": "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Rigs/ABP_Manny_PostProcess",
    "ABP_Quinn_PostProcess": "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Rigs/ABP_Quinn_PostProcess",
    "B_Projectile_Boss_Fireball": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_Projectile_Boss_Fireball",
    "B_Projectile_ThrowingKnife": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_Projectile_ThrowingKnife",
    # AI Tasks
    "BTT_ClearKey": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ClearKey",
    "BTT_GetCurrentLocation": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetCurrentLocation",
    "BTT_GetRandomPoint": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint",
    "BTT_GetRandomPointNearStartPosition": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPointNearStartPosition",
    "BTT_GetStrafePointAroundTarget": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePointAroundTarget",
    "BTT_PatrolPath": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_PatrolPath",
    "BTT_SetKey": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetKey",
    "BTT_SetMovementMode": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetMovementMode",
    "BTT_SimpleMoveTo": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo",
    "BTT_SwitchState": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState",
    "BTT_SwitchToPreviousState": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchToPreviousState",
    "BTT_ToggleFocus": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus",
    "BTT_TryExecuteAbility": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility",
    # AI Services
    "BTS_ChaseBounds": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_ChaseBounds",
    "BTS_DistanceCheck": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_DistanceCheck",
    "BTS_IsTargetDead": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_IsTargetDead",
    "BTS_SetMovementModeBasedOnDistance": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_SetMovementModeBasedOnDistance",
    "BTS_TryGetAbility": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility",
    # Misc
    "B_PatrolPath": "/Game/SoulslikeFramework/Blueprints/_AI/Misc/B_PatrolPath",
    "B_Discovery": "/Game/SoulslikeFramework/Data/Stats/Misc/B_Discovery",
    # Interfaces
    "BPI_Interactable": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Interactable",
    "BPI_Item": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Item",
    "BPI_MainMenu": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_MainMenu",
    "BPI_NPC": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_NPC",
    "BPI_Player": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Player",
    "BPI_Projectile": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Projectile",
    "BPI_RestingPoint": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_RestingPoint",
    "BPI_StatEntry": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_StatEntry",
    "BPI_GenericCharacter": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_GenericCharacter",
    "BPI_Enemy": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Enemy",
    "BPI_GameInstance": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_GameInstance",
    "BPI_BossDoor": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_BossDoor",
    "BPI_DestructibleHelper": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_DestructibleHelper",
    "BPI_Executable": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Executable",
    "BPI_ExecutionIndicator": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_ExecutionIndicator",
    "BPI_EnemyHealthbar": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_EnemyHealthbar",
    # AnimBlueprints - CORRECT PATHS
    "ABP_SoulslikeNPC": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "ABP_SoulslikeEnemy": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "ABP_SoulslikeBossNew": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "ABP_SoulslikeCharacter_Additive": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
}

# Search paths for finding Blueprints
SEARCH_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/Components",
    "/Game/SoulslikeFramework/Blueprints/_BaseClasses",
    "/Game/SoulslikeFramework/Blueprints/Global",
    "/Game/SoulslikeFramework/Blueprints/_Characters",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies",
    "/Game/SoulslikeFramework/Blueprints/_Characters/NPCs",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks",
    "/Game/SoulslikeFramework/Blueprints/_AI/Services",
    "/Game/SoulslikeFramework/Blueprints/_AI",
    "/Game/SoulslikeFramework/Data/Stats/StatClasses",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses",
    "/Game/SoulslikeFramework/Data/Actions/ActionClasses",
    "/Game/SoulslikeFramework/Data/Buffs/BuffClasses",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables",
    "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies",
    "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates",
    "/Game/SoulslikeFramework/Blueprints/Interfaces",
    # Widget paths
    "/Game/SoulslikeFramework/Widgets/Crafting",
    "/Game/SoulslikeFramework/Widgets/Equipment",
    "/Game/SoulslikeFramework/Widgets/HUD",
    "/Game/SoulslikeFramework/Widgets/Inventory",
    "/Game/SoulslikeFramework/Widgets/MainMenu",
    "/Game/SoulslikeFramework/Widgets/Radar",
    "/Game/SoulslikeFramework/Widgets/RestMenu",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu",
    "/Game/SoulslikeFramework/Widgets/Stats",
    "/Game/SoulslikeFramework/Widgets/Status",
    "/Game/SoulslikeFramework/Widgets/Utility",
    "/Game/SoulslikeFramework/Widgets/Vendor",
    "/Game/SoulslikeFramework/Widgets/World",
    "/Game/SoulslikeFramework/Widgets/_Debug",
    "/Game/SoulslikeFramework/Widgets/_Generic",
]

# Skip list - Blueprints to keep in Blueprint (not migrate to C++)
# Demo/showcase Blueprints are editor-time tools, not gameplay code
SKIP_LIST = {
    "B_DemoRoom",           # Procedural room generation - editor tool
    "B_DemoDisplay",        # Demo display setup - editor tool  
    "B_Demo_TimeSlider",    # Time slider widget - demo only
    "B_ShowcaseEnemy_ExitCombat",   # Demo showcase helper
    "B_ShowcaseEnemy_StatDisplay",  # Demo showcase helper
}

def find_bp(name):
    """Find a Blueprint by name, checking overrides first then search paths"""
    name_str = str(name)

    # Skip known problematic Blueprints
    if name_str in SKIP_LIST:
        unreal.log_warning("  find_bp: SKIPPING " + name_str + " (known to crash engine)")
        return None

    unreal.log_warning("  find_bp: Looking for " + name_str)

    # Check path override first - USE ONLY OVERRIDES to avoid crashes
    if name_str in PATH_OVERRIDES:
        try:
            override_path = str(PATH_OVERRIDES[name_str])
            unreal.log_warning("  find_bp: Checking override path: " + override_path)
            # Check if asset exists before loading to avoid crash
            exists = unreal.EditorAssetLibrary.does_asset_exist(override_path)
            unreal.log_warning("  find_bp: exists=" + str(exists))
            if exists:
                asset = unreal.EditorAssetLibrary.load_asset(override_path)
                if asset:
                    unreal.log_warning("  find_bp: Loaded from override")
                    return asset
        except Exception as e:
            unreal.log_error("  Error loading override: " + str(e))
        # If override failed, don't search other paths (they crash the engine)
        unreal.log_warning("  find_bp: Override failed, returning None")
        return None

    # Search through standard paths - only if not in overrides
    for i in range(len(SEARCH_PATHS)):
        try:
            path_str = str(SEARCH_PATHS[i])
            full_path = path_str + "/" + name_str
            # Check if asset exists before loading
            if unreal.EditorAssetLibrary.does_asset_exist(full_path):
                unreal.log_warning("  find_bp: Found at " + full_path)
                asset = unreal.EditorAssetLibrary.load_asset(full_path)
                if asset:
                    return asset
        except:
            pass

    unreal.log_warning("  find_bp: Not found anywhere")
    return None

def run():
    unreal.log_warning("=" * 60)
    unreal.log_warning("LOAD-FIRST MIGRATION")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Strategy: Load ALL Blueprints first, then clear ALL, then reparent ALL")
    unreal.log_warning("This prevents cascade loading from triggering compile of partially-cleared BPs")

    # Blueprints that crash during loading due to async loader REINST_ issues
    # These have complex component hierarchies that cause engine crashes
    # Only skip truly problematic Blueprints
    # Priority characters moved to PRIORITY_CHARACTER_MAP for explicit processing
    LOAD_SKIP_LIST = [
        # All Blueprints now have C++ classes - no skipping needed
        # Previously skipped items now process normally:
        # - B_Soulslike_Boss_Malgareth -> SLFBossMalgareth
        # - B_Soulslike_Enemy_Guard -> SLFEnemyGuard
        # - B_Soulslike_Enemy_Showcase -> SLFEnemyShowcase
    ]

    all_bps = []  # (bp, name, cpp_class, bp_type)

    # =========================================================================
    # PHASE 1: LOAD ALL BLUEPRINTS FIRST
    # This completes all cascade loading while Blueprints are still intact
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 1: LOAD ALL BLUEPRINTS")
    print("=" * 60)

    # 1A: Load Priority AnimNotifyStates
    print("")
    print("=== Loading Priority AnimNotifyStates ===")
    for name, cpp in PRIORITY_ANIM_NOTIFY_STATES.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "priority_ans"))
                print("  " + name + ": loaded")
            else:
                print("  " + name + ": not found")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1B: Load AnimBlueprints
    print("")
    print("=== Loading AnimBlueprints ===")
    for name, cpp in ANIM_BP_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "anim_bp"))
                print("  " + name + ": loaded")
            else:
                print("  " + name + ": not found")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1B2: Load AnimBlueprints that should be reparented (player's AnimBP)
    print("")
    print("=== Loading AnimBlueprints (Reparent) ===")
    for name, cpp in ANIM_BP_REPARENT_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "anim_bp_reparent"))
                print("  " + name + ": loaded")
            else:
                print("  " + name + ": not found")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1C: Load Priority Characters (MUST come before NPC Characters)
    print("")
    print("=== Loading Priority Characters ===")
    priority_char_loaded = 0
    for name, cpp in PRIORITY_CHARACTER_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "priority_character"))
                priority_char_loaded += 1
                print("  " + name + ": loaded")
            else:
                print("  " + name + ": not found")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))
    print("Loaded " + str(priority_char_loaded) + " priority characters")

    # 1D: Load NPC Characters
    print("")
    print("=== Loading NPC Characters ===")
    for name, cpp in NPC_CHARACTER_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "npc_character"))
                print("  " + name + ": loaded")
            else:
                print("  " + name + ": not found")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1D: Load Regular Blueprints
    print("")
    print("=== Loading Regular Blueprints ===")
    migration_keys = list(MIGRATION_MAP.keys())
    load_skipped = 0
    for i in range(len(migration_keys)):
        name = str(migration_keys[i])
        cpp = str(MIGRATION_MAP[name])
        if name in LOAD_SKIP_LIST:
            load_skipped += 1
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "regular"))
        except Exception as e:
            print("  " + name + ": load error - " + str(e))
    print("  Loaded " + str(len([x for x in all_bps if x[3] == "regular"])) + " regular Blueprints, skipped " + str(load_skipped))

    # 1E: Load Data Assets
    print("")
    print("=== Loading Data Assets ===")
    for name, cpp in DATA_ASSET_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            data_paths = [
                "/Game/SoulslikeFramework/Data/Dialog",
                "/Game/SoulslikeFramework/Data",
            ]
            bp = None
            for path in data_paths:
                full_path = path + "/" + name
                if unreal.EditorAssetLibrary.does_asset_exist(full_path):
                    bp = unreal.EditorAssetLibrary.load_asset(full_path)
                    break
            if bp:
                all_bps.append((bp, name, cpp, "data_asset"))
                print("  " + name + ": loaded")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1F: Load Keep-Vars Blueprints
    print("")
    print("=== Loading Keep-Vars Blueprints ===")
    for name, cpp in KEEP_VARS_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "keep_vars"))
                print("  " + name + ": loaded")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1G: Load Reparent-Only Blueprints (keep EventGraph, just reparent)
    print("")
    print("=== Loading Reparent-Only Blueprints ===")
    for name, cpp in REPARENT_ONLY_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "reparent_only"))
                print("  " + name + ": loaded")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    # 1H: Load Interfaces
    print("")
    print("=== Loading Interfaces ===")
    for name, cpp in INTERFACE_MAP.items():
        if name in LOAD_SKIP_LIST:
            print("  " + name + ": SKIPPED (crash prevention)")
            continue
        try:
            bp = find_bp(name)
            if bp:
                all_bps.append((bp, name, cpp, "interface"))
                print("  " + name + ": loaded")
        except Exception as e:
            print("  " + name + ": load error - " + str(e))

    print("")
    print("Total loaded: " + str(len(all_bps)))

    # =========================================================================
    # PHASE 2: CLEAR ALL BLUEPRINTS
    # Now that all are loaded, clear them in batch (no cascade loading issues)
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 2: CLEAR ALL BLUEPRINTS")
    print("=" * 60)

    cleared = 0
    clear_errors = 0
    skipped_interfaces = 0
    skipped_reparent_only = 0
    for bp, name, cpp, bp_type in all_bps:
        try:
            # SKIP Blueprint interfaces entirely - they only contain function signatures
            # that callers depend on. Clearing them breaks all interface calls.
            if bp_type == "interface":
                skipped_interfaces += 1
                print("  " + name + ": SKIPPED (Blueprint interface - keep signatures)")
                continue

            # SKIP reparent_only Blueprints - keep EventGraph for external callers
            if bp_type == "reparent_only":
                skipped_reparent_only += 1
                print("  " + name + ": SKIPPED (reparent_only - keep EventGraph for external callers)")
                continue

            if bp_type == "anim_bp" or bp_type == "anim_bp_reparent" or bp_type == "keep_vars":
                # AnimBPs: Clear EventGraph but keep variables (AnimGraph reads them)
                # ABP_SoulslikeCharacter_Additive: Reparented to C++, EnumRedirects handle type mapping
                # C++ NativeUpdateAnimation sets overlay states from EquipmentManager on Controller
                unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            else:
                # Full clear for everything else
                unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)

            # Remove interfaces except for AnimBlueprints
            # AnimBPs use interfaces for animation layers (ALI_OverlayStates, ALI_LocomotionStates)
            if bp_type != "anim_bp":
                unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)

            cleared += 1
        except Exception as e:
            print("  " + name + ": clear error - " + str(e))
            clear_errors += 1

    print("Cleared: " + str(cleared) + ", Errors: " + str(clear_errors))

    # Count by type
    priority_cleared = len([x for x in all_bps if x[3] == "priority_ans"])
    priority_char_cleared = len([x for x in all_bps if x[3] == "priority_character"])
    anim_bp_cleared = len([x for x in all_bps if x[3] == "anim_bp"])
    npc_cleared = len([x for x in all_bps if x[3] == "npc_character"])
    regular_cleared = len([x for x in all_bps if x[3] == "regular"])
    data_cleared = len([x for x in all_bps if x[3] == "data_asset"])
    keep_cleared = len([x for x in all_bps if x[3] == "keep_vars"])
    iface_cleared = len([x for x in all_bps if x[3] == "interface"])

    # =========================================================================
    # FLUSH: Force GC and wait before reparenting
    # This helps ensure all loading/compilation is complete
    # =========================================================================
    print("")
    print("=== Flushing: Collect garbage and wait ===")
    import gc
    gc.collect()
    unreal.SystemLibrary.collect_garbage()
    # Small wait to let async operations complete
    import time
    time.sleep(2.0)
    print("Flush complete")

    # Blueprints that crash during reparent due to async loader issues
    # All character and interactable Blueprints now migrated to C++
    REPARENT_SKIP_LIST = [
        # Skip Blueprints that crash during reparent - but NOT B_BaseCharacter
        # B_BaseCharacter is the critical parent and MUST be reparented
        # "B_Container",  # Removed from skip - now processed normally
        # B_PickupItem removed - must be reparented for item pickup to work
    ]

    # =========================================================================
    # PHASE 3: REPARENT ALL BLUEPRINTS
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 3: REPARENT ALL BLUEPRINTS")
    print("=" * 60)
    reparent_ok = 0
    reparent_fail = 0
    reparent_skip = 0
    for bp, name, cpp, bp_type in all_bps:
        try:
            if bp_type == "interface":
                reparent_ok += 1  # Interfaces don't reparent
                continue
            if bp_type == "anim_bp":
                # AnimBPs: Reparent if cpp target is set, otherwise just clear EventGraph
                if cpp is None:
                    reparent_skip += 1
                    print("  " + name + ": SKIPPED (AnimBP - no C++ target, EventGraph cleared only)")
                    continue
                # Else: fall through to reparent (ABP_SoulslikeCharacter_Additive has cpp target)
            if name in REPARENT_SKIP_LIST:
                reparent_skip += 1
                print("  " + name + ": SKIPPED (complex hierarchy)")
                continue
            ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp)
            if ok:
                reparent_ok += 1
                print("  " + name + ": reparented")
            else:
                reparent_fail += 1
                print("  " + name + ": FAILED reparent to " + cpp)
        except Exception as e:
            reparent_fail += 1
            print("  " + name + ": ERROR - " + str(e))
    print("Reparent: " + str(reparent_ok) + " ok, " + str(reparent_fail) + " failed, " + str(reparent_skip) + " skipped")

    # =========================================================================
    # PHASE 4: SAVE ALL BLUEPRINTS
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 4: SAVE ALL BLUEPRINTS")
    print("=" * 60)
    saved = 0
    for bp, name, cpp, bp_type in all_bps:
        try:
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            saved += 1
        except Exception as e:
            print("  " + name + ": save error - " + str(e))
    print("Saved: " + str(saved))

    # =========================================================================
    # PHASE 4B: PLAYER ANIMBP REPARENTED TO C++
    # ABP_SoulslikeCharacter_Additive IS reparented - EnumRedirects handle type mapping.
    # C++ NativeUpdateAnimation reads overlay states from EquipmentManager on Controller.
    # =========================================================================
    print("")
    print("=== PHASE 4B: Player AnimBP EventGraph preserved ===")

    # =========================================================================
    # PHASE 5: COMPILE - SKIPPED
    # The assets are already saved. Compile will happen on editor open.
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 5: COMPILE - SKIPPED")
    print("=" * 60)
    print("Compiling character Blueprints to preserve component hierarchy...")
    compile_ok = 0
    compile_fail = 0
    # Compile keep_vars blueprints (B_BaseCharacter, B_Soulslike_Character)
    # These MUST be compiled for their SCS components to work at runtime
    for bp, name, cpp, bp_type in all_bps:
        if bp_type == "keep_vars":
            try:
                result = unreal.SLFAutomationLibrary.compile_and_save(bp)
                if result:
                    print("  " + name + ": COMPILED OK")
                    compile_ok += 1
                else:
                    print("  " + name + ": compile returned false")
                    compile_fail += 1
            except Exception as e:
                print("  " + name + ": compile error - " + str(e))
                compile_fail += 1
    print("Compile: " + str(compile_ok) + " ok, " + str(compile_fail) + " failed")

    # =========================================================================
    # PHASE 6: POST-MIGRATION VALIDATION
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 6: POST-MIGRATION VALIDATION")
    print("=" * 60)

    # Validate key components have expected properties
    validation_results = []

    # Components to validate with their key properties
    VALIDATION_TARGETS = {
        "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager": ["Actions", "AvailableActions", "StaminaRegenDelay"],
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager": ["Stats", "StatTable"],
        "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager": ["CurrentTarget", "LockedTarget"],
        "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer": ["BufferQueue", "BufferOpen"],
    }

    for bp_path, properties in VALIDATION_TARGETS.items():
        try:
            bp = unreal.EditorAssetLibrary.load_asset(bp_path)
            if not bp:
                validation_results.append(("FAIL", bp_path, "Could not load Blueprint"))
                continue

            gen_class = bp.generated_class()
            if not gen_class:
                validation_results.append(("FAIL", bp_path, "No generated class"))
                continue

            cdo = unreal.get_default_object(gen_class)
            if not cdo:
                validation_results.append(("FAIL", bp_path, "Could not get CDO"))
                continue

            # Check each property exists
            for prop_name in properties:
                try:
                    val = cdo.get_editor_property(prop_name)
                    validation_results.append(("OK", bp_path.split("/")[-1], prop_name + " accessible"))
                except Exception as e:
                    validation_results.append(("FAIL", bp_path.split("/")[-1], prop_name + " error: " + str(e)))

        except Exception as e:
            validation_results.append(("FAIL", bp_path, "Exception: " + str(e)))

    # Print validation results
    val_ok = sum(1 for r in validation_results if r[0] == "OK")
    val_fail = sum(1 for r in validation_results if r[0] == "FAIL")

    for status, component, msg in validation_results:
        if status == "FAIL":
            print("  FAIL: " + component + " - " + msg)

    print("Validation: " + str(val_ok) + " OK, " + str(val_fail) + " FAIL")

    # =========================================================================
    # POST-MIGRATION: Dodge Montages
    # =========================================================================
    print("")
    print("=" * 60)
    print("POST-MIGRATION: Dodge Montages")
    print("=" * 60)

    DODGE_MONTAGES = {
        "forward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F",
        "forward_left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FL.AM_SLF_Dodge_FL",
        "forward_right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FR.AM_SLF_Dodge_FR",
        "left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_L.AM_SLF_Dodge_L",
        "right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_R.AM_SLF_Dodge_R",
        "backward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_B.AM_SLF_Dodge_B",
        "backward_left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BL.AM_SLF_Dodge_BL",
        "backward_right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BR.AM_SLF_Dodge_BR",
        "backstep": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Backstep.AM_SLF_Backstep",
    }

    try:
        dodge_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
        if dodge_asset:
            dodge_montages = dodge_asset.get_editor_property('DodgeMontages')
            montages_set = 0
            for prop_name, montage_path in DODGE_MONTAGES.items():
                montage = unreal.load_object(None, montage_path)
                if montage:
                    setattr(dodge_montages, prop_name, montage)
                    montages_set += 1
            dodge_asset.set_editor_property('DodgeMontages', dodge_montages)
            unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge", only_if_is_dirty=False)
            print("Dodge montages: " + str(montages_set) + "/9 set")
        else:
            print("Could not load DA_Action_Dodge")
    except Exception as e:
        print("Dodge montages ERROR: " + str(e))

    # =========================================================================
    # POST-MIGRATION: Item Niagara Systems
    # =========================================================================
    print("")
    print("=" * 60)
    print("POST-MIGRATION: Item Niagara Systems")
    print("=" * 60)

    NIAGARA_PATHS = {
        "DA_Apple": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLootSimple.NS_ItemLootSimple",
        "DA_BossMace": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_CrimsonEverbloom": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLootSimple.NS_ItemLootSimple",
        "DA_Cube": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLootSimple.NS_ItemLootSimple",
        "DA_ExampleArmor": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ExampleArmor02": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ExampleBracers": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ExampleGreaves": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ExampleHat": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ExampleHelmet": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ExampleTalisman": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_Greatsword": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_HealthFlask": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_Katana": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_Lantern": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLootSimple.NS_ItemLootSimple",
        "DA_PoisonSword": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_PrisonKey": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLootSimple.NS_ItemLootSimple",
        "DA_Shield01": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_Sword01": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_Sword02": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLoot.NS_ItemLoot",
        "DA_ThrowingKnife": "/Game/SoulslikeFramework/VFX/Systems/NS_ItemLootSimple.NS_ItemLootSimple",
    }

    niagara_set = 0
    niagara_failed = 0
    for asset_name, niagara_path in NIAGARA_PATHS.items():
        try:
            item_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/" + asset_name)
            if item_asset:
                niagara_asset = unreal.EditorAssetLibrary.load_asset(niagara_path)
                if niagara_asset:
                    item_asset.set_editor_property('world_niagara_system', niagara_asset)
                    unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Data/Items/" + asset_name, only_if_is_dirty=False)
                    niagara_set += 1
                else:
                    niagara_failed += 1
            else:
                niagara_failed += 1
        except Exception as e:
            niagara_failed += 1
    print("Niagara systems: " + str(niagara_set) + "/" + str(len(NIAGARA_PATHS)) + " set, " + str(niagara_failed) + " failed")

    # =========================================================================
    # POST-MIGRATION: Character DefaultMeshAsset
    # =========================================================================
    print("")
    print("=" * 60)
    print("POST-MIGRATION: Character DefaultMeshAsset")
    print("=" * 60)

    DEFAULT_MESH_ASSETS = {
        "DA_Manny": "/Game/SoulslikeFramework/Data/BaseCharacters/DA_MannyMeshDefault",
        "DA_Quinn": "/Game/SoulslikeFramework/Data/BaseCharacters/DA_QuinnMeshDefault",
    }

    mesh_set = 0
    mesh_failed = 0
    for asset_name, mesh_path in DEFAULT_MESH_ASSETS.items():
        try:
            char_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/BaseCharacters/" + asset_name)
            if char_asset:
                mesh_asset = unreal.EditorAssetLibrary.load_asset(mesh_path)
                if mesh_asset:
                    char_asset.set_editor_property('default_mesh_asset', mesh_asset)
                    unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Data/BaseCharacters/" + asset_name, only_if_is_dirty=False)
                    print("  " + asset_name + ": " + mesh_path.split("/")[-1])
                    mesh_set += 1
                else:
                    print("  " + asset_name + ": FAILED - mesh not found")
                    mesh_failed += 1
            else:
                print("  " + asset_name + ": FAILED - asset not found")
                mesh_failed += 1
        except Exception as e:
            print("  " + asset_name + ": ERROR - " + str(e))
            mesh_failed += 1
    print("DefaultMeshAsset: " + str(mesh_set) + "/" + str(len(DEFAULT_MESH_ASSETS)) + " set")

    # =========================================================================
    # POST-MIGRATION: Add Tab Key for Menu
    # =========================================================================
    # Add Tab as alternate key for IA_GameMenu (ESC exits PIE by default)
    # NOTE: UE5.7 uses default_key_mappings instead of deprecated mappings property
    print("")
    print("=" * 60)
    print("POST-MIGRATION: Add Tab Key for Menu")
    print("=" * 60)

    try:
        imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")
        ia_game_menu = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu")

        if imc and ia_game_menu:
            # UE5.7 API: use default_key_mappings.mappings
            key_mappings_data = imc.get_editor_property('default_key_mappings')
            mappings = key_mappings_data.get_editor_property('mappings')
            mappings_list = list(mappings)

            # Check if Tab mapping already exists
            tab_exists = False
            for m in mappings_list:
                action = m.get_editor_property('action')
                if action and action.get_path_name() == ia_game_menu.get_path_name():
                    key = m.get_editor_property('key')
                    key_name = key.get_editor_property('key_name')
                    if "Tab" in str(key_name):
                        tab_exists = True
                        break

            if not tab_exists:
                # Create new mapping for Tab key
                new_mapping = unreal.EnhancedActionKeyMapping()
                new_mapping.set_editor_property('action', ia_game_menu)

                # Create Tab key using set_editor_property (UE5.7 API)
                tab_key = unreal.Key()
                tab_key.set_editor_property('key_name', 'Tab')
                new_mapping.set_editor_property('key', tab_key)

                # Add to mappings and save
                mappings_list.append(new_mapping)
                key_mappings_data.set_editor_property('mappings', mappings_list)
                imc.set_editor_property('default_key_mappings', key_mappings_data)
                unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay", only_if_is_dirty=False)
                print("Tab key added to IA_GameMenu")
            else:
                print("Tab key already exists for IA_GameMenu")
        else:
            print("Could not load IMC_Gameplay or IA_GameMenu")
    except Exception as e:
        print("Tab key binding ERROR: " + str(e))

    # Summary
    print("")
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print("Total Blueprints: " + str(len(all_bps)))
    print("Phase 0: priority_ans=" + str(priority_cleared) + ", priority_chars=" + str(priority_char_cleared) + ", anim_bps=" + str(anim_bp_cleared) + ", npc_chars=" + str(npc_cleared))
    print("Phase 1: regular=" + str(cleared) + ", keep_vars=" + str(keep_cleared) + ", interfaces=" + str(iface_cleared) + ", data_assets=" + str(data_cleared))
    print("Reparent: " + str(reparent_ok) + " ok, " + str(reparent_fail) + " failed")
    print("Saved: " + str(saved))
    print("Compile: " + str(compile_ok) + " ok, " + str(compile_fail) + " failed")
    print("Validation: " + str(val_ok) + " OK, " + str(val_fail) + " FAIL")
    print("=" * 60)

run()
