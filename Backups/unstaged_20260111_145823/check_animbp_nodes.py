# check_animbp_nodes.py
# Check how many nodes are in the AnimBP's EventGraph

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP NODE COUNT CHECK")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP: {animbp_path}")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent Class: {parent}")

    # Get node count
    log("")
    log("Node counts per graph:")
    try:
        result = unreal.SLFAutomationLibrary.get_blueprint_graph_nodes(bp)
        for line in result.split('\n')[:20]:
            if line.strip():
                log(f"  {line}")
    except Exception as e:
        log(f"ERROR: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
