# show_blend_connections.py
# Show what the BlendListByEnum ActiveEnumValue pins are connected to

import unreal

def log(msg):
    print(f"[ShowConn] {msg}")
    unreal.log_warning(f"[ShowConn] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 70)
    log("SHOWING BLEND NODE CONNECTIONS")
    log("=" * 70)

    animbp = unreal.load_asset(ANIMBP_PATH)
    if not animbp:
        log("ERROR: Could not load AnimBP")
        return

    # Use export function to get detailed state
    try:
        state = unreal.SLFAutomationLibrary.export_anim_graph_state(animbp, "")
        # Filter for BlendListByEnum info
        for line in state.split('\n'):
            if 'BlendListByEnum' in line or 'ActiveEnumValue' in line or 'OverlayState' in line:
                log(line)
    except Exception as e:
        log(f"export_anim_graph_state error: {e}")

if __name__ == "__main__":
    main()
