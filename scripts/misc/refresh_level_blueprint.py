#!/usr/bin/env python3
"""
Refresh Level Blueprint nodes to fix stale pin connections after C++ migration.

This script attempts to:
1. Load the Level Blueprint
2. Reconstruct nodes with stale pins
3. Save the asset

Run this after C++ changes that might break Level Blueprint pin connections.
"""

import unreal

def log(msg):
    print(f"[RefreshLevelBP] {msg}")
    unreal.log_warning(f"[RefreshLevelBP] {msg}")

LEVEL_BP_PATHS = [
    "/Game/SoulslikeFramework/Maps/L_Demo_Showcase",
]

def refresh_level_blueprint(level_path):
    """Attempt to refresh a level blueprint's nodes."""
    log(f"Processing: {level_path}")

    # Load the level
    level_asset = unreal.EditorAssetLibrary.load_asset(level_path)
    if not level_asset:
        log(f"  ERROR: Could not load level: {level_path}")
        return False

    log(f"  Loaded level: {level_asset.get_name()}")

    # Get the level's Blueprint (Level Script Actor)
    # The level blueprint is embedded in the level asset
    # We need to mark it as modified and compile it

    # Try to get the level blueprint
    if hasattr(level_asset, 'get_level_script_blueprint'):
        level_bp = level_asset.get_level_script_blueprint()
        if level_bp:
            log(f"  Found Level Script Blueprint: {level_bp.get_name()}")

            # Mark as modified
            if hasattr(unreal, 'EditorAssetLibrary'):
                unreal.EditorAssetLibrary.mark_dirty(level_path)
                log("  Marked level as dirty")

            # Try to compile the Blueprint
            if hasattr(unreal, 'BlueprintEditorLibrary'):
                try:
                    compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(level_bp)
                    log(f"  Compile result: {compile_result}")
                except Exception as e:
                    log(f"  Compile error: {e}")

            # Try to reconstruct nodes (this is the key function for refreshing)
            if hasattr(unreal, 'BlueprintEditorLibrary') and hasattr(unreal.BlueprintEditorLibrary, 'reconstruct_all_nodes'):
                try:
                    unreal.BlueprintEditorLibrary.reconstruct_all_nodes(level_bp)
                    log("  Reconstructed all nodes")
                except Exception as e:
                    log(f"  Reconstruct error: {e}")

            return True

    log("  Could not find Level Script Blueprint")
    return False

def main():
    log("=" * 70)
    log("REFRESHING LEVEL BLUEPRINTS")
    log("=" * 70)

    success_count = 0
    fail_count = 0

    for level_path in LEVEL_BP_PATHS:
        if refresh_level_blueprint(level_path):
            success_count += 1
        else:
            fail_count += 1

    log("")
    log("=" * 70)
    log(f"DONE - Success: {success_count}, Failed: {fail_count}")
    log("")
    log("If errors persist, open the Level Blueprint in editor and:")
    log("  1. Right-click problematic nodes (red X)")
    log("  2. Select 'Refresh Node'")
    log("  3. Re-wire any disconnected pins")
    log("  4. Save the Level")
    log("=" * 70)

if __name__ == "__main__":
    main()
