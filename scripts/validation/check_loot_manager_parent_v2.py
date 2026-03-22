# check_loot_manager_parent_v2.py
# Check AC_LootDropManager parent class via export

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING AC_LootDropManager PARENT CLASS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    # Export text
    try:
        export_text = unreal.EditorAssetLibrary.export_asset_to_string(bp_path)
        if export_text:
            # Search for ParentClass or parent info
            lines = export_text.split('\n')
            for i, line in enumerate(lines):
                if 'parentclass' in line.lower() or 'parent' in line.lower() and 'class' in line.lower():
                    unreal.log_warning(f"Line {i}: {line[:200]}")
    except Exception as e:
        unreal.log_warning(f"Export error: {e}")

    # Check C++ class
    unreal.log_warning("\n--- Checking C++ class UAC_LootDropManager ---")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.AC_LootDropManager")
        if cpp_class:
            unreal.log_warning(f"C++ class found: {cpp_class.get_name()}")
        else:
            unreal.log_warning("C++ class NOT found at /Script/SLFConversion.AC_LootDropManager")
    except Exception as e:
        unreal.log_warning(f"Error loading C++ class: {e}")

    # Try ULootDropManagerComponent instead
    try:
        cpp_class2 = unreal.load_class(None, "/Script/SLFConversion.LootDropManagerComponent")
        if cpp_class2:
            unreal.log_warning(f"Alt C++ class found: {cpp_class2.get_name()}")
    except:
        pass

    unreal.log_warning("\n" + "=" * 70)

if __name__ == "__main__":
    main()
