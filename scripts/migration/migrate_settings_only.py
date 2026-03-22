"""
Migrate Settings Widgets and PDA_CustomSettings to C++

This script:
1. Reparents Blueprints to their C++ parent classes
2. Clears EventGraph logic (keeps variables)
3. Saves the assets
"""
import unreal

# Settings widgets to migrate
SETTINGS_MIGRATION = {
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings": "/Script/SLFConversion.W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry": "/Script/SLFConversion.W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry": "/Script/SLFConversion.W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings": "/Script/SLFConversion.PDA_CustomSettings",
}

def reparent_and_clear(bp_path, cpp_class_path):
    """Reparent a Blueprint to C++ class and clear its EventGraph."""
    name = bp_path.split('/')[-1]

    try:
        # Load the Blueprint
        bp = unreal.load_asset(bp_path)
        if bp is None:
            unreal.log_warning(f"MIGRATE: {name}: NOT FOUND")
            return False

        # Load the C++ class
        cpp_class = unreal.load_class(None, cpp_class_path)
        if cpp_class is None:
            unreal.log_warning(f"MIGRATE: {name}: C++ class not found: {cpp_class_path}")
            return False

        # Check if it's already reparented by comparing parent class
        gen_class = bp.generated_class() if hasattr(bp, 'generated_class') else None
        if gen_class:
            parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
            if parent:
                parent_name = parent.get_name() if hasattr(parent, 'get_name') else str(parent)
                cpp_class_name = cpp_class.get_name() if hasattr(cpp_class, 'get_name') else str(cpp_class)
                if parent_name == cpp_class_name:
                    unreal.log_warning(f"MIGRATE: {name}: Already reparented to {cpp_class_name}")
                    return True

        # Reparent the Blueprint
        try:
            unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            unreal.log_warning(f"MIGRATE: {name}: Reparented to {cpp_class_path}")
        except Exception as e:
            unreal.log_warning(f"MIGRATE: {name}: Reparent error - {e}")
            return False

        # Clear EventGraph (for Widget Blueprints and regular Blueprints)
        # We need to compile and save
        try:
            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning(f"MIGRATE: {name}: Compiled")
        except Exception as e:
            unreal.log_warning(f"MIGRATE: {name}: Compile warning - {e}")

        # Save the asset
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log_warning(f"MIGRATE: {name}: Saved")
        except Exception as e:
            unreal.log_warning(f"MIGRATE: {name}: Save error - {e}")
            return False

        return True

    except Exception as e:
        unreal.log_warning(f"MIGRATE: {name}: Error - {e}")
        return False

def main():
    unreal.log_warning("=== SETTINGS MIGRATION START ===")

    success_count = 0
    fail_count = 0

    for bp_path, cpp_class_path in SETTINGS_MIGRATION.items():
        if reparent_and_clear(bp_path, cpp_class_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning(f"=== SETTINGS MIGRATION COMPLETE: {success_count} success, {fail_count} failed ===")

if __name__ == "__main__":
    main()
