"""
Delete B_SkyManager and create a fresh one inheriting from SLFSkyManager
This ensures no conflicting EventGraph nodes
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
BP_PACKAGE = "/Game/SoulslikeFramework/Blueprints/Sky/"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CREATE FRESH B_SkyManager")
    unreal.log_warning("=" * 70)

    # Step 1: Delete existing Blueprint
    unreal.log_warning("\nStep 1: Deleting existing Blueprint...")
    if unreal.EditorAssetLibrary.does_asset_exist(BP_PATH):
        try:
            deleted = unreal.EditorAssetLibrary.delete_asset(BP_PATH)
            unreal.log_warning(f"  Deleted: {deleted}")
        except Exception as e:
            unreal.log_warning(f"  Delete error: {e}")
    else:
        unreal.log_warning("  Blueprint doesn't exist")

    # Step 2: Load the C++ parent class
    cpp_class = unreal.load_class(None, CPP_CLASS)
    if not cpp_class:
        unreal.log_warning(f"ERROR: Could not load C++ class: {CPP_CLASS}")
        return
    unreal.log_warning(f"\nStep 2: Loaded C++ class: {cpp_class.get_name()}")

    # Step 3: Create a new Blueprint
    unreal.log_warning("\nStep 3: Creating new Blueprint...")
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('parent_class', cpp_class)
    
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    new_bp = asset_tools.create_asset("B_SkyManager", BP_PACKAGE, unreal.Blueprint, factory)
    
    if new_bp:
        unreal.log_warning(f"  Created: {new_bp.get_name()}")
        
        # Compile
        unreal.BlueprintEditorLibrary.compile_blueprint(new_bp)
        unreal.log_warning("  Compiled")
        
        # Save
        unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning("  Saved")
        
        # Verify parent
        gen_class = new_bp.generated_class()
        if gen_class:
            is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
            unreal.log_warning(f"\n  Is child of SLFSkyManager: {is_child}")
    else:
        unreal.log_warning("  ERROR: Failed to create Blueprint")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("DONE")
    unreal.log_warning("=" * 70)

main()
