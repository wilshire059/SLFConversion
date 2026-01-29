import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

paths = [
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask.DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/Spells/DA_FireballSpell.DA_FireballSpell",
]

log("=" * 70)
log("VERIFYING SPELL CLASSES")
log("=" * 70)

for path in paths:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"\n{path}")
        log(f"  Class: {asset.get_class().get_name()}")
        log(f"  Class Path: {asset.get_class().get_path_name()}")
        
        # Check category
        item_info = asset.get_editor_property("item_information")
        if item_info:
            cat_data = item_info.get_editor_property("category")
            if cat_data:
                log(f"  Category: {cat_data.get_editor_property('category')}")
            
            usable = item_info.get_editor_property("usable")
            log(f"  Usable: {usable}")
            
            item_class = item_info.get_editor_property("item_class")
            log(f"  ItemClass: {item_class}")
    else:
        log(f"\n{path}")
        log("  ERROR: Could not load")

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/verify_spells.txt", 'w') as f:
    f.write('\n'.join(output))
log("\nDone!")
