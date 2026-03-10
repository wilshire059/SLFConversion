import unreal

paths = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_QuitConfirmation",
]

for bp_path in paths:
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning("CHECKPARENT: Could not load: {}".format(bp_path))
        continue

    gen_class = bp.generated_class()
    if gen_class:
        name = gen_class.get_name()
        unreal.log_warning("CHECKPARENT: {} generated_class name: {}".format(bp_path.split("/")[-1], name))

        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                level = cdo.get_editor_property("player_level")
                unreal.log_warning("CHECKPARENT: {} CDO player_level: {}".format(bp_path.split("/")[-1], level))
            except Exception as e:
                unreal.log_warning("CHECKPARENT: {} CDO no player_level: {}".format(bp_path.split("/")[-1], str(e)))

            unreal.log_warning("CHECKPARENT: {} CDO python type: {}".format(bp_path.split("/")[-1], type(cdo).__name__))
    else:
        unreal.log_warning("CHECKPARENT: {}: no generated_class".format(bp_path.split("/")[-1]))
