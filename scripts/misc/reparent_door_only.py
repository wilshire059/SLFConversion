"""
Reparent only B_Door to C++ AB_Door class
"""
import unreal

def reparent_door():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    new_parent_path = "/Script/SLFConversion.B_Door"  # AB_Door in C++

    print(f"Loading Blueprint: {bp_path}")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loading new parent class: {new_parent_path}")
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        print(f"ERROR: Could not load parent class {new_parent_path}")
        return False

    print(f"Reparenting {bp.get_name()} to {new_parent.get_name()}")

    # Reparent
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
    print(f"Reparent result: {result}")

    # Compile
    print("Compiling...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Save
    print("Saving...")
    save_result = unreal.EditorAssetLibrary.save_asset(bp_path)
    print(f"Save result: {save_result}")

    return result

reparent_door()
