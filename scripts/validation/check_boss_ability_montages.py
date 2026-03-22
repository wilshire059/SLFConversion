"""Check Boss Ability Montage Assignments

This script extracts and compares the Montage property from boss AI ability data assets
between the working project (SLFConversion) and the backup (bp_only).
"""

import unreal

def check_boss_abilities():
    print("\n" + "="*80)
    print("BOSS ABILITY MONTAGE CHECK")
    print("="*80)

    boss_ability_paths = [
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FiveSwings",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FourSwings",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_HeavyAttack+Projectile",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_FastLongCombo",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_JukeSwings",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_SevenSwings",
        "/Game/SoulslikeFramework/Data/AI_Abilities/Boss/DA_AI_Ability_Boss_JumpAttackClose",
    ]

    # Also check enemy abilities
    enemy_ability_paths = [
        "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Melee01",
        "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Melee02",
        "/Game/SoulslikeFramework/Data/AI_Abilities/DA_AI_Ability_Melee03",
    ]

    all_paths = boss_ability_paths + enemy_ability_paths

    results = {}

    for path in all_paths:
        print(f"\nChecking: {path.split('/')[-1]}")
        asset = unreal.load_asset(path)

        if not asset:
            print(f"  ERROR: Asset not found!")
            results[path] = {"error": "Asset not found"}
            continue

        # Get class info
        class_name = asset.get_class().get_name()
        parent_class = asset.get_class().get_super_class()
        parent_name = parent_class.get_name() if parent_class else "None"

        print(f"  Class: {class_name} (Parent: {parent_name})")

        # Try to get Montage property
        montage = None

        # Try direct property access for C++ parent
        try:
            montage = asset.get_editor_property("montage")
            if montage:
                print(f"  Montage (direct): {montage.get_path_name()}")
            else:
                print(f"  Montage (direct): NULL")
        except Exception as e:
            print(f"  Montage (direct): FAILED - {e}")

        # Also try reflection for Blueprint properties
        if not montage:
            try:
                for prop in asset.get_class().properties():
                    prop_name = str(prop.get_name())
                    if "montage" in prop_name.lower():
                        print(f"  Found property: {prop_name}")
            except Exception as e:
                pass

        # Get Score property
        try:
            score = asset.get_editor_property("score")
            print(f"  Score: {score}")
        except Exception as e:
            print(f"  Score: FAILED - {e}")

        # Get Cooldown property
        try:
            cooldown = asset.get_editor_property("cooldown")
            print(f"  Cooldown: {cooldown}")
        except Exception as e:
            print(f"  Cooldown: FAILED - {e}")

        results[path] = {
            "class": class_name,
            "montage": str(montage.get_path_name()) if montage else None
        }

    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)

    for path, data in results.items():
        name = path.split('/')[-1]
        if "error" in data:
            print(f"  {name}: ERROR")
        elif data.get("montage"):
            montage_name = data["montage"].split('/')[-1].split('.')[0]
            print(f"  {name}: {montage_name}")
        else:
            print(f"  {name}: *** NO MONTAGE ***")

    print("\n" + "="*80)
    return results

if __name__ == "__main__":
    check_boss_abilities()
