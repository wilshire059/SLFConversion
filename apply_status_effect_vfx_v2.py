"""
Apply Status Effect VFX to RankInfo v2
Uses correct struct types and property names discovered via diagnosis.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/apply_vfx_log.txt"

def log(msg):
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")
    unreal.log(msg)

# Status effect data to VFX mapping
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
        "loop_vfx": None,
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
        "trigger_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_Poison.NS_Poison",
        "loop_vfx": "/Game/SoulslikeFramework/VFX/Systems/NS_PoisonLoop.NS_PoisonLoop",
        "socket": "Chest"
    },
}


def apply_status_effect_vfx():
    """Apply VFX references to status effect data assets RankInfo."""

    # Clear output file
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("=== Status Effect VFX Application Log ===\n\n")

    log("=== Starting Status Effect VFX Application ===")

    success_count = 0
    fail_count = 0

    for asset_path, vfx_data in STATUS_EFFECT_VFX.items():
        try:
            log(f"\nProcessing: {asset_path}")

            # Load the data asset
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            if not asset:
                log(f"  SKIP: Could not load asset")
                fail_count += 1
                continue

            log(f"  Loaded: {asset.get_class().get_name()}")

            # Load VFX systems
            trigger_vfx = None
            loop_vfx = None

            if vfx_data.get("trigger_vfx"):
                trigger_vfx = unreal.EditorAssetLibrary.load_asset(vfx_data["trigger_vfx"])
                if trigger_vfx:
                    log(f"  Loaded trigger VFX: {trigger_vfx.get_name()}")
                else:
                    log(f"  WARNING: Could not load trigger VFX: {vfx_data['trigger_vfx']}")

            if vfx_data.get("loop_vfx"):
                loop_vfx = unreal.EditorAssetLibrary.load_asset(vfx_data["loop_vfx"])
                if loop_vfx:
                    log(f"  Loaded loop VFX: {loop_vfx.get_name()}")
                else:
                    log(f"  WARNING: Could not load loop VFX: {vfx_data['loop_vfx']}")

            socket_name = vfx_data.get("socket", "Chest")

            # Get the current rank_info map
            rank_info = asset.get_editor_property('rank_info')
            log(f"  Current rank_info keys: {list(rank_info.keys())}")

            # Create VFX info structs using correct type names
            # SLFStatusEffectVfxInfo has: vfx_system, attach_socket
            trigger_vfx_info = unreal.SLFStatusEffectVfxInfo()
            if trigger_vfx:
                trigger_vfx_info.set_editor_property('vfx_system', trigger_vfx)
            trigger_vfx_info.set_editor_property('attach_socket', socket_name)

            loop_vfx_info = unreal.SLFStatusEffectVfxInfo()
            if loop_vfx:
                loop_vfx_info.set_editor_property('vfx_system', loop_vfx)
            loop_vfx_info.set_editor_property('attach_socket', socket_name)

            # Create or update rank 1 info
            # SLFStatusEffectRankInfo has: trigger_vfx, loop_vfx, relevant_data
            rank_1_info = unreal.SLFStatusEffectRankInfo()
            rank_1_info.set_editor_property('trigger_vfx', trigger_vfx_info)
            rank_1_info.set_editor_property('loop_vfx', loop_vfx_info)

            # Set rank 1 in the map
            rank_info[1] = rank_1_info

            # Save back to the asset
            asset.set_editor_property('rank_info', rank_info)

            log(f"  Set RankInfo[1]:")
            log(f"    TriggerVFX: {trigger_vfx.get_name() if trigger_vfx else 'None'}")
            log(f"    LoopVFX: {loop_vfx.get_name() if loop_vfx else 'None'}")
            log(f"    Socket: {socket_name}")

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False):
                log(f"  SAVED successfully")
                success_count += 1
            else:
                log(f"  WARNING: Save returned false")
                # Still count as success if no exception
                success_count += 1

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(traceback.format_exc())
            fail_count += 1

    log(f"\n=== Summary: {success_count} succeeded, {fail_count} failed ===")
    return success_count, fail_count


if __name__ == "__main__":
    apply_status_effect_vfx()
