"""
Resave B_SkyManager to ensure SCS components are properly linked after reparenting
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("RESAVING B_SkyManager")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Get generated class info
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")

        # Check parent
        try:
            parent = gen_class.get_super_class()
            if parent:
                unreal.log_warning(f"Parent class: {parent.get_name()}")
        except:
            pass

    # Force recompile the Blueprint
    unreal.log_warning("\nRecompiling Blueprint...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compile completed")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    # Save the Blueprint
    unreal.log_warning("\nSaving Blueprint...")
    try:
        success = unreal.EditorAssetLibrary.save_asset(BP_PATH)
        unreal.log_warning(f"  Save success: {success}")
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Also check and resave the level
    unreal.log_warning("\nResaving level L_Demo_Showcase...")
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        success = unreal.EditorAssetLibrary.save_asset(level_path)
        unreal.log_warning(f"  Level save success: {success}")
    except Exception as e:
        unreal.log_warning(f"  Level save error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
