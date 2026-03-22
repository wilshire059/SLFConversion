import unreal

# Load the C++ class directly and check its CDO
cpp_path = "/Script/SLFConversion.SLFSoulslikeCharacter"
try:
    cpp_class = unreal.load_class(None, cpp_path)
    if cpp_class:
        unreal.log_warning("=== C++ SLFSoulslikeCharacter ===")
        unreal.log_warning("Class: " + cpp_class.get_name())
        
        cdo = unreal.get_default_object(cpp_class)
        if cdo:
            unreal.log_warning("CDO: " + cdo.get_name())
            
            # Get all components
            comps = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning("Components: " + str(len(comps)))
            for c in comps:
                unreal.log_warning("  - " + c.get_name() + " (" + c.get_class().get_name() + ")")
            
            # Check main mesh
            if hasattr(cdo, 'mesh'):
                mesh = cdo.mesh
                if mesh:
                    try:
                        skel_mesh = mesh.get_skinned_asset() if hasattr(mesh, 'get_skinned_asset') else None
                        unreal.log_warning("CharacterMesh0 SkeletalMesh: " + (skel_mesh.get_name() if skel_mesh else "NONE"))
                    except:
                        unreal.log_warning("CharacterMesh0: Error getting mesh")
    else:
        unreal.log_warning("Failed to load C++ class")
except Exception as e:
    unreal.log_warning("Error: " + str(e))
