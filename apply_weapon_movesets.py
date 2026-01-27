# apply_weapon_movesets.py
# Apply MovesetWeapons to weapon PDAs in SLFConversion
# Run on SLFConversion project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Weapon PDA -> Moveset mapping
# Note: DA_Sword01 uses DA_Animset_LightSword (not DA_Animset_Sword)
WEAPON_MOVESET_MAP = {
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana": "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana",
}

def apply_movesets():
    log("=" * 70)
    log("APPLYING WEAPON MOVESETS")
    log("=" * 70)

    for pda_path, moveset_path in WEAPON_MOVESET_MAP.items():
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"{'='*60}")

        # Load PDA
        asset = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not asset:
            log(f"  ERROR: Could not load PDA")
            continue

        # Load moveset
        moveset = unreal.EditorAssetLibrary.load_asset(moveset_path)
        if not moveset:
            log(f"  WARNING: Could not load moveset at {moveset_path}")
            # Try to find the moveset
            log(f"  Searching for moveset assets...")
            continue

        try:
            # Get ItemInformation struct
            item_info = asset.get_editor_property("item_information")

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property("equipment_details")

            # Set MovesetWeapons
            current_moveset = equip_details.get_editor_property("moveset_weapons")
            log(f"  Current MovesetWeapons: {current_moveset.get_name() if current_moveset else 'None'}")

            equip_details.set_editor_property("moveset_weapons", moveset)

            # Verify
            new_moveset = equip_details.get_editor_property("moveset_weapons")
            log(f"  New MovesetWeapons: {new_moveset.get_name() if new_moveset else 'None'}")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(pda_path)
            log(f"  SAVED: {pda_name}")

        except Exception as e:
            log(f"  ERROR: {e}")

if __name__ == "__main__":
    apply_movesets()
    log("\n\nMOVESET APPLICATION COMPLETE")
