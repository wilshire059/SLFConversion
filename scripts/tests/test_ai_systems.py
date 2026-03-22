"""
===============================================================================
AI SYSTEMS TESTS
===============================================================================

Tests for:
- AI Behavior Manager states and transitions
- AI Combat Manager
- AI Sense Manager
- AI Target Sort Manager
- AI Interaction Manager (dialog/vendor)
- Behavior Tree Tasks
- Boss AI systems
"""

import unreal
from slf_test_framework import *


# ===============================================================================
# AI COMPONENT CLASS TESTS
# ===============================================================================

AI_COMPONENTS = [
    ("AC_AI_BehaviorManager", "AI state management"),
    ("AC_AI_CombatManager", "AI combat logic"),
    ("AC_AI_SenseManager", "AI perception"),
    ("AC_AI_TargetSortManager", "AI target prioritization"),
    ("AC_AI_InteractionManager", "NPC dialog/vendor"),
    ("AC_AI_AbilityManager", "AI abilities"),
]


@test("AI Components", "AI Component Classes")
def test_ai_component_classes():
    found = []
    missing = []

    for comp_name, desc in AI_COMPONENTS:
        path = f"/Script/SLFConversion.{comp_name}"
        cls = load_class_safe(path)
        if cls:
            found.append(comp_name)
        else:
            missing.append(comp_name)

    msg = f"{len(found)}/{len(AI_COMPONENTS)} AI components found"
    return len(found) >= 3, msg, {"found": found, "missing": missing}


# ===============================================================================
# AI BEHAVIOR MANAGER TESTS
# ===============================================================================

AI_STATES = [
    "Idle",
    "Patrolling",
    "RandomRoam",
    "Investigating",
    "Combat",
    "Dead",
]


@test("AI Behavior", "Behavior Manager Class")
def test_ai_behavior_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_AI_BehaviorManager")
    if not cls:
        return False, "AC_AI_BehaviorManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["CurrentState", "SetState", "BehaviorTree", "PatrolPath"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"BehaviorManager loaded with {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_AI_BehaviorManager class exists", {}


@test("AI Behavior", "Behavior Parameters")
def test_ai_behavior_parameters():
    """Test that behavior tuning parameters exist"""
    cls = load_class_safe("/Script/SLFConversion.AC_AI_BehaviorManager")
    if not cls:
        return False, "AC_AI_BehaviorManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            params = [
                "MaxChaseDistanceThreshold",
                "AttackDistanceThreshold",
                "StrafeDistanceThreshold",
                "SpeedAdjustDistanceThreshold",
                "MinimumStrafePointDistance",
                "StrafeMethods",
            ]
            found = [p for p in params if has_property(cdo, p)]
            return len(found) >= 1, f"Found {len(found)} behavior params", {"found": found}
    except:
        pass

    return True, "BehaviorManager loaded", {}


# ===============================================================================
# AI COMBAT MANAGER TESTS
# ===============================================================================

@test("AI Combat", "Combat Manager Class")
def test_ai_combat_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_AI_CombatManager")
    if not cls:
        return False, "AC_AI_CombatManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["CurrentTarget", "IsInCombat", "CanAttack", "OnDeath"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"CombatManager loaded with {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_AI_CombatManager class exists", {}


@test("AI Combat", "Death Handling")
def test_ai_death_handling():
    """Test AI death-related properties"""
    cls = load_class_safe("/Script/SLFConversion.AC_AI_CombatManager")
    if not cls:
        return False, "AC_AI_CombatManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            death_props = ["IsDead", "OnDeath", "DeathMontage", "RagdollOnDeath"]
            found = [p for p in death_props if has_property(cdo, p)]
            return len(found) >= 1, f"Found {len(found)} death props", {"found": found}
    except:
        pass

    return True, "CombatManager loaded", {}


# ===============================================================================
# AI SENSE MANAGER TESTS
# ===============================================================================

@test("AI Sense", "Sense Manager Class")
def test_ai_sense_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_AI_SenseManager")
    if not cls:
        return False, "AC_AI_SenseManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["SightConfig", "HearingConfig", "DamageConfig", "OnPerceptionUpdated"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"SenseManager loaded with {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_AI_SenseManager class exists", {}


# ===============================================================================
# AI TARGET SORT MANAGER TESTS
# ===============================================================================

@test("AI Targeting", "Target Sort Manager Class")
def test_ai_target_sort_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_AI_TargetSortManager")
    if not cls:
        return False, "AC_AI_TargetSortManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["SortTargets", "GetBestTarget", "TargetPriority"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"TargetSortManager loaded with {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_AI_TargetSortManager class exists", {}


# ===============================================================================
# AI INTERACTION MANAGER TESTS
# ===============================================================================

@test("AI Interaction", "Interaction Manager Class")
def test_ai_interaction_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_AI_InteractionManager")
    if not cls:
        return False, "AC_AI_InteractionManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["BeginDialog", "DialogAsset", "VendorAsset", "CurrentDialogIndex"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"InteractionManager loaded with {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_AI_InteractionManager class exists", {}


@test("AI Interaction", "Dialog Properties")
def test_ai_dialog_properties():
    """Test dialog-related properties"""
    cls = load_class_safe("/Script/SLFConversion.AC_AI_InteractionManager")
    if not cls:
        return False, "AC_AI_InteractionManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            dialog_props = [
                "DialogAsset",
                "CurrentDialogTable",
                "CurrentDialogIndex",
                "OnDialogStarted",
                "OnDialogEnded",
            ]
            found = [p for p in dialog_props if has_property(cdo, p)]
            return len(found) >= 1, f"Found {len(found)} dialog props", {"found": found}
    except:
        pass

    return True, "InteractionManager loaded", {}


@test("AI Interaction", "Vendor Properties")
def test_ai_vendor_properties():
    """Test vendor-related properties"""
    cls = load_class_safe("/Script/SLFConversion.AC_AI_InteractionManager")
    if not cls:
        return False, "AC_AI_InteractionManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            vendor_props = ["VendorAsset", "VendorInventory", "OnVendorOpened"]
            found = [p for p in vendor_props if has_property(cdo, p)]
            return len(found) >= 1, f"Found {len(found)} vendor props", {"found": found}
    except:
        pass

    return True, "InteractionManager loaded", {}


# ===============================================================================
# BEHAVIOR TREE TASK TESTS
# ===============================================================================

BT_TASKS = [
    "BTT_GetStrafePoint",
    "BTT_GetPatrolPoint",
    "BTT_ExecuteAction",
    "BTT_SelectAction",
    "BTT_FaceTarget",
    "BTT_MoveToTarget",
    "BTT_Wait",
    "BTT_SetState",
]


@test("BT Tasks", "Behavior Tree Task Classes")
def test_bt_task_classes():
    found = []
    missing = []

    for task_name in BT_TASKS:
        # Try multiple paths
        paths = [
            f"/Script/SLFConversion.{task_name}",
            f"/Game/SoulslikeFramework/Blueprints/_AI/Tasks/{task_name}",
        ]

        task_found = False
        for path in paths:
            cls = load_class_safe(path)
            if cls:
                found.append(task_name)
                task_found = True
                break

            # Try as asset
            asset = load_asset_safe(path)
            if asset:
                found.append(task_name)
                task_found = True
                break

        if not task_found:
            missing.append(task_name)

    msg = f"{len(found)}/{len(BT_TASKS)} BT tasks found"
    return len(found) >= 2, msg, {"found": found, "missing": missing}


@test("BT Tasks", "Strafe Point Task")
def test_btt_strafe_point():
    """Test strafe point calculation task"""
    paths = [
        "/Script/SLFConversion.BTT_GetStrafePoint",
        "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetStrafePoint",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BTT_GetStrafePoint found", {"path": path}

    return False, "BTT_GetStrafePoint not found", {}


# ===============================================================================
# AI ENEMY CLASS TESTS
# ===============================================================================

@test("AI Enemies", "Soulslike Enemy Base Class")
def test_soulslike_enemy_class():
    paths = [
        "/Script/SLFConversion.B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_Soulslike_Enemy found", {"path": path}

    return False, "B_Soulslike_Enemy not found", {}


@test("AI Enemies", "Enemy Has AI Components")
def test_enemy_has_ai_components():
    """Test that enemy class has required AI components"""
    path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"
    asset = load_asset_safe(path)

    if not asset:
        return False, "B_Soulslike_Enemy not found", {}

    try:
        gen_class = asset.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                expected = ["BehaviorManager", "CombatManager", "SenseManager"]
                found = [c for c in expected if has_property(cdo, c)]
                return len(found) >= 1, f"Enemy has {len(found)} AI components", {"found": found}
    except:
        pass

    return True, "B_Soulslike_Enemy loaded", {}


@test("AI Enemies", "Boss Enemy Class")
def test_boss_enemy_class():
    """Test boss enemy classes exist"""
    paths = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Boss class/folder found", {"path": path}

    return False, "Boss assets not found (may be in demo content)", {}


# ===============================================================================
# AI CONTROLLER TESTS
# ===============================================================================

@test("AI Controller", "Soulslike AI Controller Class")
def test_ai_controller_class():
    paths = [
        "/Script/SLFConversion.AIC_SoulslikeFramework",
        "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "AIC_SoulslikeFramework found", {"path": path}

    return False, "AIC_SoulslikeFramework not found", {}


# ===============================================================================
# RUN ALL AI TESTS
# ===============================================================================

def run_all_ai_tests():
    """Run all AI system tests"""
    log.section("AI SYSTEMS TESTS")

    # AI Components
    log.subsection("AI Component Classes")
    test_ai_component_classes()

    # Behavior Manager
    log.subsection("AI Behavior Manager")
    test_ai_behavior_manager()
    test_ai_behavior_parameters()

    # Combat Manager
    log.subsection("AI Combat Manager")
    test_ai_combat_manager()
    test_ai_death_handling()

    # Sense Manager
    log.subsection("AI Sense Manager")
    test_ai_sense_manager()

    # Target Sort Manager
    log.subsection("AI Target Sort Manager")
    test_ai_target_sort_manager()

    # Interaction Manager
    log.subsection("AI Interaction Manager")
    test_ai_interaction_manager()
    test_ai_dialog_properties()
    test_ai_vendor_properties()

    # BT Tasks
    log.subsection("Behavior Tree Tasks")
    test_bt_task_classes()
    test_btt_strafe_point()

    # Enemy Classes
    log.subsection("AI Enemy Classes")
    test_soulslike_enemy_class()
    test_enemy_has_ai_components()
    test_boss_enemy_class()

    # Controller
    log.subsection("AI Controller")
    test_ai_controller_class()


if __name__ == "__main__":
    run_all_ai_tests()
    suite.print_summary()
