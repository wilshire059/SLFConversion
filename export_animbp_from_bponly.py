"""
Export AnimGraph from bp_only project to see working LL_TwoHanded implementation
"""
import unreal

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_bponly_export.txt"

def main():
    log("Exporting AnimGraph from bp_only project...")

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

    # Search for LL_TwoHanded graphs
    log("\n=== Looking for LL_TwoHanded graphs ===")
    lines = result.split('\n')
    for i, line in enumerate(lines):
        if "TwoHanded" in line or "LL_" in line:
            # Print context around the match
            for j in range(max(0, i-2), min(len(lines), i+10)):
                log(lines[j])
            log("---")

    log("\n[DONE]")

if __name__ == "__main__":
    main()
