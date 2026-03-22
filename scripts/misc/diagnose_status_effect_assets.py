"""
Diagnose Status Effect Data Assets
Writes detailed output to a file for debugging.
"""

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/status_effect_diagnosis.txt"

def log(msg):
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")
    unreal.log(msg)

def diagnose():
    # Clear output file
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("=== Status Effect Data Asset Diagnosis ===\n\n")

    # Test assets
    test_assets = [
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
    ]

    # Test VFX
    test_vfx = [
        "/Game/SoulslikeFramework/VFX/Systems/NS_Poison.NS_Poison",
        "/Game/SoulslikeFramework/VFX/Systems/NS_PoisonLoop.NS_PoisonLoop",
    ]

    log("=== Testing VFX Asset Loading ===")
    for vfx_path in test_vfx:
        vfx = unreal.EditorAssetLibrary.load_asset(vfx_path)
        if vfx:
            log(f"OK: Loaded VFX: {vfx_path}")
            log(f"    Class: {vfx.get_class().get_name()}")
        else:
            log(f"FAIL: Could not load VFX: {vfx_path}")

    log("\n=== Testing Status Effect Data Assets ===")
    for asset_path in test_assets:
        log(f"\n--- {asset_path} ---")

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"FAIL: Could not load asset")
            continue

        log(f"OK: Loaded asset")
        log(f"    Class: {asset.get_class().get_name()}")

        # List all properties
        props = [p for p in dir(asset) if not p.startswith('_')]
        rank_props = [p for p in props if 'rank' in p.lower()]
        vfx_props = [p for p in props if 'vfx' in p.lower()]

        log(f"    Total props: {len(props)}")
        log(f"    Rank-related: {rank_props}")
        log(f"    VFX-related: {vfx_props}")

        # Check for rank_info
        if hasattr(asset, 'rank_info'):
            log(f"    Has rank_info attribute: YES")
            try:
                rank_info = asset.get_editor_property('rank_info')
                log(f"    rank_info type: {type(rank_info)}")
                if hasattr(rank_info, 'keys'):
                    log(f"    rank_info keys: {list(rank_info.keys())}")
                if hasattr(rank_info, '__len__'):
                    log(f"    rank_info length: {len(rank_info)}")
            except Exception as e:
                log(f"    Error reading rank_info: {e}")
        else:
            log(f"    Has rank_info attribute: NO")

        # Check for status_effect_tag
        if hasattr(asset, 'status_effect_tag'):
            try:
                tag = asset.get_editor_property('status_effect_tag')
                log(f"    status_effect_tag: {tag}")
            except Exception as e:
                log(f"    Error reading status_effect_tag: {e}")

    log("\n=== Checking Available Struct Types ===")
    struct_names = [
        'SLFStatusEffectVfxInfo',
        'FSLFStatusEffectVfxInfo',
        'StatusEffectVfxInfo',
        'SLFStatusEffectRankInfo',
        'FSLFStatusEffectRankInfo',
        'StatusEffectRankInfo',
    ]

    for name in struct_names:
        if hasattr(unreal, name):
            log(f"OK: unreal.{name} exists")
            try:
                instance = getattr(unreal, name)()
                log(f"    Can instantiate: YES")
                instance_props = [p for p in dir(instance) if not p.startswith('_')]
                log(f"    Properties: {[p for p in instance_props if not callable(getattr(instance, p, None))][:10]}")
            except Exception as e:
                log(f"    Cannot instantiate: {e}")
        else:
            log(f"MISSING: unreal.{name} does not exist")

    log("\n=== Diagnosis Complete ===")

if __name__ == "__main__":
    diagnose()
