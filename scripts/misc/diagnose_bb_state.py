# diagnose_bb_state.py
# Diagnose the BB_Standard State key configuration

import unreal

def main():
    print("=" * 70)
    print("DIAGNOSING BB_STANDARD STATE KEY CONFIGURATION")
    print("=" * 70)

    # Load the blackboard asset
    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        print(f"ERROR: Could not load {bb_path}")
        return

    print(f"\nBlackboard: {bb.get_name()}")
    print(f"Class: {bb.get_class().get_name()}")

    # Try to get keys
    try:
        # Use export_text to see raw data
        export = unreal.EditorAssetLibrary.export_text(bb_path)

        # Find the State key configuration
        print("\n--- Looking for State key in export ---")
        lines = export.split('\n')
        in_keys_section = False
        state_key_found = False

        for i, line in enumerate(lines):
            if 'EntryName="State"' in line or 'EntryName=State' in line:
                state_key_found = True
                print(f"\nFound State key at line {i}:")
                # Print surrounding context
                for j in range(max(0, i-2), min(len(lines), i+10)):
                    print(f"  {j}: {lines[j][:200]}")
                break

        if not state_key_found:
            # Search for State in raw export
            print("\nSearching for 'State' in raw export...")
            for i, line in enumerate(lines):
                if 'State' in line and ('Key' in line or 'Entry' in line or 'Enum' in line or 'Int' in line):
                    print(f"  Line {i}: {line[:150]}")

        # Also look for enum type references
        print("\n--- Looking for Enum type references ---")
        for i, line in enumerate(lines):
            if 'BlackboardKeyType_Enum' in line or 'BlackboardKeyType_Int' in line:
                print(f"  Line {i}: {line[:200]}")

        # Check for E_AI_States reference
        print("\n--- Looking for E_AI_States references ---")
        for i, line in enumerate(lines):
            if 'E_AI_States' in line or 'ESLFAIStates' in line:
                print(f"  Line {i}: {line[:200]}")

    except Exception as e:
        print(f"Error examining blackboard: {e}")

    # Now check the BT decorators
    print("\n" + "=" * 70)
    print("CHECKING BT_COMBAT DECORATOR CONFIGURATION")
    print("=" * 70)

    bt_combat_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    bt_combat = unreal.load_asset(bt_combat_path)

    if bt_combat:
        try:
            export = unreal.EditorAssetLibrary.export_text(bt_combat_path)
            lines = export.split('\n')

            print("\n--- Looking for State decorator in BT_Combat ---")
            for i, line in enumerate(lines):
                if 'State' in line or 'IntValue' in line or 'Decorator' in line:
                    if 'Combat' in line or 'IntValue' in line or 'BlackboardKey' in line:
                        print(f"  Line {i}: {line[:200]}")

        except Exception as e:
            print(f"Error: {e}")
    else:
        print(f"Could not load {bt_combat_path}")

    print("\n" + "=" * 70)
    print("DIAGNOSIS COMPLETE")
    print("=" * 70)

if __name__ == "__main__":
    main()
