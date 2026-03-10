# Simple item class check
import unreal

output = []
def log(msg):
    print(msg)
    unreal.log(str(msg))
    output.append(str(msg))

paths = [
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask.DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/Spells/DA_FireballSpell.DA_FireballSpell",
]

for path in paths:
    log(f"\n=== Checking: {path} ===")
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"  Class: {asset.get_class().get_name()}")
        log(f"  Class Path: {asset.get_class().get_path_name()}")
    else:
        log("  ERROR: Could not load")

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/single_item_check.txt", 'w') as f:
    f.write('\n'.join(output))
log(f"\nWrote to: single_item_check.txt")
