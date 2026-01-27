import unreal

def log(msg):
    unreal.log_warning(f"[SM_DIAG] {msg}")

def main():
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return
    
    log("=" * 70)
    log("STATE MACHINE STRUCTURE")
    log("=" * 70)
    
    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Get state machine structure for "States" (the main locomotion states)
        result = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "States")
        for line in result.split('\n'):
            log(line)
        
        log("")
        log("=" * 70)
        log("STATE MACHINE: Locomotion")
        log("=" * 70)
        result2 = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "Locomotion")
        for line in result2.split('\n'):
            log(line)
    else:
        log("[ERROR] SLFAutomationLibrary not found")

if __name__ == "__main__":
    main()
