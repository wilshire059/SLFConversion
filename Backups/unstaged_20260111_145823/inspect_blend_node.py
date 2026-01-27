# inspect_blend_node.py - Inspect BlendListByEnum node structure in detail
import unreal

def log(msg):
    unreal.log_warning(f"[INSPECT] {msg}")

def main():
    log("=" * 70)
    log("INSPECTING BLENDLISTBYENUM NODE PROPERTIES")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)

    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if hasattr(unreal, 'SLFAutomationLibrary'):
        # Use detailed node inspection
        result = unreal.SLFAutomationLibrary.inspect_blend_list_by_enum_nodes(bp)
        if result:
            for line in result.split('\n'):
                log(line)
        else:
            log("Function returned empty or doesn't exist")

if __name__ == "__main__":
    main()
