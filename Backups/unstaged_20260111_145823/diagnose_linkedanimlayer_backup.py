# diagnose_linkedanimlayer_backup.py
# Run the AAA-quality LinkedAnimLayer diagnosis on backup project

import unreal

ANIMBP = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/linkedanimlayer_diagnosis_backup.txt"

def log(msg):
    print(msg)
    unreal.log(msg)

def main():
    log("=== LinkedAnimLayer Diagnosis (BACKUP) ===")
    
    # Load AnimBP
    asset = unreal.EditorAssetLibrary.load_asset(ANIMBP)
    if not asset:
        log(f"ERROR: Failed to load {ANIMBP}")
        return
    
    log(f"Loaded: {asset.get_name()}")
    
    # Get automation library
    automation_lib = unreal.SLFAutomationLibrary
    
    # Run diagnosis
    log("\n--- Running DiagnoseLinkedAnimLayerNodes ---")
    diagnosis = automation_lib.diagnose_linked_anim_layer_nodes(asset)
    log(diagnosis)
    
    # Save to file
    with open(OUTPUT_FILE, "w") as f:
        f.write(diagnosis)
    log(f"\nSaved diagnosis to: {OUTPUT_FILE}")
    
    # Also get compile errors
    log("\n--- Running GetBlueprintCompileErrors ---")
    errors = automation_lib.get_blueprint_compile_errors(asset)
    log(errors)

if __name__ == "__main__":
    main()
