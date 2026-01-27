"""
Reparent B_SkyManager to SLFSkyManager C++ class
Using BlueprintEditorLibrary API properly
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REPARENT B_SkyManager TO SLFSkyManager")
    unreal.log_warning("=" * 70)

    # Load Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load Blueprint: {BP_PATH}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # Get current parent via generated class
    gen_class = bp.generated_class()
    if gen_class:
        try:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"Current parent: {parent.get_name() if parent else 'None'}")
        except:
            unreal.log_warning("Could not get current parent")

    # Load target C++ class
    cpp_class = unreal.load_class(None, CPP_CLASS)
    if not cpp_class:
        unreal.log_warning(f"ERROR: Could not load C++ class: {CPP_CLASS}")
        return

    unreal.log_warning(f"Target C++ class: {cpp_class.get_name()}")

    # Reparent using BlueprintEditorLibrary
    unreal.log_warning("\nReparenting...")
    try:
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log_warning(f"Reparent result: {success}")
    except Exception as e:
        unreal.log_warning(f"Reparent error: {e}")
        return

    if not success:
        unreal.log_warning("ERROR: Reparenting failed")
        return

    # Compile
    unreal.log_warning("\nCompiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("Compiled successfully")
    except Exception as e:
        unreal.log_warning(f"Compile error: {e}")

    # Save
    unreal.log_warning("\nSaving...")
    try:
        saved = unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning(f"Saved: {saved}")
    except Exception as e:
        unreal.log_warning(f"Save error: {e}")

    # Verify
    unreal.log_warning("\nVerifying...")
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    gen_class = bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        unreal.log_warning(f"New parent: {parent.get_name() if parent else 'None'}")

        # Check if now child of SLFSkyManager
        is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
        unreal.log_warning(f"Is child of SLFSkyManager: {is_child}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("DONE")
    unreal.log_warning("=" * 70)

main()
