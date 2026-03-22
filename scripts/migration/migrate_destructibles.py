# migrate_destructibles.py
# Migrate destructible-related Blueprints to C++ classes

import unreal

# Migration map: (Blueprint path, C++ class path)
MIGRATION_MAP = [
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField", "/Script/SLFConversion.B_Chaos_ForceField"),
    ("/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible", "/Script/SLFConversion.B_Destructible"),
]


def migrate_blueprint(bp_path, cpp_path):
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
    unreal.log_warning(f"  Full clear...")
    try:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        unreal.log_warning(f"  Cleared all logic")
    except Exception as e:
        unreal.log_error(f"  Clear error: {e}")
        return False

    # Remove interfaces (now implemented in C++)
    unreal.log_warning(f"  Removing interfaces...")
    try:
        unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
    except Exception as e:
        unreal.log_warning(f"  Remove interfaces note: {e}")

    # Save
    unreal.log_warning(f"  Saving cleared Blueprint...")
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    except:
        pass

    # Reload
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"  Could not reload Blueprint")
        return False

    unreal.log_warning(f"  Target C++ class: {cpp_path}")

    # Reparent using SLFAutomationLibrary
    unreal.log_warning(f"  Reparenting...")
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
        if result:
            unreal.log_warning(f"  SUCCESS - Reparented!")
        else:
            unreal.log_error(f"  FAILED - Reparent returned false")
            return False
    except Exception as e:
        unreal.log_error(f"  FAILED - Error: {e}")
        return False

    # Compile and save
    unreal.log_warning(f"  Compiling and saving...")
    try:
        unreal.SLFAutomationLibrary.compile_and_save(bp)
    except Exception as e:
        unreal.log_warning(f"  Compile/save note: {e}")

    # Final save
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
    except:
        pass

    # Verify
    bp = unreal.load_asset(bp_path)
    if bp:
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

    return True


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("DESTRUCTIBLES MIGRATION")
    unreal.log_warning("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_path in MIGRATION_MAP:
        if migrate_blueprint(bp_path, cpp_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning(f"MIGRATION COMPLETE: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
