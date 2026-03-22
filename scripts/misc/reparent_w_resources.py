import unreal

# Reparent W_Resources to C++ class
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
new_parent = "/Script/SLFConversion.W_Resources"

bp = unreal.load_asset(bp_path)
if bp:
    print(f"Loaded: {bp.get_name()}")

    # Load C++ parent class
    parent_class = unreal.load_class(None, new_parent)
    if parent_class:
        print(f"Found C++ parent: {parent_class.get_name()}")

        # Reparent using BlueprintEditorLibrary
        success = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, parent_class)
        print(f"Reparent result: {success}")

        if success:
            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"Saved {bp_path}")
    else:
        print(f"Could not find parent class: {new_parent}")
else:
    print(f"Could not load: {bp_path}")
