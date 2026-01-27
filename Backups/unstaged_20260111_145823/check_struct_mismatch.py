"""Check which items have struct mismatch warnings"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ITEM_PATHS = [
    # Armor
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    # Weapons
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    # Other
    "/Game/SoulslikeFramework/Data/Items/DA_Apple",
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
]

log("=" * 60)
log("Loading items to check for struct mismatch warnings")
log("Watch the LogClass warnings above each item")
log("=" * 60)

for path in ITEM_PATHS:
    log(f"\n--- Loading: {path.split('/')[-1]} ---")
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"  Loaded OK, class: {asset.get_class().get_name()}")
    else:
        log(f"  FAILED TO LOAD")

log("\n" + "=" * 60)
