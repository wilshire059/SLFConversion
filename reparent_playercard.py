import unreal

# Force save after reparent (reparent_blueprint returns false for widgets but CDO type changes correctly)
paths = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_QuitConfirmation",
]

for bp_path in paths:
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning("SAVE: Could not load: {}".format(bp_path))
        continue

    name = bp_path.split("/")[-1]

    # Check current CDO type
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        unreal.log_warning("SAVE: {} CDO type: {}".format(name, type(cdo).__name__))

    # Force save
    saved = unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning("SAVE: {} saved: {}".format(name, saved))
