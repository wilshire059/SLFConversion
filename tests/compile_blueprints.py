# compile_blueprints.py
# Quick test to compile Blueprints and count errors

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/compile_test_output.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("BLUEPRINT COMPILATION TEST")
    results.append("=" * 60)

    # Get all Blueprints in the SoulslikeFramework folder
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Test a few key Blueprints that had FItemInfo issues
    test_bps = [
        "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework",
        "/Game/SoulslikeFramework/Blueprints/UI/W_HUD",
        "/Game/SoulslikeFramework/Blueprints/UI/W_Inventory",
        "/Game/SoulslikeFramework/Blueprints/Items/B_Interactable_Item",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
    ]

    success_count = 0
    fail_count = 0

    for bp_path in test_bps:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if bp:
            try:
                # Try to compile
                unreal.KismetEditorUtilities.compile_blueprint(bp)
                gen_class = bp.generated_class()
                if gen_class:
                    results.append(f"OK: {bp_path}")
                    success_count += 1
                else:
                    results.append(f"FAIL (no class): {bp_path}")
                    fail_count += 1
            except Exception as e:
                results.append(f"ERROR: {bp_path} - {e}")
                fail_count += 1
        else:
            results.append(f"NOT FOUND: {bp_path}")
            fail_count += 1

    results.append("")
    results.append(f"Success: {success_count}, Fail: {fail_count}")

    # Write results
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))

run()
