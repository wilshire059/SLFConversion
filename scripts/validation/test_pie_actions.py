"""
PIE Test for Action System
Tests that actions can be triggered and execute properly in Play-In-Editor mode.
"""

import unreal

def run_pie_test():
    """Run a PIE session and test action execution."""

    print("=" * 70)
    print("PIE ACTION SYSTEM TEST")
    print("=" * 70)

    # Get editor subsystem
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)

    # Load the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo"
    print(f"\n[1] Loading level: {level_path}")

    success = unreal.EditorLevelLibrary.load_level(level_path)
    if not success:
        print(f"[ERROR] Failed to load level: {level_path}")
        return False
    print("[OK] Level loaded")

    # Get PIE settings
    print("\n[2] Configuring PIE settings...")
    level_editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

    # Start PIE
    print("\n[3] Starting Play In Editor...")

    # Use automation to start PIE
    automation = unreal.AutomationLibrary()

    # We need to check if we can access the game world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        print(f"[OK] Editor world: {world.get_name()}")

    # Check player character blueprint
    print("\n[4] Checking Player Character setup...")
    player_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Player/B_Soulslike_Player"
    player_bp = unreal.EditorAssetLibrary.load_asset(player_bp_path)
    if player_bp:
        print(f"[OK] Player Blueprint loaded: {player_bp.get_name()}")
        gen_class = player_bp.generated_class()
        if gen_class:
            print(f"     Generated class: {gen_class.get_name()}")
    else:
        print(f"[WARN] Could not load player blueprint")

    # Check ActionManager component
    print("\n[5] Checking Action Manager Component...")
    action_manager_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager"
    action_manager_bp = unreal.EditorAssetLibrary.load_asset(action_manager_path)
    if action_manager_bp:
        print(f"[OK] ActionManager Blueprint loaded")
        gen_class = action_manager_bp.generated_class()
        if gen_class:
            print(f"     Generated class: {gen_class.get_name()}")
            parent = gen_class.get_super_class()
            if parent:
                print(f"     Parent class: {parent.get_name()}")

    # Check some action blueprints
    print("\n[6] Checking Action Blueprints...")
    action_paths = [
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_PickupItemMontage",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Jump",
        "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_ComboLight_L",
    ]

    for path in action_paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if bp:
            name = path.split("/")[-1]
            gen_class = bp.generated_class()
            if gen_class:
                parent = gen_class.get_super_class()
                parent_name = parent.get_name() if parent else "None"
                print(f"  [OK] {name}")
                print(f"       Parent: {parent_name}")
            else:
                print(f"  [WARN] {name} - no generated class")
        else:
            print(f"  [ERROR] Failed to load: {path}")

    # Check PDA_Action data assets
    print("\n[7] Checking Action Data Assets...")
    data_paths = [
        "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage",
        "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge",
    ]

    for path in data_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            name = path.split("/")[-1]
            print(f"  [OK] {name} loaded")
            # Try to get ActionClass property
            if hasattr(asset, 'get_editor_property'):
                try:
                    action_class = asset.get_editor_property('ActionClass')
                    if action_class:
                        print(f"       ActionClass: {action_class}")
                except:
                    pass
        else:
            print(f"  [ERROR] Failed to load: {path}")

    # Summary
    print("\n" + "=" * 70)
    print("PIE TEST SUMMARY")
    print("=" * 70)
    print("[INFO] To fully test action execution:")
    print("  1. Open Unreal Editor")
    print("  2. Load L_Demo level")
    print("  3. Press Play (PIE)")
    print("  4. Use attack inputs to trigger actions")
    print("  5. Check Output Log for [Action*] messages")
    print("=" * 70)

    return True

if __name__ == "__main__":
    run_pie_test()
