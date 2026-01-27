"""Fix armor struct mismatch by re-saving assets"""
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
log("FIXING ARMOR STRUCT MISMATCH BY RE-SAVING")
log("=" * 60)

for path in ARMOR_PATHS:
    name = path.split('/')[-1]
    log(f"\nProcessing: {name}")

    # Load the asset (this triggers the mismatch warning but still loads)
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log(f"  [ERROR] Failed to load")
        continue

    # Mark it dirty and save - this re-serializes with correct struct name
    asset.modify()
    success = unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False)
    if success:
        log(f"  [OK] Re-saved successfully")
    else:
        log(f"  [ERROR] Failed to save")

log("\n" + "=" * 60)
log("DONE - Now reload to verify warnings are gone")
log("=" * 60)
