"""
Check if B_StatusEffectArea Blueprint is properly reparented to C++ class.
"""

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/bp_check_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(msg)

def check_blueprint():
    """Check B_StatusEffectArea Blueprint parent."""

    bp_path = "/Game/SoulslikeFramework/Data/StatusEffects/B_StatusEffectArea"
    log(f"=== Checking B_StatusEffectArea Blueprint ===")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"Failed to load: {bp_path}")
        return

    log(f"Loaded Blueprint: {bp.get_name()}")

    # Get the parent class directly from the Blueprint
    try:
        parent_class = bp.get_editor_property("parent_class")
        log(f"Blueprint parent_class: {parent_class}")
        if parent_class:
            log(f"Parent class name: {parent_class.get_name()}")
    except Exception as e:
        log(f"Error getting parent_class: {e}")

    # Try loading the C++ class
    expected_parent = "/Script/SLFConversion.B_StatusEffectArea"
    log(f"\nExpected C++ class: {expected_parent}")

    cpp_class = unreal.load_class(None, expected_parent)
    if cpp_class:
        log(f"C++ class found: {cpp_class.get_name()}")

        # Check the C++ class has our properties
        cdo = unreal.get_default_object(cpp_class)
        if cdo:
            log(f"C++ CDO: {cdo.get_name()}")
            try:
                val = cdo.get_editor_property("status_effect_to_apply")
                log(f"  status_effect_to_apply (C++ CDO): {val}")
            except Exception as e:
                log(f"  status_effect_to_apply error: {e}")
    else:
        log("C++ class NOT found!")

    # Now test if we can reparent
    log("\n=== Attempting to reparent Blueprint ===")
    if cpp_class:
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            log(f"Reparent result: {result}")

            # Save the Blueprint
            if result:
                unreal.EditorAssetLibrary.save_asset(bp_path)
                log("Saved Blueprint after reparent")
        except Exception as e:
            log(f"Reparent error: {e}")

# Run
check_blueprint()

# Write log
with open(LOG_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
