# check_backup_animlayers.py
# Check if the Animation Layer issue exists in the backup

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/backup_animlayer_check.txt"

def log(msg):
    print(f"[BackupCheck] {msg}")
    unreal.log(f"[BackupCheck] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

# Clear output
with open(OUTPUT_FILE, "w") as f:
    f.write("")

log("=" * 60)
log("CHECK LINKED ANIM GRAPH FUNCTIONS")
log("=" * 60)

# Check the linked anim graphs for layer functions
LINKED_ANIM_GRAPHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/_Misc/ALI_LocomotionStates",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/_Misc/ALI_OverlayStates",
]

for path in LINKED_ANIM_GRAPHS:
    log(f"\n{path}")
    abp = unreal.EditorAssetLibrary.load_asset(path)
    if not abp:
        log("  ERROR: Could not load")
        continue

    log(f"  Name: {abp.get_name()}")
    log(f"  Class: {abp.get_class().get_name()}")

    # Get functions
    try:
        functions = unreal.SLFAutomationLibrary.get_blueprint_functions(abp)
        log(f"  Functions ({len(functions)}):")
        for func in functions:
            log(f"    - {func}")
    except Exception as e:
        log(f"  Functions error: {e}")

    # Check for LL_ named layers
    log(f"\n  Looking for LL_ layer functions:")
    for func in functions:
        if func.startswith("LL_"):
            log(f"    FOUND: {func}")

log("\n" + "=" * 60)
log("SUMMARY")
log("=" * 60)
log("If the LL_ layers are not found in the linked anim graphs,")
log("the issue exists in the source asset (not caused by migration).")
log(f"\nFull log saved to: {OUTPUT_FILE}")
