# diagnose_animlayers.py
# Diagnose Animation Layer issues in ABP_SoulslikeCharacter_Additive

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/animlayer_diagnosis.txt"

def log(msg):
    print(f"[AnimLayerDiag] {msg}")
    unreal.log(f"[AnimLayerDiag] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

# Clear output
with open(OUTPUT_FILE, "w") as f:
    f.write("")

log("=" * 60)
log("ANIMATION LAYER DIAGNOSIS")
log("=" * 60)

ABP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

abp = unreal.EditorAssetLibrary.load_asset(ABP_PATH)
if not abp:
    log(f"ERROR: Could not load {ABP_PATH}")
else:
    log(f"Loaded: {abp.get_name()}")
    log(f"Class: {abp.get_class().get_name()}")

    # Check if it implements any interfaces
    log("\n--- INTERFACES ---")
    try:
        # Get implemented interfaces
        interfaces = abp.get_editor_property("implemented_interfaces")
        log(f"Implemented interfaces: {interfaces}")
    except Exception as e:
        log(f"Interface error: {e}")

    # Check skeleton
    log("\n--- SKELETON ---")
    try:
        skeleton = abp.get_editor_property("target_skeleton")
        if skeleton:
            log(f"Target Skeleton: {skeleton.get_name()}")
        else:
            log("Target Skeleton: None")
    except Exception as e:
        log(f"Skeleton error: {e}")

    # Check parent class
    log("\n--- PARENT CLASS ---")
    try:
        parent = abp.get_editor_property("parent_class")
        if parent:
            log(f"Parent Class: {parent.get_name()}")
        else:
            log("Parent Class: None")
    except Exception as e:
        log(f"Parent error: {e}")

    # Try to get all graph names using automation library
    log("\n--- GRAPHS (via automation) ---")
    try:
        functions = unreal.SLFAutomationLibrary.get_blueprint_functions(abp)
        log(f"Functions ({len(functions)}): {functions}")
    except Exception as e:
        log(f"Functions error: {e}")

    # Get variables
    log("\n--- VARIABLES ---")
    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(abp)
        log(f"Variables ({len(variables)}): {variables[:10]}...")
    except Exception as e:
        log(f"Variables error: {e}")

    # Check the Animation Layer Interface assets
    log("\n" + "=" * 60)
    log("CHECKING ANIMATION LAYER INTERFACES")
    log("=" * 60)

    ALI_PATHS = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/_Misc/ALI_LocomotionStates",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/_Misc/ALI_OverlayStates",
    ]

    for ali_path in ALI_PATHS:
        ali = unreal.EditorAssetLibrary.load_asset(ali_path)
        if ali:
            log(f"\n{ali_path}")
            log(f"  Name: {ali.get_name()}")
            log(f"  Class: {ali.get_class().get_name()}")
        else:
            log(f"\n{ali_path}: NOT FOUND")

log("\n" + "=" * 60)
log("COMPLETE")
log("=" * 60)
log(f"Full log saved to: {OUTPUT_FILE}")
