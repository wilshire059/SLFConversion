"""
Apply WeaponAbility property to weapon data assets.
Run this on SLFConversion project to restore weapon abilities that were lost during migration.
"""
import unreal
import sys

# Weapons that should have DA_ExampleWeaponAbility assigned
# Based on bp_only T3D exports
WEAPON_ABILITIES = {
    "/Game/SoulslikeFramework/Data/Items/DA_Katana": "/Game/SoulslikeFramework/Data/WeaponAbilities/DA_ExampleWeaponAbility",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword": "/Game/SoulslikeFramework/Data/WeaponAbilities/DA_ExampleWeaponAbility",
}

OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/apply_weapon_abilities_log.txt"

def log(msg):
    unreal.log(msg)
    unreal.log_warning(msg)  # Try warning level for visibility
    with open(OUTPUT_FILE, 'a') as f:
        f.write(msg + "\n")

def apply_weapon_abilities():
    """Apply WeaponAbility to weapon data assets."""
    # Clear output file
    with open(OUTPUT_FILE, 'w') as f:
        f.write("=== Apply Weapon Abilities Log ===\n")

    success_count = 0
    fail_count = 0

    # Load the weapon ability asset
    ability_path = "/Game/SoulslikeFramework/Data/WeaponAbilities/DA_ExampleWeaponAbility"
    ability_asset = unreal.EditorAssetLibrary.load_asset(ability_path)
    if not ability_asset:
        log(f"ERROR: Could not load weapon ability asset: {ability_path}")
        return

    log(f"Loaded weapon ability: {ability_asset.get_name()}")

    for weapon_path, _ in WEAPON_ABILITIES.items():
        weapon_asset = unreal.EditorAssetLibrary.load_asset(weapon_path)
        if not weapon_asset:
            log(f"SKIP: Could not load {weapon_path}")
            fail_count += 1
            continue

        weapon_name = weapon_path.split("/")[-1]

        try:
            # Get the ItemInformation struct
            item_info = weapon_asset.get_editor_property("item_information")
            if item_info is None:
                log(f"WARN: {weapon_name} has no item_information property")
                fail_count += 1
                continue

            log(f"DEBUG: Got item_info for {weapon_name}")

            # Get EquipmentDetails struct
            equip_details = item_info.get_editor_property("equipment_details")
            if equip_details is None:
                log(f"WARN: {weapon_name} has no equipment_details in item_information")
                fail_count += 1
                continue

            log(f"DEBUG: Got equipment_details for {weapon_name}")

            # Check current WeaponAbility value
            current_ability = equip_details.get_editor_property("weapon_ability")
            log(f"DEBUG: Current weapon_ability for {weapon_name}: {current_ability}")

            # Set WeaponAbility property
            equip_details.set_editor_property("weapon_ability", ability_asset)

            # Set it back (structs are copied, not referenced)
            item_info.set_editor_property("equipment_details", equip_details)
            weapon_asset.set_editor_property("item_information", item_info)

            # Verify it was set
            verify_info = weapon_asset.get_editor_property("item_information")
            verify_details = verify_info.get_editor_property("equipment_details")
            verify_ability = verify_details.get_editor_property("weapon_ability")
            log(f"DEBUG: After set, weapon_ability for {weapon_name}: {verify_ability}")

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(weapon_path):
                log(f"OK: {weapon_name} -> DA_ExampleWeaponAbility")
                success_count += 1
            else:
                log(f"ERROR: Failed to save {weapon_name}")
                fail_count += 1

        except Exception as e:
            log(f"ERROR: {weapon_name} - {e}")
            import traceback
            log(traceback.format_exc())
            fail_count += 1

    log(f"Applied weapon abilities: {success_count} succeeded, {fail_count} failed")

if __name__ == "__main__":
    apply_weapon_abilities()
