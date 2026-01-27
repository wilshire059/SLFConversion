"""Run BT structure export"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_EXPORT] {msg}")

def main():
    log("=" * 70)
    log("EXPORTING BEHAVIOR TREE STRUCTURES")
    log("=" * 70)

    result = unreal.SLFAutomationLibrary.export_all_bt_subtrees()

    # The function already saves to file and logs, but let's confirm
    log("")
    log("Export complete! Check migration_cache/bt_structure_export.txt")

if __name__ == "__main__":
    main()
