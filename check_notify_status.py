import unreal

# Check notify Blueprint status WITHOUT modifying anything
notify_paths = [
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_RegisterAttackSequence",
]

for path in notify_paths:
    bp = unreal.load_asset(path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            # Walk up the class hierarchy
            current = gen_class
            hierarchy = []
            while current:
                hierarchy.append(current.get_name())
                # Get parent - use static_class approach
                current = current.get_class_path_name()
                break

            # Get parent class via text export
            export_text = unreal.EditorAssetLibrary.get_path_name_for_loaded_asset(bp)
            print(f"\n{path.split('/')[-1]}:")
            print(f"  Asset path: {export_text}")
            print(f"  Generated class: {gen_class.get_name()}")

            # Check if it has the QueuedSection property
            try:
                cdo = unreal.get_default_object(gen_class)
                if hasattr(cdo, 'queued_section'):
                    print(f"  QueuedSection property: EXISTS")
                else:
                    print(f"  QueuedSection property: NOT FOUND")
            except Exception as e:
                print(f"  CDO check error: {e}")
    else:
        print(f"{path.split('/')[-1]}: NOT FOUND")
