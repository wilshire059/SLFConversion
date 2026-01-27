"""Check if Niagara system is still set on armor items."""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
]

log("=" * 60)
log("CHECKING NIAGARA SYSTEMS")
log("=" * 60)

for path in PATHS:
    name = path.split('/')[-1]
    log(f"\n--- {name} ---")

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log("  [ERROR] Failed to load")
        continue

    try:
        # Check C++ property first
        niagara = asset.get_editor_property('world_niagara_system')
        log(f"  WorldNiagaraSystem (C++): {niagara}")
    except Exception as e:
        log(f"  WorldNiagaraSystem (C++): not found - {e}")

    try:
        # Check nested struct
        item_info = asset.get_editor_property('item_information')
        niagara_nested = item_info.get_editor_property('world_niagara_system')
        log(f"  ItemInfo.WorldNiagaraSystem: {niagara_nested}")
    except Exception as e:
        log(f"  ItemInfo.WorldNiagaraSystem: {e}")

log("\n" + "=" * 60)
