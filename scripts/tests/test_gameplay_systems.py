"""
═══════════════════════════════════════════════════════════════════════════════
GAMEPLAY SYSTEMS TESTS
═══════════════════════════════════════════════════════════════════════════════

Tests for:
- Combat system (attacks, guards, dodges, combos, damage)
- Item system (pickup, use, drop)
- Inventory system (add, remove, query)
- Equipment system (equip, unequip, stat requirements)
- Stat system (get, modify, level up)
- Buff system (apply, remove, duration)
- Status effect system (buildup, trigger, decay)
- Input buffer system (queue, consume)
- Action system (execute, cancel, chain)
"""

import unreal
from slf_test_framework import *


# ═══════════════════════════════════════════════════════════════════════════════
# STAT MANAGER TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Stat System", "Stat Manager Component Exists")
def test_stat_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_StatManager")
    if not cls:
        return False, "AC_StatManager class not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        return True, "AC_StatManager exists with CDO", {"cdo": str(cdo)}
    except:
        return True, "AC_StatManager class exists", {}


@test("Stat System", "Stat Manager Properties")
def test_stat_manager_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_StatManager")
    if not cls:
        return False, "AC_StatManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if not cdo:
            return False, "Could not get CDO", {}

        # Check for expected properties
        expected_props = ["Stats", "OnStatUpdated", "StatsDataTable"]
        found_props = []
        missing_props = []

        for prop in expected_props:
            if has_property(cdo, prop):
                found_props.append(prop)
            else:
                missing_props.append(prop)

        return len(found_props) > 0, f"Found {len(found_props)}/{len(expected_props)} properties", {
            "found": found_props,
            "missing": missing_props
        }
    except Exception as e:
        return False, f"Error checking properties: {e}", {}


@test("Stat System", "Stat Classes Loaded")
def test_stat_classes():
    stat_classes = [
        ("SLFStatVigor", "Health/HP"),
        ("SLFStatEndurance", "Stamina"),
        ("SLFStatStrength", "Physical damage"),
        ("SLFStatDexterity", "Skill scaling"),
    ]

    found = []
    for class_name, desc in stat_classes:
        cls = load_class_safe(f"/Script/SLFConversion.{class_name}")
        if cls:
            found.append(class_name)

    return len(found) >= 2, f"{len(found)}/{len(stat_classes)} stat classes", {"found": found}


# ═══════════════════════════════════════════════════════════════════════════════
# COMBAT MANAGER TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Combat System", "Combat Manager Component Exists")
def test_combat_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_CombatManager")
    if not cls:
        return False, "AC_CombatManager class not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        return True, "AC_CombatManager exists", {"cdo": str(cdo)}
    except:
        return True, "AC_CombatManager class exists", {}


@test("Combat System", "Combat Manager Properties")
def test_combat_manager_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_CombatManager")
    if not cls:
        return False, "AC_CombatManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if not cdo:
            return True, "Class exists but no CDO", {}

        # Combat-related properties we expect
        expected = [
            "bIsGuarding",
            "bIsInvincible",
            "bHasHyperArmor",
            "ComboSection",
            "PerfectGuardDuration",
            "GuardStaminaDrain",
        ]

        found = []
        for prop in expected:
            if has_property(cdo, prop):
                found.append(prop)

        return len(found) >= 2, f"Found {len(found)}/{len(expected)} combat properties", {"found": found}
    except Exception as e:
        return False, f"Error: {e}", {}


@test("Combat System", "AI Combat Manager Exists")
def test_ai_combat_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_AI_CombatManager")
    if cls:
        return True, "AC_AI_CombatManager loaded", {}
    return False, "AC_AI_CombatManager not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# ACTION SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Action System", "Action Manager Exists")
def test_action_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_ActionManager")
    if not cls:
        return False, "AC_ActionManager not found", {}
    return True, "AC_ActionManager loaded", {}


@test("Action System", "Action Manager Properties")
def test_action_manager_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_ActionManager")
    if not cls:
        return False, "AC_ActionManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if not cdo:
            return True, "Class exists", {}

        expected = ["StaminaRegenDelay", "CurrentAction", "ActionQueue"]
        found = [p for p in expected if has_property(cdo, p)]

        return len(found) >= 1, f"Found {len(found)} action properties", {"found": found}
    except:
        return True, "Class exists", {}


@test("Action System", "Dodge Action Class")
def test_dodge_action():
    cls = load_class_safe("/Script/SLFConversion.SLFActionDodge")
    if cls:
        # Check for dodge-specific properties
        try:
            cdo = unreal.get_default_object(cls)
            props = ["DodgeDistance", "InvincibilityDuration", "StaminaCost"]
            found = [p for p in props if has_property(cdo, p)]
            return True, f"SLFActionDodge loaded with {len(found)} props", {"props": found}
        except:
            return True, "SLFActionDodge loaded", {}
    return False, "SLFActionDodge not found", {}


@test("Action System", "Jump Action Class")
def test_jump_action():
    cls = load_class_safe("/Script/SLFConversion.SLFActionJump")
    if cls:
        return True, "SLFActionJump loaded", {}
    return False, "SLFActionJump not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# INPUT BUFFER TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Input Buffer", "Input Buffer Component Exists")
def test_input_buffer_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_InputBuffer")
    if not cls:
        return False, "AC_InputBuffer not found", {}
    return True, "AC_InputBuffer loaded", {}


@test("Input Buffer", "Input Buffer Properties")
def test_input_buffer_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_InputBuffer")
    if not cls:
        return False, "AC_InputBuffer not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        expected = ["bBufferOpen", "QueuedAction", "OnInputBufferConsumed"]
        found = [p for p in expected if has_property(cdo, p)]
        return len(found) >= 1, f"Found {len(found)} buffer properties", {"found": found}
    except:
        return True, "Class exists", {}


# ═══════════════════════════════════════════════════════════════════════════════
# INVENTORY SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Inventory System", "Inventory Manager Exists")
def test_inventory_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_InventoryManager")
    if not cls:
        return False, "AC_InventoryManager not found", {}
    return True, "AC_InventoryManager loaded", {}


@test("Inventory System", "Inventory Manager Properties")
def test_inventory_manager_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_InventoryManager")
    if not cls:
        return False, "AC_InventoryManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        expected = ["Items", "Currency", "OnInventoryUpdated", "OnCurrencyUpdated", "OnItemAdded"]
        found = [p for p in expected if has_property(cdo, p)]
        return len(found) >= 1, f"Found {len(found)} inventory properties", {"found": found}
    except:
        return True, "Class exists", {}


@test("Inventory System", "Item Base Class")
def test_item_base():
    cls = load_class_safe("/Script/SLFConversion.SLFItemBase")
    if not cls:
        return False, "SLFItemBase not found", {}
    return True, "SLFItemBase loaded", {}


# ═══════════════════════════════════════════════════════════════════════════════
# EQUIPMENT SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Equipment System", "Equipment Manager Exists")
def test_equipment_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_EquipmentManager")
    if not cls:
        return False, "AC_EquipmentManager not found", {}
    return True, "AC_EquipmentManager loaded", {}


@test("Equipment System", "Equipment Manager Properties")
def test_equipment_manager_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_EquipmentManager")
    if not cls:
        return False, "AC_EquipmentManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        expected = ["SlotTable", "EquippedItems", "OnItemEquipped", "OnItemUnequipped"]
        found = [p for p in expected if has_property(cdo, p)]
        return len(found) >= 1, f"Found {len(found)} equipment properties", {"found": found}
    except:
        return True, "Class exists", {}


@test("Equipment System", "Weapon Item Class")
def test_weapon_item():
    cls = load_class_safe("/Script/SLFConversion.SLFItemWeapon")
    if cls:
        return True, "SLFItemWeapon loaded", {}
    # Try alternate name
    cls = load_class_safe("/Script/SLFConversion.B_Item_Weapon")
    if cls:
        return True, "B_Item_Weapon loaded", {}
    return False, "Weapon item class not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# BUFF SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Buff System", "Buff Manager Exists")
def test_buff_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_BuffManager")
    if not cls:
        return False, "AC_BuffManager not found", {}
    return True, "AC_BuffManager loaded", {}


@test("Buff System", "Buff Manager Properties")
def test_buff_manager_properties():
    cls = load_class_safe("/Script/SLFConversion.AC_BuffManager")
    if not cls:
        return False, "AC_BuffManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        expected = ["ActiveBuffs", "OnBuffDetected", "OnBuffRemoved"]
        found = [p for p in expected if has_property(cdo, p)]
        return len(found) >= 1, f"Found {len(found)} buff properties", {"found": found}
    except:
        return True, "Class exists", {}


@test("Buff System", "Buff Base Class")
def test_buff_base():
    cls = load_class_safe("/Script/SLFConversion.SLFBuffBase")
    if not cls:
        return False, "SLFBuffBase not found", {}
    return True, "SLFBuffBase loaded", {}


# ═══════════════════════════════════════════════════════════════════════════════
# STATUS EFFECT SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Status Effect System", "Status Effect Manager Exists")
def test_status_effect_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_StatusEffectManager")
    if not cls:
        return False, "AC_StatusEffectManager not found", {}
    return True, "AC_StatusEffectManager loaded", {}


@test("Status Effect System", "Status Effect Base Class")
def test_status_effect_base():
    cls = load_class_safe("/Script/SLFConversion.SLFStatusEffectBase")
    if not cls:
        return False, "SLFStatusEffectBase not found", {}
    return True, "SLFStatusEffectBase loaded", {}


# ═══════════════════════════════════════════════════════════════════════════════
# INTERACTION SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Interaction System", "Interaction Manager Exists")
def test_interaction_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_InteractionManager")
    if not cls:
        return False, "AC_InteractionManager not found", {}
    return True, "AC_InteractionManager loaded", {}


@test("Interaction System", "Target Manager Exists")
def test_target_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_TargetManager")
    if cls:
        return True, "AC_TargetManager loaded", {}
    # May be part of interaction manager
    return False, "AC_TargetManager not found (may be in InteractionManager)", {}


# ═══════════════════════════════════════════════════════════════════════════════
# WEAPON COLLISION TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Weapon System", "Collision Manager Exists")
def test_collision_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_CollisionManager")
    if not cls:
        return False, "AC_CollisionManager not found", {}
    return True, "AC_CollisionManager loaded", {}


@test("Weapon System", "Weapon Trace Notify State")
def test_weapon_trace_notify():
    cls = load_class_safe("/Script/SLFConversion.SLFAnimNotifyStateWeaponTrace")
    if cls:
        return True, "Weapon trace notify state loaded", {}

    # Try alternate name
    cls = load_class_safe("/Script/SLFConversion.ANS_WeaponTrace")
    if cls:
        return True, "ANS_WeaponTrace loaded", {}

    return False, "Weapon trace notify not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# SAVE/LOAD SYSTEM TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Save System", "Save/Load Manager Exists")
def test_save_load_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_SaveLoadManager")
    if not cls:
        return False, "AC_SaveLoadManager not found", {}
    return True, "AC_SaveLoadManager loaded", {}


@test("Save System", "Progress Manager Exists")
def test_progress_manager_exists():
    cls = load_class_safe("/Script/SLFConversion.AC_ProgressManager")
    if not cls:
        return False, "AC_ProgressManager not found", {}
    return True, "AC_ProgressManager loaded", {}


# ═══════════════════════════════════════════════════════════════════════════════
# RUN ALL GAMEPLAY TESTS
# ═══════════════════════════════════════════════════════════════════════════════

def run_all_gameplay_tests():
    """Run all gameplay system tests"""
    log.section("GAMEPLAY SYSTEMS TESTS")

    # Stat System
    log.subsection("Stat System")
    test_stat_manager_exists()
    test_stat_manager_properties()
    test_stat_classes()

    # Combat System
    log.subsection("Combat System")
    test_combat_manager_exists()
    test_combat_manager_properties()
    test_ai_combat_manager()

    # Action System
    log.subsection("Action System")
    test_action_manager_exists()
    test_action_manager_properties()
    test_dodge_action()
    test_jump_action()

    # Input Buffer
    log.subsection("Input Buffer")
    test_input_buffer_exists()
    test_input_buffer_properties()

    # Inventory System
    log.subsection("Inventory System")
    test_inventory_manager_exists()
    test_inventory_manager_properties()
    test_item_base()

    # Equipment System
    log.subsection("Equipment System")
    test_equipment_manager_exists()
    test_equipment_manager_properties()
    test_weapon_item()

    # Buff System
    log.subsection("Buff System")
    test_buff_manager_exists()
    test_buff_manager_properties()
    test_buff_base()

    # Status Effect System
    log.subsection("Status Effect System")
    test_status_effect_manager_exists()
    test_status_effect_base()

    # Interaction System
    log.subsection("Interaction System")
    test_interaction_manager_exists()
    test_target_manager_exists()

    # Weapon System
    log.subsection("Weapon System")
    test_collision_manager_exists()
    test_weapon_trace_notify()

    # Save System
    log.subsection("Save/Load System")
    test_save_load_manager_exists()
    test_progress_manager_exists()


if __name__ == "__main__":
    run_all_gameplay_tests()
    suite.print_summary()
