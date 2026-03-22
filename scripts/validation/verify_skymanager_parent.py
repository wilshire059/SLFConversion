"""
Verify B_SkyManager is properly reparented to SLFSkyManager
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("VERIFY B_SkyManager PARENT CLASS")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("ERROR: No generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Get parent class using different methods
    unreal.log_warning("\nParent class check:")

    # Method 1: Get parent from Blueprint directly
    try:
        parent_class = bp.get_editor_property('parent_class')
        if parent_class:
            unreal.log_warning(f"  Blueprint.parent_class: {parent_class.get_name()}")
        else:
            unreal.log_warning(f"  Blueprint.parent_class: None")
    except Exception as e:
        unreal.log_warning(f"  Blueprint.parent_class error: {e}")

    # Method 2: Check if gen_class has SLFSkyManager in parent chain
    unreal.log_warning("\nFull parent chain (via class hierarchy):")
    cpp_class = unreal.load_class(None, CPP_CLASS)
    if cpp_class:
        unreal.log_warning(f"  Target C++ class: {cpp_class.get_name()}")

        # Check if gen_class is child of SLFSkyManager
        try:
            is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
            unreal.log_warning(f"  B_SkyManager_C is child of SLFSkyManager: {is_child}")
        except Exception as e:
            unreal.log_warning(f"  class_is_child_of error: {e}")
    else:
        unreal.log_warning(f"  Could not load C++ class: {CPP_CLASS}")

    # If not properly parented, reparent it
    try:
        parent_class = bp.get_editor_property('parent_class')
        parent_name = parent_class.get_name() if parent_class else "None"

        if parent_name != "SLFSkyManager":
            unreal.log_warning(f"\n** Blueprint parent is {parent_name}, NOT SLFSkyManager **")
            unreal.log_warning("Attempting to reparent...")

            if cpp_class:
                success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
                unreal.log_warning(f"  Reparent result: {success}")

                if success:
                    # Recompile
                    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
                    unreal.log_warning("  Recompiled")

                    # Save
                    unreal.EditorAssetLibrary.save_asset(BP_PATH)
                    unreal.log_warning("  Saved")

                    # Verify
                    parent_class = bp.get_editor_property('parent_class')
                    unreal.log_warning(f"  New parent: {parent_class.get_name() if parent_class else 'None'}")
        else:
            unreal.log_warning(f"\n** Blueprint is correctly parented to SLFSkyManager **")

    except Exception as e:
        unreal.log_warning(f"Reparenting error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
