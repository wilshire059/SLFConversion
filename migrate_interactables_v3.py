# migrate_interactables_v3.py
# Migrate B_Interactable and B_Container using SLFAutomationLibrary

import unreal

# Map Blueprint to C++ class - same as run_migration.py
KEEP_VARS = [
    # Blueprint name, BP path, C++ class
    ("B_Interactable", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "/Script/SLFConversion.SLFInteractableBase"),
]

REGULAR_MIGRATION = [
    ("B_Container", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container", "/Script/SLFConversion.B_Container"),
]


def migrate_keep_vars():
    """Migrate keep-vars Blueprints (preserve variables, clear graphs only)"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("MIGRATING KEEP-VARS BLUEPRINTS")
    unreal.log_warning("=" * 60)

    for name, bp_path, cpp_path in KEEP_VARS:
        unreal.log_warning(f"\n=== {name} ===")

        # Load Blueprint
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_error(f"  Could not load: {bp_path}")
            continue

        # Clear graphs but keep variables using C++ automation library
        unreal.log_warning(f"  Clearing graphs (keeping variables)...")
        try:
            unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            unreal.log_warning(f"  Cleared graphs")
        except Exception as e:
            unreal.log_error(f"  Clear error: {e}")
            continue

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
            continue

        # Load C++ class
        cpp_class = unreal.load_class(None, cpp_path)
        if not cpp_class:
            unreal.log_error(f"  Could not load C++ class: {cpp_path}")
            continue
        unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

        # Reparent
        unreal.log_warning(f"  Reparenting...")
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            if result:
                unreal.log_warning(f"  SUCCESS!")
            else:
                unreal.log_error(f"  FAILED - reparent returned false")
        except Exception as e:
            unreal.log_error(f"  FAILED: {e}")
            continue

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        except:
            pass

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        except:
            pass


def migrate_regular():
    """Migrate regular Blueprints (full clear)"""
    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("MIGRATING REGULAR BLUEPRINTS")
    unreal.log_warning("=" * 60)

    for name, bp_path, cpp_path in REGULAR_MIGRATION:
        unreal.log_warning(f"\n=== {name} ===")

        # Load Blueprint
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_error(f"  Could not load: {bp_path}")
            continue

        # Full clear using C++ automation library
        unreal.log_warning(f"  Full clear...")
        try:
            unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
            unreal.log_warning(f"  Cleared all logic")
        except Exception as e:
            unreal.log_error(f"  Clear error: {e}")
            continue

        # Remove interfaces
        try:
            unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
        except:
            pass

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        except:
            pass

        # Reload
        bp = unreal.load_asset(bp_path)
        if not bp:
            continue

        # Load C++ class
        cpp_class = unreal.load_class(None, cpp_path)
        if not cpp_class:
            unreal.log_error(f"  Could not load C++ class: {cpp_path}")
            continue
        unreal.log_warning(f"  C++ class: {cpp_class.get_name()}")

        # Reparent
        unreal.log_warning(f"  Reparenting...")
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            if result:
                unreal.log_warning(f"  SUCCESS!")
            else:
                unreal.log_error(f"  FAILED - reparent returned false")
        except Exception as e:
            unreal.log_error(f"  FAILED: {e}")
            continue

        # Compile
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        except:
            pass

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        except:
            pass


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("INTERACTABLES MIGRATION V3")
    unreal.log_warning("Using SLFAutomationLibrary")
    unreal.log_warning("=" * 60)

    # First migrate B_Interactable (keep-vars)
    migrate_keep_vars()

    # Then migrate B_Container (regular)
    migrate_regular()

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("MIGRATION COMPLETE")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
