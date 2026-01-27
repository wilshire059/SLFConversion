"""
Migrate BB_Standard State key to use C++ ESLFAIStates enum
Uses SLFAutomationLibrary
"""
import unreal

def log(msg):
    unreal.log_warning(f"[MIGRATE_BB] {msg}")

def main():
    log("=" * 70)
    log("MIGRATING BB_STANDARD STATE KEY TO C++ ENUM")
    log("=" * 70)

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"

    # Use SLFAutomationLibrary to migrate
    result = unreal.SLFAutomationLibrary.migrate_blackboard_enum_to_cpp(bb_path, "State")
    log(f"Result: {result}")

    log("=" * 70)

if __name__ == "__main__":
    main()
