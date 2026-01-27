"""
Sequential migration for B_Interactable and B_RestingPoint
Fully migrates B_Interactable first, then B_RestingPoint
"""
import unreal

# Blueprints to migrate in order
MIGRATION_ORDER = [
    {
        "name": "B_Interactable",
        "bp_path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
        "cpp_class": "/Script/SLFConversion.SLFInteractableBase"
    },
    {
        "name": "B_RestingPoint",
        "bp_path": "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
        "cpp_class": "/Script/SLFConversion.SLFRestingPointBase"
    },
]


def migrate_single(item):
    """Fully migrate a single Blueprint"""
    name = item["name"]
    bp_path = item["bp_path"]
    cpp_class_path = item["cpp_class"]

    unreal.log(f"=== Migrating: {name} ===")

    # Load Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log(f"  ERROR: Could not load Blueprint")
        return False

    unreal.log(f"  Loaded")

    # Step 1: Clear graphs
    try:
        unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
        unreal.log(f"  Cleared graphs")
    except Exception as e:
        unreal.log(f"  Clear error: {e}")
        return False

    # Step 2: Remove SCS components that conflict with C++ components
    try:
        # Remove DefaultSceneRoot - C++ parent now provides this
        result = unreal.SLFAutomationLibrary.remove_scs_component_by_class(bp, "SceneComponent")
        unreal.log(f"  Removed SCS SceneComponent: {result}")
    except Exception as e:
        unreal.log(f"  Remove SCS component error: {e}")

    # Step 3: Remove interfaces
    try:
        result = unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
        unreal.log(f"  Removed {result} interfaces")
    except Exception as e:
        unreal.log(f"  Remove interfaces error: {e}")

    # Step 3: Save after clearing
    try:
        result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        unreal.log(f"  Pre-reparent save: {result}")
    except Exception as e:
        unreal.log(f"  Pre-reparent save error: {e}")

    # Step 4: Load C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log(f"  ERROR: Could not load C++ class")
        return False

    # Step 5: Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log(f"  Reparent result: {result}")
    except Exception as e:
        unreal.log(f"  Reparent error: {e}")
        return False

    # Step 6: Compile
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log(f"  Compiled")
    except Exception as e:
        unreal.log(f"  Compile error: {e}")

    # Step 7: Final save
    try:
        result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        unreal.log(f"  Final save: {result}")
    except Exception as e:
        unreal.log(f"  Final save error: {e}")

    unreal.log(f"  DONE")
    return True


def migrate_interactables():
    """Migrate Blueprints sequentially"""

    unreal.log("=== Starting Sequential Migration ===")

    for item in MIGRATION_ORDER:
        success = migrate_single(item)
        if not success:
            unreal.log(f"  Migration failed for {item['name']}, stopping")
            break

    unreal.log("=== Migration Complete ===")


if __name__ == "__main__":
    migrate_interactables()
