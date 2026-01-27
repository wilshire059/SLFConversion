# check_loot_manager_parent.py
# Check what AC_LootDropManager Blueprint inherits from

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING AC_LootDropManager PARENT CLASS")
    unreal.log_warning("=" * 70)

    # Try to find the AC_LootDropManager Blueprint
    possible_paths = [
        "/Game/SoulslikeFramework/Blueprints/_Components/AC_LootDropManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager",
        "/Game/SoulslikeFramework/Components/AC_LootDropManager",
    ]

    bp = None
    for path in possible_paths:
        bp = unreal.load_asset(path)
        if bp:
            unreal.log_warning(f"Found at: {path}")
            break

    if not bp:
        # Search for it
        unreal.log_warning("Searching for AC_LootDropManager...")
        asset_reg = unreal.AssetRegistryHelpers.get_asset_registry()
        all_assets = asset_reg.get_all_assets()
        for asset in all_assets:
            name = str(asset.asset_name)
            if "lootdropmanager" in name.lower():
                unreal.log_warning(f"  Found: {asset.object_path}")
                bp = unreal.load_asset(str(asset.object_path))
                if bp:
                    break

    if not bp:
        unreal.log_error("Could not find AC_LootDropManager Blueprint")
        return

    # Check parent class
    unreal.log_warning(f"\nBlueprint: {bp.get_name()}")
    unreal.log_warning(f"Class: {bp.get_class().get_name()}")

    try:
        parent = bp.get_editor_property("parent_class")
        if parent:
            parent_name = parent.get_name()
            unreal.log_warning(f"Parent class: {parent_name}")

            # Check if it's our C++ class
            if "UAC_LootDropManager" in parent_name or parent_name == "AC_LootDropManager":
                unreal.log_warning("*** Parent is the C++ UAC_LootDropManager class ***")
            else:
                unreal.log_warning(f"*** Parent is NOT UAC_LootDropManager - it's {parent_name} ***")
                unreal.log_warning("*** Cast will FAIL! Need to reparent the Blueprint ***")
        else:
            unreal.log_warning("Parent class: None")
    except Exception as e:
        unreal.log_warning(f"Error getting parent: {e}")

    # Also check generated class
    try:
        gen_class = bp.generated_class()
        if gen_class:
            unreal.log_warning(f"\nGenerated class: {gen_class.get_name()}")

            # Walk up parent chain
            unreal.log_warning("\nClass hierarchy:")
            current = gen_class
            depth = 0
            while current and depth < 10:
                unreal.log_warning(f"  {'  ' * depth}{current.get_name()}")
                current = current.get_super_class()
                depth += 1
    except Exception as e:
        unreal.log_warning(f"Error getting generated class: {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
