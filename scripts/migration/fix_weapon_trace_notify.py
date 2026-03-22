"""
fix_weapon_trace_notify.py
Reparents ANS_WeaponTrace Blueprint to C++ UANS_WeaponTrace and clears its EventGraph.
Also ensures AC_CollisionManager is reparented to C++ UCollisionManagerComponent.

Uses SLFAutomationLibrary for proper Blueprint graph clearing.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/fix_weapon_trace_notify.py" ^
  -stdout -unattended -nosplash
"""

import unreal


def fix_anim_notify_state(bp_path, cpp_class_path, name):
    """Fix an AnimNotifyState Blueprint by clearing graphs and reparenting to C++."""

    # Load the Blueprint
    blueprint = unreal.load_asset(bp_path)
    if not blueprint:
        unreal.log_error(f"FAIL: Could not load Blueprint at {bp_path}")
        return False

    unreal.log_warning(f"Processing {name}...")

    # Get current parent class
    gen_class = unreal.load_object(None, bp_path + "_C")
    if gen_class:
        current_parent = gen_class.get_super_class()
        current_parent_name = current_parent.get_name() if current_parent else 'None'
        unreal.log_warning(f"  Current parent: {current_parent_name}")

    # Step 1: Clear all Blueprint logic (this removes the broken function calls)
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(blueprint)
        if result:
            unreal.log_warning(f"  OK: Cleared Blueprint logic")
        else:
            unreal.log_warning(f"  WARNING: Clear logic returned False")
    except Exception as e:
        unreal.log_error(f"  ERROR: Clear logic failed - {str(e)}")
        return False

    # Step 2: Reparent to C++ class
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(blueprint, cpp_class_path)
        if result:
            unreal.log_warning(f"  OK: Reparented to C++")
        else:
            unreal.log_warning(f"  WARNING: Reparent returned False (may already be correct parent)")
    except Exception as e:
        unreal.log_error(f"  ERROR: Reparent failed - {str(e)}")
        return False

    # Step 3: Compile and save
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(blueprint)
        if result:
            unreal.log_warning(f"  OK: Compiled and saved")
        else:
            unreal.log_error(f"  FAIL: Compile/save failed")
            return False
    except Exception as e:
        unreal.log_error(f"  ERROR: Compile/save failed - {str(e)}")
        return False

    return True


def fix_component_blueprint(bp_path, cpp_class_path, name):
    """Fix a component Blueprint by clearing graphs and reparenting to C++."""

    # Load the Blueprint
    blueprint = unreal.load_asset(bp_path)
    if not blueprint:
        unreal.log_warning(f"SKIP: Could not load Blueprint at {bp_path}")
        return False

    unreal.log_warning(f"Processing {name}...")

    # Get current parent class
    gen_class = unreal.load_object(None, bp_path + "_C")
    if gen_class:
        current_parent = gen_class.get_super_class()
        current_parent_name = current_parent.get_name() if current_parent else 'None'
        unreal.log_warning(f"  Current parent: {current_parent_name}")

        # Check if already correct
        expected_parent = cpp_class_path.split('.')[-1]
        if current_parent and current_parent.get_name() == expected_parent:
            unreal.log_warning(f"  SKIP: Already parented to {expected_parent}")
            return True

    # Step 1: Clear all Blueprint logic
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(blueprint)
        if result:
            unreal.log_warning(f"  OK: Cleared Blueprint logic")
        else:
            unreal.log_warning(f"  WARNING: Clear logic returned False")
    except Exception as e:
        unreal.log_error(f"  ERROR: Clear logic failed - {str(e)}")
        return False

    # Step 2: Reparent to C++ class
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(blueprint, cpp_class_path)
        if result:
            unreal.log_warning(f"  OK: Reparented to C++")
        else:
            unreal.log_warning(f"  WARNING: Reparent returned False")
    except Exception as e:
        unreal.log_error(f"  ERROR: Reparent failed - {str(e)}")
        return False

    # Step 3: Compile and save
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(blueprint)
        if result:
            unreal.log_warning(f"  OK: Compiled and saved")
        else:
            unreal.log_error(f"  FAIL: Compile/save failed")
            return False
    except Exception as e:
        unreal.log_error(f"  ERROR: Compile/save failed - {str(e)}")
        return False

    return True


# Main execution
unreal.log_warning("=== FIXING WEAPON TRACE SYSTEM ===")
success_count = 0
fail_count = 0

try:
    # Fix AC_CollisionManager first (dependency for ANS_WeaponTrace)
    if fix_component_blueprint(
        "/Game/SoulslikeFramework/Blueprints/Components/AC_CollisionManager",
        "/Script/SLFConversion.CollisionManagerComponent",
        "AC_CollisionManager"
    ):
        success_count += 1
    else:
        fail_count += 1

    # Fix ANS_WeaponTrace (the main culprit)
    if fix_anim_notify_state(
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_WeaponTrace",
        "/Script/SLFConversion.ANS_WeaponTrace",
        "ANS_WeaponTrace"
    ):
        success_count += 1
    else:
        fail_count += 1

    # Also fix ANS_AI_WeaponTrace if it exists
    if fix_anim_notify_state(
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_AI_WeaponTrace",
        "/Script/SLFConversion.SLFAnimNotifyStateWeaponTrace",
        "ANS_AI_WeaponTrace"
    ):
        success_count += 1
    else:
        fail_count += 1

    # Fix ANS_FistTrace for unarmed combat
    if fix_anim_notify_state(
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_FistTrace",
        "/Script/SLFConversion.SLFAnimNotifyStateFistTrace",
        "ANS_FistTrace"
    ):
        success_count += 1
    else:
        fail_count += 1

    unreal.log_warning("")
    unreal.log_warning("=== WEAPON TRACE FIX COMPLETE ===")
    unreal.log_warning(f"Success: {success_count}")
    unreal.log_warning(f"Failed/Skipped: {fail_count}")

except Exception as e:
    unreal.log_error(f"Script failed: {str(e)}")
    import traceback
    unreal.log_error(traceback.format_exc())
