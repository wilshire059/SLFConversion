# fix_montage_notifies.py
# Fix ANS_RegisterAttackSequence notify instances on all combo montages
# Uses C++ SLFAutomationLibrary functions

import unreal

def log(msg):
    unreal.log_warning(f"[FIX_MONTAGE] {msg}")

def main():
    log("=" * 70)
    log("FIXING MONTAGE ANS_RegisterAttackSequence NOTIFIES")
    log("=" * 70)

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Call the C++ function to fix all montages
    log("Calling SLFAutomationLibrary.fix_all_montage_combo_notifies()")
    total_fixed = unreal.SLFAutomationLibrary.fix_all_montage_combo_notifies()

    log("")
    log("=" * 70)
    log(f"COMPLETE: Fixed {total_fixed} notify instances")
    log("=" * 70)

if __name__ == "__main__":
    main()
