# migrate_b_container_only.py
# Migrate ONLY B_Container (run AFTER B_Interactable is saved)

import unreal

BP_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
CPP_PATH = "/Script/SLFConversion.B_Container"


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("MIGRATING B_CONTAINER ONLY")
    unreal.log_warning("=" * 60)

    # Load Blueprint
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        unreal.log_error(f"Could not load: {BP_PATH}")
        return

    # Full clear
    unreal.log_warning("  Full clear...")
    try:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        unreal.log_warning("  Cleared all logic")
    except Exception as e:
        unreal.log_error(f"  Clear error: {e}")
        return

    # Remove interfaces
    unreal.log_warning("  Removing interfaces...")
    try:
        unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
    except Exception as e:
        unreal.log_warning(f"  Remove interfaces error: {e}")

    # Save cleared Blueprint
    unreal.log_warning("  Saving cleared Blueprint...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH, only_if_is_dirty=False)
    except:
        pass

    # Reload
    bp = unreal.load_asset(BP_PATH)
    if not bp:
        unreal.log_error("Could not reload Blueprint")
        return

    unreal.log_warning(f"  Target C++ class: {CPP_PATH}")

    # Reparent using SLFAutomationLibrary (skips validation/compile)
    unreal.log_warning("  Reparenting (via SLFAutomationLibrary)...")
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, CPP_PATH)
        if result:
            unreal.log_warning("  SUCCESS - Reparented!")
        else:
            unreal.log_error("  FAILED - Reparent returned false")
            return
    except Exception as e:
        unreal.log_error(f"  FAILED - Error: {e}")
        return

    # Compile and save
    unreal.log_warning("  Compiling and saving...")
    try:
        unreal.SLFAutomationLibrary.compile_and_save(bp)
    except Exception as e:
        unreal.log_warning(f"  Compile/save note: {e}")

    # Final save
    unreal.log_warning("  Final save...")
    try:
        unreal.EditorAssetLibrary.save_asset(BP_PATH, only_if_is_dirty=False)
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Verify
    bp = unreal.load_asset(BP_PATH)
    if bp:
        try:
            gen_class = bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        root = cdo.get_editor_property('root_component')
                        root_name = root.get_name() if root else "None"
                        unreal.log_warning(f"  Root component: {root_name}")
                    except:
                        pass
        except:
            pass

    unreal.log_warning("=" * 60)
    unreal.log_warning("B_CONTAINER MIGRATION COMPLETE")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
