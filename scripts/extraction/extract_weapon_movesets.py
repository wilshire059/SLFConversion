# extract_weapon_movesets.py
# Extract MovesetWeapons from weapon PDAs in bp_only
# Run on bp_only project

import unreal
import json

def log(msg):
    unreal.log_warning(str(msg))

WEAPON_PDAS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
]

def extract_movesets():
    log("=" * 70)
    log("EXTRACTING WEAPON MOVESETS FROM BP_ONLY")
    log("=" * 70)

    results = {}

    for pda_path in WEAPON_PDAS:
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"{'='*60}")

        asset = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not asset:
            log(f"  ERROR: Could not load")
            continue

        try:
            # Get ItemInformation
            item_info = asset.get_editor_property("item_information")

            # Get EquipmentDetails
            equip_details = item_info.get_editor_property("equipment_details")

            # Get MovesetWeapons
            moveset = equip_details.get_editor_property("moveset_weapons")

            if moveset:
                moveset_path = moveset.get_path_name()
                moveset_name = moveset.get_name()
                log(f"  MovesetWeapons: {moveset_name}")
                log(f"  Path: {moveset_path}")

                results[pda_name] = {
                    "moveset_path": moveset_path,
                    "moveset_name": moveset_name
                }

                # Also check Guard_L and Guard_R
                try:
                    guard_l = moveset.get_editor_property("guard_l")
                    guard_r = moveset.get_editor_property("guard_r")
                    log(f"  Guard_L: {guard_l.get_name() if guard_l else 'None'}")
                    log(f"  Guard_R: {guard_r.get_name() if guard_r else 'None'}")

                    if guard_l:
                        results[pda_name]["guard_l"] = guard_l.get_path_name()
                    if guard_r:
                        results[pda_name]["guard_r"] = guard_r.get_path_name()
                except Exception as e:
                    log(f"  Error getting guard sequences: {e}")
            else:
                log(f"  MovesetWeapons: None")
                results[pda_name] = {"moveset_path": None}

        except Exception as e:
            log(f"  ERROR: {e}")

    # Save results
    output_path = "C:/scripts/SLFConversion/migration_cache/weapon_moveset_data.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)
    log(f"\n\nSaved to: {output_path}")

if __name__ == "__main__":
    extract_movesets()
