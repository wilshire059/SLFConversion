import unreal

# Check W_Resources parent and properties
bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"

bp = unreal.load_asset(bp_path)
if bp:
    print(f"W_Resources loaded: {bp.get_name()}")

    # Get parent class directly from the Blueprint
    parent = unreal.EditorAssetLibrary.get_metadata_tag(bp_path, "ParentClass")
    print(f"ParentClass metadata: {parent}")

    # Try to export text to see parent
    text = unreal.EditorAssetLibrary.export_text(bp_path)
    if text:
        # Find ParentClass line
        for line in text.split('\n')[:50]:
            if 'ParentClass' in line or 'BlueprintParentClass' in line:
                print(f"Found: {line.strip()}")
else:
    print("Could not load W_Resources")
