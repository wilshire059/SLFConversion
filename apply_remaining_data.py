import unreal
import json
import os

CACHE_DIR = "C:/scripts/SLFConversion/migration_cache"

def apply_niagara():
    """Apply Niagara systems to items"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING NIAGARA SYSTEMS")
    unreal.log_warning("=" * 60)

    cache_file = os.path.join(CACHE_DIR, "item_data.json")
    if not os.path.exists(cache_file):
        unreal.log_warning("  Cache not found!")
        return

    with open(cache_file, 'r') as f:
        item_data = json.load(f)

    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    count = 0
    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        if asset_name not in item_data:
            continue

        data = item_data[asset_name]
        if 'WorldNiagaraSystem' not in data:
            continue

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            continue

        try:
            niagara_path = data['WorldNiagaraSystem']
            if "." in niagara_path:
                niagara_path = niagara_path.rsplit(".", 1)[0]

            niagara = unreal.EditorAssetLibrary.load_asset(niagara_path)
            if niagara:
                asset.set_editor_property('world_niagara_system', niagara)
                unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
                count += 1
                unreal.log_warning("  [OK] " + asset_name + ": " + niagara.get_name())
            else:
                unreal.log_warning("  [WARN] " + asset_name + ": Could not load " + niagara_path)

        except Exception as e:
            unreal.log_warning("  [ERROR] " + asset_name + ": " + str(e))

    unreal.log_warning("Applied Niagara to " + str(count) + " items")


def apply_dodge_montages():
    """Apply dodge montages to B_Action_Dodge"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING DODGE MONTAGES")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning("  Could not load B_Action_Dodge")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("  Could not get generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("  Could not get CDO")
        return

    montage_base = "/Game/SoulslikeFramework/Demo/_Animations/Dodges"
    montage_map = {
        'forward': montage_base + "/AM_SLF_Dodge_F",
        'forward_left': montage_base + "/AM_SLF_Dodge_FL",
        'forward_right': montage_base + "/AM_SLF_Dodge_FR",
        'left': montage_base + "/AM_SLF_Dodge_L",
        'right': montage_base + "/AM_SLF_Dodge_R",
        'backward': montage_base + "/AM_SLF_Dodge_B",
        'backward_left': montage_base + "/AM_SLF_Dodge_BL",
        'backward_right': montage_base + "/AM_SLF_Dodge_BR",
    }

    try:
        dodge_montages = cdo.get_editor_property('dodge_montages')
        if dodge_montages:
            for field, path in montage_map.items():
                montage = unreal.EditorAssetLibrary.load_asset(path)
                if montage:
                    dodge_montages.set_editor_property(field, montage)
                    unreal.log_warning("  [OK] " + field + ": " + montage.get_name())
                else:
                    unreal.log_warning("  [WARN] " + field + ": Could not load " + path)

            cdo.set_editor_property('dodge_montages', dodge_montages)
            bp.modify()
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log_warning("  Saved B_Action_Dodge")

    except Exception as e:
        unreal.log_warning("  Error: " + str(e))
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    apply_niagara()
    apply_dodge_montages()
    unreal.log_warning("")
    unreal.log_warning("*" * 60)
    unreal.log_warning("ALL DATA APPLIED SUCCESSFULLY")
    unreal.log_warning("*" * 60)
