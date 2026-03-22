# add_tab_key.py
# Add Tab key binding for IA_GameMenu in UE5.7

import unreal

def add_tab_key():
    results = []
    results.append("=" * 60)
    results.append("Adding Tab Key for IA_GameMenu")
    results.append("=" * 60)

    imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")
    ia_game_menu = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu")

    if not imc or not ia_game_menu:
        results.append("ERROR: Could not load IMC_Gameplay or IA_GameMenu")
        with open("C:/scripts/SLFConversion/add_tab_result.txt", "w") as f:
            f.write("\n".join(results))
        return

    # Get the default_key_mappings struct
    key_mappings_data = imc.get_editor_property('default_key_mappings')
    mappings = key_mappings_data.get_editor_property('mappings')
    mappings_list = list(mappings)

    results.append(f"\nTotal mappings: {len(mappings_list)}")

    # Find IA_GameMenu entries and check their keys
    tab_found = False
    game_menu_indices = []

    for i, m in enumerate(mappings_list):
        action = m.get_editor_property('action')
        if action and action.get_path_name() == ia_game_menu.get_path_name():
            game_menu_indices.append(i)
            key = m.get_editor_property('key')
            key_name = key.get_editor_property('key_name')
            results.append(f"  Found IA_GameMenu at index {i}: key={key_name}")
            if "Tab" in str(key_name):
                tab_found = True

    if tab_found:
        results.append("\nTab key already exists for IA_GameMenu!")
    else:
        results.append("\nAdding Tab key binding...")

        # Create new mapping
        new_mapping = unreal.EnhancedActionKeyMapping()
        new_mapping.set_editor_property('action', ia_game_menu)

        # Create the Tab key using set_editor_property
        tab_key = unreal.Key()
        tab_key.set_editor_property('key_name', 'Tab')
        new_mapping.set_editor_property('key', tab_key)

        # Add to mappings
        mappings_list.append(new_mapping)

        # Set back to the struct
        key_mappings_data.set_editor_property('mappings', mappings_list)

        # Set struct back to IMC
        imc.set_editor_property('default_key_mappings', key_mappings_data)

        # Save the asset
        save_result = unreal.EditorAssetLibrary.save_asset(
            "/Game/SoulslikeFramework/Input/IMC_Gameplay",
            only_if_is_dirty=False
        )
        results.append(f"Save result: {save_result}")

        if save_result:
            results.append("\n[OK] Tab key added successfully!")
        else:
            results.append("\n[FAIL] Failed to save IMC_Gameplay")

    results.append("\n" + "=" * 60)

    # Write results
    with open("C:/scripts/SLFConversion/add_tab_result.txt", "w") as f:
        f.write("\n".join(results))

add_tab_key()
