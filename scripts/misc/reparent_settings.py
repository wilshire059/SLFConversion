import unreal

# Assets to reparent
assets = [
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings", "/Script/SLFConversion.W_Settings"),
    ("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", "/Script/SLFConversion.W_Settings_Entry"),
]

for bp_path, cpp_class_path in assets:
    unreal.log(f"Processing {bp_path}")
    bp = unreal.load_asset(bp_path)
    if bp:
        cpp_class = unreal.load_class(None, cpp_class_path)
        if cpp_class:
            unreal.log(f"  Found C++ class: {cpp_class_path}")
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            unreal.log(f"  Reparent result: {result}")
            if result:
                unreal.EditorAssetLibrary.save_asset(bp_path)
                unreal.log(f"  Saved {bp_path}")
        else:
            unreal.log_error(f"  ERROR: Could not load C++ class {cpp_class_path}")
    else:
        unreal.log_error(f"ERROR: Could not load {bp_path}")

unreal.log("Reparenting complete")
