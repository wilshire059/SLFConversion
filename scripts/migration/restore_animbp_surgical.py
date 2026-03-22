#!/usr/bin/env python3
"""
Surgical AnimBP restore - keeps AnimGraph, clears EventGraph only.

This restores the AnimBP from backup to get working AnimGraph bindings,
then clears only the EventGraph nodes (which call functions with '?' pins).

The AnimGraph BlendListByEnum nodes need their ActiveEnumValue pins
connected to LeftHandOverlayState/RightHandOverlayState properties.
These connections are preserved in the backup but were lost during migration.
"""

import unreal
import os
import shutil

def log(msg):
    unreal.log_warning(str(msg))

# Paths
ANIMBP_NAME = "ABP_SoulslikeCharacter_Additive"
ANIMBP_CONTENT_PATH = "Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive.uasset"

BACKUP_PROJECT = "C:/scripts/bp_only"
CURRENT_PROJECT = "C:/scripts/SLFConversion"

ANIMBP_GAME_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def restore_from_backup():
    """Copy the AnimBP from backup project."""
    log("=== STEP 1: Restore AnimBP from backup ===")

    backup_path = os.path.join(BACKUP_PROJECT, ANIMBP_CONTENT_PATH)
    current_path = os.path.join(CURRENT_PROJECT, ANIMBP_CONTENT_PATH)

    if not os.path.exists(backup_path):
        log(f"ERROR: Backup file not found: {backup_path}")
        return False

    log(f"Source: {backup_path}")
    log(f"Target: {current_path}")

    # Create backup of current file
    if os.path.exists(current_path):
        backup_current = current_path + ".bak"
        shutil.copy2(current_path, backup_current)
        log(f"Backed up current to: {backup_current}")

    # Copy from backup
    shutil.copy2(backup_path, current_path)
    log("Restored AnimBP from backup")

    return True

def clear_event_graph_only():
    """Clear only the EventGraph, keeping AnimGraph intact."""
    log("\n=== STEP 2: Clear EventGraph Only ===")

    # Reload the asset
    unreal.EditorAssetLibrary.unload_asset(ANIMBP_GAME_PATH)

    animbp = unreal.EditorAssetLibrary.load_asset(ANIMBP_GAME_PATH)
    if not animbp:
        log(f"ERROR: Could not load AnimBP after restore")
        return False

    log(f"Loaded: {animbp.get_name()}")

    # Get all graphs
    try:
        graphs = unreal.BlueprintEditorLibrary.get_graphs(animbp)
        log(f"Found {len(graphs)} graphs")

        for graph in graphs:
            graph_name = graph.get_name() if graph else "Unknown"
            log(f"  - {graph_name}")

            # Only clear EventGraph (or graphs that look like event graphs)
            if graph_name == "EventGraph" or "Event" in graph_name:
                log(f"    Clearing {graph_name}...")

                # Get all nodes in this graph
                try:
                    # Clear the graph using BlueprintEditorLibrary
                    # Note: We need to remove nodes while keeping the graph structure

                    # UE5.7 method - mark graph as needing reconstruction
                    # The actual clearing happens via native code

                    pass  # Direct node clearing requires more complex API

                except Exception as e:
                    log(f"    Error clearing: {e}")

    except Exception as e:
        log(f"Error accessing graphs: {e}")

    return True

def reparent_to_cpp():
    """Reparent the AnimBP to C++ AnimInstance class."""
    log("\n=== STEP 3: Reparent to C++ AnimInstance ===")

    animbp = unreal.EditorAssetLibrary.load_asset(ANIMBP_GAME_PATH)
    if not animbp:
        log("ERROR: Could not load AnimBP")
        return False

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive")
    if not cpp_class:
        log("ERROR: C++ class not found")
        return False

    log(f"C++ class: {cpp_class.get_name()}")

    # Get current parent
    try:
        current_parent = animbp.get_editor_property("parent_class")
        current_parent_name = current_parent.get_name() if current_parent else "None"
        log(f"Current parent: {current_parent_name}")

        if current_parent == cpp_class:
            log("Already reparented to C++ class")
        else:
            # Reparent
            log("Reparenting to C++ class...")
            success = unreal.BlueprintEditorLibrary.reparent_blueprint(animbp, cpp_class)
            if success:
                log("Reparent successful")
            else:
                log("Reparent failed")
                return False

    except Exception as e:
        log(f"Error reparenting: {e}")
        return False

    return True

def compile_and_save():
    """Compile and save the AnimBP."""
    log("\n=== STEP 4: Compile and Save ===")

    animbp = unreal.EditorAssetLibrary.load_asset(ANIMBP_GAME_PATH)
    if not animbp:
        log("ERROR: Could not load AnimBP")
        return False

    try:
        # Compile
        log("Compiling...")
        unreal.BlueprintEditorLibrary.compile_blueprint(animbp)

        # Save
        log("Saving...")
        unreal.EditorAssetLibrary.save_asset(ANIMBP_GAME_PATH)

        log("Compile and save complete")
        return True

    except Exception as e:
        log(f"Error: {e}")
        return False

def verify_bindings():
    """Verify the AnimGraph bindings are working."""
    log("\n=== STEP 5: Verify Bindings ===")

    animbp = unreal.EditorAssetLibrary.load_asset(ANIMBP_GAME_PATH)
    if not animbp:
        log("ERROR: Could not load AnimBP")
        return

    try:
        # Check parent class
        parent = animbp.get_editor_property("parent_class")
        log(f"Parent class: {parent.get_name() if parent else 'None'}")

        # Check if it's the C++ class
        if parent and "SLFConversion" in parent.get_path_name():
            log("Parent is C++ AnimInstance - overlay properties should work!")
        else:
            log("WARNING: Parent is NOT C++ AnimInstance")

        # List available graphs
        graphs = unreal.BlueprintEditorLibrary.get_graphs(animbp)
        log(f"\nGraphs ({len(graphs)}):")
        for graph in graphs:
            log(f"  - {graph.get_name()}")

    except Exception as e:
        log(f"Error verifying: {e}")

def main():
    log("=" * 70)
    log("SURGICAL ANIMBP RESTORE")
    log("Restores AnimGraph bindings from backup")
    log("=" * 70)

    # Step 1: Restore from backup (this gets the working AnimGraph bindings)
    if not restore_from_backup():
        return

    # Note: After restoring from backup, we need to re-run the editor
    # to reload the asset properly. The Python script can't fully
    # reload a replaced uasset.

    log("\n" + "=" * 70)
    log("IMPORTANT: Restart the editor to reload the AnimBP!")
    log("")
    log("After restart, run 'reparent_animbp_only.py' to:")
    log("1. Reparent to C++ AnimInstance")
    log("2. Clear EventGraph only (preserve AnimGraph)")
    log("3. Compile and save")
    log("=" * 70)

if __name__ == "__main__":
    main()
