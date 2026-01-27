"""
Check weapon EquipSlots tags in bp_only backup.
"""
import unreal
import json

def check_bponly_equip_slots():
    """Check the EquipSlots tags of weapon items in bp_only."""

    # Note: This script runs in SLFConversion but we need to check if bp_only has the data
    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    ]

    results = {}

    for path in weapon_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            try:
                item_info = asset.get_editor_property("item_information")
                equipment_details = item_info.get_editor_property("equipment_details")
                equip_slots = equipment_details.get_editor_property("equip_slots")

                # Get gameplay tags as array
                tags = equip_slots.get_gameplay_tag_array() if hasattr(equip_slots, 'get_gameplay_tag_array') else []
                tag_strs = [str(t) for t in tags] if tags else []

                results[asset.get_name()] = {
                    "equip_slots_count": len(tags) if tags else 0,
                    "equip_slots_tags": tag_strs,
                    "equip_slots_str": str(equip_slots),
                }
            except Exception as e:
                results[asset.get_name()] = {"error": str(e)}

    output_path = "C:/scripts/slfconversion/migration_cache/check_equip_slots_detail.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)

if __name__ == "__main__":
    check_bponly_equip_slots()
