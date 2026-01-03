import unreal

# Check B_Soulslike_Character parent
bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    unreal.log_warning("=== B_Soulslike_Character ===")
    
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning("Generated class: " + gen_class.get_name())
        unreal.log_warning("Path: " + gen_class.get_path_name())
        
        # Try to get parent chain
        current = gen_class
        depth = 0
        while current and depth < 10:
            try:
                class_name = current.get_name()
                unreal.log_warning("  [" + str(depth) + "] " + class_name)
                # Try to get parent
                parent = current.get_outer()
                if parent == current:
                    break
                current = parent
                depth += 1
            except:
                break

# Also check the native C++ class directly
try:
    cpp_class = unreal.find_class("SLFSoulslikeCharacter")
    if cpp_class:
        unreal.log_warning("=== C++ Class SLFSoulslikeCharacter ===")
        cdo = unreal.get_default_object(cpp_class)
        if cdo:
            unreal.log_warning("CDO: " + cdo.get_name())
            comps = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning("Components: " + str(len(comps)))
            for c in comps:
                unreal.log_warning("  - " + c.get_name() + " (" + c.get_class().get_name() + ")")
except Exception as e:
    unreal.log_warning("Error: " + str(e))
