# export_animbp_complete.py
# Export complete AnimBP state using C++ function

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_cpp_export_current.txt"

def main():
    print("[Export] Starting complete AnimBP export via C++...")
    unreal.log_warning("[Export] Starting complete AnimBP export via C++...")

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        print("[ERROR] Could not load AnimBP")
        unreal.log_warning("[ERROR] Could not load AnimBP")
        return

    print(f"[Export] Loaded: {bp.get_name()}")
    unreal.log_warning(f"[Export] Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        print("[ERROR] SLFAutomationLibrary not available")
        unreal.log_warning("[ERROR] SLFAutomationLibrary not available")
        return

    # Call the C++ export function
    result = unreal.SLFAutomationLibrary.export_anim_graph_complete(bp, OUTPUT_PATH)

    print(f"[Export] Export complete. Result length: {len(result)} chars")
    unreal.log_warning(f"[Export] Export complete. Result length: {len(result)} chars")
    print(f"[Export] Saved to: {OUTPUT_PATH}")
    unreal.log_warning(f"[Export] Saved to: {OUTPUT_PATH}")

if __name__ == "__main__":
    main()
