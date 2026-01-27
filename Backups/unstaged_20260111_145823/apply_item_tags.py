# apply_item_tags.py
# Apply GameplayTags to item assets

import unreal

def log(msg):
    print(msg)
    unreal.log_warning(str(msg))

# Item tag mappings from apply_item_data.py
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
    "DA_PrisonKey01": "SoulslikeFramework.Items.Examples.PrisonKey",
    "DA_Shield": "SoulslikeFramework.Items.Examples.Shield",
    "DA_Sword01": "SoulslikeFramework.Items.Examples.Sword01",
    "DA_Sword02": "SoulslikeFramework.Items.Examples.Sword02",
    "DA_ThrowingKnife": "SoulslikeFramework.Items.Examples.ThrowingKnife",
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

            # Request the gameplay tag
            tag = unreal.GameplayTag.request_gameplay_tag(unreal.Name(tag_str))

            if tag.is_valid():
                item_info.set_editor_property('item_tag', tag)
                item.set_editor_property('item_information', item_info)
                unreal.EditorAssetLibrary.save_loaded_asset(item)
                log(f"[OK] {item_name}: {tag_str}")
                success_count += 1
            else:
                log(f"[WARN] {item_name}: Tag not valid: {tag_str}")
                error_count += 1

        except Exception as e:
            log(f"[ERROR] {item_name}: {e}")
            error_count += 1

    log(f"\n=== COMPLETE: {success_count} ok, {error_count} errors ===")

if __name__ == "__main__":
    main()
