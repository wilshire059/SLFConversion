"""Quick test to load armor assets and check for struct warnings"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ARMOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHat",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleGreaves",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleTalisman",
]

log("=" * 60)
log("TESTING ARMOR ASSET LOAD")
log("=" * 60)

for path in ARMOR_PATHS:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        # Try to access ItemInformation
        try:
            item_info = asset.get_editor_property('item_information')
            display_name = item_info.get_editor_property('display_name')
            log(f"[OK] {path.split('/')[-1]}: DisplayName = '{display_name}'")
        except Exception as e:
            log(f"[ERR] {path.split('/')[-1]}: Failed to get ItemInformation - {e}")
    else:
        log(f"[ERR] {path.split('/')[-1]}: Failed to load")

log("=" * 60)
log("TEST COMPLETE")
log("=" * 60)
