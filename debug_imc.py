# debug_imc.py
# Debug IMC_Gameplay structure to understand how to add Tab key

import unreal

def debug():
    results = []
    results.append("=" * 60)
    results.append("IMC_Gameplay Debug")
    results.append("=" * 60)

    imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")
    ia_game_menu = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu")

    if imc:
        results.append("\nIMC class: " + str(type(imc)))

        # List all properties
        results.append("\nAvailable properties:")
        for prop in dir(imc):
            if not prop.startswith('_') and not callable(getattr(imc, prop, None)):
                try:
                    val = imc.get_editor_property(prop)
                    results.append(f"  {prop}: {type(val).__name__}")
                except:
                    pass

        # Try mappings (deprecated)
        results.append("\n--- Using deprecated 'mappings' API ---")
        try:
            mappings = list(imc.get_editor_property('mappings'))
            results.append(f"Total mappings: {len(mappings)}")
            for i, m in enumerate(mappings[:10]):
                action = m.get_editor_property('action')
                key = m.get_editor_property('key')
                action_name = action.get_name() if action else "None"
                results.append(f"  [{i}] {action_name}: {key}")
        except Exception as e:
            results.append(f"Error: {e}")

        # Try default_key_mappings (new API)
        results.append("\n--- Using 'default_key_mappings' API ---")
        try:
            key_mappings = imc.get_editor_property('default_key_mappings')
            if key_mappings:
                mappings_list = list(key_mappings)
                results.append(f"Total mappings: {len(mappings_list)}")
                for i, m in enumerate(mappings_list[:10]):
                    action = m.get_editor_property('action')
                    key = m.get_editor_property('key')
                    action_name = action.get_name() if action else "None"
                    results.append(f"  [{i}] {action_name}: {key}")
            else:
                results.append("default_key_mappings is None/empty")
        except Exception as e:
            results.append(f"Error: {e}")

        # Find IA_GameMenu specifically
        results.append("\n--- Finding IA_GameMenu ---")
        if ia_game_menu:
            results.append(f"IA_GameMenu found: {ia_game_menu.get_name()}")
            results.append(f"IA_GameMenu path: {ia_game_menu.get_path_name()}")
        else:
            results.append("IA_GameMenu NOT FOUND")

    else:
        results.append("Could not load IMC_Gameplay")

    # Write results
    output = "\n".join(results)
    with open("C:/scripts/SLFConversion/debug_imc_result.txt", "w") as f:
        f.write(output)

debug()
