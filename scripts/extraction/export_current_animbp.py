"""
Export complete AnimGraph from current SLFConversion project
Including all Animation Layer implementations (LL_TwoHanded_Right, etc.)
"""
import unreal

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_current_complete.txt"

def main():
    log("Exporting complete AnimGraph from current project...")

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Export complete AnimGraph - this includes ALL graphs including layer implementations
    result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, OUTPUT_PATH)
    log(f"Export result: {len(result)} chars saved to {OUTPUT_PATH}")

    # Search for LL_TwoHanded graphs
    log("\n=== Looking for LL_TwoHanded graphs ===")
    lines = result.split('\n')
    in_twohanded = False
    for i, line in enumerate(lines):
        if "GRAPH[" in line and "TwoHanded" in line:
            in_twohanded = True
            log(f"\n=== FOUND GRAPH ===")
        if in_twohanded:
            log(line)
            if line.strip().startswith("GRAPH[") and "TwoHanded" not in line:
                in_twohanded = False
                log("=== END GRAPH ===\n")

    # Also search for any SequencePlayer nodes that might reference poses
    log("\n=== Looking for SequencePlayer nodes ===")
    for i, line in enumerate(lines):
        if "SequencePlayer" in line or "Sequence" in line and "2h" in line.lower():
            for j in range(max(0, i-5), min(len(lines), i+10)):
                log(lines[j])
            log("---")

    log("\n[DONE]")

if __name__ == "__main__":
    main()
