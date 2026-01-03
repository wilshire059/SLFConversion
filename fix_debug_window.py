import unreal

bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow")
if bp:
    unreal.BlueprintFixerLibrary.reconstruct_all_nodes(bp)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow")
    print("W_DebugWindow fixed and saved")
else:
    print("Could not load W_DebugWindow")
