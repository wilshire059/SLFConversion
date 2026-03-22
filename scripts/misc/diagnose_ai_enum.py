"""
Diagnose E_AI_States enum and BB_Standard State key
Uses SLFAutomationLibrary C++ functions
"""
import unreal

def log(msg):
    unreal.log_warning(f"[ENUM_DIAG] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING E_AI_STATES ENUM AND BLACKBOARD STATE KEY")
    log("=" * 70)

    # Export Blueprint enum values
    log("\n--- EXPORTING BLUEPRINT ENUM ---")
    enum_path = "/Game/SoulslikeFramework/Enums/E_AI_States"
    result1 = unreal.SLFAutomationLibrary.export_blueprint_enum_values(enum_path)
    for line in result1.split('\n'):
        log(line)

    # Diagnose blackboard State key
    log("\n--- DIAGNOSING BLACKBOARD STATE KEY ---")
    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    result2 = unreal.SLFAutomationLibrary.diagnose_blackboard_state_key(bb_path)
    for line in result2.split('\n'):
        log(line)

    log("=" * 70)
    log("DIAGNOSIS COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
