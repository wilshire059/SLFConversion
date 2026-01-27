# clear_animbp_only.py
# Clear ONLY AnimBP EventGraphs while preserving AnimGraph and variables

import unreal

ANIMBP_PATHS = {
    "ABP_SoulslikeEnemy": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "ABP_SoulslikeBossNew": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "ABP_SoulslikeNPC": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
}

def main():
    print("=== Clearing AnimBP EventGraphs (Keep Variables) ===")
    print("")
    
    success = 0
    failed = 0
    
    for name, path in ANIMBP_PATHS.items():
        print(f"Processing: {name}")
        
        # Load the AnimBlueprint
        bp = unreal.load_asset(path)
        if not bp:
            print(f"  [FAIL] Cannot load asset")
            failed += 1
            continue
        
        try:
            # Clear EventGraph but keep variables (for AnimGraph Property Access)
            unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            print(f"  [OK] Cleared EventGraph")
            
            # Compile
            result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            print(f"  [OK] Compiled: {result}")
            
            # Save
            unreal.EditorAssetLibrary.save_asset(path)
            print(f"  [OK] Saved")
            
            success += 1
        except Exception as e:
            print(f"  [FAIL] Error: {e}")
            failed += 1
    
    print("")
    print(f"=== Complete: {success} cleared, {failed} failed ===")

if __name__ == "__main__":
    main()
