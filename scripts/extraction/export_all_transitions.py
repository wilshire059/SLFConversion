# export_all_transitions.py
# Export ALL transition graphs from AnimBP without filtering

import unreal
import json
import os

def log(msg):
    print(f"[Export] {msg}")
    unreal.log_warning(f"[Export] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_all_transitions_current.txt"

def main():
    log("=" * 70)
    log("EXPORTING ALL TRANSITION GRAPHS")
    log("=" * 70)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Use empty string to get ALL transitions
    log("Getting ALL transitions (empty keyword)...")
    all_transitions = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")

    log(f"Output length: {len(all_transitions)} chars")

    # Save to file
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w', encoding='utf-8') as f:
        f.write(all_transitions)

    log(f"Saved to {OUTPUT_PATH}")

    # Also print a summary
    transition_count = all_transitions.count("=== TRANSITION #")
    log(f"Total transitions found: {transition_count}")

    log("[SUCCESS] Export complete")

if __name__ == "__main__":
    main()
