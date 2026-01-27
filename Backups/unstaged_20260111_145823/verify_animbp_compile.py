# verify_animbp_compile.py
# Verify AnimBPs can be loaded and report any compile errors

import unreal

ANIMBPS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
]

def log(msg):
    print(f"[AnimBP Verify] {msg}")
    unreal.log(f"[AnimBP Verify] {msg}")

def main():
    log("=== AnimBP Compile Verification ===")

    all_ok = True

    for asset_path in ANIMBPS:
        asset_name = asset_path.split("/")[-1]

        # Load the asset
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            log(f"[FAIL] {asset_name}: Could not load")
            all_ok = False
            continue

        # Get blueprint
        blueprint = unreal.EditorAssetLibrary.find_asset_data(asset_path).get_asset()

        # Check compile status
        try:
            status = unreal.KismetEditorUtilities.compile_blueprint(blueprint)
            log(f"[COMPILE] {asset_name}: Compiled (status={status})")
        except Exception as e:
            log(f"[ERROR] {asset_name}: Compile error - {e}")
            all_ok = False

    if all_ok:
        log("=== RESULT: All AnimBPs compile successfully ===")
    else:
        log("=== RESULT: Some AnimBPs have issues ===")

if __name__ == "__main__":
    main()
