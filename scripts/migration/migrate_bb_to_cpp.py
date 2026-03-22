"""
Migrate BB_Standard State key from E_AI_States to ESLFAIStates
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_MIGRATE] {msg}")

def main():
    log("=" * 70)
    log("MIGRATING BB_STANDARD STATE KEY TO C++ ENUM")
    log("=" * 70)
    
    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    result = unreal.SLFAutomationLibrary.migrate_blackboard_enum_to_cpp(bb_path, "State")
    
    for line in result.split('\n'):
        log(line)
    
    log("=" * 70)
    log("MIGRATION COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
