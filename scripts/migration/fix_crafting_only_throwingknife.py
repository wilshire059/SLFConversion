#!/usr/bin/env python
"""
fix_crafting_only_throwingknife.py
==================================
Resets crafting enabled state to match bp_only:
- Only DA_ThrowingKnife should have bCrafingUnlocked = True
- All other items should have bCrafingUnlocked = False

Run in Unreal Editor with SLFConversion project loaded.
"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

def fix_crafting_state():
    """Reset crafting enabled state - only ThrowingKnife should be craftable"""
    log("=" * 80)
    log("FIXING CRAFTING STATE - Only DA_ThrowingKnife should be craftable")
    log("=" * 80)

    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get all assets in Items folder
    assets = asset_registry.get_assets_by_path("/Game/SoulslikeFramework/Data/Items", recursive=True)

    log(f"Found {len(assets)} assets in Items folder")

    disabled_count = 0
    enabled_count = 0
    error_count = 0

    for asset_data in assets:
        asset = asset_data.get_asset()
        if asset is None:
            continue

        class_name = asset.get_class().get_name()
        if "PDA_Item" not in class_name:
            continue

        asset_name = asset.get_name()

        try:
            # Only DA_ThrowingKnife should be craftable (matches bp_only)
            should_be_craftable = (asset_name == "DA_ThrowingKnife")

            current_state = asset.get_editor_property("crafing_unlocked")

            if current_state != should_be_craftable:
                asset.set_editor_property("crafing_unlocked", should_be_craftable)
                unreal.EditorAssetLibrary.save_loaded_asset(asset)

                if should_be_craftable:
                    log(f"  ENABLED crafting: {asset_name}")
                    enabled_count += 1
                else:
                    log(f"  Disabled crafting: {asset_name}")
                    disabled_count += 1
            else:
                # Already correct state
                if should_be_craftable:
                    log(f"  Already craftable: {asset_name}")
                # Don't log items that are already correctly disabled

        except Exception as e:
            log(f"  ERROR processing {asset_name}: {e}")
            error_count += 1

    log("")
    log("=" * 80)
    log(f"COMPLETE: Enabled {enabled_count}, Disabled {disabled_count}, Errors {error_count}")
    log("Only DA_ThrowingKnife is now craftable (matches bp_only behavior)")
    log("=" * 80)

if __name__ == "__main__":
    fix_crafting_state()
