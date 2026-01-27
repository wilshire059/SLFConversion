# verify_item_class_link.py
# Verify that PDAs have ItemClass properly set to weapon Blueprints
# Run on SLFConversion project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Weapon PDAs and expected ItemClass
WEAPON_LINKS = {
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01":
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword":
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana":
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01":
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
}

def verify_links():
    log("=" * 70)
    log("VERIFYING WEAPON PDA -> BLUEPRINT LINKS")
    log("=" * 70)

    for pda_path, expected_bp_path in WEAPON_LINKS.items():
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"{'='*60}")

        asset = unreal.EditorAssetLibrary.load_asset(pda_path)
        if not asset:
            log(f"  ERROR: Could not load PDA")
            continue

        try:
            # Get ItemInformation
            item_info = asset.get_editor_property("item_information")

            # Get ItemClass
            item_class = item_info.get_editor_property("item_class")
            if item_class:
                class_path = item_class.get_path_name()
                log(f"  ItemClass: {class_path}")

                # Check if it matches expected
                expected_class_path = expected_bp_path + "_C"
                if expected_class_path in class_path or expected_bp_path.split("/")[-1] in class_path:
                    log(f"  MATCH: Links to correct Blueprint")
                else:
                    log(f"  MISMATCH: Expected {expected_bp_path}")
            else:
                log(f"  ItemClass: NONE (not set!)")
                log(f"  Expected: {expected_bp_path}")

        except Exception as e:
            log(f"  ERROR: {e}")

if __name__ == "__main__":
    verify_links()
    log("\n\nVERIFICATION COMPLETE")
