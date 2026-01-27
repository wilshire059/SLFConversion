import unreal

bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

bp = unreal.load_asset(bp_path)
if bp:
    # Check parent blueprint  
    try:
        parent_class = bp.get_editor_property('parent_class')
        print(f"Parent class property: {parent_class}")
    except:
        print("Could not get parent_class")
    
    # Check skeleton
    try:
        skel = bp.get_editor_property('target_skeleton')
        print(f"Target skeleton: {skel}")
    except:
        print("Could not get target_skeleton")
        
    # Check generated class
    gen_class = bp.generated_class()
    print(f"Generated class: {gen_class}")
    
    # Check CDO
    cdo = unreal.get_default_object(gen_class)
    print(f"CDO type: {type(cdo)}")
