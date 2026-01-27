"""
Diagnose the decorator's blackboard key selector configuration
Check if the key selector properly matches the blackboard key type
"""
import unreal

def log(msg):
    unreal.log_warning(f"[DEC_KEY] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING DECORATOR BLACKBOARD KEY CONFIGURATION")
    log("=" * 70)

    # Use C++ function to get detailed decorator info
    result = unreal.SLFAutomationLibrary.diagnose_decorator_key_selectors(
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    )

    if result:
        for line in result.split('\n'):
            log(line)
    else:
        log("[ERROR] No result from diagnose_decorator_key_selectors")

    # Also check BT_Idle
    log("")
    log("=" * 40)
    log("BT_Idle:")
    result2 = unreal.SLFAutomationLibrary.diagnose_decorator_key_selectors(
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle"
    )
    if result2:
        for line in result2.split('\n'):
            log(line)

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
