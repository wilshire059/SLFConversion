# extract_weapon_data_raw.py
# Extract weapon data from bp_only using raw asset text export
# Run on bp_only project

import unreal
import json
import re

def log(msg):
    unreal.log_warning(str(msg))

# Weapon PDAs
WEAPON_PDAS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
]

# Shield PDA
SHIELD_PDA = "/Game/SoulslikeFramework/Data/Items/DA_Shield"

def export_asset_text(path):
    """Export asset as text using AssetExportTask"""
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        return None

    # Try to get the generated class and CDO for Blueprint assets
    try:
        if hasattr(asset, 'generated_class'):
            gen_class = asset.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    return str(cdo.__dict__) if hasattr(cdo, '__dict__') else "CDO found but no dict"
    except Exception as e:
        log(f"CDO error: {e}")

    return None

def extract_weapon_data():
    log("=" * 70)
    log("EXTRACTING WEAPON DATA (RAW)")
    log("=" * 70)

    results = {}

    # Try to find DA_Shield
    log("\n--- Searching for shield data ---")
    shield = unreal.EditorAssetLibrary.load_asset(SHIELD_PDA)
    if not shield:
        # Try alternate paths
        alt_paths = [
            "/Game/SoulslikeFramework/Data/Items/Weapons/DA_Shield",
            "/Game/SoulslikeFramework/Data/Items/Equipment/DA_Shield",
        ]
        for alt in alt_paths:
            log(f"Trying: {alt}")
            shield = unreal.EditorAssetLibrary.load_asset(alt)
            if shield:
                log(f"Found at: {alt}")
                break

    # Get all assets in Items folder
    log("\n--- Listing all Items data assets ---")
    items_path = "/Game/SoulslikeFramework/Data/Items"
    try:
        # Find all assets
        all_assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True)
        for asset_path in all_assets[:50]:  # Limit output
            if "DA_" in asset_path:
                log(f"  Found: {asset_path}")
    except Exception as e:
        log(f"List error: {e}")

    # Check for weapon data assets specifically
    log("\n--- Examining weapon PDAs ---")

    for pda_path in WEAPON_PDAS:
        pda_name = pda_path.split("/")[-1]
        log(f"\n=== {pda_name} ===")

        asset = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not asset:
            log("  Not found")
            continue

        log(f"  Type: {type(asset)}")
        log(f"  Class: {asset.get_class().get_name()}")

        # Get all properties using dir()
        props = [p for p in dir(asset) if not p.startswith('_')]
        log(f"  Properties: {props[:30]}")  # First 30

        # Try to get specific known properties
        try_props = [
            'item_information', 'ItemInformation',
            'equipment_details', 'EquipmentDetails',
            'attachment_sockets', 'AttachmentSockets',
            'equip_slots', 'EquipSlots'
        ]

        for prop in try_props:
            try:
                val = asset.get_editor_property(prop)
                log(f"  {prop}: {val}")
            except:
                pass

    # Save
    output = "C:/scripts/SLFConversion/migration_cache/weapon_raw_data.txt"
    with open(output, "w") as f:
        f.write("Check log output\n")
    log(f"\n\nSaved to: {output}")

if __name__ == "__main__":
    extract_weapon_data()
