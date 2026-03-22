#!/usr/bin/env python3
"""
test_action_manager.py
Test the AC_ActionManager component to verify Actions and AvailableActions TMaps
"""

import unreal

def test_action_manager():
    """Load the player character and check ActionManager state"""
    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning("[Test] ActionManager Validation Test")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    # Load the player character Blueprint CDO
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"[Test] Could not load: {bp_path}")
        return False

    unreal.log_warning(f"[Test] Loaded: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("[Test] No generated class")
        return False

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("[Test] Could not get CDO")
        return False

    unreal.log_warning(f"[Test] CDO: {cdo.get_name()}")

    # Find ActionManager component
    action_manager = None
    components = cdo.get_components_by_class(unreal.ActorComponent)

    unreal.log_warning(f"[Test] Found {len(components)} components")

    for comp in components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        unreal.log_warning(f"  - {comp_name} ({comp_class})")

        if "ActionManager" in comp_name or "ActionManager" in comp_class:
            action_manager = comp
            break

    if not action_manager:
        unreal.log_error("[Test] ActionManager component not found")
        return False

    unreal.log_warning(f"[Test] Found ActionManager: {action_manager.get_name()}")

    # Check Actions TMap
    try:
        actions = action_manager.get_editor_property("Actions")
        if actions is not None:
            action_count = len(actions) if hasattr(actions, '__len__') else 0
            unreal.log_warning(f"[Test] Actions TMap count: {action_count}")
        else:
            unreal.log_warning("[Test] Actions TMap is None")
    except Exception as e:
        unreal.log_warning(f"[Test] Could not read Actions: {e}")

    # Check AvailableActions TMap
    try:
        available = action_manager.get_editor_property("AvailableActions")
        if available is not None:
            avail_count = len(available) if hasattr(available, '__len__') else 0
            unreal.log_warning(f"[Test] AvailableActions TMap count: {avail_count}")
        else:
            unreal.log_warning("[Test] AvailableActions TMap is None")
    except Exception as e:
        unreal.log_warning(f"[Test] Could not read AvailableActions: {e}")

    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning("[Test] Test Complete")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    return True


# Run test
if __name__ == "__main__":
    test_action_manager()
