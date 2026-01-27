# fix_player_animbp_full.py
# Full fix for player AnimBP: clear ALL Blueprint logic, keep only AnimGraph

import unreal

def log(msg):
    unreal.log_warning(f"[FullFix] {msg}")

def main():
    log("=== Full Player AnimBP Fix ===")
    
    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    cpp_parent = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"
    
    # Load the AnimBP
    bp = unreal.load_asset(bp_path)
    if not bp:
        log("ERROR: Cannot load AnimBP")
        return False
    
    # Step 1: Check parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Step 1: Current parent: {current_parent}")
    
    if "ABP_SoulslikeCharacter_Additive" not in current_parent:
        log(f"Step 1: Reparenting to {cpp_parent}")
        success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
        if not success:
            log("ERROR: Reparenting failed")
            return False
        log("Step 1: Reparenting successful")
    else:
        log("Step 1: Already reparented to C++ class")
    
    # Step 2: Clear ALL Blueprint logic (EventGraph AND function graphs)
    # This is crucial because:
    # - NativeUpdateAnimation() already sets all the variables
    # - The Blueprint function graphs conflict with C++ properties
    log("Step 2: Clearing ALL Blueprint logic...")
    clear_result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
    log(f"Step 2: Clear result: {clear_result}")
    
    # Step 3: Compile
    log("Step 3: Compiling...")
    compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    log(f"Step 3: Compile result: {compile_result}")
    
    # Step 4: Save
    log("Step 4: Saving...")
    save_result = unreal.EditorAssetLibrary.save_asset(bp_path)
    log(f"Step 4: Save result: {save_result}")
    
    # Verify
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Final compile status:\n{errors}")
    
    log("=== Full Player AnimBP Fix Done ===")
    return True

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[FullFix] SUCCESS")
    else:
        unreal.log_error("[FullFix] FAILED")
