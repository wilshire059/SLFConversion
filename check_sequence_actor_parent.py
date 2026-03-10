import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    print(f"Blueprint loaded: {bp_path}")
    print(f"Blueprint class: {bp.get_class().get_name()}")

    # Get parent class
    parent_class = bp.parent_class
    if parent_class:
        print(f"Parent class: {parent_class.get_name()}")
        print(f"Parent class path: {parent_class.get_path_name()}")
    else:
        print("No parent class found")
else:
    print(f"ERROR: Could not load: {bp_path}")
