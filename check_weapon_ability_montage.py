"""
Check if DA_ExampleWeaponAbility has a montage assigned.
"""
import unreal

OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/weapon_ability_check.txt"

def check_weapon_ability():
    with open(OUTPUT_FILE, 'w') as f:
        ability_path = "/Game/SoulslikeFramework/Data/WeaponAbilities/DA_ExampleWeaponAbility"
        ability = unreal.EditorAssetLibrary.load_asset(ability_path)

        if not ability:
            f.write(f"ERROR: Could not load {ability_path}\n")
            return

        f.write(f"Loaded: {ability.get_name()}\n")
        f.write(f"Class: {ability.get_class().get_name()}\n\n")

        # Check all properties
        f.write("Properties:\n")

        try:
            icon = ability.get_editor_property("icon")
            f.write(f"  Icon: {icon}\n")
        except Exception as e:
            f.write(f"  Icon: ERROR - {e}\n")

        try:
            name = ability.get_editor_property("ability_name")
            f.write(f"  AbilityName: {name}\n")
        except Exception as e:
            f.write(f"  AbilityName: ERROR - {e}\n")

        try:
            stat = ability.get_editor_property("affected_stat")
            f.write(f"  AffectedStat: {stat}\n")
        except Exception as e:
            f.write(f"  AffectedStat: ERROR - {e}\n")

        try:
            cost = ability.get_editor_property("cost")
            f.write(f"  Cost: {cost}\n")
        except Exception as e:
            f.write(f"  Cost: ERROR - {e}\n")

        try:
            montage = ability.get_editor_property("montage")
            f.write(f"  Montage: {montage}\n")
        except Exception as e:
            f.write(f"  Montage: ERROR - {e}\n")

        try:
            effect = ability.get_editor_property("additional_effect_class")
            f.write(f"  AdditionalEffectClass: {effect}\n")
        except Exception as e:
            f.write(f"  AdditionalEffectClass: ERROR - {e}\n")

        f.write("\nDone.\n")

if __name__ == "__main__":
    check_weapon_ability()
