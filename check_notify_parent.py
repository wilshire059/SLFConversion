import unreal

notify_paths = [
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_RegisterAttackSequence",
]

for path in notify_paths:
    bp = unreal.load_asset(path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            # Get parent class name directly from generated class
            class_name = gen_class.get_name()
            # Use BlueprintEditorLibrary to get parent
            parent_bp = unreal.BlueprintEditorLibrary.get_blueprint_parent(bp)
            if parent_bp:
                print(f"{path.split('/')[-1]}: Parent Blueprint = {parent_bp.get_name()}")
            else:
                # Try to get native parent
                print(f"{path.split('/')[-1]}: Class = {class_name}, checking native parent...")
        else:
            print(f"{path.split('/')[-1]}: No generated class")
    else:
        print(f"{path.split('/')[-1]}: NOT FOUND")
