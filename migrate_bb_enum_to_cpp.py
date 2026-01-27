"""
Migrate BB_Standard State key from Blueprint enum E_AI_States to C++ enum ESLFAIStates
Uses SLFAutomationLibrary.MigrateBlackboardEnumToCpp
"""
import unreal

def main():
    print("=" * 70)
    print("MIGRATING BB_STANDARD STATE KEY TO C++ ENUM")
    print("=" * 70)

    # Run the C++ migration function
    result = unreal.SLFAutomationLibrary.migrate_blackboard_enum_to_cpp(
        "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard",
        "State"
    )

    print(result)

    print("=" * 70)
    print("MIGRATION COMPLETE")
    print("=" * 70)

if __name__ == "__main__":
    main()
