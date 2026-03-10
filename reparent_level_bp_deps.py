#!/usr/bin/env python3
"""
Reparent B_SequenceActor and GI_SoulslikeFramework to their C++ parent classes.

This script:
1. Reparents B_SequenceActor to AB_SequenceActor
2. Reparents GI_SoulslikeFramework to UGI_SoulslikeFramework
3. Clears EventGraphs (logic is now in C++)
4. Compiles and reports errors
"""

import unreal

def log(msg):
    print(f"[ReparentLevelDeps] {msg}")
    unreal.log_warning(f"[ReparentLevelDeps] {msg}")

# Blueprints to reparent
# NOTE: Class paths from run_migration.py - these are the correct C++ parent classes
REPARENT_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor": {
        # AB_SequenceActor - Unreal strips "A" prefix for class registry
        # C++ class: AB_SequenceActor
        # Registry path: /Script/SLFConversion.B_SequenceActor
        "parent": "/Script/SLFConversion.B_SequenceActor",
        "clear_graphs": True,
    },
    "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework": {
        # USLFGameInstance - Unreal strips "U" prefix for class registry
        # C++ class: USLFGameInstance
        # Registry path: /Script/SLFConversion.SLFGameInstance
        "parent": "/Script/SLFConversion.SLFGameInstance",
        "clear_graphs": True,
    },
}

def reparent_and_clear(bp_path, config):
    """Reparent a Blueprint to C++ and clear its graphs."""
    parent_path = config["parent"]
    clear_graphs = config.get("clear_graphs", True)

    log(f"Processing: {bp_path}")
    log(f"  New parent: {parent_path}")

    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"  ERROR: Could not load Blueprint")
        return False

    log(f"  Loaded: {bp.get_name()}")

    # Get current parent class name
    try:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            parent_name = parent.get_name() if parent else "None"
        else:
            parent_name = "Unknown"
    except:
        parent_name = "Unknown"
    log(f"  Current parent: {parent_name}")

    # Reparent using SLFAutomationLibrary
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, parent_path)
        log(f"  Reparent result: {result}")

        if not result:
            log(f"  ERROR: Reparenting failed")
            return False

        # Reload after reparenting
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not reload after reparent")
            return False

        # Get new parent class name
        try:
            gen_class = bp.generated_class()
            if gen_class:
                parent = gen_class.get_super_class()
                parent_name = parent.get_name() if parent else "None"
            else:
                parent_name = "Unknown"
        except:
            parent_name = "Unknown"
        log(f"  New parent: {parent_name}")

        # Clear graphs if requested
        if clear_graphs:
            log(f"  Clearing EventGraph nodes...")
            clear_result = unreal.SLFAutomationLibrary.clear_all_event_graph_nodes(bp_path, True)
            log(f"  Clear result: {clear_result}")

        # Compile and save
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log(f"  Compile result: {compile_result}")

        # Get any compile errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors and len(errors) > 0:
            log(f"  Compile errors: {errors}")
        else:
            log(f"  No compile errors")

        return True

    except Exception as e:
        log(f"  ERROR: {e}")
        return False

def main():
    log("=" * 70)
    log("REPARENTING LEVEL BLUEPRINT DEPENDENCIES TO C++")
    log("=" * 70)
    log("")

    success_count = 0
    fail_count = 0

    for bp_path, config in REPARENT_MAP.items():
        if reparent_and_clear(bp_path, config):
            success_count += 1
        else:
            fail_count += 1
        log("")

    log("=" * 70)
    log(f"DONE - Success: {success_count}, Failed: {fail_count}")
    log("=" * 70)
    log("")
    log("Next step: Open the Level Blueprint (L_Demo_Showcase) and check for errors.")
    log("The Level BP may have stale pins that need to be reconnected to new C++ property names.")

if __name__ == "__main__":
    main()
