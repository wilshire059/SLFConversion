# migrate_armor_only.py
# Surgical migration script - ONLY reparents armor data assets
# Does NOT touch any other Blueprints
#
# Run with:
# "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
#   "C:/scripts/SLFConversion/SLFConversion.uproject" ^
#   -run=pythonscript -script="C:/scripts/SLFConversion/migrate_armor_only.py" ^
#   -stdout -unattended -nosplash

import unreal
import json
import os
import re

# Only these assets will be touched - NOTHING ELSE
ARMOR_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleTalisman",
]

# C++ parent class for armor items
CPP_PARENT = "/Script/SLFConversion.PDA_Item"

# Character asset paths for mesh mapping
CHARACTER_ASSET_PATHS = {
    "DA_Quinn": "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn",
    "DA_Manny": "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Manny",
}


def log(msg):
    print(msg)
    unreal.log_warning(msg)  # Use log_warning to ensure visibility


def reparent_armor_assets():
    """Reparent only the armor data assets to C++ PDA_Item"""
    log("=" * 70)
    log("SURGICAL ARMOR MIGRATION - Reparenting Only Armor Assets")
    log("=" * 70)

    # Load C++ parent class
    cpp_class = unreal.load_class(None, CPP_PARENT)
    if not cpp_class:
        log(f"[ERROR] Could not load C++ parent class: {CPP_PARENT}")
        return False

    log(f"Loaded C++ parent: {cpp_class.get_name()}")

    success_count = 0
    fail_count = 0

    for asset_path in ARMOR_ASSETS:
        log(f"\n[{asset_path.split('/')[-1]}]")

        # Load the Blueprint asset
        bp = unreal.load_asset(asset_path)
        if not bp:
            log(f"  [SKIP] Could not load asset")
            fail_count += 1
            continue

        # Check if it's a Blueprint
        if not isinstance(bp, unreal.Blueprint):
            log(f"  [SKIP] Not a Blueprint (already native or data asset)")
            success_count += 1
            continue

        # Check current parent
        current_parent = bp.get_editor_property("parent_class")
        log(f"  Current parent: {current_parent.get_name() if current_parent else 'None'}")

        # Reparent to C++ class
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            if result:
                log(f"  [OK] Reparented to {cpp_class.get_name()}")

                # Save the asset
                unreal.EditorAssetLibrary.save_asset(asset_path)
                log(f"  [SAVED]")
                success_count += 1
            else:
                log(f"  [FAIL] Reparent returned false")
                fail_count += 1
        except Exception as e:
            log(f"  [ERROR] {e}")
            fail_count += 1

    log("\n" + "=" * 70)
    log(f"REPARENTING COMPLETE: {success_count} succeeded, {fail_count} failed")
    log("=" * 70)

    return fail_count == 0


def apply_armor_mesh_data():
    """Apply cached armor mesh data to the reparented assets"""
    cache_path = "C:/scripts/SLFConversion/migration_cache/armor_mesh_data.json"

    if not os.path.exists(cache_path):
        log(f"[WARN] No armor mesh cache found at {cache_path}")
        log("       Run extract_armor_data.py on backup first")
        return False

    log("\n" + "=" * 70)
    log("APPLYING CACHED ARMOR MESH DATA")
    log("=" * 70)

    with open(cache_path, 'r') as f:
        armor_cache = json.load(f)

    log(f"Loaded {len(armor_cache)} armor items from cache")

    # Load character data assets for mapping
    character_assets = {}
    for char_name, char_path in CHARACTER_ASSET_PATHS.items():
        asset = unreal.EditorAssetLibrary.load_asset(char_path)
        if asset:
            character_assets[char_name] = asset
            log(f"Loaded character asset: {char_name}")

    success_count = 0
    fail_count = 0

    for asset_name, data in armor_cache.items():
        log(f"\n[{asset_name}]")
        log(f"  Path: {data['path']}")

        meshes = data.get('skeletal_mesh_info', {})

        if not meshes:
            log(f"  [SKIP] No mesh data to apply")
            continue

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(data['path'])
        if not asset:
            log(f"  [SKIP] Could not load asset")
            fail_count += 1
            continue

        # Get the ItemInformation struct
        try:
            item_info = asset.get_editor_property("item_information")
            equip_details = item_info.get_editor_property("equipment_details")

            # Get the current skeletal mesh info map
            mesh_map = equip_details.get_editor_property("skeletal_mesh_info")

            # Clear and rebuild the map with character asset keys
            mesh_map.clear()

            meshes_applied = 0
            for char_name, mesh_path in meshes.items():
                if char_name in character_assets:
                    char_asset = character_assets[char_name]
                    mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
                    if mesh:
                        # Create soft object ptr for character
                        char_soft = unreal.SoftObjectPath(CHARACTER_ASSET_PATHS[char_name])
                        mesh_soft = unreal.SoftObjectPath(mesh_path)

                        mesh_map[char_asset] = mesh
                        log(f"  Applying mesh: {char_name} -> {mesh_path.split('/')[-1]}")
                        meshes_applied += 1

            # Set the map back
            equip_details.set_editor_property("skeletal_mesh_info", mesh_map)
            item_info.set_editor_property("equipment_details", equip_details)
            asset.set_editor_property("item_information", item_info)

            # Save
            unreal.EditorAssetLibrary.save_asset(data['path'])
            log(f"  [SAVED] Applied {meshes_applied} meshes")
            success_count += 1

        except Exception as e:
            log(f"  [ERROR] {e}")
            fail_count += 1

    log("\n" + "=" * 70)
    log(f"MESH APPLICATION COMPLETE: {success_count} succeeded, {fail_count} failed")
    log("=" * 70)

    return fail_count == 0


def main():
    log("\n")
    log("*" * 70)
    log("* SURGICAL ARMOR MIGRATION")
    log("* Only touches armor data assets - nothing else")
    log("*" * 70)

    # Step 1: Reparent armor assets
    reparent_success = reparent_armor_assets()

    # Step 2: Apply cached mesh data
    mesh_success = apply_armor_mesh_data()

    log("\n")
    log("*" * 70)
    if reparent_success and mesh_success:
        log("* SURGICAL MIGRATION COMPLETE - SUCCESS")
    else:
        log("* SURGICAL MIGRATION COMPLETE - WITH ISSUES")
    log("*" * 70)


if __name__ == "__main__":
    main()
