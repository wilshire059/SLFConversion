# validate_animgraph_vars.py
# Validates that AnimBP variables were renamed correctly and Property Access nodes work

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/animgraph_validation.txt"

def log(msg):
    print(f"[Validate] {msg}")
    unreal.log(f"[Validate] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

# Clear output file
with open(OUTPUT_FILE, "w") as f:
    f.write("")

log("=" * 60)
log("ANIMGRAPH VARIABLE VALIDATION")
log("=" * 60)

# Blueprints with the renamed variables
ASSETS_TO_CHECK = [
    ("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
     ["bIsAccelerating", "bIsBlocking", "bIsFalling", "IsResting"]),
    ("/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
     ["IsGuarding"]),
]

total_found = 0
total_expected = 0

for asset_path, expected_vars in ASSETS_TO_CHECK:
    log(f"\nChecking: {asset_path}")

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        log(f"  ERROR: Could not load asset!")
        continue

    log(f"  Loaded: {asset.get_name()}")

    # Get variables using automation library
    try:
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(asset)
        log(f"  Variables found: {len(variables)}")

        for var in variables:
            log(f"    - {var}")

        # Check for expected variables (without "?")
        for expected in expected_vars:
            total_expected += 1
            if expected in variables:
                log(f"  [OK] Found '{expected}'")
                total_found += 1
            else:
                # Check for the old "?" version
                old_name = expected + "?"
                if old_name in variables:
                    log(f"  [FAIL] Found old name '{old_name}' - rename failed!")
                else:
                    log(f"  [WARN] Neither '{expected}' nor '{old_name}' found")

    except Exception as e:
        log(f"  ERROR: {e}")

# Check for compile errors
log("\n" + "=" * 60)
log("COMPILE CHECK")
log("=" * 60)

# Compile the AnimBP to check for errors
abp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
abp = unreal.EditorAssetLibrary.load_asset(abp_path)
if abp:
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(abp)
        if result:
            log(f"  [OK] ABP_SoulslikeCharacter_Additive compiled successfully!")
        else:
            log(f"  [WARN] Compile returned false (may have warnings)")
    except Exception as e:
        log(f"  [ERROR] Compile failed: {e}")

log("\n" + "=" * 60)
log(f"SUMMARY: {total_found}/{total_expected} expected variables found")
log("=" * 60)

if total_found == total_expected:
    log("\nSUCCESS: All renamed variables are present!")
else:
    log("\nWARNING: Some variables may not have been renamed correctly")

log(f"\nFull log saved to: {OUTPUT_FILE}")
