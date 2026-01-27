# migrate_interactables_v4.py
# Migrate B_Interactable and B_Container - FULL CLEAR both since all variables in C++

import unreal

# Both use FULL clear since C++ has all the variables
MIGRATION_ORDER = [
    # (Blueprint path, C++ class path)
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "/Script/SLFConversion.SLFInteractableBase"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container", "/Script/SLFConversion.B_Container"),
]


def migrate_blueprint(bp_path, cpp_class_path):
    """Migrate a single Blueprint to C++ parent"""
    bp_name = bp_path.split('/')[-1]
    unreal.log_warning(f"\n{'='*50}")
    unreal.log_warning(f"Migrating {bp_name}")
    unreal.log_warning(f"{'='*50}")

    # Load Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not load: {bp_path}")
        return False

    # Full clear using C++ automation library
    unreal.log_warning(f"  Full clear (including variables)...")
    try:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        unreal.log_warning(f"  Cleared all logic and variables")
    except Exception as e:
        unreal.log_error(f"  Clear error: {e}")
        return False

    # Remove interfaces
    unreal.log_warning(f"  Removing interfaces...")
    try:
        unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
    except Exception as e:
        unreal.log_warning(f"  Remove interfaces error: {e}")

    # Save
    unreal.log_warning(f"  Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    except:
        pass

    # Reload
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not reload Blueprint")
        return False

    # Load C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_error(f"  Could not load C++ class: {cpp_class_path}")
        return False
    unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

    # Reparent
    unreal.log_warning(f"  Reparenting...")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            unreal.log_warning(f"  SUCCESS - Reparented!")
        else:
            unreal.log_error(f"  FAILED - Reparent returned false")
            return False
    except Exception as e:
        unreal.log_error(f"  FAILED - Error: {e}")
        return False

    # Compile
    unreal.log_warning(f"  Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    except Exception as e:
        unreal.log_warning(f"  Compile note: {e}")

    # Final save
    unreal.log_warning(f"  Final save...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    except Exception as e:
        unreal.log_warning(f"  Save error: {e}")

    # Verify root component
    bp = unreal.load_asset(bp_path)
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

    return True


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("INTERACTABLES MIGRATION V4")
    unreal.log_warning("Full clear both Blueprints (variables now in C++)")
    unreal.log_warning("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_class_path in MIGRATION_ORDER:
        if migrate_blueprint(bp_path, cpp_class_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning(f"MIGRATION COMPLETE: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
