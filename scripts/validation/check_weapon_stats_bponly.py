# check_weapon_stats_bponly.py
# Check weapon StatChanges in bp_only project
# Discovers property structure since BP data assets differ from C++ reparented ones
import unreal

def check_weapons():
    output_file = "C:/scripts/SLFConversion/migration_cache/weapon_stats_bponly.txt"

    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    ]

    with open(output_file, 'w') as f:
        f.write("="*80 + "\n")
        f.write("WEAPON STAT CHANGES CHECK (bp_only project)\n")
        f.write("="*80 + "\n\n")

        for path in weapon_paths:
            asset = unreal.EditorAssetLibrary.load_asset(path)
            if not asset:
                f.write(f"[{path.split('/')[-1]}] NOT FOUND\n\n")
                continue

            f.write(f"[{asset.get_name()}]\n")
            f.write(f"  Class: {asset.get_class().get_name()}\n")

            # Use export_text to get all properties
            export_text = unreal.EditorAssetLibrary.get_metadata_tag(path, "")

            # Try to get the full text export via SystemLibrary
            try:
                # Export to text and search for Weight or StatChanges
                text = str(asset)
                f.write(f"  Object str: {text[:200]}...\n")
            except:
                pass

            # Try different property paths that Blueprint data assets might use
            property_names_to_try = [
                "item_information",
                "ItemInformation",
                "Item Information",
                "Equipment Details",
                "EquipmentDetails",
                "equipment_details",
                "StatChanges",
                "stat_changes",
                "Stat Changes",
            ]

            for prop_name in property_names_to_try:
                try:
                    value = asset.get_editor_property(prop_name)
                    f.write(f"  Property '{prop_name}': FOUND - {type(value).__name__}\n")

                    # If it's a struct, try to get nested properties
                    if hasattr(value, 'get_editor_property'):
                        for nested in ["equipment_details", "EquipmentDetails", "stat_changes", "StatChanges"]:
                            try:
                                nested_val = value.get_editor_property(nested)
                                f.write(f"    .{nested}: FOUND - {type(nested_val).__name__}\n")

                                # If it's the stat changes map
                                if hasattr(nested_val, 'items') or isinstance(nested_val, dict):
                                    f.write(f"    StatChanges count: {len(nested_val)}\n")
                                    for k, v in nested_val.items():
                                        tag_str = str(k) if k else "NONE"
                                        delta = v.get_editor_property("delta") if hasattr(v, 'get_editor_property') else str(v)
                                        f.write(f"      - {tag_str}: {delta}\n")
                            except:
                                pass
                except Exception as e:
                    pass  # Property doesn't exist

            # Also try direct export_text approach to find Weight
            try:
                # Get asset registry data
                asset_data = unreal.EditorAssetLibrary.find_asset_data(path)
                if asset_data:
                    tags = asset_data.get_tag_value_map()
                    f.write(f"  AssetData tags count: {len(tags)}\n")
            except:
                pass

            f.write("\n")

        f.write("="*80 + "\n")
        f.write("\nChecking armor for comparison...\n")
        f.write("="*80 + "\n\n")

        # Also check an armor piece for comparison
        armor_path = "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor"
        armor = unreal.EditorAssetLibrary.load_asset(armor_path)
        if armor:
            f.write(f"[{armor.get_name()}]\n")
            f.write(f"  Class: {armor.get_class().get_name()}\n")

            for prop_name in property_names_to_try:
                try:
                    value = armor.get_editor_property(prop_name)
                    f.write(f"  Property '{prop_name}': FOUND - {type(value).__name__}\n")
                except:
                    pass

    unreal.log(f"Output saved to: {output_file}")

if __name__ == "__main__":
    check_weapons()
