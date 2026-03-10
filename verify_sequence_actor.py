import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    print(f"Blueprint: {bp.get_name()}")
    print(f"Parent class: {bp.parent_class.get_name() if bp.parent_class else 'None'}")
    print(f"Parent path: {bp.parent_class.get_path_name() if bp.parent_class else 'None'}")

    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                seq = cdo.get_editor_property("sequence_to_play")
                print(f"sequence_to_play: {seq.get_name() if seq else 'None'}")
                print(f"sequence_to_play path: {seq.get_path_name() if seq else 'None'}")
            except Exception as e:
                print(f"Error getting sequence_to_play: {e}")
else:
    print(f"ERROR: Could not load: {bp_path}")
