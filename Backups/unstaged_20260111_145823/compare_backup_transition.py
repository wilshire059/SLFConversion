# compare_backup_transition.py
# Copy backup AnimBP temporarily and compare transitions

import unreal
import shutil
import os

def log(msg):
    unreal.log_warning(f"[COMPARE] {msg}")

def main():
    log("=" * 70)
    log("COMPARING BACKUP VS CURRENT ANIMBP TRANSITIONS")
    log("=" * 70)

    # Paths
    backup_path = "C:/scripts/bp_only/Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive.uasset"
    temp_path = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive_BACKUP_COMPARE.uasset"

    current_asset_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    backup_asset_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive_BACKUP_COMPARE"

    # Copy backup file
    log(f"Copying backup from: {backup_path}")
    if os.path.exists(backup_path):
        shutil.copy2(backup_path, temp_path)
        log(f"Copied to: {temp_path}")
    else:
        log(f"[ERROR] Backup file not found!")
        return

    # Load both assets
    log("")
    log("Loading assets...")

    current_bp = unreal.load_asset(current_asset_path)
    backup_bp = unreal.load_asset(backup_asset_path)

    if not current_bp:
        log("[ERROR] Could not load current AnimBP")
        return
    if not backup_bp:
        log("[ERROR] Could not load backup AnimBP")
        return

    log(f"Current: {current_bp.get_name()}")
    log(f"Backup: {backup_bp.get_name()}")

    # Compare transitions
    if hasattr(unreal, 'SLFAutomationLibrary'):
        log("")
        log("=" * 70)
        log("BACKUP TRANSITIONS (ORIGINAL):")
        log("=" * 70)
        backup_result = unreal.SLFAutomationLibrary.inspect_transition_graph(backup_bp, "")
        for line in backup_result.split('\n'):
            log(line)

        log("")
        log("=" * 70)
        log("CURRENT TRANSITIONS:")
        log("=" * 70)
        current_result = unreal.SLFAutomationLibrary.inspect_transition_graph(current_bp, "")
        for line in current_result.split('\n'):
            log(line)

    # Clean up temp file
    log("")
    log("Cleaning up temp file...")
    try:
        os.remove(temp_path)
        log("Temp file removed")
    except:
        log("Could not remove temp file (may need manual cleanup)")

    log("")
    log("=" * 70)
    log("COMPARISON COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
