import unreal

bp_path = '/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework'
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    unreal.log(f"=== Blueprint: {bp.get_name()} ===")

    gen_class = bp.generated_class()
    if gen_class:
        unreal.log(f"Generated class: {gen_class.get_name()}")

        # Traverse parent chain
        current = gen_class
        parents = []
        while current:
            parents.append(current.get_name())
            parent = current.static_class().get_outer()
            if hasattr(current, 'get_super_struct'):
                current = current.get_super_struct()
            else:
                break
            if len(parents) > 10:
                break

        unreal.log(f"Parent chain: {' -> '.join(parents)}")

        # Check if it's a Blueprint or C++ class
        cdo = unreal.get_default_object(gen_class)
        cdo_class = cdo.get_class()
        unreal.log(f"CDO class: {cdo_class.get_name()}")

        # Check class flags to determine if it's Blueprint
        unreal.log(f"CDO type: {type(cdo)}")

else:
    unreal.log_error("Blueprint not found")
