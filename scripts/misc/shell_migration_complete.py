# shell_migration_complete.py
# FULL C++ MIGRATION - ALL Blueprints become empty shells
import unreal

# =============================================================================
# MIGRATION_MAP: Blueprints with C++ base classes (clear + reparent)
# ALL 354 logic-bearing items included proactively
# =============================================================================
MIGRATION_MAP = {
    # =========================================================================
    # COMPONENTS (17) - Excludes AnimBP-referenced ones in KEEP_VARS_MAP
    # =========================================================================
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
    # AI COMPONENTS (4) - AC_AI_CombatManager is in KEEP_VARS_MAP
    "AC_AI_Boss": "/Script/SLFConversion.AIBossComponent",
    "AC_AI_BehaviorManager": "/Script/SLFConversion.AIBehaviorManagerComponent",
    "AC_AI_InteractionManager": "/Script/SLFConversion.AIInteractionManagerComponent",
    # NOTE: AC_MovementManager and AC_TargetManager are C++ only - no Blueprint counterparts

    # =========================================================================
    # GAME FRAMEWORK (7) - All framework classes
    # =========================================================================
    "GM_SoulslikeFramework": "/Script/SLFConversion.SLFGameMode",
    "GM_Menu_SoulslikeFramework": "/Script/SLFConversion.SLFMenuGameMode",
    "GI_SoulslikeFramework": "/Script/SLFConversion.SLFGameInstance",
    "PC_SoulslikeFramework": "/Script/SLFConversion.SLFPlayerController",
    "PC_Menu_SoulslikeFramework": "/Script/SLFConversion.SLFMenuPlayerController",
    "GS_SoulslikeFramework": "/Script/SLFConversion.SLFGameState",
    "PS_SoulslikeFramework": "/Script/SLFConversion.SLFPlayerState",

    # =========================================================================
    # AI CONTROLLER (1)
    # =========================================================================
    "AIC_SoulslikeFramework": "/Script/SLFConversion.SLFAIController",

    # =========================================================================
    # CHARACTERS (8) - All character classes
    # =========================================================================
    "B_BaseCharacter": "/Script/SLFConversion.SLFBaseCharacter",
    "B_Soulslike_Character": "/Script/SLFConversion.SLFSoulslikeCharacter",
    "B_Soulslike_Enemy": "/Script/SLFConversion.SLFSoulslikeEnemy",
    "B_Soulslike_Boss": "/Script/SLFConversion.SLFSoulslikeBoss",
    "B_Soulslike_Boss_Malgareth": "/Script/SLFConversion.SLFBossMalgareth",
    "B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "B_Soulslike_Enemy_Guard": "/Script/SLFConversion.SLFEnemyGuard",
    "B_Soulslike_Enemy_Showcase": "/Script/SLFConversion.SLFEnemyShowcase",
    "B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",

    # =========================================================================
    # BASE CLASSES (12) - Core game object bases
    # =========================================================================
    "B_StatusEffect": "/Script/SLFConversion.SLFStatusEffectBase",
    "B_Stat": "/Script/SLFConversion.SLFStatBase",
    "B_Buff": "/Script/SLFConversion.SLFBuffBase",
    "B_Action": "/Script/SLFConversion.SLFActionBase",
    "B_Item": "/Script/SLFConversion.SLFItemBase",
    "B_Interactable": "/Script/SLFConversion.SLFInteractableBase",
    "B_Ladder": "/Script/SLFConversion.SLFLadderBase",
    "B_Door": "/Script/SLFConversion.SLFDoorBase",
    "B_Container": "/Script/SLFConversion.SLFContainer",
    "B_RestingPoint": "/Script/SLFConversion.SLFRestingPointBase",
    "B_BaseProjectile": "/Script/SLFConversion.SLFProjectileBase",
    "B_AbilityEffectBase": "/Script/SLFConversion.SLFAbilityEffectBase",

    # =========================================================================
    # STAT CLASSES (18) - All stat types
    # =========================================================================
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

    # =========================================================================
    # ATTACK POWER CLASSES (6)
    # =========================================================================
    "B_AP_Physical": "/Script/SLFConversion.SLFAPPhysical",
    "B_AP_Fire": "/Script/SLFConversion.SLFAPFire",
    "B_AP_Frost": "/Script/SLFConversion.SLFAPFrost",
    "B_AP_Lightning": "/Script/SLFConversion.SLFAPLightning",
    "B_AP_Magic": "/Script/SLFConversion.SLFAPMagic",
    "B_AP_Holy": "/Script/SLFConversion.SLFAPHoly",

    # =========================================================================
    # DAMAGE NEGATION CLASSES (6)
    # =========================================================================
    "B_DN_Physical": "/Script/SLFConversion.SLFDNPhysical",
    "B_DN_Fire": "/Script/SLFConversion.SLFDNFire",
    "B_DN_Frost": "/Script/SLFConversion.SLFDNFrost",
    "B_DN_Lightning": "/Script/SLFConversion.SLFDNLightning",
    "B_DN_Magic": "/Script/SLFConversion.SLFDNMagic",
    "B_DN_Holy": "/Script/SLFConversion.SLFDNHoly",

    # =========================================================================
    # STATUS EFFECT CLASSES (11)
    # =========================================================================
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

    # =========================================================================
    # BUFF CLASSES (2)
    # =========================================================================
    "B_Buff_AttackPower": "/Script/SLFConversion.SLFBuffAttackPower",

    # =========================================================================
    # ACTION CLASSES (29)
    # =========================================================================
    "B_Action_Backstab": "/Script/SLFConversion.SLFActionBackstab",
    "B_Action_ComboHeavy": "/Script/SLFConversion.SLFActionComboHeavy",
    "B_Action_ComboLight_L": "/Script/SLFConversion.SLFActionComboLightL",
    "B_Action_ComboLight_R": "/Script/SLFConversion.SLFActionComboLightR",
    "B_Action_Crouch": "/Script/SLFConversion.SLFActionCrouch",
    "B_Action_Dodge": "/Script/SLFConversion.SLFActionDodge",
    "B_Action_DrinkFlask_HP": "/Script/SLFConversion.SLFActionDrinkFlaskHP",
    "B_Action_DualWieldAttack": "/Script/SLFConversion.SLFActionDualWieldAttack",
    "B_Action_Execute": "/Script/SLFConversion.SLFActionExecute",
    "B_Action_GuardCancel": "/Script/SLFConversion.SLFActionGuardCancel",
    "B_Action_GuardEnd": "/Script/SLFConversion.SLFActionGuardEnd",
    "B_Action_GuardStart": "/Script/SLFConversion.SLFActionGuardStart",
    "B_Action_Jump": "/Script/SLFConversion.SLFActionJump",
    "B_Action_JumpAttack": "/Script/SLFConversion.SLFActionJumpAttack",
    "B_Action_PickupItemMontage": "/Script/SLFConversion.SLFActionPickupItemMontage",
    "B_Action_ScrollWheel_LeftHand": "/Script/SLFConversion.SLFActionScrollWheelLeftHand",
    "B_Action_ScrollWheel_RightHand": "/Script/SLFConversion.SLFActionScrollWheelRightHand",
    "B_Action_ScrollWheel_Tools": "/Script/SLFConversion.SLFActionScrollWheelTools",
    "B_Action_SprintAttack": "/Script/SLFConversion.SLFActionSprintAttack",
    "B_Action_StartSprinting": "/Script/SLFConversion.SLFActionStartSprinting",
    "B_Action_StopSprinting": "/Script/SLFConversion.SLFActionStopSprinting",
    "B_Action_ThrowProjectile": "/Script/SLFConversion.SLFActionThrowProjectile",
    "B_Action_TwoHandedStance_L": "/Script/SLFConversion.SLFActionTwoHandedStanceL",
    "B_Action_TwoHandedStance_R": "/Script/SLFConversion.SLFActionTwoHandedStanceR",
    "B_Action_UseEquippedTool": "/Script/SLFConversion.SLFActionUseEquippedTool",
    "B_Action_WeaponAbility": "/Script/SLFConversion.SLFActionWeaponAbility",

    # =========================================================================
    # ITEM CLASSES (14)
    # =========================================================================
    "B_Item_Weapon": "/Script/SLFConversion.SLFItemWeapon",
    "B_Item_Weapon_BossMace": "/Script/SLFConversion.SLFItemWeaponBossMace",
    "B_Item_Weapon_Greatsword": "/Script/SLFConversion.SLFItemWeaponGreatsword",
    "B_Item_Weapon_Katana": "/Script/SLFConversion.SLFItemWeaponKatana",
    "B_Item_Weapon_PoisonSword": "/Script/SLFConversion.SLFItemWeaponPoisonSword",
    "B_Item_Weapon_Shield": "/Script/SLFConversion.SLFItemWeaponShield",
    "B_Item_Weapon_SwordExample01": "/Script/SLFConversion.SLFItemWeaponSword01",
    "B_Item_Weapon_SwordExample02": "/Script/SLFConversion.SLFItemWeaponSword02",
    "B_Item_AI_Weapon": "/Script/SLFConversion.SLFItemAIWeapon",
    "B_Item_AI_Weapon_BossMace": "/Script/SLFConversion.SLFItemAIWeaponBossMace",
    "B_Item_AI_Weapon_Greatsword": "/Script/SLFConversion.SLFItemAIWeaponGreatsword",
    "B_Item_AI_Weapon_Sword": "/Script/SLFConversion.SLFItemAIWeaponSword",
    "B_Item_Lantern": "/Script/SLFConversion.SLFItemLantern",
    "B_PickupItem": "/Script/SLFConversion.SLFPickupItem",

    # =========================================================================
    # INTERACTABLE/WORLD ACTOR CLASSES (11)
    # =========================================================================
    "B_Door_Demo": "/Script/SLFConversion.SLFDoorDemo",
    "B_BossDoor": "/Script/SLFConversion.SLFBossDoor",
    "B_Discovery": "/Script/SLFConversion.SLFDiscovery",
    "B_DeathTrigger": "/Script/SLFConversion.SLFDeathTrigger",
    "B_DeathCurrency": "/Script/SLFConversion.SLFDeathCurrency",
    "B_Torch": "/Script/SLFConversion.SLFTorch",
    "B_Destructible": "/Script/SLFConversion.SLFDestructible",
    "B_LocationActor": "/Script/SLFConversion.SLFLocationActor",
    "B_PatrolPath": "/Script/SLFConversion.SLFPatrolPath",
    "B_SequenceActor": "/Script/SLFConversion.SLFSequenceActor",
    "B_SkyManager": "/Script/SLFConversion.SLFSkyManager",

    # =========================================================================
    # PROJECTILE CLASSES (3)
    # =========================================================================
    "B_Projectile_Boss_Fireball": "/Script/SLFConversion.SLFProjectileBossFireball",
    "B_Projectile_ThrowingKnife": "/Script/SLFConversion.SLFProjectileThrowingKnife",
    "B_Chaos_ForceField": "/Script/SLFConversion.SLFChaosForceField",

    # =========================================================================
    # DEMO/SHOWCASE CLASSES (5)
    # =========================================================================
    "B_Demo_TimeSlider": "/Script/SLFConversion.SLFDemoTimeSlider",
    "B_DemoDisplay": "/Script/SLFConversion.SLFDemoDisplay",
    "B_DemoRoom": "/Script/SLFConversion.SLFDemoRoom",
    "B_ShowcaseEnemy_ExitCombat": "/Script/SLFConversion.SLFShowcaseEnemyExitCombat",
    "B_ShowcaseEnemy_StatDisplay": "/Script/SLFConversion.SLFShowcaseEnemyStatDisplay",

    # =========================================================================
    # AI TASKS (13) - Behavior Tree Tasks
    # Uses direct matching class names from AI/ folder
    # =========================================================================
    "BTT_SimpleMoveTo": "/Script/SLFConversion.BTT_SimpleMoveTo",
    "BTT_SwitchState": "/Script/SLFConversion.BTT_SwitchState",
    "BTT_SwitchToPreviousState": "/Script/SLFConversion.BTT_SwitchToPreviousState",
    "BTT_GetRandomPoint": "/Script/SLFConversion.BTT_GetRandomPoint",
    "BTT_ToggleFocus": "/Script/SLFConversion.BTT_ToggleFocus",
    "BTT_TryExecuteAbility": "/Script/SLFConversion.BTT_TryExecuteAbility",
    "BTT_ClearKey": "/Script/SLFConversion.BTT_ClearKey",
    "BTT_SetKey": "/Script/SLFConversion.BTT_SetKey",
    "BTT_SetMovementMode": "/Script/SLFConversion.BTT_SetMovementMode",
    "BTT_GetCurrentLocation": "/Script/SLFConversion.BTT_GetCurrentLocation",
    "BTT_GetRandomPointNearStartPosition": "/Script/SLFConversion.BTT_GetRandomPointNearStartPosition",
    "BTT_GetStrafePointAroundTarget": "/Script/SLFConversion.BTT_GetStrafePointAroundTarget",
    "BTT_PatrolPath": "/Script/SLFConversion.BTT_PatrolPath",

    # =========================================================================
    # AI SERVICES (5) - Behavior Tree Services
    # Uses direct matching class names from AI/ folder
    # =========================================================================
    "BTS_DistanceCheck": "/Script/SLFConversion.BTS_DistanceCheck",
    "BTS_IsTargetDead": "/Script/SLFConversion.BTS_IsTargetDead",
    "BTS_ChaseBounds": "/Script/SLFConversion.BTS_ChaseBounds",
    "BTS_SetMovementModeBasedOnDistance": "/Script/SLFConversion.BTS_SetMovementModeBasedOnDistance",
    "BTS_TryGetAbility": "/Script/SLFConversion.BTS_TryGetAbility",

    # =========================================================================
    # SAVE GAME CLASSES (2)
    # =========================================================================
    "SG_SaveSlots": "/Script/SLFConversion.SLFSaveSlots",
    "SG_SoulslikeFramework": "/Script/SLFConversion.SLFSaveGame",

    # =========================================================================
    # FUNCTION LIBRARY (1)
    # =========================================================================
    "BFL_Helper": "/Script/SLFConversion.SLFHelperLibrary",

    # =========================================================================
    # MACRO LIBRARIES (2) - Will be converted to inline C++ or helper functions
    # =========================================================================
    "BML_HelperMacros": "/Script/SLFConversion.SLFHelperMacros",
    "BML_StructConversion": "/Script/SLFConversion.SLFStructConversion",

    # =========================================================================
    # ANIMATION NOTIFIES (13) - Single-frame notifies
    # Uses direct matching class names from AnimNotifies/ folder
    # =========================================================================
    "AN_AdjustStat": "/Script/SLFConversion.AN_AdjustStat",
    "AN_AI_SpawnProjectile": "/Script/SLFConversion.AN_AI_SpawnProjectile",
    "AN_AoeDamage": "/Script/SLFConversion.AN_AoeDamage",
    "AN_CameraShake": "/Script/SLFConversion.AN_CameraShake",
    "AN_FootstepTrace": "/Script/SLFConversion.AN_FootstepTrace",
    "AN_InterruptMontage": "/Script/SLFConversion.AN_InterruptMontage",
    "AN_LaunchField": "/Script/SLFConversion.AN_LaunchField",
    "AN_PlayCameraSequence": "/Script/SLFConversion.AN_PlayCameraSequence",
    "AN_SetAiState": "/Script/SLFConversion.AN_SetAiState",
    "AN_SetMovementMode": "/Script/SLFConversion.AN_SetMovementMode",
    "AN_SpawnProjectile": "/Script/SLFConversion.AN_SpawnProjectile",
    "AN_TryGuard": "/Script/SLFConversion.AN_TryGuard",
    "AN_WorldCameraShake": "/Script/SLFConversion.AN_WorldCameraShake",

    # =========================================================================
    # ANIMATION NOTIFY STATES (12) - Multi-frame notifies
    # Uses direct matching class names from AnimNotifies/ folder
    # =========================================================================
    "ANS_AI_FistTrace": "/Script/SLFConversion.ANS_AI_FistTrace",
    "ANS_AI_RotateTowardsTarget": "/Script/SLFConversion.ANS_AI_RotateTowardsTarget",
    "ANS_AI_Trail": "/Script/SLFConversion.ANS_AI_Trail",
    "ANS_AI_WeaponTrace": "/Script/SLFConversion.ANS_AI_WeaponTrace",
    "ANS_FistTrace": "/Script/SLFConversion.ANS_FistTrace",
    "ANS_HyperArmor": "/Script/SLFConversion.ANS_HyperArmor",
    "ANS_InputBuffer": "/Script/SLFConversion.ANS_InputBuffer",
    "ANS_InvincibilityFrame": "/Script/SLFConversion.ANS_InvincibilityFrame",
    "ANS_RegisterAttackSequence": "/Script/SLFConversion.ANS_RegisterAttackSequence",
    "ANS_ToggleChaosField": "/Script/SLFConversion.ANS_ToggleChaosField",
    "ANS_Trail": "/Script/SLFConversion.ANS_Trail",
    "ANS_WeaponTrace": "/Script/SLFConversion.ANS_WeaponTrace",
}

# =============================================================================
# KEEP_VARS_MAP: Components referenced by AnimBPs - must keep BP variables
# These use ClearGraphsKeepVariables instead of ClearAllBlueprintLogic
# =============================================================================
KEEP_VARS_MAP = {
    "AC_CombatManager": "/Script/SLFConversion.CombatManagerComponent",
    "AC_ActionManager": "/Script/SLFConversion.ActionManagerComponent",
    "AC_EquipmentManager": "/Script/SLFConversion.EquipmentManagerComponent",
    "AC_InteractionManager": "/Script/SLFConversion.InteractionManagerComponent",
    "AC_AI_CombatManager": "/Script/SLFConversion.AICombatManagerComponent",
}

# =============================================================================
# INTERFACE_MAP: Blueprint Interfaces -> C++ UInterface
# =============================================================================
INTERFACE_MAP = {
    "BPI_AIC": "/Script/SLFConversion.SLFInterface_AIC",
    "BPI_BossDoor": "/Script/SLFConversion.SLFInterface_BossDoor",
    "BPI_Controller": "/Script/SLFConversion.SLFInterface_Controller",
    "BPI_DestructibleHelper": "/Script/SLFConversion.SLFInterface_DestructibleHelper",
    "BPI_Enemy": "/Script/SLFConversion.SLFInterface_Enemy",
    "BPI_EnemyHealthbar": "/Script/SLFConversion.SLFInterface_EnemyHealthbar",
    "BPI_Executable": "/Script/SLFConversion.SLFInterface_Executable",
    "BPI_ExecutionIndicator": "/Script/SLFConversion.SLFInterface_ExecutionIndicator",
    "BPI_GameInstance": "/Script/SLFConversion.SLFInterface_GameInstance",
    "BPI_GenericCharacter": "/Script/SLFConversion.SLFInterface_GenericCharacter",
    "BPI_Interactable": "/Script/SLFConversion.SLFInterface_Interactable",
    "BPI_Item": "/Script/SLFConversion.SLFInterface_Item",
    "BPI_MainMenu": "/Script/SLFConversion.SLFInterface_MainMenu",
    "BPI_NPC": "/Script/SLFConversion.SLFInterface_NPC",
    "BPI_Player": "/Script/SLFConversion.SLFInterface_Player",
    "BPI_Projectile": "/Script/SLFConversion.SLFInterface_Projectile",
    "BPI_RestingPoint": "/Script/SLFConversion.SLFInterface_RestingPoint",
    "BPI_StatEntry": "/Script/SLFConversion.SLFInterface_StatEntry",
}

# =============================================================================
# ANIMBP_MAP: Animation Blueprints with C++ AnimInstance base classes
# Uses direct matching class names from Animation/ folder
# =============================================================================
ANIMBP_MAP = {
    "ABP_SoulslikeBossNew": "/Script/SLFConversion.ABP_SoulslikeBossNew",
    "ABP_SoulslikeEnemy": "/Script/SLFConversion.ABP_SoulslikeEnemy",
    "ABP_SoulslikeNPC": "/Script/SLFConversion.ABP_SoulslikeNPC",
    "ABP_SoulslikeCharacter_Additive": "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive",
    "ABP_Manny_PostProcess": "/Script/SLFConversion.ABP_Manny_PostProcess",
    "ABP_Quinn_PostProcess": "/Script/SLFConversion.ABP_Quinn_PostProcess",
}

ANIMBP_PATHS = {
    "ABP_SoulslikeBossNew": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "ABP_SoulslikeEnemy": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "ABP_SoulslikeNPC": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "ABP_SoulslikeCharacter_Additive": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "ABP_Manny_PostProcess": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_Manny_PostProcess",
    "ABP_Quinn_PostProcess": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_Quinn_PostProcess",
}

# Asset path mappings for non-standard locations
PATH_OVERRIDES = {
    # Framework
    "GM_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework",
    "GM_Menu_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/MainMenu/GM_Menu_SoulslikeFramework",
    "GI_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework",
    "PC_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework",
    "PC_Menu_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/MainMenu/PC_Menu_SoulslikeFramework",
    "GS_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/GS_SoulslikeFramework",
    "PS_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/PS_SoulslikeFramework",
    "AIC_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework",
    # Characters
    "B_BaseCharacter": "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
    "B_Soulslike_Character": "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "B_Soulslike_Enemy": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "B_Soulslike_Boss": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "B_Soulslike_Boss_Malgareth": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    "B_Soulslike_NPC": "/Game/SoulslikeFramework/Blueprints/_Characters/NPCs/B_Soulslike_NPC",
    "B_Soulslike_Enemy_Guard": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "B_Soulslike_Enemy_Showcase": "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
    "B_Soulslike_NPC_ShowcaseGuide": "/Game/SoulslikeFramework/Blueprints/_Characters/NPCs/B_Soulslike_NPC_ShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Game/SoulslikeFramework/Blueprints/_Characters/NPCs/B_Soulslike_NPC_ShowcaseVendor",
    # AI Tasks
    "BTT_SimpleMoveTo": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo",
    "BTT_SwitchState": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState",
    "BTT_SwitchToPreviousState": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchToPreviousState",
    "BTT_GetRandomPoint": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint",
    "BTT_ToggleFocus": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ToggleFocus",
    "BTT_TryExecuteAbility": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility",
    "BTT_ClearKey": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ClearKey",
    "BTT_SetKey": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetKey",
    "BTT_SetMovementMode": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SetMovementMode",
    "BTT_GetCurrentLocation": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetCurrentLocation",
    "BTT_GetRandomPointNearStartPosition": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPointNearStartPosition",
    "BTT_GetStrafePointAroundTarget": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePointAroundTarget",
    "BTT_PatrolPath": "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_PatrolPath",
    # AI Services
    "BTS_DistanceCheck": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_DistanceCheck",
    "BTS_IsTargetDead": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_IsTargetDead",
    "BTS_ChaseBounds": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_ChaseBounds",
    "BTS_SetMovementModeBasedOnDistance": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_SetMovementModeBasedOnDistance",
    "BTS_TryGetAbility": "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility",
    # Base Classes
    "B_StatusEffect": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "B_Stat": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Stat",
    "B_Buff": "/Game/SoulslikeFramework/Data/Buffs/BuffClasses/B_Buff",
    "B_Action": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action",
    "B_Item": "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Item",
    "B_Interactable": "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Interactable",
    "B_Ladder": "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_Ladder",
    "B_Door": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
    "B_Container": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    "B_RestingPoint": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    "B_BaseProjectile": "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_BaseProjectile",
    "B_AbilityEffectBase": "/Game/SoulslikeFramework/Blueprints/_BaseClasses/B_AbilityEffectBase",
    # Stats
    "B_HP": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_HP",
    "B_FP": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_FP",
    "B_Stamina": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Stamina",
    "B_Poise": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Poise",
    "B_Vigor": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Vigor",
    "B_Mind": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Mind",
    "B_Endurance": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Endurance",
    "B_Strength": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Strength",
    "B_Dexterity": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Dexterity",
    "B_Intelligence": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Intelligence",
    "B_Faith": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Faith",
    "B_Arcane": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Arcane",
    "B_Weight": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Weight",
    "B_Stance": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Stance",
    "B_IncantationPower": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_IncantationPower",
    "B_Resistance_Focus": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Resistance_Focus",
    "B_Resistance_Immunity": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Resistance_Immunity",
    "B_Resistance_Robustness": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Resistance_Robustness",
    "B_Resistance_Vitality": "/Game/SoulslikeFramework/Data/Stats/StatClasses/B_Resistance_Vitality",
    # Attack Power
    "B_AP_Physical": "/Game/SoulslikeFramework/Data/Stats/StatClasses/AttackPower/B_AP_Physical",
    "B_AP_Fire": "/Game/SoulslikeFramework/Data/Stats/StatClasses/AttackPower/B_AP_Fire",
    "B_AP_Frost": "/Game/SoulslikeFramework/Data/Stats/StatClasses/AttackPower/B_AP_Frost",
    "B_AP_Lightning": "/Game/SoulslikeFramework/Data/Stats/StatClasses/AttackPower/B_AP_Lightning",
    "B_AP_Magic": "/Game/SoulslikeFramework/Data/Stats/StatClasses/AttackPower/B_AP_Magic",
    "B_AP_Holy": "/Game/SoulslikeFramework/Data/Stats/StatClasses/AttackPower/B_AP_Holy",
    # Damage Negation
    "B_DN_Physical": "/Game/SoulslikeFramework/Data/Stats/StatClasses/DamageNegation/B_DN_Physical",
    "B_DN_Fire": "/Game/SoulslikeFramework/Data/Stats/StatClasses/DamageNegation/B_DN_Fire",
    "B_DN_Frost": "/Game/SoulslikeFramework/Data/Stats/StatClasses/DamageNegation/B_DN_Frost",
    "B_DN_Lightning": "/Game/SoulslikeFramework/Data/Stats/StatClasses/DamageNegation/B_DN_Lightning",
    "B_DN_Magic": "/Game/SoulslikeFramework/Data/Stats/StatClasses/DamageNegation/B_DN_Magic",
    "B_DN_Holy": "/Game/SoulslikeFramework/Data/Stats/StatClasses/DamageNegation/B_DN_Holy",
    # Status Effects
    "B_StatusEffect_Bleed": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Bleed",
    "B_StatusEffect_Burn": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Burn",
    "B_StatusEffect_Corruption": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Corruption",
    "B_StatusEffect_Frostbite": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Frostbite",
    "B_StatusEffect_Madness": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Madness",
    "B_StatusEffect_Plague": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Plague",
    "B_StatusEffect_Poison": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Poison",
    "B_StatusEffectArea": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_StatusEffectArea",
    "B_StatusEffectBuildup": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffectBuildup",
    "B_StatusEffectOneShot": "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffectOneShot",
    # Buffs
    "B_Buff_AttackPower": "/Game/SoulslikeFramework/Data/Buffs/BuffClasses/B_Buff_AttackPower",
    # Actions
    "B_Action_Backstab": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_Backstab",
    "B_Action_ComboHeavy": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ComboHeavy",
    "B_Action_ComboLight_L": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ComboLight_L",
    "B_Action_ComboLight_R": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ComboLight_R",
    "B_Action_Crouch": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_Crouch",
    "B_Action_Dodge": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_Dodge",
    "B_Action_DrinkFlask_HP": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_DrinkFlask_HP",
    "B_Action_DualWieldAttack": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_DualWieldAttack",
    "B_Action_Execute": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_Execute",
    "B_Action_GuardCancel": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_GuardCancel",
    "B_Action_GuardEnd": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_GuardEnd",
    "B_Action_GuardStart": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_GuardStart",
    "B_Action_Jump": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_Jump",
    "B_Action_JumpAttack": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_JumpAttack",
    "B_Action_PickupItemMontage": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_PickupItemMontage",
    "B_Action_ScrollWheel_LeftHand": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ScrollWheel_LeftHand",
    "B_Action_ScrollWheel_RightHand": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ScrollWheel_RightHand",
    "B_Action_ScrollWheel_Tools": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ScrollWheel_Tools",
    "B_Action_SprintAttack": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_SprintAttack",
    "B_Action_StartSprinting": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_StartSprinting",
    "B_Action_StopSprinting": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_StopSprinting",
    "B_Action_ThrowProjectile": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_ThrowProjectile",
    "B_Action_TwoHandedStance_L": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_TwoHandedStance_L",
    "B_Action_TwoHandedStance_R": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_TwoHandedStance_R",
    "B_Action_UseEquippedTool": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_UseEquippedTool",
    "B_Action_WeaponAbility": "/Game/SoulslikeFramework/Data/Actions/ActionClasses/B_Action_WeaponAbility",
    # Items
    "B_Item_Weapon": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/B_Item_Weapon",
    "B_Item_Weapon_BossMace": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_BossMace",
    "B_Item_Weapon_Greatsword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_Greatsword",
    "B_Item_Weapon_Katana": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_Katana",
    "B_Item_Weapon_PoisonSword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_PoisonSword",
    "B_Item_Weapon_Shield": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_Shield",
    "B_Item_Weapon_SwordExample01": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_SwordExample01",
    "B_Item_Weapon_SwordExample02": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Weapons/B_Item_Weapon_SwordExample02",
    "B_Item_AI_Weapon": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/AI/B_Item_AI_Weapon",
    "B_Item_AI_Weapon_BossMace": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/AI/Weapons/B_Item_AI_Weapon_BossMace",
    "B_Item_AI_Weapon_Greatsword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/AI/Weapons/B_Item_AI_Weapon_Greatsword",
    "B_Item_AI_Weapon_Sword": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/AI/Weapons/B_Item_AI_Weapon_Sword",
    "B_Item_Lantern": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/Tools/B_Item_Lantern",
    "B_PickupItem": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/B_PickupItem",
    # World Actors
    "B_Door_Demo": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo",
    "B_BossDoor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
    "B_Discovery": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Discovery",
    "B_DeathTrigger": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_DeathTrigger",
    "B_DeathCurrency": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_DeathCurrency",
    "B_Torch": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Torch",
    "B_Destructible": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Destructible",
    "B_LocationActor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_LocationActor",
    "B_PatrolPath": "/Game/SoulslikeFramework/Blueprints/_AI/B_PatrolPath",
    "B_SequenceActor": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor",
    "B_SkyManager": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SkyManager",
    # Projectiles
    "B_Projectile_Boss_Fireball": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_Projectile_Boss_Fireball",
    "B_Projectile_ThrowingKnife": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_Projectile_ThrowingKnife",
    "B_Chaos_ForceField": "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
    # Demo
    "B_Demo_TimeSlider": "/Game/SoulslikeFramework/Demo/Blueprints/B_Demo_TimeSlider",
    "B_DemoDisplay": "/Game/SoulslikeFramework/Demo/Blueprints/B_DemoDisplay",
    "B_DemoRoom": "/Game/SoulslikeFramework/Demo/Blueprints/B_DemoRoom",
    "B_ShowcaseEnemy_ExitCombat": "/Game/SoulslikeFramework/Demo/Blueprints/B_ShowcaseEnemy_ExitCombat",
    "B_ShowcaseEnemy_StatDisplay": "/Game/SoulslikeFramework/Demo/Blueprints/B_ShowcaseEnemy_StatDisplay",
    # Save Games
    "SG_SaveSlots": "/Game/SoulslikeFramework/Blueprints/Global/SaveGames/SG_SaveSlots",
    "SG_SoulslikeFramework": "/Game/SoulslikeFramework/Blueprints/Global/SaveGames/SG_SoulslikeFramework",
    # Libraries
    "BFL_Helper": "/Game/SoulslikeFramework/Blueprints/Global/BFL_Helper",
    "BML_HelperMacros": "/Game/SoulslikeFramework/Blueprints/Global/BML_HelperMacros",
    "BML_StructConversion": "/Game/SoulslikeFramework/Blueprints/Global/BML_StructConversion",
    # Animation Notifies
    "AN_AdjustStat": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_AdjustStat",
    "AN_AI_SpawnProjectile": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_AI_SpawnProjectile",
    "AN_AoeDamage": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_AoeDamage",
    "AN_CameraShake": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_CameraShake",
    "AN_FootstepTrace": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_FootstepTrace",
    "AN_InterruptMontage": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_InterruptMontage",
    "AN_LaunchField": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_LaunchField",
    "AN_PlayCameraSequence": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_PlayCameraSequence",
    "AN_SetAiState": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_SetAiState",
    "AN_SetMovementMode": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_SetMovementMode",
    "AN_SpawnProjectile": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_SpawnProjectile",
    "AN_TryGuard": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_TryGuard",
    "AN_WorldCameraShake": "/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/AN_WorldCameraShake",
    # Animation Notify States
    "ANS_AI_FistTrace": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_AI_FistTrace",
    "ANS_AI_RotateTowardsTarget": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_AI_RotateTowardsTarget",
    "ANS_AI_Trail": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_AI_Trail",
    "ANS_AI_WeaponTrace": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_AI_WeaponTrace",
    "ANS_FistTrace": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_FistTrace",
    "ANS_HyperArmor": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_HyperArmor",
    "ANS_InputBuffer": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_InputBuffer",
    "ANS_InvincibilityFrame": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_InvincibilityFrame",
    "ANS_RegisterAttackSequence": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_RegisterAttackSequence",
    "ANS_ToggleChaosField": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_ToggleChaosField",
    "ANS_Trail": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_Trail",
    "ANS_WeaponTrace": "/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/ANS_WeaponTrace",
    # Interfaces
    "BPI_AIC": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_AIC",
    "BPI_BossDoor": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_BossDoor",
    "BPI_Controller": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Controller",
    "BPI_DestructibleHelper": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_DestructibleHelper",
    "BPI_Enemy": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Enemy",
    "BPI_EnemyHealthbar": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_EnemyHealthbar",
    "BPI_Executable": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Executable",
    "BPI_ExecutionIndicator": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_ExecutionIndicator",
    "BPI_GameInstance": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_GameInstance",
    "BPI_GenericCharacter": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_GenericCharacter",
    "BPI_Interactable": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Interactable",
    "BPI_Item": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Item",
    "BPI_MainMenu": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_MainMenu",
    "BPI_NPC": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_NPC",
    "BPI_Player": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Player",
    "BPI_Projectile": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Projectile",
    "BPI_RestingPoint": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_RestingPoint",
    "BPI_StatEntry": "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_StatEntry",
}

def find_bp(name):
    if name in PATH_OVERRIDES:
        a = unreal.EditorAssetLibrary.load_asset(PATH_OVERRIDES[name])
        if a: return a
    paths = [
        f"/Game/SoulslikeFramework/Blueprints/Components/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_BaseClasses/{name}",
        f"/Game/SoulslikeFramework/Blueprints/Global/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_Characters/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_Characters/NPCs/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_AI/Tasks/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_AI/Services/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_AI/{name}",
        f"/Game/SoulslikeFramework/Data/Stats/StatClasses/{name}",
        f"/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/{name}",
        f"/Game/SoulslikeFramework/Data/Actions/ActionClasses/{name}",
        f"/Game/SoulslikeFramework/Data/Buffs/BuffClasses/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_WorldActors/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_WorldActors/Items/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_Animation/Notifies/{name}",
        f"/Game/SoulslikeFramework/Blueprints/_Animation/NotifyStates/{name}",
        f"/Game/SoulslikeFramework/Blueprints/Interfaces/{name}",
    ]
    for p in paths:
        a = unreal.EditorAssetLibrary.load_asset(p)
        if a: return a
    return None

# =============================================================================
# DEFERRED COMPILATION APPROACH
# Clear ALL Blueprints first (no compile), then reparent, then compile at end
# =============================================================================

def clear_bp(name, keep_vars=False):
    """Clear Blueprint logic WITHOUT compiling"""
    bp = find_bp(name)
    if not bp:
        return None
    if keep_vars:
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
    else:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
    return bp

def reparent_bp(bp, cpp):
    """Reparent Blueprint to C++ class"""
    if not bp:
        return False
    return unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp)

def compile_and_save_bp(bp):
    """Compile and save a Blueprint"""
    if not bp:
        return False
    return unreal.SLFAutomationLibrary.compile_and_save(bp)

# Legacy functions for compatibility (but now they return the BP for deferred compile)
def migrate(name, cpp):
    """Regular Blueprint migration: clear first, then reparent"""
    print(f"Processing: {name}")
    bp = find_bp(name)
    if not bp:
        print(f"  SKIP: Not found")
        return False
    unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp)
    if not ok:
        print(f"  ERROR: Reparent failed (C++ class may not exist: {cpp})")
        return False
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    print(f"  OK")
    return True

def migrate_keep_vars(name, cpp):
    """Migration for components referenced by AnimBPs: keep variables, clear graphs only"""
    print(f"Processing (keep vars): {name}")
    bp = find_bp(name)
    if not bp:
        print(f"  SKIP: Not found")
        return False
    unreal.SLFAutomationLibrary.clear_graphs_keep_variables(bp)
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp)
    if not ok:
        print(f"  ERROR: Reparent failed")
        return False
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    print(f"  OK (variables kept for AnimBP compatibility)")
    return True

def migrate_interface(name, cpp):
    """Interface migration: clear interface functions, link to C++ interface"""
    print(f"Processing Interface: {name}")
    bp = find_bp(name)
    if not bp:
        print(f"  SKIP: Not found")
        return False
    # Clear all interface function implementations
    unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
    # Note: Interfaces don't reparent the same way - they implement C++ interfaces
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    print(f"  OK (cleared)")
    return True

def migrate_animbp(name, cpp):
    """AnimBlueprint migration: Clear EventGraph, remove component refs, then reparent"""
    print(f"Processing AnimBP: {name}")
    path = ANIMBP_PATHS.get(name)
    if not path:
        print(f"  SKIP: No path defined")
        return False
    bp = unreal.EditorAssetLibrary.load_asset(path)
    if not bp:
        print(f"  SKIP: Not found at {path}")
        return False

    # Step 1: Clear EventGraph FIRST
    print(f"  Clearing EventGraph...")
    unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)

    # Step 2: Remove component reference variables
    component_ref_vars = [
        "AC_AI_CombatManager", "AC AI Combat Manager",
        "SoulslikeEnemy", "SoulslikeBoss", "SoulslikeCharacter",
        "SoulslikeNpc", "EquipmentComponent", "MovementComponent",
    ]
    removed = 0
    for var_name in component_ref_vars:
        if unreal.SLFAutomationLibrary.remove_variable(bp, var_name):
            removed += 1
    if removed > 0:
        print(f"  Removed {removed} component reference variables")

    # Step 3: Reparent to C++ AnimInstance
    print(f"  Reparenting to {cpp}...")
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp)
    if not ok:
        print(f"  ERROR: Reparent failed")
        return False

    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    print(f"  OK (AnimBP migrated)")
    return True

# =============================================================================
# WIDGET_PATHS: Directories to scan for Widget Blueprints
# =============================================================================
WIDGET_PATHS = [
    "/Game/SoulslikeFramework/Widgets",
]

def migrate_widget(asset_path, widget_name):
    """Migrate Widget Blueprint: clear graphs, reparent to C++"""
    cpp_class = f"/Script/SLFConversion.{widget_name}"
    try:
        bp = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not bp:
            return False
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_class)
        if ok:
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            return True
        else:
            unreal.log_warning(f"  Widget {widget_name}: Reparent failed (C++ class may not exist)")
            return False
    except Exception as e:
        unreal.log_warning(f"  Widget {widget_name}: Exception - {e}")
        return False

def discover_widgets():
    """Discover all Widget Blueprints in the widget directories"""
    widgets = []
    seen_names = set()
    for widget_dir in WIDGET_PATHS:
        try:
            # Get list of assets - use list() to ensure we have a proper Python list
            assets = list(unreal.EditorAssetLibrary.list_assets(widget_dir, recursive=True, include_folder=False))
            for asset_path in assets:
                # Ensure asset_path is a string
                asset_path_str = str(asset_path)
                if "." in asset_path_str:
                    widget_name = asset_path_str.split(".")[-1]
                    if widget_name.endswith("_C") or widget_name in seen_names:
                        continue
                    seen_names.add(widget_name)
                    try:
                        asset = unreal.EditorAssetLibrary.load_asset(asset_path_str)
                        if asset:
                            class_name = str(asset.get_class().get_name()) if hasattr(asset, 'get_class') else ""
                            if 'WidgetBlueprint' in class_name:
                                widgets.append((asset_path_str, widget_name))
                    except:
                        pass
        except Exception as e:
            unreal.log_warning(f"Error scanning {widget_dir}: {e}")
    return widgets

def run():
    print("=" * 60)
    print("FULL C++ MIGRATION - DEFERRED COMPILATION")
    print("=" * 60)
    print(f"MIGRATION_MAP: {len(MIGRATION_MAP)} items")
    print(f"KEEP_VARS_MAP: {len(KEEP_VARS_MAP)} items")
    print(f"INTERFACE_MAP: {len(INTERFACE_MAP)} items")
    print(f"ANIMBP_MAP: {len(ANIMBP_MAP)} items")
    print("=" * 60)

    # Collect all Blueprints for deferred compilation
    all_bps = []  # List of (bp, name, cpp_class) tuples

    # =========================================================================
    # PHASE 1: CLEAR ALL BLUEPRINTS (NO COMPILE)
    # =========================================================================
    unreal.log_warning("=== Phase 1A: Clear AnimBP EventGraphs (no compile) ===")
    animbp_cleared = 0
    for name, cpp in ANIMBP_MAP.items():
        path = ANIMBP_PATHS.get(name)
        if path:
            try:
                bp = unreal.EditorAssetLibrary.load_asset(path)
                if bp:
                    # Clear EventGraph only for AnimBPs (no compile)
                    cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
                    # Remove component reference variables
                    for var_name in ["AC_AI_CombatManager", "AC AI Combat Manager",
                                     "SoulslikeEnemy", "SoulslikeBoss", "SoulslikeCharacter",
                                     "SoulslikeNpc", "EquipmentComponent", "MovementComponent"]:
                        unreal.SLFAutomationLibrary.remove_variable(bp, var_name)
                    all_bps.append((bp, name, cpp, "animbp"))
                    animbp_cleared += 1
                    print(f"  {name}: cleared {cleared} nodes")
            except Exception as e:
                unreal.log_error(f"  {name}: Exception - {e}")

    unreal.log_warning("=== Phase 1B: Clear Regular Blueprints (no compile) ===")
    regular_cleared = 0
    for name, cpp in MIGRATION_MAP.items():
        bp = find_bp(name)
        if bp:
            unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
            all_bps.append((bp, name, cpp, "regular"))
            regular_cleared += 1
            print(f"  {name}: cleared")
        else:
            print(f"  {name}: SKIP - not found")

    unreal.log_warning("=== Phase 1C: Clear KEEP_VARS Blueprints (no compile) ===")
    keep_vars_cleared = 0
    for name, cpp in KEEP_VARS_MAP.items():
        bp = find_bp(name)
        if bp:
            unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            all_bps.append((bp, name, cpp, "keep_vars"))
            keep_vars_cleared += 1
            print(f"  {name}: cleared (keeping vars)")
        else:
            print(f"  {name}: SKIP - not found")

    unreal.log_warning("=== Phase 1D: Clear Interface Blueprints (no compile) ===")
    interface_cleared = 0
    for name, cpp in INTERFACE_MAP.items():
        bp = find_bp(name)
        if bp:
            unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
            all_bps.append((bp, name, cpp, "interface"))
            interface_cleared += 1
            print(f"  {name}: cleared")
        else:
            print(f"  {name}: SKIP - not found")

    # SKIP widgets for now - they don't have matching C++ classes
    # unreal.log_warning("=== Phase 1E: Clear Widget Blueprints (no compile) ===")
    widget_cleared = 0
    print(f"  Widgets SKIPPED (no C++ classes defined yet)")

    unreal.log_warning(f"Phase 1 Complete: {len(all_bps)} Blueprints cleared (no compilation)")

    # =========================================================================
    # PHASE 2: REPARENT ALL BLUEPRINTS
    # =========================================================================
    unreal.log_warning("=== Phase 2: Reparent ALL Blueprints ===")
    reparent_success = 0
    reparent_fail = 0
    for bp, name, cpp, bp_type in all_bps:
        if bp_type == "interface":
            # Interfaces don't reparent - they're already done
            reparent_success += 1
            continue
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp)
        if ok:
            reparent_success += 1
            print(f"  {name}: reparented to {cpp.split('.')[-1]}")
        else:
            reparent_fail += 1
            print(f"  {name}: FAILED to reparent to {cpp}")

    unreal.log_warning(f"Phase 2 Complete: {reparent_success} succeeded, {reparent_fail} failed")

    # =========================================================================
    # PHASE 3: SAVE ALL BLUEPRINTS
    # =========================================================================
    unreal.log_warning("=== Phase 3: Save ALL Blueprints ===")
    save_count = 0
    for bp, name, cpp, bp_type in all_bps:
        try:
            unreal.EditorAssetLibrary.save_loaded_asset(bp)
            save_count += 1
        except Exception as e:
            print(f"  {name}: Save failed - {e}")

    unreal.log_warning(f"Phase 3 Complete: {save_count} Blueprints saved")

    # =========================================================================
    # PHASE 4: COMPILE ALL BLUEPRINTS
    # =========================================================================
    unreal.log_warning("=== Phase 4: Compile ALL Blueprints ===")
    compile_success = 0
    compile_fail = 0
    for bp, name, cpp, bp_type in all_bps:
        try:
            ok = unreal.SLFAutomationLibrary.compile_and_save(bp)
            if ok:
                compile_success += 1
            else:
                compile_fail += 1
                print(f"  {name}: compile errors")
        except Exception as e:
            compile_fail += 1
            print(f"  {name}: compile exception - {e}")

    # =========================================================================
    # SUMMARY
    # =========================================================================
    unreal.log_warning("=" * 60)
    unreal.log_warning("MIGRATION COMPLETE (DEFERRED COMPILATION):")
    unreal.log_warning(f"  Phase 1 (Clear): {len(all_bps)} Blueprints cleared")
    unreal.log_warning(f"    - AnimBPs: {animbp_cleared}")
    unreal.log_warning(f"    - Regular: {regular_cleared}")
    unreal.log_warning(f"    - KeepVars: {keep_vars_cleared}")
    unreal.log_warning(f"    - Interfaces: {interface_cleared}")
    unreal.log_warning(f"    - Widgets: {widget_cleared}")
    unreal.log_warning(f"  Phase 2 (Reparent): {reparent_success} succeeded, {reparent_fail} failed")
    unreal.log_warning(f"  Phase 3 (Save): {save_count} saved")
    unreal.log_warning(f"  Phase 4 (Compile): {compile_success} succeeded, {compile_fail} failed")
    unreal.log_warning("=" * 60)

# Auto-execute when run as script
run()
