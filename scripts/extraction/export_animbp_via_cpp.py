# export_animbp_via_cpp.py
# Use C++ SLFAutomationLibrary to export AnimBP state

import unreal
import json
import os

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_detailed_current.json"

def main():
    log("=" * 70)
    log("EXPORTING ANIMBP STATE VIA C++ AUTOMATION")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Use C++ function to get detailed diagnosis
    log("Getting DiagnoseAnimBP output...")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    log("Getting InspectTransitionGraph output for key transitions...")
    idle_transitions = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "IDLE")
    cycle_transitions = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "CYCLE")
    all_transitions = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "Transition")

    # Get state machine structure
    log("Getting Locomotion state machine structure...")
    locomotion_sm = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, "Locomotion")

    export_data = {
        "asset_path": ANIMBP_PATH,
        "diagnosis": diagnosis,
        "locomotion_state_machine": locomotion_sm,
        "idle_transitions": idle_transitions,
        "cycle_transitions": cycle_transitions,
        "all_transitions": all_transitions
    }

    # Save to file
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w', encoding='utf-8') as f:
        json.dump(export_data, f, indent=2)

    log(f"Saved to {OUTPUT_PATH}")
    log(f"Diagnosis length: {len(diagnosis)} chars")
    log(f"All transitions length: {len(all_transitions)} chars")
    log("[SUCCESS] Export complete")

if __name__ == "__main__":
    main()
