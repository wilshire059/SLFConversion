# verify_item_data.py
# Quick check that item DisplayNames are set

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/item_verify.txt"

ITEMS_TO_CHECK = [
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/DA_Apple",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
]

def log(msg):
    print(msg)
    unreal.log(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def main():
    with open(OUTPUT_FILE, "w") as f:
        f.write("")
    
    log("=== Item Data Verification ===\n")
    
    for path in ITEMS_TO_CHECK:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            log(f"[SKIP] {path.split('/')[-1]} - not found")
            continue
        
        try:
            item_info = asset.get_editor_property('item_information')
            display_name = item_info.get_editor_property('display_name')
            item_tag = item_info.get_editor_property('item_tag')
            
            log(f"[CHECK] {path.split('/')[-1]}")
            log(f"  DisplayName: '{display_name}'")
            log(f"  ItemTag: '{item_tag}'")
            log("")
        except Exception as e:
            log(f"[ERROR] {path.split('/')[-1]}: {e}")
    
    log("=== Verification Complete ===")

if __name__ == "__main__":
    main()
