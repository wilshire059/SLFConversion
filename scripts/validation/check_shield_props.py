"""
Check shield item properties - list all available properties
"""
import unreal

SHIELD_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
]

def main():
    for asset_path in SHIELD_ASSETS:
        asset = unreal.load_asset(asset_path)
        if not asset:
            print(f"ERROR: Could not load {asset_path}")
            continue

        asset_name = asset_path.split("/")[-1]
        print(f"\n=== {asset_name} ===")
        print(f"  Class: {asset.get_class().get_name()}")

        # Try common property names
        property_names = [
            "item_information",
            "ItemInformation",
            "item_info",
            "ItemInfo",
            "equipment_details",
            "EquipmentDetails",
        ]

        for prop_name in property_names:
            try:
                value = asset.get_editor_property(prop_name)
                print(f"  Property '{prop_name}': {type(value)}")
            except Exception as e:
                pass  # Property doesn't exist

        # Use dir() to list all attributes
        print(f"\n  All attributes containing 'item' or 'equip':")
        for attr in dir(asset):
            if 'item' in attr.lower() or 'equip' in attr.lower():
                print(f"    - {attr}")

if __name__ == "__main__":
    main()
