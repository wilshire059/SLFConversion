import unreal

output = []
output.append("=" * 60)
output.append("DEBUG: Dodge Migration")
output.append("=" * 60)

DODGE_MONTAGES = {
    "Forward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F",
    "ForwardLeft": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FL.AM_SLF_Dodge_FL",
    "ForwardRight": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FR.AM_SLF_Dodge_FR",
    "Left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_L.AM_SLF_Dodge_L",
    "Right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_R.AM_SLF_Dodge_R",
    "Backward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_B.AM_SLF_Dodge_B",
    "BackwardLeft": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BL.AM_SLF_Dodge_BL",
    "BackwardRight": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BR.AM_SLF_Dodge_BR",
    "Backstep": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Backstep.AM_SLF_Backstep",
}

PYTHON_PROP_NAMES = {
    "Forward": "forward",
    "ForwardLeft": "forward_left",
    "ForwardRight": "forward_right",
    "Left": "left",
    "Right": "right",
    "Backward": "backward",
    "BackwardLeft": "backward_left",
    "BackwardRight": "backward_right",
    "Backstep": "backstep",
}

# Step 1: Check if montage assets exist
output.append("")
output.append("Step 1: Checking montage assets...")
for name, path in DODGE_MONTAGES.items():
    exists = unreal.EditorAssetLibrary.does_asset_exist(path)
    output.append("  " + name + ": " + ("EXISTS" if exists else "MISSING"))

# Step 2: Try to load montages
output.append("")
output.append("Step 2: Loading montage assets...")
loaded_montages = {}
for name, path in DODGE_MONTAGES.items():
    try:
        montage = unreal.load_object(None, path)
        if montage:
            loaded_montages[name] = montage
            output.append("  " + name + ": LOADED - " + str(type(montage)))
        else:
            output.append("  " + name + ": load_object returned None")
    except Exception as e:
        output.append("  " + name + ": EXCEPTION - " + str(e))

# Step 3: Load DA_Action_Dodge
output.append("")
output.append("Step 3: Loading DA_Action_Dodge...")
dodge_asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
dodge_asset = unreal.EditorAssetLibrary.load_asset(dodge_asset_path)

if not dodge_asset:
    output.append("  FAILED to load DA_Action_Dodge!")
else:
    output.append("  Class: " + str(dodge_asset.get_class().get_name()))

    # Step 4: Get DodgeMontages struct
    output.append("")
    output.append("Step 4: Getting DodgeMontages struct...")
    try:
        dodge_struct = dodge_asset.get_editor_property('DodgeMontages')
        output.append("  Type: " + str(type(dodge_struct)))

        # Step 5: Set montages on the struct
        output.append("")
        output.append("Step 5: Setting montages on struct...")
        set_count = 0
        for name, montage in loaded_montages.items():
            python_prop = PYTHON_PROP_NAMES[name]
            try:
                setattr(dodge_struct, python_prop, montage)
                # Verify it was set
                verify = getattr(dodge_struct, python_prop)
                if verify:
                    output.append("  " + python_prop + ": SET and VERIFIED")
                    set_count += 1
                else:
                    output.append("  " + python_prop + ": SET but verify returned None")
            except Exception as e:
                output.append("  " + python_prop + ": EXCEPTION - " + str(e))

        # Step 6: Write struct back to asset
        output.append("")
        output.append("Step 6: Writing struct back to asset...")
        try:
            dodge_asset.set_editor_property('DodgeMontages', dodge_struct)
            output.append("  set_editor_property: SUCCESS")
        except Exception as e:
            output.append("  set_editor_property: EXCEPTION - " + str(e))

        # Step 7: Save the asset
        output.append("")
        output.append("Step 7: Saving asset...")
        try:
            result = unreal.EditorAssetLibrary.save_asset(dodge_asset_path, only_if_is_dirty=False)
            output.append("  save_asset: " + str(result))
        except Exception as e:
            output.append("  save_asset: EXCEPTION - " + str(e))

        # Step 8: Verify by re-loading
        output.append("")
        output.append("Step 8: Verifying by re-loading...")
        dodge_asset2 = unreal.EditorAssetLibrary.load_asset(dodge_asset_path)
        if dodge_asset2:
            dodge_struct2 = dodge_asset2.get_editor_property('DodgeMontages')
            for python_prop in PYTHON_PROP_NAMES.values():
                val = getattr(dodge_struct2, python_prop)
                if val:
                    output.append("  " + python_prop + ": " + str(val.get_name()))
                else:
                    output.append("  " + python_prop + ": NULL")

    except Exception as e:
        output.append("  EXCEPTION getting DodgeMontages: " + str(e))

output.append("")
output.append("=" * 60)
output.append("DEBUG COMPLETE")
output.append("=" * 60)

# Write to file
with open("C:/scripts/SLFConversion/debug_dodge_result.txt", "w") as f:
    f.write("\n".join(output))

print("Results written to debug_dodge_result.txt")
