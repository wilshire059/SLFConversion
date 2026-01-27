# check_animbp_compile.py
# Check AnimBP compile status and Speed property binding

import unreal

ANIMBP = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def log(msg):
    print(msg)
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=== AnimBP Compile Check ===")

    asset = unreal.EditorAssetLibrary.load_asset(ANIMBP)
    if not asset:
        log(f"ERROR: Failed to load {ANIMBP}")
        return

    log(f"AnimBP: {asset.get_name()}")

    # Check compile errors using C++ automation library
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(asset)

    if errors:
        log(f"\n=== COMPILE ERRORS ===")
        log(errors)
    else:
        log("[OK] AnimBP compiles successfully!")

    # Check generated class properties
    log("")
    log("=== CHECKING GENERATED CLASS ===")
    gen_class = asset.generated_class()
    if gen_class:
        log(f"Generated Class: {gen_class.get_name()}")

        # Get CDO and check properties
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"CDO available: {cdo.get_name()}")

            # Check Speed property
            try:
                speed_val = cdo.get_editor_property('speed')
                log(f"[OK] Speed property accessible, default value: {speed_val}")
            except Exception as e:
                log(f"[ERROR] Speed property NOT accessible: {e}")

            # Check IsResting
            try:
                rest_val = cdo.get_editor_property('is_resting')
                log(f"[OK] IsResting property accessible, default value: {rest_val}")
            except Exception as e:
                log(f"[ERROR] IsResting property NOT accessible: {e}")

            # Check IsCrouched
            try:
                crouch_val = cdo.get_editor_property('is_crouched')
                log(f"[OK] IsCrouched property accessible, default value: {crouch_val}")
            except Exception as e:
                log(f"[ERROR] IsCrouched property NOT accessible: {e}")

    # Check Blueprint variables (should be empty after reparenting to C++)
    log("")
    log("=== BLUEPRINT VARIABLES (should be empty) ===")
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(asset)
    if bp_vars:
        for var in bp_vars:
            log(f"  - {var}")
        log(f"WARNING: Found {len(bp_vars)} Blueprint variables. These might conflict with C++ properties!")
    else:
        log("[OK] No Blueprint variables (all provided by C++ parent)")

if __name__ == "__main__":
    main()
