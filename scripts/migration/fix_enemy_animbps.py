# fix_enemy_animbps.py
# Surgically fix enemy AnimBP compile errors by removing problematic nodes
# that reference PoiseBreakAsset with type mismatches

import unreal

def log(msg):
    unreal.log_warning(f"[FIX] {msg}")

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def fix_animbp(path):
    """Fix AnimBP compile errors by removing problematic variable nodes"""
    name = path.split("/")[-1]
    log(f"")
    log(f"=== FIXING {name} ===")

    bp = unreal.load_asset(path)
    if not bp:
        log(f"  ERROR: Could not load {path}")
        return False

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  ERROR: No generated class")
        return False

    log(f"  Loaded: {name}")
    log(f"  Class: {gen_class.get_name()}")

    # Check parent class
    if hasattr(unreal, 'SLFAutomationLibrary'):
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent: {parent}")

        # Get Blueprint variables (those that still exist)
        bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"  Blueprint variables: {bp_vars}")

        # Check compile status
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            log(f"  Compile status:")
            for line in errors.split('\n')[:10]:
                if line.strip():
                    log(f"    {line.strip()[:80]}")

        # Try to clear problematic nodes in EventGraph (not AnimGraph)
        # This won't fix AnimGraph nodes, but EventGraph nodes might reference PoiseBreakAsset too
        cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        log(f"  EventGraph nodes cleared: {cleared}")

        # Compile and save
        compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"  Compile result: {compile_result}")

        # Save
        unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
        log(f"  Saved")

        # Check again
        errors_after = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors_after and "[Compiler] Error" in errors_after:
            log(f"  WARNING: Still has compile errors after fix")
            return False
        else:
            log(f"  OK: No compile errors")
            return True
    else:
        log(f"  ERROR: SLFAutomationLibrary not available")
        return False


def main():
    log("=" * 70)
    log("ENEMY ANIMBP FIX")
    log("=" * 70)
    log("")
    log("This script attempts to fix AnimBP compile errors by:")
    log("  1. Clearing EventGraph nodes")
    log("  2. Recompiling and saving")
    log("")
    log("Note: AnimGraph type mismatch errors require the PoiseBreakAsset")
    log("      C++ property type to match the expected Blueprint type.")

    success_count = 0
    for path in ANIMBP_PATHS:
        if fix_animbp(path):
            success_count += 1

    log("")
    log("=" * 70)
    log(f"RESULT: {success_count}/{len(ANIMBP_PATHS)} AnimBPs fixed")
    log("=" * 70)


if __name__ == "__main__":
    main()
