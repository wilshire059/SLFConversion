# check_rename_api.py
# Check what rename APIs are available in Unreal Python

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/api_check.txt"

# Clear output file
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
with open(OUTPUT_FILE, "w") as f:
    f.write("")

def log(msg):
    print(f"[CheckAPI] {msg}")
    unreal.log(f"[CheckAPI] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

def main():
    log("=" * 60)
    log("CHECKING AVAILABLE RENAME APIS")
    log("=" * 60)

    # Check BlueprintEditorLibrary methods
    log("")
    log("BlueprintEditorLibrary methods:")
    for attr in dir(unreal.BlueprintEditorLibrary):
        if not attr.startswith('_'):
            log(f"  {attr}")

    # Check if rename_member_variable exists
    log("")
    log("Checking for rename methods:")
    if hasattr(unreal.BlueprintEditorLibrary, 'rename_member_variable'):
        log("  rename_member_variable: EXISTS")
    else:
        log("  rename_member_variable: NOT FOUND")

    if hasattr(unreal.BlueprintEditorLibrary, 'rename_variable'):
        log("  rename_variable: EXISTS")
    else:
        log("  rename_variable: NOT FOUND")

    # Check Kismet library
    log("")
    log("KismetEditorUtilities methods:")
    if hasattr(unreal, 'KismetEditorUtilities'):
        for attr in dir(unreal.KismetEditorUtilities):
            if 'rename' in attr.lower():
                log(f"  {attr}")
    else:
        log("  KismetEditorUtilities not found")

    # Check EditorUtilityLibrary
    log("")
    log("EditorUtilityLibrary methods:")
    if hasattr(unreal, 'EditorUtilityLibrary'):
        for attr in dir(unreal.EditorUtilityLibrary):
            if 'rename' in attr.lower():
                log(f"  {attr}")
    else:
        log("  EditorUtilityLibrary not found")

    # Check FBlueprintEditorUtils equivalent
    log("")
    log("Looking for blueprint utils...")
    for module_attr in dir(unreal):
        if 'blueprint' in module_attr.lower() and 'util' in module_attr.lower():
            log(f"  Found: {module_attr}")

    # Try to load the AnimBP and examine its properties
    log("")
    log("Examining AnimBlueprint properties...")
    asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive")
    if asset:
        log(f"  Loaded: {asset.get_name()}")
        log(f"  Type: {type(asset).__name__}")

        # Check new_variables
        if hasattr(asset, 'new_variables'):
            new_vars = asset.get_editor_property('new_variables')
            log(f"  new_variables count: {len(new_vars)}")
            for i, var in enumerate(new_vars):
                var_name = str(var.var_name)
                if "?" in var_name:
                    log(f"    [{i}] {var_name}")
                    log(f"        var_guid: {var.var_guid}")
                    log(f"        type: {type(var).__name__}")
                    # Check what we can do with the variable descriptor
                    log(f"        attributes: {[a for a in dir(var) if not a.startswith('_')][:10]}")

    log("=" * 60)

if __name__ == "__main__":
    main()
