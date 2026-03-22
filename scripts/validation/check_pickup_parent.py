import unreal

# Check parent class of B_Action_PickupItemMontage
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_PickupItemMontage"

bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    unreal.log(f"Blueprint Name: {bp.get_name()}")
    unreal.log(f"Blueprint Class: {bp.get_class().get_name()}")

    # Get parent class through blueprint property
    try:
        parent = bp.get_editor_property('parent_class')
        unreal.log(f"Parent Class: {parent.get_name() if parent else 'None'}")
    except:
        pass

    # Generated class info
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log(f"Generated Class: {gen_class.get_name()}")
        unreal.log(f"Generated Class Path: {gen_class.get_path_name()}")

        # Use static_class() equivalent approach
        try:
            # Get outer/parent info
            outer = gen_class.get_outer()
            unreal.log(f"Outer: {outer.get_name() if outer else 'None'}")
        except:
            pass
else:
    unreal.log_error(f"Could not load: {bp_path}")
