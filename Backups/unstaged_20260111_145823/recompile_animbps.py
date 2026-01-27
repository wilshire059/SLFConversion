# recompile_animbps.py
# Force recompile all AnimBPs after migration fixes

import unreal

ANIMBP_PATHS = [
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
]

def log(msg):
    unreal.log_warning(f"[Recompile] {msg}")

def main():
    log("=== Recompiling All AnimBPs ===")
    
    success_count = 0
    error_count = 0
    
    for path in ANIMBP_PATHS:
        name = path.split("/")[-1]
        log(f"Processing: {name}")
        
        bp = unreal.load_asset(path)
        if not bp:
            log(f"  ERROR: Cannot load {name}")
            error_count += 1
            continue
        
        # Get parent
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent: {parent}")
        
        # Force compile
        try:
            result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log(f"  Compile result: {result}")
        except Exception as e:
            log(f"  Compile exception: {e}")
        
        # Check errors
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        
        # Extract summary
        if "Errors: 0" in errors and "Warnings: 0" in errors:
            log(f"  [OK] No errors")
            success_count += 1
        else:
            # Extract just the error/warning counts
            for line in errors.split("\n"):
                if "Errors:" in line or "Warnings:" in line or "[ERROR]" in line or "[WARNING]" in line:
                    log(f"  {line.strip()}")
            error_count += 1
        
        # Save
        unreal.EditorAssetLibrary.save_asset(path)
        log(f"  Saved")
    
    log(f"=== Summary: {success_count} OK, {error_count} with issues ===")
    
    return error_count == 0

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[Recompile] ALL ANIMBPS COMPILED SUCCESSFULLY")
    else:
        unreal.log_warning("[Recompile] SOME ANIMBPS HAVE ISSUES")
