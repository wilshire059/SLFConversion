"""
AnimBP Migration - Preserve Wiring Approach

This script:
1. Reparents AnimBP to C++ ABP_SoulslikeCharacter_Additive
2. Migrates BlendListByEnum nodes to C++ ESLFOverlayState enum
3. Fixes property access paths (? suffix -> no suffix)
4. Does NOT remove Blueprint variables (preserves transition wiring)

The key difference is that we let the C++ UPROPERTY variables shadow
the Blueprint variables. The AnimGraph property access nodes will find
the C++ properties, while the transition graph wiring stays intact.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
CPP_PARENT = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

OLD_ENUM_PATH = "/Game/SoulslikeFramework/Enums/E_OverlayState.E_OverlayState"
NEW_ENUM_PATH = "/Script/SLFConversion.ESLFOverlayState"


def log(msg):
    print(f"[MigratePreserve] {msg}")
    unreal.log_warning(f"[MigratePreserve] {msg}")


def main():
    log("=" * 70)
    log("ANIMBP MIGRATION - PRESERVE WIRING")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return False

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return False

    # Check current parent
    try:
        current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Current parent: {current_parent}")
    except:
        current_parent = "Unknown"

    # Step 1: Migrate BlendListByEnum to C++ enum
    log("")
    log("=== Step 1: Migrate BlendListByEnum to C++ Enum ===")
    try:
        migrated = unreal.SLFAutomationLibrary.migrate_blend_list_by_enum_to_cpp(
            bp, OLD_ENUM_PATH, NEW_ENUM_PATH)
        log(f"Migrated {migrated} BlendListByEnum nodes")
    except Exception as e:
        log(f"[WARN] Enum migration: {e}")

    # Step 2: Fix property access paths
    log("")
    log("=== Step 2: Fix Property Access Paths ===")

    property_renames = {
        "IsGuarding?": "IsGuarding",
        "bIsAccelerating?": "bIsAccelerating",
        "bIsBlocking?": "bIsBlocking",
        "bIsFalling?": "bIsFalling",
        "IsResting?": "IsResting",
    }

    total_fixes = 0
    for old_name, new_name in property_renames.items():
        try:
            fixed = unreal.SLFAutomationLibrary.fix_property_access_paths(bp, old_name, new_name)
            if fixed > 0:
                log(f"  Fixed '{old_name}' -> '{new_name}': {fixed}")
                total_fixes += fixed
        except Exception as e:
            log(f"  [WARN] {old_name}: {e}")

    log(f"Total property access fixes: {total_fixes}")

    # Step 3: Reparent to C++
    log("")
    log("=== Step 3: Reparent to C++ ===")

    if "ABP_SoulslikeCharacter_Additive" not in current_parent:
        log(f"Reparenting to {CPP_PARENT}...")
        try:
            success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_PARENT)
            if success:
                log("[OK] Reparented")
            else:
                log("[WARN] Reparent returned False")
        except Exception as e:
            log(f"[ERROR] Reparent failed: {e}")
    else:
        log("[OK] Already parented to C++")

    # Step 4: Compile
    log("")
    log("=== Step 4: Compile ===")

    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("Compile completed")
    except Exception as e:
        log(f"[WARN] Compile: {e}")

    # Check warnings
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        warning_count = errors.count("will never be taken")
        log(f"'will never be taken' warnings: {warning_count}")

        if warning_count > 0:
            log("[INFO] Transitions need repair - running fix...")
            # Run transition repair
            try:
                repaired = unreal.SLFAutomationLibrary.repair_transition_wiring(bp)
                log(f"Repaired {repaired} transition connections")
            except:
                pass

            try:
                fixed = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
                log(f"Fixed {fixed} broken transitions")
            except:
                pass

            # Recompile
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Step 5: Save
    log("")
    log("=== Step 5: Save ===")

    try:
        saved = unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH, only_if_is_dirty=False)
        log(f"Save result: {saved}")
    except Exception as e:
        log(f"[ERROR] Save failed: {e}")

    # Final check
    log("")
    log("=== Final Status ===")

    bp = unreal.load_asset(ANIMBP_PATH)

    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent: {parent}")
    except:
        pass

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        warning_count = errors.count("will never be taken")
        log(f"Remaining warnings: {warning_count}")

        # Show any remaining issues
        if warning_count > 0:
            for line in errors.split('\n'):
                if "will never be taken" in line:
                    log(f"  {line.strip()}")

    log("")
    log("=" * 70)
    log("MIGRATION COMPLETE")
    log("=" * 70)

    return True


if __name__ == "__main__":
    main()
