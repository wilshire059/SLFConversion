"""Check Niagara system on armor items vs weapons"""
import unreal

def log(msg):
    unreal.log_warning(str(msg))

ARMOR_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleBracers",
]

WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_Katana",
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01",
]

log("=" * 60)
log("CHECKING NIAGARA ON ARMOR vs WEAPONS")
log("=" * 60)

log("\n--- ARMOR ITEMS ---")
for path in ARMOR_PATHS:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        try:
            # Check top-level WorldNiagaraSystem first
            try:
                top_niagara = asset.get_editor_property('world_niagara_system')
                log(f"[{path.split('/')[-1]}] Top-level WorldNiagaraSystem: {top_niagara}")
            except:
                log(f"[{path.split('/')[-1]}] No top-level world_niagara_system property")

            # Check nested in ItemInformation
            item_info = asset.get_editor_property('item_information')
            niagara = item_info.get_editor_property('world_niagara_system')
            log(f"[{path.split('/')[-1]}] ItemInfo.WorldNiagaraSystem: {niagara}")
        except Exception as e:
            log(f"[{path.split('/')[-1]}] Error: {e}")
    else:
        log(f"[{path.split('/')[-1]}] FAILED TO LOAD")

log("\n--- WEAPON ITEMS ---")
for path in WEAPON_PATHS:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        try:
            # Check top-level WorldNiagaraSystem first
            try:
                top_niagara = asset.get_editor_property('world_niagara_system')
                log(f"[{path.split('/')[-1]}] Top-level WorldNiagaraSystem: {top_niagara}")
            except:
                log(f"[{path.split('/')[-1]}] No top-level world_niagara_system property")

            # Check nested in ItemInformation
            item_info = asset.get_editor_property('item_information')
            niagara = item_info.get_editor_property('world_niagara_system')
            log(f"[{path.split('/')[-1]}] ItemInfo.WorldNiagaraSystem: {niagara}")
        except Exception as e:
            log(f"[{path.split('/')[-1]}] Error: {e}")
    else:
        log(f"[{path.split('/')[-1]}] FAILED TO LOAD")

log("=" * 60)
