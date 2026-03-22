# Export AnimGraph and check for broken bIsBlocking/bIsFalling connections
import unreal

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_cpp_export_after_fix.txt"

def main():
    log("Exporting AnimGraph state after fix...")
    
    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return
        
    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return
    
    # Export complete AnimGraph
    result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, OUTPUT_PATH)
    log(f"Export result: {len(result)} chars saved to {OUTPUT_PATH}")
    
    # Check for specific connections
    log("\nChecking for bIsBlocking connections...")
    if "K2Node_VariableGet_7" in result:
        # Find the bIsBlocking section
        lines = result.split('\n')
        in_blocking_section = False
        for i, line in enumerate(lines):
            if "K2Node_VariableGet_7" in line or "bIsBlocking" in line:
                in_blocking_section = True
            if in_blocking_section and "LinkedTo" in line:
                log(f"  {line.strip()}")
                if "LinkedTo (0)" in line:
                    log("  ^^^ DISCONNECTED!")
            if in_blocking_section and line.strip().startswith("NODE["):
                in_blocking_section = False
    
    log("\nChecking for bIsFalling connections...")
    if "K2Node_VariableGet_8" in result:
        lines = result.split('\n')
        in_falling_section = False
        for i, line in enumerate(lines):
            if "K2Node_VariableGet_8" in line or "bIsFalling" in line:
                in_falling_section = True
            if in_falling_section and "LinkedTo" in line:
                log(f"  {line.strip()}")
                if "LinkedTo (0)" in line:
                    log("  ^^^ DISCONNECTED!")
            if in_falling_section and line.strip().startswith("NODE["):
                in_falling_section = False
    
    log("\n[DONE]")

if __name__ == "__main__":
    main()
