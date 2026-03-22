#!/usr/bin/env python
# enable_crafting_test.py
# Enable crafting on some test items to verify the crafting system works

import unreal

# Items to enable crafting on (corrected paths based on actual Content folder)
TEST_ITEMS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/DA_Apple",
    "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife",
]

# Write to log file since stdout capture is unreliable
LOG_FILE = "C:/scripts/SLFConversion/crafting_enable_log.txt"

def log(msg):
    """Log to both file and unreal log"""
    unreal.log(msg)
    with open(LOG_FILE, "a") as f:
        f.write(msg + "\n")

def enable_crafting():
    """Enable crafting on test items"""

    # Clear log file
    with open(LOG_FILE, "w") as f:
        f.write("=== Enable Crafting Log ===\n")

    success_count = 0
    fail_count = 0

    for item_path in TEST_ITEMS:
        # Try loading the asset
        asset = unreal.EditorAssetLibrary.load_asset(item_path)

        if asset is None:
            log(f"Could not load: {item_path}")
            fail_count += 1
            continue

        # Check if it's a PDA_Item
        asset_class = asset.get_class()
        class_name = asset_class.get_name() if asset_class else "Unknown"

        log(f"Loaded: {item_path} (class: {class_name})")

        # List all properties that contain "craft" or "unlocked"
        all_props = [p for p in dir(asset) if not p.startswith('_')]
        craft_props = [p for p in all_props if 'craft' in p.lower() or 'unlocked' in p.lower()]
        log(f"  Properties with craft/unlock: {craft_props}")

        # Try to set crafing_unlocked property (various possible names)
        try:
            # Try different property name variations
            property_set = False
            for prop_name in ['b_crafing_unlocked', 'crafing_unlocked', 'bCrafingUnlocked', 'CrafingUnlocked']:
                if hasattr(asset, prop_name):
                    asset.set_editor_property(prop_name, True)
                    log(f"  Set {prop_name} = True")
                    property_set = True
                    success_count += 1
                    break

            if not property_set:
                log(f"  No crafing_unlocked property found!")
                log(f"  First 30 properties: {all_props[:30]}")
                fail_count += 1
                continue

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(item_path)
            log(f"  Saved!")

        except Exception as e:
            log(f"  ERROR: {e}")
            fail_count += 1

    log(f"\n=== Summary ===")
    log(f"Success: {success_count}")
    log(f"Failed: {fail_count}")

if __name__ == "__main__":
    enable_crafting()
