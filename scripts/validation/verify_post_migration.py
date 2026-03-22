# verify_post_migration.py
# Check if POST-MIGRATION steps ran successfully (UE5.7 compatible)

import unreal

def verify():
    results = []
    results.append("=" * 60)
    results.append("POST-MIGRATION VERIFICATION")
    results.append("=" * 60)

    # Check dodge montages
    results.append("\n1. Checking Dodge Montages...")
    dodge_asset = unreal.EditorAssetLibrary.load_asset(
        "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
    )

    if dodge_asset:
        try:
            dodge_montages = dodge_asset.get_editor_property('DodgeMontages')
            forward = getattr(dodge_montages, 'forward', None)
            results.append("   Forward montage: " + (str(forward.get_name()) if forward else "NOT SET"))

            backward = getattr(dodge_montages, 'backward', None)
            results.append("   Backward montage: " + (str(backward.get_name()) if backward else "NOT SET"))

            backstep = getattr(dodge_montages, 'backstep', None)
            results.append("   Backstep montage: " + (str(backstep.get_name()) if backstep else "NOT SET"))

            if forward and backward and backstep:
                results.append("   [OK] Dodge montages are set!")
            else:
                results.append("   [FAIL] Dodge montages are NOT set")
        except Exception as e:
            results.append("   [ERROR] " + str(e))
    else:
        results.append("   [FAIL] Could not load DA_Action_Dodge")

    # Check Tab key binding using UE5.7 API
    results.append("\n2. Checking Tab Key Binding...")
    imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")
    ia_game_menu = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu")

    if imc and ia_game_menu:
        try:
            # UE5.7 API: use default_key_mappings.mappings
            key_mappings_data = imc.get_editor_property('default_key_mappings')
            mappings = list(key_mappings_data.get_editor_property('mappings'))

            tab_found = False
            game_menu_keys = []

            for m in mappings:
                action = m.get_editor_property('action')
                if action and action.get_path_name() == ia_game_menu.get_path_name():
                    key = m.get_editor_property('key')
                    key_name = str(key.get_editor_property('key_name'))
                    game_menu_keys.append(key_name)
                    if "Tab" in key_name:
                        tab_found = True

            results.append("   IA_GameMenu keys: " + str(game_menu_keys))
            if tab_found:
                results.append("   [OK] Tab key is bound to IA_GameMenu!")
            else:
                results.append("   [FAIL] Tab key NOT found for IA_GameMenu")
        except Exception as e:
            results.append("   [ERROR] " + str(e))
    else:
        results.append("   [FAIL] Could not load IMC_Gameplay or IA_GameMenu")

    results.append("\n" + "=" * 60)
    results.append("VERIFICATION COMPLETE")
    results.append("=" * 60)

    # Write results to file
    output = "\n".join(results)
    with open("C:/scripts/SLFConversion/verify_result.txt", "w") as f:
        f.write(output)

    # Also log to Unreal
    unreal.log(output)

verify()
