"""
Migrate AnimBP BlendListByEnum nodes from Blueprint E_OverlayState to C++ ESLFOverlayState.

This script:
1. Loads the AnimBP from the restored backup (bp_only)
2. Migrates all BlendListByEnum nodes that use E_OverlayState to ESLFOverlayState
3. Saves the AnimBP

The C++ function handles:
- Saving all pin connections
- Calling ReloadEnum() with the C++ enum
- Reconstructing the node
- Restoring all pin connections
"""

import unreal

def log(msg):
    unreal.log_warning(f"[MigrateOverlay] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OLD_ENUM_PATH = "/Game/SoulslikeFramework/Enums/E_OverlayState.E_OverlayState"
NEW_ENUM_PATH = "/Script/SLFConversion.ESLFOverlayState"

def main():
    log("=" * 70)
    log("MIGRATE OVERLAY ENUM TO C++")
    log("=" * 70)

    # Check if SLFAutomationLibrary is available
    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available - rebuild required")
        return

    # Load the AnimBP
    log(f"Loading AnimBP: {ANIMBP_PATH}")
    animbp = unreal.load_asset(ANIMBP_PATH)
    if not animbp:
        log(f"[ERROR] Could not load AnimBP at {ANIMBP_PATH}")
        return

    log(f"Loaded: {animbp.get_name()}")

    # Call the migration function
    log("")
    log(f"Migrating BlendListByEnum nodes from:")
    log(f"  Old: {OLD_ENUM_PATH}")
    log(f"  New: {NEW_ENUM_PATH}")
    log("")

    migrated_count = unreal.SLFAutomationLibrary.migrate_blend_list_by_enum_to_cpp(
        animbp,
        OLD_ENUM_PATH,
        NEW_ENUM_PATH
    )

    log("")
    log(f"Result: Migrated {migrated_count} BlendListByEnum nodes")

    if migrated_count > 0:
        log("[OK] Migration complete - AnimBP saved")
    else:
        log("[INFO] No nodes needed migration (already using C++ enum or different enum)")

    log("=" * 70)

if __name__ == "__main__":
    main()
