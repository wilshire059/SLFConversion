# check_blend_bindings.py
# Check BlendListByEnum bindings in AnimBP

import unreal

def log(msg):
    unreal.log_warning(f"[BLEND] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BLENDLISTBYENUM BINDINGS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    # Check if there's a diagnostic function
    if hasattr(unreal, 'SLFAutomationLibrary'):
        diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Find BlendListByEnum sections
        lines = diagnostic.split('\n')
        in_blend_section = False
        for line in lines:
            if 'BLEND POSES BY ENUM' in line or 'BlendListByEnum' in line:
                in_blend_section = True
            if in_blend_section:
                log(line)
                if line.strip() == '' and in_blend_section:
                    in_blend_section = False

    # Also check variables
    log("")
    log("=== OVERLAY STATE VARIABLES ===")
    bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    for v in bp_vars:
        if 'Overlay' in v or 'overlay' in v:
            log(f"  {v}")

    # Export state for detailed inspection
    log("")
    log("=== EXPORTING ANIMGRAPH STATE ===")
    state = unreal.SLFAutomationLibrary.export_anim_graph_state(bp, "C:/scripts/SLFConversion/migration_cache/current_animgraph_state.txt")
    log(f"Exported to: C:/scripts/SLFConversion/migration_cache/current_animgraph_state.txt")

    # Show first 100 lines
    if state:
        for line in state.split('\n')[:100]:
            log(line)

if __name__ == "__main__":
    main()
