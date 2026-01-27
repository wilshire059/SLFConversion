# fix_broken_transitions.py
# Fix all broken transition variable bindings in the AnimBP
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[FIX] {msg}")

def main():
    log("=" * 70)
    log("FIXING BROKEN ANIMBP TRANSITIONS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Run the auto-fix function
    log("")
    log("Running FixAllBrokenTransitions...")
    fixed_count = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
    log(f"Fixed {fixed_count} broken connections")

    if fixed_count > 0:
        # Save the AnimBP
        log("")
        log("Saving AnimBP...")
        unreal.EditorAssetLibrary.save_asset(animbp_path)
        log("Saved!")

        # Run diagnosis again to verify
        log("")
        log("=== VERIFYING TRANSITIONS AFTER FIX ===")
        result = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")
        for line in result.split('\n'):
            log(line)
    else:
        log("No transitions needed fixing (or fix function couldn't match variables)")

    log("")
    log("=== FIX COMPLETE ===")

if __name__ == "__main__":
    main()
