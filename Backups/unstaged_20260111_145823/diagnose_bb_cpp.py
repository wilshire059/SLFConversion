"""
Diagnose blackboard keys using C++ function
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_DIAG] {msg}")

def main():
    log("=" * 70)
    log("BLACKBOARD DIAGNOSTIC VIA C++")
    log("=" * 70)

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"

    # Use C++ function to diagnose
    result = unreal.SLFAutomationLibrary.diagnose_blackboard_keys(bb_path)

    log("")
    log("Result:")
    for line in result.split('\n'):
        log(line)

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
