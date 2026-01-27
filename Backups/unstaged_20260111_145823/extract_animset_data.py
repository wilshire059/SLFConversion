"""
extract_animset_data.py
Extract weapon animset montage data from backup project
Run on bp_only project to extract montage references
"""
import unreal
import json
import os

def log(msg):
    unreal.log_warning(f"[EXTRACT_ANIMSET] {msg}")

# Output path
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animset_data.json"

def main():
    log("=" * 70)
    log("EXTRACTING WEAPON ANIMSET DATA")
    log("=" * 70)

    animset_paths = [
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Unarmed",
    ]

    # Montage property mappings (snake_case in Python, original name in Blueprint)
    montage_props = [
        ('one_h_light_combo_montage_r', '1h_LightComboMontage_R'),
        ('one_h_light_combo_montage_l', '1h_LightComboMontage_L'),
        ('two_h_light_combo_montage', '2h_LightComboMontage'),
        ('one_h_heavy_combo_montage_r', '1h_HeavyComboMontage_R'),
        ('one_h_heavy_combo_montage_l', '1h_HeavyComboMontage_L'),
        ('two_h_heavy_combo_montage', '2h_HeavyComboMontage'),
        ('light_dual_wield_montage', 'LightDualWieldMontage'),
        ('heavy_dual_wield_montage', 'HeavyDualWieldMontage'),
        ('jump_attack_montage', 'JumpAttackMontage'),
        ('sprint_attack_montage', 'SprintAttackMontage'),
        ('guard_r', 'Guard_R'),
        ('guard_r_hit', 'Guard_R_Hit'),
        ('guard_l', 'Guard_L'),
        ('guard_l_hit', 'Guard_L_Hit'),
    ]

    all_data = {}

    for path in animset_paths:
        asset_name = path.split('/')[-1]
        log(f"\n--- {asset_name} ---")

        asset = unreal.load_asset(path)
        if not asset:
            log(f"  [ERROR] Could not load: {path}")
            continue

        animset_data = {}

        for snake_name, display_name in montage_props:
            try:
                montage_ref = asset.get_editor_property(snake_name)
                if montage_ref:
                    # Handle TSoftObjectPtr
                    if hasattr(montage_ref, 'get_path_name'):
                        asset_path = montage_ref.get_path_name()
                        if asset_path and asset_path != "None":
                            animset_data[snake_name] = asset_path
                            log(f"  {display_name}: {asset_path}")
                    elif hasattr(montage_ref, 'get_asset') and montage_ref.get_asset():
                        asset_path = montage_ref.get_asset().get_path_name()
                        animset_data[snake_name] = asset_path
                        log(f"  {display_name}: {asset_path}")
                    else:
                        # Try export_text for soft references
                        export = unreal.SLFAutomationLibrary.export_text(asset) if hasattr(unreal, 'SLFAutomationLibrary') else ""
                        # Parse from export text if needed
                        pass
            except Exception as e:
                log(f"  [WARN] Could not read {display_name}: {e}")

        if animset_data:
            all_data[path] = animset_data
            log(f"  Total: {len(animset_data)} montages found")
        else:
            log(f"  [WARN] No montages found - checking via export_text")
            # Try export_text as fallback
            try:
                if hasattr(unreal, 'SLFAutomationLibrary'):
                    export = unreal.SLFAutomationLibrary.export_text(asset)
                    if export:
                        # Look for montage references in export text
                        import re
                        montage_pattern = r"([^=]+)=.*?AnimMontage'([^']+)'"
                        matches = re.findall(montage_pattern, export)
                        for prop_name, montage_path in matches:
                            prop_name = prop_name.strip().split('=')[0].strip()
                            # Convert to snake_case
                            snake = prop_name.replace(' ', '_').lower()
                            animset_data[snake] = montage_path
                            log(f"  {prop_name}: {montage_path}")

                        # Also look for AnimSequence
                        seq_pattern = r"([^=]+)=.*?AnimSequence'([^']+)'"
                        seq_matches = re.findall(seq_pattern, export)
                        for prop_name, seq_path in seq_matches:
                            prop_name = prop_name.strip().split('=')[0].strip()
                            snake = prop_name.replace(' ', '_').lower()
                            animset_data[snake] = seq_path
                            log(f"  {prop_name}: {seq_path}")

                        if animset_data:
                            all_data[path] = animset_data
                            log(f"  Total via export: {len(animset_data)} references found")
            except Exception as e:
                log(f"  [ERROR] export_text failed: {e}")

    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(all_data, f, indent=2)

    log(f"\n=== Saved {len(all_data)} animsets to {OUTPUT_PATH} ===")

if __name__ == "__main__":
    main()
