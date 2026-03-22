# apply_item_tags.py
# Apply GameplayTags to item assets

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

# Item tag mappings
ITEM_TAGS = {
    "DA_Apple": "SoulslikeFramework.Items.Examples.RejuvenatingApple",
    "DA_BossMace": "SoulslikeFramework.Items.Examples.BossMace",
    "DA_CrimsonEverbloom": "SoulslikeFramework.Items.Examples.Flower",
    "DA_Cube": "SoulslikeFramework.Items.Examples.Cube",
    "DA_ExampleArmor": "SoulslikeFramework.Items.Examples.Armor",
    "DA_ExampleArmor02": "SoulslikeFramework.Items.Examples.Armor02",
    "DA_ExampleBracers": "SoulslikeFramework.Items.Examples.Bracers",
    "DA_ExampleGreaves": "SoulslikeFramework.Items.Examples.Greaves",
    "DA_ExampleHat": "SoulslikeFramework.Items.Examples.Hat",
    "DA_ExampleHelmet": "SoulslikeFramework.Items.Examples.Helmet",
    "DA_ExampleTalisman": "SoulslikeFramework.Items.Examples.Talisman",
    "DA_Greatsword": "SoulslikeFramework.Items.Examples.Greatsword",
    "DA_HealthFlask": "SoulslikeFramework.Items.Examples.HealthFlask",
    "DA_Katana": "SoulslikeFramework.Items.Examples.Katana",
    "DA_Lantern": "SoulslikeFramework.Items.Examples.Lantern",
    "DA_PoisonSword": "SoulslikeFramework.Items.Examples.Venomfang",
    "DA_Sword01": "SoulslikeFramework.Items.Examples.Sword01",
    "DA_Sword02": "SoulslikeFramework.Items.Examples.Sword02",
    "DA_ThrowingKnife": "SoulslikeFramework.Items.Examples.ThrowingKnife",
    "DA_MagicSpell": "SoulslikeFramework.Items.Examples.MagicSpell",
    "DA_FireballSpell": "SoulslikeFramework.Items.Examples.FireballSpell",
    "DA_IceShardSpell": "SoulslikeFramework.Items.Examples.IceShardSpell",
    "DA_LightningBoltSpell": "SoulslikeFramework.Items.Examples.LightningBoltSpell",
    "DA_HolyOrbSpell": "SoulslikeFramework.Items.Examples.HolyOrbSpell",
    "DA_PoisonBlobSpell": "SoulslikeFramework.Items.Examples.PoisonBlobSpell",
    "DA_ArcaneMissileSpell": "SoulslikeFramework.Items.Examples.ArcaneMissileSpell",
    "DA_BlackholeSpell": "SoulslikeFramework.Items.Examples.BlackholeSpell",
    "DA_BeamSpell": "SoulslikeFramework.Items.Examples.BeamSpell",
    "DA_AOEExplosionSpell": "SoulslikeFramework.Items.Examples.AOEExplosionSpell",
    "DA_ShieldSpell": "SoulslikeFramework.Items.Examples.ShieldSpell",
    "DA_HealingSpell": "SoulslikeFramework.Items.Examples.HealingSpell",
    "DA_SummoningCircleSpell": "SoulslikeFramework.Items.Examples.SummoningCircleSpell",
}

def main():
    log("=== Applying Item Tags ===")

    success_count = 0
    error_count = 0

    for item_name, tag_str in ITEM_TAGS.items():
        item_path = f"/Game/SoulslikeFramework/Data/Items/{item_name}"

        item = unreal.EditorAssetLibrary.load_asset(item_path)
        if not item:
            log(f"[SKIP] {item_name} not found")
            continue

        try:
            # Get ItemInformation struct
            item_info = item.get_editor_property('item_information')
            if not item_info:
                log(f"[ERROR] {item_name} has no item_information")
                error_count += 1
                continue

            # Get the current tag struct and set its tag_name property directly
            item_tag = item_info.get_editor_property('item_tag')

            # Set tag_name on the GameplayTag struct
            item_tag.set_editor_property('tag_name', unreal.Name(tag_str))

            # Set the modified tag back to item_info
            item_info.set_editor_property('item_tag', item_tag)

            # Set item_info back to item
            item.set_editor_property('item_information', item_info)

            # Save the asset
            unreal.EditorAssetLibrary.save_loaded_asset(item)
            log(f"[OK] {item_name}: {tag_str}")
            success_count += 1

        except Exception as e:
            log(f"[ERROR] {item_name}: {e}")
            error_count += 1

    log(f"\n=== COMPLETE: {success_count} ok, {error_count} errors ===")

if __name__ == "__main__":
    main()
