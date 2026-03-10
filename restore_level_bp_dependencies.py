#!/usr/bin/env python3
"""
Restore B_SequenceActor and GI_SoulslikeFramework from bp_only WITHOUT reparenting.

The Level Blueprint has serialized nodes that reference Blueprint variable FNames.
C++ property FNames cannot have spaces or "?" characters, so these two Blueprints
must remain as pure Blueprints to work with the Level Blueprint.

This is a surgical fix - only affects these two specific Blueprints.
"""

import unreal
import os
import shutil

def log(msg):
    print(f"[RestoreLevelDeps] {msg}")
    unreal.log_warning(f"[RestoreLevelDeps] {msg}")

# Assets that must remain as pure Blueprints for Level BP compatibility
LEVEL_BP_DEPENDENCIES = {
    # B_SequenceActor - Level BP uses SpawnActor with "Sequence to Play" and "Settings" pins
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor": {
        "src": "C:/scripts/bp_only/Content/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor.uasset",
        "dst": "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor.uasset",
    },
    # GI_SoulslikeFramework - Level BP uses Get node for "First Time on Demo Level?"
    "/Game/SoulslikeFramework/GameFramework/GI_SoulslikeFramework": {
        "src": "C:/scripts/bp_only/Content/SoulslikeFramework/GameFramework/GI_SoulslikeFramework.uasset",
        "dst": "C:/scripts/SLFConversion/Content/SoulslikeFramework/GameFramework/GI_SoulslikeFramework.uasset",
    },
}

def restore_assets():
    """Copy Blueprint assets from bp_only to SLFConversion without reparenting."""
    log("=" * 70)
    log("RESTORING LEVEL BLUEPRINT DEPENDENCIES FROM BP_ONLY")
    log("=" * 70)
    log("")
    log("These assets must remain as pure Blueprints because the Level Blueprint")
    log("has serialized node pins referencing FNames with spaces/special chars:")
    log("  - 'Sequence to Play' (B_SequenceActor)")
    log("  - 'Settings' (B_SequenceActor)")
    log("  - 'First Time on Demo Level?' (GI_SoulslikeFramework)")
    log("")

    success_count = 0
    fail_count = 0

    for asset_path, paths in LEVEL_BP_DEPENDENCIES.items():
        src = paths["src"]
        dst = paths["dst"]

        log(f"Restoring: {asset_path}")
        log(f"  From: {src}")
        log(f"  To: {dst}")

        if not os.path.exists(src):
            log(f"  ERROR: Source file not found!")
            fail_count += 1
            continue

        try:
            # Ensure destination directory exists
            dst_dir = os.path.dirname(dst)
            if not os.path.exists(dst_dir):
                os.makedirs(dst_dir)

            # Copy the file
            shutil.copy2(src, dst)
            log(f"  OK: Restored successfully")
            success_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            fail_count += 1

    log("")
    log("=" * 70)
    log(f"DONE - Restored: {success_count}, Failed: {fail_count}")
    log("")
    log("IMPORTANT: These Blueprints should NOT be reparented to C++!")
    log("The Level Blueprint requires the original Blueprint property FNames.")
    log("=" * 70)

    return success_count > 0

def main():
    restore_assets()

if __name__ == "__main__":
    main()
