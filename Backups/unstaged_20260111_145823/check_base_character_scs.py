# Check B_BaseCharacter's SCS components
# Run with: UnrealEditor-Cmd.exe ... -run=pythonscript -script="check_base_character_scs.py"

import unreal

def check_scs():
    # Load B_BaseCharacter
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
    bp = unreal.load_asset(bp_path)

    if not bp:
        print("ERROR: Could not load B_BaseCharacter from " + bp_path)
        return

    print("=" * 60)
    print("B_BaseCharacter Blueprint Analysis")
    print("=" * 60)

    # Check parent class - Blueprint has parent_class attribute
    if hasattr(bp, 'parent_class') and bp.parent_class:
        print("Parent class: " + str(bp.parent_class.get_name()))
    else:
        print("Parent class: Unknown (no parent_class attribute)")

    # Check SCS
    print("\n--- SCS Components (GetBlueprintSCSComponents) ---")
    scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    if scs_comps:
        for comp in scs_comps:
            print("  " + comp)
    else:
        print("  (No SCS components found)")

    # Check variables
    print("\n--- Blueprint Variables ---")
    vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    if vars:
        for v in vars:
            print("  " + v)
    else:
        print("  (No variables found)")

    # Also check B_Soulslike_Character
    player_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    player_bp = unreal.load_asset(player_bp_path)

    if player_bp:
        print("\n" + "=" * 60)
        print("B_Soulslike_Character Blueprint Analysis")
        print("=" * 60)

        if hasattr(player_bp, 'parent_class') and player_bp.parent_class:
            print("Parent class: " + str(player_bp.parent_class.get_name()))
        else:
            print("Parent class: Unknown")

        print("\n--- SCS Components ---")
        player_scs = unreal.SLFAutomationLibrary.get_blueprint_scs_components(player_bp)
        if player_scs:
            for comp in player_scs:
                print("  " + comp)
        else:
            print("  (No SCS components found)")

    # Check AC_StatManager
    stat_bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    stat_bp = unreal.load_asset(stat_bp_path)

    if stat_bp:
        print("\n" + "=" * 60)
        print("AC_StatManager Blueprint Analysis")
        print("=" * 60)
        if hasattr(stat_bp, 'parent_class') and stat_bp.parent_class:
            print("Parent class: " + str(stat_bp.parent_class.get_name()))
        else:
            print("Parent class: Unknown")
    else:
        print("\nERROR: Could not load AC_StatManager")

    print("\n" + "=" * 60)
    print("Analysis Complete")
    print("=" * 60)

check_scs()
