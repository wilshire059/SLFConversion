"""
Reduce poison sword damage from testing values.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/reduce_poison_sword_damage.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Reducing Poison Sword Damage")
    unreal.log_warning("=" * 70)

    poison_sword_path = "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword"

    asset = unreal.load_asset(poison_sword_path)
    if not asset:
        unreal.log_error(f"Failed to load {poison_sword_path}")
        return

    unreal.log_warning(f"Loaded: {asset.get_name()}")

    # Get ItemInformation struct
    try:
        item_info = asset.get_editor_property('item_information')
        if item_info:
            # Get weapon details
            weapon_details = item_info.get_editor_property('weapon_details')
            if weapon_details:
                # Get and reduce damage values
                damage_values = weapon_details.get_editor_property('damage_values')
                if damage_values:
                    unreal.log_warning(f"Current damage values: {damage_values}")

                    # Reduce to reasonable values
                    # Original testing values were very high, reduce to normal weapon levels
                    new_damage = {}
                    for key, val in damage_values.items():
                        # Reduce damage to 10% of testing value, minimum 10
                        new_val = max(10, int(val * 0.1))
                        new_damage[key] = new_val
                        unreal.log_warning(f"  {key}: {val} -> {new_val}")

                    weapon_details.set_editor_property('damage_values', new_damage)
                    unreal.log_warning("Damage values reduced!")

        # Also check for base damage property
        if hasattr(asset, 'base_damage'):
            old_damage = asset.get_editor_property('base_damage')
            new_damage = max(15.0, old_damage * 0.15)
            asset.set_editor_property('base_damage', new_damage)
            unreal.log_warning(f"Base damage: {old_damage} -> {new_damage}")

    except Exception as e:
        unreal.log_warning(f"Exception accessing damage: {e}")

    # Try direct CDO approach for weapon stats
    try:
        gen_class = asset.generated_class() if hasattr(asset, 'generated_class') else None
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Check for attack power or damage properties
                for prop_name in ['base_physical_damage', 'base_damage', 'physical_attack_power', 'attack_power']:
                    if hasattr(cdo, prop_name):
                        try:
                            old_val = cdo.get_editor_property(prop_name)
                            if isinstance(old_val, (int, float)) and old_val > 50:
                                new_val = max(15, old_val * 0.15)
                                cdo.set_editor_property(prop_name, new_val)
                                unreal.log_warning(f"CDO {prop_name}: {old_val} -> {new_val}")
                        except:
                            pass
    except Exception as e:
        unreal.log_warning(f"CDO approach: {e}")

    # Save
    unreal.EditorAssetLibrary.save_asset(poison_sword_path)
    unreal.log_warning(f"Saved {poison_sword_path}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
