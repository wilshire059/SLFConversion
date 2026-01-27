"""
Refresh B_SkyManager from bp_only and reparent to C++
This ensures SCS components are properly preserved after reparenting
"""
import unreal
import os
import shutil

BP_ONLY_ASSET = "C:/scripts/bp_only/Content/SoulslikeFramework/Blueprints/Sky/B_SkyManager.uasset"
SLFCONV_ASSET = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/Sky/B_SkyManager.uasset"
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("REFRESH B_SkyManager FROM BP_ONLY")
    unreal.log_warning("=" * 70)

    # Step 1: Check if bp_only asset exists
    if not os.path.exists(BP_ONLY_ASSET):
        unreal.log_warning(f"ERROR: bp_only asset not found: {BP_ONLY_ASSET}")
        return

    unreal.log_warning(f"Source: {BP_ONLY_ASSET}")
    unreal.log_warning(f"Destination: {SLFCONV_ASSET}")

    # Step 2: Copy the asset
    try:
        shutil.copy2(BP_ONLY_ASSET, SLFCONV_ASSET)
        unreal.log_warning("  Copied successfully")
    except Exception as e:
        unreal.log_warning(f"  Copy error: {e}")
        return

    # Step 3: Reload the asset registry
    unreal.log_warning("\nReloading asset...")
    try:
        # Force reload
        bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
        if not bp:
            unreal.log_warning("  ERROR: Could not reload Blueprint")
            return
        unreal.log_warning(f"  Reloaded: {bp.get_name()}")
    except Exception as e:
        unreal.log_warning(f"  Reload error: {e}")
        return

    # Step 4: Get current parent
    try:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"  Current parent: {parent.get_name() if parent else 'None'}")
    except Exception as e:
        unreal.log_warning(f"  Parent check error: {e}")

    # Step 5: Reparent to C++ class
    unreal.log_warning("\nReparenting to SLFSkyManager...")
    try:
        cpp_class = unreal.load_class(None, CPP_CLASS)
        if cpp_class:
            unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")
            success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            unreal.log_warning(f"  Reparent result: {success}")
        else:
            unreal.log_warning(f"  ERROR: Could not load C++ class")
            return
    except Exception as e:
        unreal.log_warning(f"  Reparent error: {e}")
        return

    # Step 6: Compile
    unreal.log_warning("\nCompiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    # Step 7: Save
    unreal.log_warning("\nSaving...")
    try:
        success = unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning(f"  Save result: {success}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Step 8: Verify parent
    unreal.log_warning("\nVerifying...")
    try:
        bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"  Final parent: {parent.get_name() if parent else 'None'}")

            # Check if child of SLFSkyManager
            cpp_class = unreal.load_class(None, CPP_CLASS)
            is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
            unreal.log_warning(f"  Is child of SLFSkyManager: {is_child}")
    except Exception as e:
        unreal.log_warning(f"  Verify error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("DONE - Please run PIE to test")
    unreal.log_warning("=" * 70)

main()
