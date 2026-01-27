# check_animbp_eventgraph.py
# Check if the AnimBP EventGraph still has logic

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("ANIMBP EVENTGRAPH CHECK")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP: {animbp_path}")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent Class: {parent}")

    # Get graph info
    log("")
    log("Getting graph information:")
    try:
        # Use the automation library to get Blueprint info
        info = unreal.SLFAutomationLibrary.get_blueprint_all_graphs_info(bp)
        log(f"Graph Info:")
        for line in info.split('\n')[:30]:
            if line.strip():
                log(f"  {line}")
    except Exception as e:
        log(f"ERROR getting graph info: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
