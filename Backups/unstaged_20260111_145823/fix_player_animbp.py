# fix_player_animbp.py
# Reparent player AnimBP to C++ AnimInstance and clear EventGraph

import unreal

def log(msg):
    unreal.log_warning(f"[FixAnimBP] {msg}")

def main():
    log("=== Starting Player AnimBP Fix ===")
    
    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    cpp_parent = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"
    
    # Load the AnimBP
    bp = unreal.load_asset(bp_path)
    if not bp:
        log("ERROR: Cannot load AnimBP")
        return False
    
    # Check current parent
    current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Current parent: {current_parent}")
    
    # Reparent to C++ class
    log(f"Reparenting to: {cpp_parent}")
    success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
    if not success:
        log("ERROR: Reparenting failed")
        return False
    log("Reparenting successful")
    
    # Check new parent
    new_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"New parent: {new_parent}")
    
    # Clear EventGraph but keep variables
    log("Clearing EventGraph (keeping variables)...")
    clear_result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
    log(f"Clear result: {clear_result}")
    
    # Compile and save
    log("Compiling and saving...")
    compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    log(f"Compile result: {compile_result}")
    
    # Check compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile errors after fix:\n{errors}")
    
    log("=== Player AnimBP Fix Complete ===")
    return True

if __name__ == "__main__":
    result = main()
    if result:
        unreal.log_warning("[FixAnimBP] SUCCESS")
    else:
        unreal.log_error("[FixAnimBP] FAILED")
