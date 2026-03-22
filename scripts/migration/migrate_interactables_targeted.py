"""
Targeted migration for B_Interactable and B_RestingPoint
Uses KEEP_VARS pattern to preserve Blueprint SCS (components with mesh assignments)
"""
import unreal

# Blueprints to migrate with their target C++ classes
KEEP_VARS_MAP = {
    "B_Interactable": "/Script/SLFConversion.SLFInteractableBase",
    "B_RestingPoint": "/Script/SLFConversion.SLFRestingPointBase",
}

# Blueprint paths
BP_PATHS = {
    "B_Interactable": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "B_RestingPoint": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
}


def migrate_interactables():
    """Migrate B_Interactable and B_RestingPoint using KEEP_VARS pattern"""

    unreal.log("=== Starting Interactables Migration ===")

    # Phase 1: Load and clear graphs (keep variables and SCS)
    unreal.log("")
    unreal.log("=== Phase 1: Clear Graphs (keep vars/SCS) ===")

    blueprints = {}
    for name in KEEP_VARS_MAP.keys():
        bp_path = BP_PATHS.get(name)
        if not bp_path:
            unreal.log(f"  ERROR: No path for {name}")
            continue

        unreal.log(f"Processing: {name}")

        # Load Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log(f"  ERROR: Could not load Blueprint")
            continue

        blueprints[name] = bp
        unreal.log(f"  Loaded: {bp.get_name()}")

        # Clear graphs but keep variables and SCS
        try:
            unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            unreal.log(f"  Cleared graphs (kept vars/SCS)")
        except Exception as e:
            unreal.log(f"  Clear error: {e}")
            continue

    # Phase 2: Save cleared Blueprints
    unreal.log("")
    unreal.log("=== Phase 2: Save Cleared Blueprints ===")

    for name, bp in blueprints.items():
        bp_path = BP_PATHS.get(name)
        try:
            result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log(f"  {name}: saved = {result}")
        except Exception as e:
            unreal.log(f"  {name}: save error = {e}")

    # Phase 2B: Remove Blueprint interface implementations
    # The C++ parent already implements the interfaces, so Blueprint implementations conflict
    unreal.log("")
    unreal.log("=== Phase 2B: Remove Blueprint Interface Implementations ===")

    for name, bp in blueprints.items():
        try:
            result = unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
            unreal.log(f"  {name}: removed {result} interfaces")
        except Exception as e:
            unreal.log(f"  {name}: remove interfaces error = {e}")

    # Phase 3: Reparent to C++ classes
    unreal.log("")
    unreal.log("=== Phase 3: Reparent to C++ Classes ===")

    for name, cpp_class_path in KEEP_VARS_MAP.items():
        bp = blueprints.get(name)
        if not bp:
            continue

        unreal.log(f"Reparenting: {name} -> {cpp_class_path}")

        # Load C++ class
        cpp_class = unreal.load_class(None, cpp_class_path)
        if not cpp_class:
            unreal.log(f"  ERROR: Could not load C++ class")
            continue

        # Reparent
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            unreal.log(f"  Reparent result: {result}")
        except Exception as e:
            unreal.log(f"  Reparent error: {e}")
            continue

    # Phase 4: Compile and save
    unreal.log("")
    unreal.log("=== Phase 4: Compile and Save ===")

    for name, bp in blueprints.items():
        bp_path = BP_PATHS.get(name)

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log(f"  {name}: compiled")
        except Exception as e:
            unreal.log(f"  {name}: compile error = {e}")

        # Save
        try:
            result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log(f"  {name}: saved = {result}")
        except Exception as e:
            unreal.log(f"  {name}: save error = {e}")

    unreal.log("")
    unreal.log("=== Migration Complete ===")


if __name__ == "__main__":
    migrate_interactables()
