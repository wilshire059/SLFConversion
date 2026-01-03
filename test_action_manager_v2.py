#!/usr/bin/env python3
"""
test_action_manager_v2.py
Test the AC_ActionManager component Blueprint CDO directly
"""

import unreal

def test_action_manager():
    """Load the AC_ActionManager component and check Actions TMap"""
    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning("[Test] AC_ActionManager CDO Validation Test")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    # Load the AC_ActionManager Blueprint CDO
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"[Test] Could not load: {bp_path}")
        return False

    unreal.log_warning(f"[Test] Loaded Blueprint: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("[Test] No generated class")
        return False

    unreal.log_warning(f"[Test] Generated class: {gen_class.get_name()}")

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("[Test] Could not get CDO")
        return False

    unreal.log_warning(f"[Test] CDO: {cdo.get_name()}")

    # List all properties
    unreal.log_warning("[Test] Checking key properties:")

    # Check Actions TMap
    try:
        actions = cdo.get_editor_property("Actions")
        if actions is not None:
            action_count = len(actions) if hasattr(actions, '__len__') else 0
            unreal.log_warning(f"[Test] ✅ Actions TMap count: {action_count}")

            if action_count > 0:
                for key, value in list(actions.items())[:5]:
                    key_str = str(key.tag_name) if hasattr(key, 'tag_name') else str(key)
                    val_str = value.get_name() if value else "None"
                    unreal.log_warning(f"       {key_str} -> {val_str}")
                if action_count > 5:
                    unreal.log_warning(f"       ... and {action_count - 5} more")
        else:
            unreal.log_warning("[Test] ⚠️ Actions TMap is None/empty")
    except Exception as e:
        unreal.log_warning(f"[Test] ❌ Could not read Actions: {e}")

    # Check AvailableActions TMap
    try:
        available = cdo.get_editor_property("AvailableActions")
        if available is not None:
            avail_count = len(available) if hasattr(available, '__len__') else 0
            unreal.log_warning(f"[Test] ✅ AvailableActions TMap count: {avail_count}")

            if avail_count > 0:
                for key, value in list(available.items())[:5]:
                    key_str = str(key.tag_name) if hasattr(key, 'tag_name') else str(key)
                    val_str = value.get_name() if value else "None"
                    unreal.log_warning(f"       {key_str} -> {val_str}")
        else:
            unreal.log_warning("[Test] ⚠️ AvailableActions TMap is None/empty")
    except Exception as e:
        unreal.log_warning(f"[Test] ❌ Could not read AvailableActions: {e}")

    # Check other relevant properties
    for prop_name in ["StaminaRegenDelay", "IsSprinting", "IsCrouched"]:
        try:
            val = cdo.get_editor_property(prop_name)
            unreal.log_warning(f"[Test] {prop_name}: {val}")
        except Exception as e:
            unreal.log_warning(f"[Test] {prop_name}: Error - {e}")

    unreal.log_warning("═══════════════════════════════════════════════════════════════")
    unreal.log_warning("[Test] Test Complete")
    unreal.log_warning("═══════════════════════════════════════════════════════════════")

    return True


# Run test
if __name__ == "__main__":
    test_action_manager()
