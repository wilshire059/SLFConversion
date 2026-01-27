"""
Check weapon EquipSlots tags to diagnose equipment slot matching.
"""
import unreal
import json

def check_weapon_equip_slots():
    """Check the EquipSlots tags of weapon items."""

    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    ]

    unreal.log("=== Weapon EquipSlots Check ===")

    results = {}

    for path in weapon_paths:
        unreal.log(f"Loading: {path}")
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            try:
                # Get ItemInformation struct
                item_info = asset.get_editor_property("item_information")

                # Get equipment details
                equipment_details = item_info.get_editor_property("equipment_details")
                equip_slots = equipment_details.get_editor_property("equip_slots")

                # Just convert to string
                equip_slots_str = str(equip_slots) if equip_slots else "None"

                unreal.log(f"RESULT: {asset.get_name()}: EquipSlots={equip_slots_str}")

                results[asset.get_name()] = {
                    "equip_slots": equip_slots_str,
                }
            except Exception as e:
                unreal.log_warning(f"  {path}: Error reading properties - {e}")
                import traceback
                unreal.log_warning(traceback.format_exc())
        else:
            unreal.log_warning(f"  {path}: Not found")

    # Save results to file
    output_path = "C:/scripts/slfconversion/migration_cache/weapon_equip_slots.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)

    unreal.log(f"Saved {len(results)} results to {output_path}")
    unreal.log("=== End EquipSlots Check ===")

if __name__ == "__main__":
    check_weapon_equip_slots()
