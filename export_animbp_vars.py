import unreal

print("=== Starting AnimBP Variable Check ===")
unreal.log("Starting AnimBP Variable Check")

bp = unreal.load_asset("/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive")
if bp:
    unreal.log(f"Loaded BP: {bp.get_name()}")

    gen_class = unreal.load_class(None, "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive.ABP_SoulslikeCharacter_Additive_C")
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        unreal.log(f"CDO: {cdo}")

        for prop_name in ['RightHandOverlayState', 'LeftHandOverlayState', 'RightHandOverlayState_0', 'LeftHandOverlayState_0']:
            try:
                val = cdo.get_editor_property(prop_name)
                unreal.log(f"  {prop_name} = {val}")
            except Exception as e:
                unreal.log(f"  {prop_name} = NOT FOUND")
else:
    unreal.log("Failed to load AnimBP")
    
unreal.log("=== AnimBP Variable Check Complete ===")
