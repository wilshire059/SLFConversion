# Add logging to the POST-MIGRATION: Dodge Montages section
with open("C:/scripts/SLFConversion/run_migration.py", "r", encoding="utf-8") as f:
    content = f.read()

old_code = '''    dodge_asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
    dodge_asset = unreal.EditorAssetLibrary.load_asset(dodge_asset_path)

    if dodge_asset:
        try:
            dodge_montages_struct = dodge_asset.get_editor_property('DodgeMontages')
            montages_set = 0

            for prop_name, montage_path in DODGE_MONTAGES.items():
                montage = unreal.load_object(None, montage_path)
                if montage:
                    python_prop = PYTHON_PROP_NAMES[prop_name]
                    setattr(dodge_montages_struct, python_prop, montage)
                    montages_set += 1

            dodge_asset.set_editor_property('DodgeMontages', dodge_montages_struct)
            unreal.EditorAssetLibrary.save_asset(dodge_asset_path, only_if_is_dirty=False)
            print("Dodge montages: " + str(montages_set) + "/9 set successfully")
        except Exception as e:
            print("Dodge montages ERROR: " + str(e))
    else:
        print("Dodge montages: Could not load DA_Action_Dodge")'''

new_code = '''    dodge_asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
    unreal.log_warning("POST-MIGRATION: Loading DA_Action_Dodge...")
    dodge_asset = unreal.EditorAssetLibrary.load_asset(dodge_asset_path)

    if dodge_asset:
        unreal.log_warning("POST-MIGRATION: DA_Action_Dodge loaded, class=" + str(dodge_asset.get_class().get_name()))
        try:
            dodge_montages_struct = dodge_asset.get_editor_property('DodgeMontages')
            unreal.log_warning("POST-MIGRATION: Got DodgeMontages struct")
            montages_set = 0

            for prop_name, montage_path in DODGE_MONTAGES.items():
                montage = unreal.load_object(None, montage_path)
                if montage:
                    python_prop = PYTHON_PROP_NAMES[prop_name]
                    setattr(dodge_montages_struct, python_prop, montage)
                    montages_set += 1
                    unreal.log_warning("POST-MIGRATION: Set " + python_prop + " = " + str(montage.get_name()))

            dodge_asset.set_editor_property('DodgeMontages', dodge_montages_struct)
            unreal.log_warning("POST-MIGRATION: set_editor_property called")
            save_result = unreal.EditorAssetLibrary.save_asset(dodge_asset_path, only_if_is_dirty=False)
            unreal.log_warning("POST-MIGRATION: save_asset result = " + str(save_result))
            print("Dodge montages: " + str(montages_set) + "/9 set successfully")
        except Exception as e:
            unreal.log_warning("POST-MIGRATION: Dodge montages ERROR: " + str(e))
            print("Dodge montages ERROR: " + str(e))
    else:
        unreal.log_warning("POST-MIGRATION: Could not load DA_Action_Dodge!")
        print("Dodge montages: Could not load DA_Action_Dodge")'''

if old_code in content:
    content = content.replace(old_code, new_code)
    with open("C:/scripts/SLFConversion/run_migration.py", "w", encoding="utf-8") as f:
        f.write(content)
    print("Added logging to POST-MIGRATION dodge section")
else:
    print("Could not find the exact code block to replace")
    # Try to find what's there
    if "POST-MIGRATION: Loading" in content:
        print("Logging already added")
    else:
        idx = content.find("dodge_asset_path")
        if idx >= 0:
            print("Found dodge_asset_path at", idx)
            print("Context:", repr(content[idx:idx+200]))
