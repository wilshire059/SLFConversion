# check_da_to_bp.py
# Check which Blueprint class each weapon data asset points to

import unreal

DATA_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword02",
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
    "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
]

def log(msg):
    print(f"[CheckDA] {msg}")
    unreal.log_warning(f"[CheckDA] {msg}")

def main():
    log("=" * 70)
    log("CHECKING DATA ASSET TO BLUEPRINT CLASS LINKS")
    log("=" * 70)

    for da_path in DATA_ASSETS:
        da_name = da_path.split('/')[-1]

        da = unreal.load_asset(da_path)
        if not da:
            log(f"{da_name}: NOT FOUND")
            continue

        # Get ItemInformation.ItemClass
        try:
            item_info = da.get_editor_property('item_information')
            item_class = item_info.get_editor_property('item_class')

            if item_class:
                class_name = item_class.get_name()
                log(f"{da_name}: ItemClass = {class_name}")
            else:
                log(f"{da_name}: ItemClass = NULL")
        except Exception as e:
            log(f"{da_name}: ERROR - {e}")

if __name__ == "__main__":
    main()
