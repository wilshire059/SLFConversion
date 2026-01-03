"""
═══════════════════════════════════════════════════════════════════════════════
CORE SYSTEMS TESTS
═══════════════════════════════════════════════════════════════════════════════

Tests for:
- Class loading (C++ and Blueprint)
- Game framework (GameMode, PlayerController, etc.)
- Character spawning
- Component availability
"""

import unreal
from slf_test_framework import *


# ═══════════════════════════════════════════════════════════════════════════════
# C++ CLASS LOADING TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("C++ Classes", "Game Mode Class")
def test_game_mode_class():
    cls = load_class_safe("/Script/SLFConversion.GM_SoulslikeFramework")
    if cls:
        # Check parent class
        try:
            cdo = unreal.get_default_object(cls)
            parent_name = cls.get_class().get_name() if cls else "Unknown"
            return True, f"GM_SoulslikeFramework loaded", {"class": str(cls), "cdo": str(cdo)}
        except Exception as e:
            return True, f"Class loaded (CDO issue: {e})", {"class": str(cls)}
    return False, "GM_SoulslikeFramework not found", {}


@test("C++ Classes", "Player Controller Class")
def test_player_controller_class():
    cls = load_class_safe("/Script/SLFConversion.PC_SoulslikeFramework")
    if cls:
        return True, "PC_SoulslikeFramework loaded", {"class": str(cls)}
    return False, "PC_SoulslikeFramework not found", {}


@test("C++ Classes", "Game State Class")
def test_game_state_class():
    cls = load_class_safe("/Script/SLFConversion.GS_SoulslikeFramework")
    if cls:
        return True, "GS_SoulslikeFramework loaded", {"class": str(cls)}
    return False, "GS_SoulslikeFramework not found", {}


@test("C++ Classes", "Player State Class")
def test_player_state_class():
    cls = load_class_safe("/Script/SLFConversion.PS_SoulslikeFramework")
    if cls:
        return True, "PS_SoulslikeFramework loaded", {"class": str(cls)}
    return False, "PS_SoulslikeFramework not found", {}


@test("C++ Classes", "Player Character Class")
def test_player_character_class():
    cls = load_class_safe("/Script/SLFConversion.B_Soulslike_Character")
    if cls:
        return True, "B_Soulslike_Character loaded", {"class": str(cls)}
    return False, "B_Soulslike_Character not found", {}


@test("C++ Classes", "Enemy Character Class")
def test_enemy_character_class():
    cls = load_class_safe("/Script/SLFConversion.B_Soulslike_Enemy")
    if cls:
        return True, "B_Soulslike_Enemy loaded", {"class": str(cls)}
    return False, "B_Soulslike_Enemy not found", {}


@test("C++ Classes", "Boss Character Class")
def test_boss_character_class():
    cls = load_class_safe("/Script/SLFConversion.B_Soulslike_Boss")
    if cls:
        return True, "B_Soulslike_Boss loaded", {"class": str(cls)}
    return False, "B_Soulslike_Boss not found", {}


@test("C++ Classes", "NPC Character Class")
def test_npc_character_class():
    cls = load_class_safe("/Script/SLFConversion.B_Soulslike_NPC")
    if cls:
        return True, "B_Soulslike_NPC loaded", {"class": str(cls)}
    return False, "B_Soulslike_NPC not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# COMPONENT CLASS TESTS
# ═══════════════════════════════════════════════════════════════════════════════

PLAYER_COMPONENTS = [
    ("AC_StatManager", "Stat/Attribute Manager"),
    ("AC_ActionManager", "Action Manager"),
    ("AC_BuffManager", "Buff Manager"),
    ("AC_CombatManager", "Combat Manager"),
    ("AC_EquipmentManager", "Equipment Manager"),
    ("AC_InventoryManager", "Inventory Manager"),
    ("AC_InputBuffer", "Input Buffer"),
    ("AC_InteractionManager", "Interaction Manager"),
    ("AC_StatusEffectManager", "Status Effect Manager"),
    ("AC_CollisionManager", "Weapon Collision Manager"),
    ("AC_LadderManager", "Ladder Manager"),
    ("AC_DebugCentral", "Debug Central"),
]

AI_COMPONENTS = [
    ("AC_AI_BehaviorManager", "AI Behavior Manager"),
    ("AC_AI_CombatManager", "AI Combat Manager"),
    ("AC_AI_SenseManager", "AI Sense Manager"),
    ("AC_AI_InteractionManager", "AI Interaction Manager"),
    ("AC_LootDropManager", "Loot Drop Manager"),
]

PC_COMPONENTS = [
    ("AC_SaveLoadManager", "Save/Load Manager"),
    ("AC_ProgressManager", "Progress Manager"),
    ("AC_RadarManager", "Radar Manager"),
]


def create_component_test(class_name: str, description: str, category: str):
    """Factory to create component class tests"""

    @test(category, f"{class_name} - {description}")
    def test_func():
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            try:
                cdo = unreal.get_default_object(cls)
                return True, f"{class_name} loaded with CDO", {"cdo_valid": cdo is not None}
            except:
                return True, f"{class_name} loaded (no CDO access)", {}
        return False, f"{class_name} not found in C++", {}

    return test_func


# Generate component tests
component_tests = []
for class_name, desc in PLAYER_COMPONENTS:
    component_tests.append(create_component_test(class_name, desc, "Player Components"))

for class_name, desc in AI_COMPONENTS:
    component_tests.append(create_component_test(class_name, desc, "AI Components"))

for class_name, desc in PC_COMPONENTS:
    component_tests.append(create_component_test(class_name, desc, "Controller Components"))


# ═══════════════════════════════════════════════════════════════════════════════
# BASE CLASS TESTS
# ═══════════════════════════════════════════════════════════════════════════════

BASE_CLASSES = [
    ("SLFActionBase", "Base class for all actions"),
    ("SLFStatBase", "Base class for all stats"),
    ("SLFBuffBase", "Base class for all buffs"),
    ("SLFStatusEffectBase", "Base class for status effects"),
    ("SLFItemBase", "Base class for items"),
]


@test("Base Classes", "Action Base")
def test_action_base():
    cls = load_class_safe("/Script/SLFConversion.SLFActionBase")
    if cls:
        return True, "SLFActionBase loaded", {}
    return False, "SLFActionBase not found", {}


@test("Base Classes", "Stat Base")
def test_stat_base():
    cls = load_class_safe("/Script/SLFConversion.SLFStatBase")
    if cls:
        return True, "SLFStatBase loaded", {}
    return False, "SLFStatBase not found", {}


@test("Base Classes", "Buff Base")
def test_buff_base():
    cls = load_class_safe("/Script/SLFConversion.SLFBuffBase")
    if cls:
        return True, "SLFBuffBase loaded", {}
    return False, "SLFBuffBase not found", {}


@test("Base Classes", "Status Effect Base")
def test_status_effect_base():
    cls = load_class_safe("/Script/SLFConversion.SLFStatusEffectBase")
    if cls:
        return True, "SLFStatusEffectBase loaded", {}
    return False, "SLFStatusEffectBase not found", {}


@test("Base Classes", "Item Base")
def test_item_base():
    cls = load_class_safe("/Script/SLFConversion.SLFItemBase")
    if cls:
        return True, "SLFItemBase loaded", {}
    return False, "SLFItemBase not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# ACTION CLASS TESTS
# ═══════════════════════════════════════════════════════════════════════════════

ACTION_CLASSES = [
    "SLFActionDodge",
    "SLFActionJump",
    "SLFActionSprint",
    "SLFActionCrouch",
    "SLFActionBackstab",
    "SLFActionDrinkFlaskHP",
    "SLFActionComboLightL",
    "SLFActionComboLightR",
    "SLFActionComboHeavy",
    "SLFActionJumpAttack",
    "SLFActionDualWieldAttack",
]


@test("Action Classes", "All Action Classes")
def test_all_action_classes():
    found = []
    missing = []

    for class_name in ACTION_CLASSES:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)
        else:
            missing.append(class_name)

    if len(found) > 0:
        msg = f"{len(found)}/{len(ACTION_CLASSES)} action classes loaded"
        debug = {"found": found, "missing": missing}
        # Partial success if at least some loaded
        return len(missing) == 0, msg, debug

    return False, "No action classes found", {"missing": missing}


# ═══════════════════════════════════════════════════════════════════════════════
# STAT CLASS TESTS
# ═══════════════════════════════════════════════════════════════════════════════

STAT_CLASSES = [
    "SLFStatVigor",
    "SLFStatEndurance",
    "SLFStatStrength",
    "SLFStatDexterity",
    "SLFStatIntelligence",
    "SLFStatFaith",
    "SLFStatArcane",
]


@test("Stat Classes", "All Stat Classes")
def test_all_stat_classes():
    found = []
    missing = []

    for class_name in STAT_CLASSES:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)
        else:
            missing.append(class_name)

    if len(found) > 0:
        msg = f"{len(found)}/{len(STAT_CLASSES)} stat classes loaded"
        return len(missing) == 0, msg, {"found": found, "missing": missing}

    return False, "No stat classes found", {"missing": missing}


# ═══════════════════════════════════════════════════════════════════════════════
# AI CLASS TESTS
# ═══════════════════════════════════════════════════════════════════════════════

BT_TASKS = [
    "BTT_SimpleMoveTo",
    "BTT_TryExecuteAbility",
    "BTT_ClearKey",
    "BTT_SetKey",
    "BTT_SwitchState",
    "BTT_PatrolPath",
    "BTT_ToggleFocus",
    "BTT_GetCurrentLocation",
    "BTT_GetRandomPoint",
    "BTT_GetStrafePointAroundTarget",
]

BT_SERVICES = [
    "BTS_DistanceCheck",
    "BTS_IsTargetDead",
    "BTS_TryGetAbility",
    "BTS_ChaseBounds",
    "BTS_SetMovementModeBasedOnDistance",
]


@test("AI Classes", "Behavior Tree Tasks")
def test_bt_tasks():
    found = []
    missing = []

    for class_name in BT_TASKS:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)
        else:
            missing.append(class_name)

    msg = f"{len(found)}/{len(BT_TASKS)} BT tasks loaded"
    return len(found) >= len(BT_TASKS) // 2, msg, {"found": found, "missing": missing}


@test("AI Classes", "Behavior Tree Services")
def test_bt_services():
    found = []
    missing = []

    for class_name in BT_SERVICES:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)
        else:
            missing.append(class_name)

    msg = f"{len(found)}/{len(BT_SERVICES)} BT services loaded"
    return len(found) >= len(BT_SERVICES) // 2, msg, {"found": found, "missing": missing}


@test("AI Classes", "AI Controller")
def test_ai_controller():
    cls = load_class_safe("/Script/SLFConversion.AIC_SoulslikeFramework")
    if cls:
        return True, "AIC_SoulslikeFramework loaded", {}
    return False, "AIC_SoulslikeFramework not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# ANIMATION NOTIFY TESTS
# ═══════════════════════════════════════════════════════════════════════════════

ANIM_NOTIFIES = [
    ("SLFAnimNotifyFootstepTrace", "Footstep audio/VFX"),
    ("SLFAnimNotifySpawnProjectile", "Projectile spawn"),
    ("SLFAnimNotifyCameraShake", "Camera shake"),
]

ANIM_NOTIFY_STATES = [
    ("SLFAnimNotifyStateWeaponTrace", "Weapon collision tracing"),
    ("SLFAnimNotifyStateInputBuffer", "Input buffer window"),
    ("SLFAnimNotifyStateInvincibility", "I-frames"),
    ("SLFAnimNotifyStateHyperArmor", "Super armor"),
]


@test("Animation System", "Anim Notifies")
def test_anim_notifies():
    found = []
    missing = []

    for class_name, desc in ANIM_NOTIFIES:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)
        else:
            missing.append(class_name)

    msg = f"{len(found)}/{len(ANIM_NOTIFIES)} anim notifies loaded"
    return len(found) > 0, msg, {"found": found, "missing": missing}


@test("Animation System", "Anim Notify States")
def test_anim_notify_states():
    found = []
    missing = []

    for class_name, desc in ANIM_NOTIFY_STATES:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)
        else:
            missing.append(class_name)

    msg = f"{len(found)}/{len(ANIM_NOTIFY_STATES)} anim notify states loaded"
    return len(found) > 0, msg, {"found": found, "missing": missing}


# ═══════════════════════════════════════════════════════════════════════════════
# BLUEPRINT ASSET TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Blueprint Assets", "Game Mode Blueprint")
def test_gm_blueprint():
    asset = load_asset_safe("/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework")
    if asset:
        # Check if it has a generated class
        try:
            gen_class = asset.generated_class()
            parent = gen_class.get_class().get_name() if gen_class else "Unknown"
            return True, "GM Blueprint loaded", {"generated_class": str(gen_class), "parent": parent}
        except:
            return True, "GM Blueprint loaded (no gen class access)", {}
    return False, "GM Blueprint not found", {}


@test("Blueprint Assets", "Player Controller Blueprint")
def test_pc_blueprint():
    asset = load_asset_safe("/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework")
    if asset:
        return True, "PC Blueprint loaded", {}
    return False, "PC Blueprint not found", {}


@test("Blueprint Assets", "Player Character Blueprint")
def test_character_blueprint():
    asset = load_asset_safe("/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character")
    if asset:
        return True, "Character Blueprint loaded", {}
    return False, "Character Blueprint not found", {}


@test("Blueprint Assets", "Level Asset")
def test_level_asset():
    # Check if level exists
    try:
        exists = unreal.EditorAssetLibrary.does_asset_exist("/Game/SoulslikeFramework/Maps/L_Demo_Showcase")
        if exists:
            return True, "L_Demo_Showcase level exists", {}
        return False, "L_Demo_Showcase not found", {}
    except Exception as e:
        return False, f"Error checking level: {e}", {}


# ═══════════════════════════════════════════════════════════════════════════════
# RUN ALL TESTS
# ═══════════════════════════════════════════════════════════════════════════════

def run_all_core_tests():
    """Run all core system tests"""
    log.section("CORE SYSTEMS TESTS")

    # C++ Classes
    test_game_mode_class()
    test_player_controller_class()
    test_game_state_class()
    test_player_state_class()
    test_player_character_class()
    test_enemy_character_class()
    test_boss_character_class()
    test_npc_character_class()

    # Run generated component tests
    for test_func in component_tests:
        test_func()

    # Base classes
    test_action_base()
    test_stat_base()
    test_buff_base()
    test_status_effect_base()
    test_item_base()

    # Action and Stat classes
    test_all_action_classes()
    test_all_stat_classes()

    # AI classes
    test_bt_tasks()
    test_bt_services()
    test_ai_controller()

    # Animation
    test_anim_notifies()
    test_anim_notify_states()

    # Blueprints
    test_gm_blueprint()
    test_pc_blueprint()
    test_character_blueprint()
    test_level_asset()


if __name__ == "__main__":
    run_all_core_tests()
    suite.print_summary()
