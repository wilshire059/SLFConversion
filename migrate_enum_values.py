# migrate_enum_values.py
# Converts E_ValueType enum values from Blueprint names to C++ names
#
# Before: Pin default value = "NewEnumerator0" or "NewEnumerator1"
# After:  Pin default value = "CurrentValue" or "MaxValue"
#
# This prepares Blueprints for E_ValueType deletion by making pins
# compatible with the C++ ESLFValueType enum.
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\migrate_enum_values.py").read())

import unreal

# Mapping from Blueprint enum values to C++ enum values
ENUM_VALUE_MAP = {
    "NewEnumerator0": "CurrentValue",
    "NewEnumerator1": "MaxValue",
    # Also handle if they're stored as display names
    "Current Value": "CurrentValue",
    "Max Value": "MaxValue",
}

def get_all_blueprints():
    """Get all Blueprint assets in the project."""
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get all Blueprint assets
    filter = unreal.ARFilter()
    filter.class_names = ["Blueprint", "WidgetBlueprint", "AnimBlueprint"]
    filter.recursive_paths = True
    filter.package_paths = ["/Game/SoulslikeFramework"]

    assets = asset_registry.get_assets(filter)
    return assets

def find_enum_references_in_blueprint(blueprint_path):
    """
    Find and fix E_ValueType references in a Blueprint.
    Returns (fixed_count, blueprint_name)
    """
    # Load the blueprint
    blueprint = unreal.EditorAssetLibrary.load_asset(blueprint_path)
    if not blueprint:
        return 0, blueprint_path.split('/')[-1]

    bp = unreal.Blueprint.cast(blueprint)
    if not bp:
        return 0, blueprint_path.split('/')[-1]

    bp_name = blueprint_path.split('/')[-1]
    fixed_count = 0

    # Get all graphs in the blueprint
    try:
        graphs = unreal.BlueprintEditorLibrary.get_all_graphs(bp)
    except:
        return 0, bp_name

    for graph in graphs:
        if not graph:
            continue

        try:
            nodes = graph.get_editor_property('nodes')
        except:
            continue

        for node in nodes:
            if not node:
                continue

            try:
                pins = node.get_editor_property('pins')
            except:
                continue

            for pin in pins:
                if not pin:
                    continue

                try:
                    # Check if this pin uses E_ValueType
                    pin_type = pin.get_editor_property('pin_type')
                    if not pin_type:
                        continue

                    # Get the pin type category
                    type_object = pin_type.get_editor_property('pin_sub_category_object')
                    if type_object and 'E_ValueType' in str(type_object):
                        # This pin uses E_ValueType
                        default_value = pin.get_editor_property('default_value')

                        if default_value in ENUM_VALUE_MAP:
                            new_value = ENUM_VALUE_MAP[default_value]
                            pin.set_editor_property('default_value', new_value)
                            fixed_count += 1
                            print(f"    Fixed pin in {bp_name}: {default_value} -> {new_value}")
                except Exception as e:
                    # Skip pins we can't process
                    pass

    if fixed_count > 0:
        # Compile and save
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.EditorAssetLibrary.save_asset(blueprint_path)
            print(f"    Compiled and saved {bp_name}")
        except Exception as e:
            print(f"    WARNING: Could not compile {bp_name}: {e}")

    return fixed_count, bp_name

def main():
    print("\n" + "="*70)
    print("  E_ValueType Enum Migration")
    print("  Converting NewEnumerator0/1 -> CurrentValue/MaxValue")
    print("="*70)

    # Known Blueprints that use E_ValueType (from error log)
    known_blueprints = [
        "/Game/SoulslikeFramework/Data/Stats/B_Stat",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
        "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    ]

    total_fixed = 0
    processed = 0

    print(f"\nProcessing {len(known_blueprints)} known Blueprints...")

    for bp_path in known_blueprints:
        print(f"\n[{processed + 1}/{len(known_blueprints)}] {bp_path.split('/')[-1]}")
        fixed, name = find_enum_references_in_blueprint(bp_path)
        total_fixed += fixed
        processed += 1
        if fixed == 0:
            print(f"    No enum pins found (may need manual fix)")

    print("\n" + "="*70)
    print(f"  COMPLETE: Fixed {total_fixed} enum values in {processed} Blueprints")
    print("="*70)

    if total_fixed == 0:
        print("\nNOTE: No automatic fixes were made.")
        print("The enum values may be stored differently than expected.")
        print("You may need to manually update the pins in each Blueprint.")
    else:
        print("\nNext steps:")
        print("1. Open each Blueprint and verify it compiles")
        print("2. Test gameplay to ensure enums work correctly")
        print("3. Then delete E_ValueType Blueprint enum")

    print("="*70)

if __name__ == "__main__" or True:
    main()
