"""
Extract enemy weapon configuration from bp_only to understand how enemies apply status effects.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/enemy_weapon_config.txt"
log_lines = []

def log(msg):
    print(msg)
    log_lines.append(str(msg))

log("=" * 70)
log("EXTRACTING ENEMY WEAPON CONFIG FROM BP_ONLY")
log("=" * 70)

# Check AI weapon Blueprints
ai_weapon_paths = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
]

for bp_path in ai_weapon_paths:
    log(f"\n--- {bp_path.split('/')[-1]} ---")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"  NOT FOUND")
        continue

    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)

            # Check for ItemInfo/ItemData property
            try:
                item_info = cdo.get_editor_property("item_info")
                if item_info:
                    log(f"  ItemInfo: {item_info.get_name() if hasattr(item_info, 'get_name') else item_info}")
            except:
                pass

            try:
                item_data = cdo.get_editor_property("item_data")
                if item_data:
                    log(f"  ItemData: {item_data.get_name() if hasattr(item_data, 'get_name') else item_data}")
            except:
                pass

            try:
                weapon_data = cdo.get_editor_property("weapon_data")
                if weapon_data:
                    log(f"  WeaponData: {weapon_data.get_name() if hasattr(weapon_data, 'get_name') else weapon_data}")
            except:
                pass

            # Try to find data asset reference
            try:
                data_asset = cdo.get_editor_property("data_asset")
                if data_asset:
                    log(f"  DataAsset: {data_asset.get_name()}")
            except:
                pass

            # List all properties
            log(f"  Properties:")
            for prop in dir(cdo):
                if not prop.startswith('_') and not callable(getattr(cdo, prop, None)):
                    try:
                        val = cdo.get_editor_property(prop)
                        if val is not None and str(val) != "None" and str(val) != "":
                            # Skip common UObject properties
                            if prop not in ['outer', 'class_', 'flags', 'path_name', 'name', 'get_class']:
                                log(f"    {prop}: {val}")
                    except:
                        pass

    except Exception as e:
        log(f"  Error: {e}")

# Check what component the enemies spawn weapons with
log("\n" + "=" * 70)
log("CHECKING ENEMY WEAPON SPAWNING")
log("=" * 70)

enemy_paths = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
]

for bp_path in enemy_paths:
    log(f"\n--- {bp_path.split('/')[-1]} ---")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"  NOT FOUND")
        continue

    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)

            # Check for weapon-related properties
            weapon_props = ['weapon', 'weapon_class', 'weapon_data', 'equipped_weapon', 'ai_weapon', 'default_weapon']
            for prop in weapon_props:
                try:
                    val = cdo.get_editor_property(prop)
                    if val:
                        log(f"  {prop}: {val}")
                except:
                    pass

    except Exception as e:
        log(f"  Error: {e}")

# Save output
with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(log_lines))
log(f"\nSaved to {OUTPUT_FILE}")
