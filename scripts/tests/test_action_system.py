# test_action_system.py
# Test that the ActionManager correctly loads actions via name derivation

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/action_system_test_output.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("ACTION SYSTEM TEST")
    results.append("=" * 60)

    try:
        # Load the player character blueprint
        player_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
        player_bp = unreal.EditorAssetLibrary.load_asset(player_path)

        if not player_bp:
            results.append(f"FAILED: Could not load player BP at {player_path}")
        else:
            results.append(f"Player Blueprint loaded: OK")
            gen_class = player_bp.generated_class()
            results.append(f"Player class: {gen_class.get_name()}")

            # Get the CDO
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                results.append("Player CDO: OK")

                # Try to find the ActionManager component
                try:
                    action_mgr = cdo.get_editor_property("ActionManager")
                    if action_mgr:
                        results.append(f"ActionManager component: {action_mgr.get_class().get_name()}")

                        # Check if it has the Actions map property
                        try:
                            actions_map = action_mgr.get_editor_property("Actions")
                            if actions_map:
                                results.append(f"Actions map: {len(actions_map)} entries")
                            else:
                                results.append("Actions map: Empty or None")
                        except Exception as e:
                            results.append(f"Actions map error: {e}")
                    else:
                        results.append("ActionManager: None")
                except Exception as e:
                    results.append(f"ActionManager error: {e}")

        # Check action data assets
        results.append("")
        results.append("--- Action Data Assets ---")

        action_assets = [
            "DA_Action_Jump",
            "DA_Action_Dodge",
            "DA_Action_StartSprinting",
            "DA_Action_StopSprinting",
            "DA_Action_Crouch",
        ]

        for asset_name in action_assets:
            asset_path = f"/Game/SoulslikeFramework/Data/Actions/ActionData/{asset_name}"
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            if asset:
                results.append(f"  {asset_name}: Loaded OK")
            else:
                results.append(f"  {asset_name}: FAILED TO LOAD")

        # Check action logic blueprints
        results.append("")
        results.append("--- Action Logic Blueprints ---")

        action_logics = [
            "B_Action_Jump",
            "B_Action_Dodge",
            "B_Action_StartSprinting",
            "B_Action_StopSprinting",
            "B_Action_Crouch",
            "B_Action_ThrowProjectile",  # Note: maps from DA_Action_Projectile
        ]

        for bp_name in action_logics:
            bp_path = f"/Game/SoulslikeFramework/Data/Actions/ActionLogic/{bp_name}"
            bp = unreal.EditorAssetLibrary.load_asset(bp_path)
            if bp:
                gen_class = bp.generated_class()
                results.append(f"  {bp_name}: {gen_class.get_name() if gen_class else 'No generated class'}")
            else:
                results.append(f"  {bp_name}: FAILED TO LOAD")

        # Check ActionManager C++ class
        results.append("")
        results.append("--- C++ Classes ---")

        try:
            # Check if UAC_ActionManager exists
            am_class = unreal.find_class("AC_ActionManager")
            if am_class:
                results.append(f"UAC_ActionManager: Found")
            else:
                results.append(f"UAC_ActionManager: NOT FOUND (may just be naming)")
        except:
            results.append("UAC_ActionManager: Could not query")

        try:
            # Check if UB_Action exists
            ba_class = unreal.find_class("B_Action")
            if ba_class:
                results.append(f"UB_Action: Found")
            else:
                results.append(f"UB_Action: NOT FOUND (may just be naming)")
        except:
            results.append("UB_Action: Could not query")

        results.append("")
        results.append("=" * 60)
        results.append("TEST COMPLETE")
        results.append("The action system will work if:")
        results.append("1. All Action Data Assets load successfully")
        results.append("2. All Action Logic Blueprints load successfully")
        results.append("3. Name derivation maps DA_Action_X -> B_Action_X correctly")
        results.append("=" * 60)

    except Exception as e:
        results.append(f"ERROR: {e}")
        import traceback
        results.append(traceback.format_exc())

    # Write results
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))

    # Also print to console
    for line in results:
        print(line)

run()
