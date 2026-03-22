"""
Apply Status Effect VFX to RankInfo

This script populates the RankInfo TMap on status effect data assets
with appropriate Niagara VFX references.

Usage: Run via UnrealEditor-Cmd.exe -run=pythonscript
"""

import unreal

# Status effect data to VFX mapping
# VFX assets are in /Game/SoulslikeFramework/VFX/Systems/
STATUS_EFFECT_VFX = {
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_Poison.NS_Poison",
        "loop_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_PoisonLoop.NS_PoisonLoop",
        "socket": "Chest"
    },
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_Burn.NS_Burn",
        "loop_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_BurnLoop.NS_BurnLoop",
        "socket": "Chest"
    },
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_BleedExplode.NS_BleedExplode",
        "loop_vfx": None,  # Bleed doesn't have a loop VFX
        "socket": "Chest"
    },
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_FrostBite.NS_FrostBite",
        "loop_vfx": None,
        "socket": "Chest"
    },
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_Madness.NS_Madness",
        "loop_vfx": None,
        "socket": "Head"
    },
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_Corruption.NS_Corruption",
        "loop_vfx": None,
        "socket": "Chest"
    },
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague": {
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_Poison.NS_Poison",  # Using poison VFX for plague
        "loop_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_PoisonLoop.NS_PoisonLoop",
        "socket": "Chest"
    },
}


def apply_status_effect_vfx():
    """Apply VFX references to status effect data assets RankInfo."""

    unreal.log("=== Starting Status Effect VFX Application ===")

    success_count = 0
    fail_count = 0

    for asset_path, vfx_data in STATUS_EFFECT_VFX.items():
        try:
            unreal.log(f"Processing: {asset_path}")

            # Load the data asset
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            if not asset:
                unreal.log_warning(f"SKIP: Could not load {asset_path}")
                fail_count += 1
                continue

            unreal.log(f"  Loaded asset: {asset.get_name()}, class: {asset.get_class().get_name()}")

            # For data assets, we work directly with the asset, not generated_class
            cdo = asset

            # Load VFX systems
            trigger_vfx = None
            loop_vfx = None

            if vfx_data.get("trigger_vfx"):
                trigger_vfx = unreal.EditorAssetLibrary.load_asset(vfx_data["trigger_vfx"])
                if not trigger_vfx:
                    unreal.log_warning(f"  Could not load trigger VFX: {vfx_data['trigger_vfx']}")
                else:
                    unreal.log(f"  Loaded trigger VFX: {trigger_vfx.get_name()}")

            if vfx_data.get("loop_vfx"):
                loop_vfx = unreal.EditorAssetLibrary.load_asset(vfx_data["loop_vfx"])
                if not loop_vfx:
                    unreal.log_warning(f"  Could not load loop VFX: {vfx_data['loop_vfx']}")
                else:
                    unreal.log(f"  Loaded loop VFX: {loop_vfx.get_name()}")

            socket_name = vfx_data.get("socket", "Chest")

            # Check what properties are available
            props = [p for p in dir(cdo) if not p.startswith('_')]
            rank_props = [p for p in props if 'rank' in p.lower()]
            vfx_props = [p for p in props if 'vfx' in p.lower()]
            unreal.log(f"  Rank-related props: {rank_props}")
            unreal.log(f"  VFX-related props: {vfx_props}")

            # Check if the asset has rank_info property
            if hasattr(cdo, 'rank_info'):
                unreal.log(f"  Found rank_info property")

                # Get current RankInfo map
                try:
                    rank_info = cdo.get_editor_property('rank_info')
                    unreal.log(f"  Current rank_info type: {type(rank_info)}")
                    unreal.log(f"  Current rank_info keys: {list(rank_info.keys()) if hasattr(rank_info, 'keys') else 'N/A'}")
                except Exception as e:
                    unreal.log_error(f"  Error getting rank_info: {e}")
                    fail_count += 1
                    continue

                # Try to create and set VFX info structs
                try:
                    # Check if struct types are available
                    unreal.log(f"  Checking struct availability...")

                    # Try different struct name formats
                    struct_names_to_try = [
                        'SLFStatusEffectVfxInfo',
                        'FSLFStatusEffectVfxInfo',
                        'StatusEffectVfxInfo',
                    ]

                    vfx_info_class = None
                    for name in struct_names_to_try:
                        if hasattr(unreal, name):
                            vfx_info_class = getattr(unreal, name)
                            unreal.log(f"  Found VFX info struct: {name}")
                            break

                    if not vfx_info_class:
                        unreal.log_warning(f"  No VFX info struct class found. Trying direct property access.")

                        # Alternative: Try to set properties directly on the TMap entry
                        # This requires creating empty entries first
                        fail_count += 1
                        continue

                    # Create trigger VFX info
                    trigger_vfx_info = vfx_info_class()
                    if trigger_vfx:
                        trigger_vfx_info.set_editor_property('vfx_system', trigger_vfx)
                    trigger_vfx_info.set_editor_property('attach_socket', socket_name)

                    # Create loop VFX info
                    loop_vfx_info = vfx_info_class()
                    if loop_vfx:
                        loop_vfx_info.set_editor_property('vfx_system', loop_vfx)
                    loop_vfx_info.set_editor_property('attach_socket', socket_name)

                    # Try to find rank info struct
                    rank_names_to_try = [
                        'SLFStatusEffectRankInfo',
                        'FSLFStatusEffectRankInfo',
                        'StatusEffectRankInfo',
                    ]

                    rank_info_class = None
                    for name in rank_names_to_try:
                        if hasattr(unreal, name):
                            rank_info_class = getattr(unreal, name)
                            unreal.log(f"  Found Rank info struct: {name}")
                            break

                    if not rank_info_class:
                        unreal.log_warning(f"  No Rank info struct class found.")
                        fail_count += 1
                        continue

                    # Create rank info
                    rank_1_info = rank_info_class()
                    rank_1_info.set_editor_property('trigger_vfx', trigger_vfx_info)
                    rank_1_info.set_editor_property('loop_vfx', loop_vfx_info)

                    # Set rank 1 in the map
                    rank_info[1] = rank_1_info

                    # Save back to the asset
                    cdo.set_editor_property('rank_info', rank_info)

                    unreal.log(f"OK: {asset_path}")
                    unreal.log(f"    TriggerVFX: {trigger_vfx.get_name() if trigger_vfx else 'None'}")
                    unreal.log(f"    LoopVFX: {loop_vfx.get_name() if loop_vfx else 'None'}")
                    unreal.log(f"    Socket: {socket_name}")

                except Exception as struct_err:
                    unreal.log_error(f"ERROR creating structs for {asset_path}: {struct_err}")
                    fail_count += 1
                    continue

                # Save the asset
                if unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False):
                    success_count += 1
                    unreal.log(f"  Saved successfully")
                else:
                    unreal.log_warning(f"  Could not save {asset_path}")
                    fail_count += 1
            else:
                unreal.log_warning(f"SKIP: {asset_path} - no rank_info property")
                fail_count += 1

        except Exception as e:
            unreal.log_error(f"ERROR: {asset_path} - {e}")
            import traceback
            unreal.log_error(traceback.format_exc())
            fail_count += 1

    unreal.log(f"=== Summary: {success_count} succeeded, {fail_count} failed ===")
    return success_count, fail_count


if __name__ == "__main__":
    apply_status_effect_vfx()
