"""
Two-pass SkyManager migration using BlueprintEditorLibrary
Based on successful patterns from other migrations
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("SKYMANAGER MIGRATION V2")
    unreal.log_warning("=" * 70)

    # ========================================
    # PHASE 1: Load Blueprint
    # ========================================
    unreal.log_warning("\n[PHASE 1] Loading Blueprint...")

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"  Loaded: {bp.get_name()}")

    # Check CDO components before migration
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                comps = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"  CDO components BEFORE: {len(comps)}")
            except:
                pass

    # ========================================
    # PHASE 2: Reparent to C++ class
    # (EventGraph will become invalid but that's OK)
    # ========================================
    unreal.log_warning("\n[PHASE 2] Reparenting to C++ class...")

    cpp_class = unreal.load_class(None, CPP_CLASS)
    if not cpp_class:
        unreal.log_warning(f"  ERROR: Could not load {CPP_CLASS}")
        return

    unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log_warning(f"  Reparent result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Reparent error: {e}")
        return

    # ========================================
    # PHASE 3: Compile (this removes invalid nodes)
    # ========================================
    unreal.log_warning("\n[PHASE 3] Compiling...")

    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    # ========================================
    # PHASE 4: Save
    # ========================================
    unreal.log_warning("\n[PHASE 4] Saving...")

    try:
        result = unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning(f"  Save result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # ========================================
    # PHASE 5: Verify
    # ========================================
    unreal.log_warning("\n[PHASE 5] Verifying...")

    # Reload to get fresh state
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    gen_class = bp.generated_class()

    if gen_class:
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

        is_child = unreal.MathLibrary.class_is_child_of(gen_class, cpp_class)
        unreal.log_warning(f"  Is child of SLFSkyManager: {is_child}")

        # Check CDO components after migration
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                comps = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"  CDO components AFTER: {len(comps)}")
                for c in comps:
                    unreal.log_warning(f"    - {c.get_name()}: {c.get_class().get_name()}")
            except Exception as e:
                unreal.log_warning(f"  Component check error: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("DONE")
    unreal.log_warning("=" * 70)

main()
