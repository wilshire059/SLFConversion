"""
Use C++ DiagnoseBlackboardKeys function to check enum type configuration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_DIAG] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING BB_Standard BLACKBOARD KEYS (via C++)")
    log("=" * 70)

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"

    result = unreal.SLFAutomationLibrary.diagnose_blackboard_keys(bb_path)

    if result:
        log("")
        for line in result.split('\n'):
            log(line)
    else:
        log("[ERROR] DiagnoseBlackboardKeys returned empty result")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
