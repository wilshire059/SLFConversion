# check_backup_animbp.py
# Check if the backup AnimBP has the same IsGuarding? error

import unreal

def log(msg):
    unreal.log_warning(f"[CheckBackup] {msg}")

def main():
    log("=" * 60)
    log("Checking Backup AnimBP Status")
    log("=" * 60)

    # Load from main project (which should have backup restored)
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"AnimBP Parent: {parent}")

    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log(f"Compile Status:")
    for line in errors.split('\n'):
        if line.strip():
            log(f"  {line}")

    # Check if there's IsGuarding? anywhere
    if "IsGuarding?" in errors:
        log("")
        log("[ISSUE] AnimBP still has 'IsGuarding?' in Property Access path")
    elif "IsGuarding" in errors:
        log("")
        log("[ISSUE] AnimBP has 'IsGuarding' issue (path still broken)")
    else:
        log("")
        log("[OK] No IsGuarding issues found")

    log("=" * 60)

if __name__ == "__main__":
    main()
