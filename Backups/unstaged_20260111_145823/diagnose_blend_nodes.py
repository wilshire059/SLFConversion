# diagnose_blend_nodes.py - Diagnose BlendListByEnum node structure
import unreal

def log(msg):
    unreal.log_warning(f"[DIAG] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING BLENDLISTBYENUM NODES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)

    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Get detailed diagnosis
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(bp)
        if diagnosis:
            for line in diagnosis.split('\n'):
                if 'BlendList' in line or 'OverlayState' in line or 'Binding' in line or 'Path' in line:
                    log(line)

if __name__ == "__main__":
    main()
