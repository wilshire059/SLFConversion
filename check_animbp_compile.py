# check_animbp_compile.py
# Check AnimBP compile status after surgical restore

import unreal

ANIMBP = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def log(msg):
    print(msg)
    unreal.log(msg)

def main():
    log("=== AnimBP Compile Check After Surgical Restore ===")

    asset = unreal.EditorAssetLibrary.load_asset(ANIMBP)
    if not asset:
        log(f"ERROR: Failed to load {ANIMBP}")
        return

    log(f"\nAnimBP: {asset.get_name()}")

    # Check compile errors using C++ automation library
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(asset)

    if errors:
        log(f"\n=== COMPILE ERRORS ===")
        log(errors)

        # Count error lines
        error_count = len([line for line in errors.split('\n') if line.strip()])
        log(f"\nTotal errors: {error_count}")
    else:
        log("\n[OK] AnimBP compiles successfully with 0 errors!")

    # Also run LinkedAnimLayer diagnosis
    log("\n=== LinkedAnimLayer Diagnosis ===")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_linked_anim_layer_nodes(asset)
    log(diagnosis)

if __name__ == "__main__":
    main()
