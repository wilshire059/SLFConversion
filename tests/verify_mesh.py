import unreal

# Check C++ SLFSoulslikeCharacter class
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
                class_name = c.get_class().get_name()
                comp_name = c.get_name()
                unreal.log_warning("  - " + comp_name + " (" + class_name + ")")

                # Check skeletal mesh component
                if class_name == "SkeletalMeshComponent":
                    try:
                        skel_mesh = c.get_skinned_asset() if hasattr(c, 'get_skinned_asset') else None
                        if skel_mesh:
                            unreal.log_warning("    -> SkeletalMesh: " + skel_mesh.get_name())
                            unreal.log_warning("    -> Path: " + skel_mesh.get_path_name())
                        else:
                            unreal.log_warning("    -> SkeletalMesh: NONE")
                    except Exception as e:
                        unreal.log_warning("    -> Error getting mesh: " + str(e))

            # Also check if there's a Mesh property directly
            if hasattr(cdo, 'mesh'):
                try:
                    mesh_comp = cdo.mesh
                    if mesh_comp:
                        skel_mesh = mesh_comp.get_skinned_asset() if hasattr(mesh_comp, 'get_skinned_asset') else None
                        if skel_mesh:
                            unreal.log_warning("CharacterMesh0 via mesh property: " + skel_mesh.get_name())
                        else:
                            unreal.log_warning("CharacterMesh0 via mesh property: NONE")
                except Exception as e:
                    unreal.log_warning("CharacterMesh0 error: " + str(e))
    else:
        unreal.log_warning("Failed to load C++ class")
except Exception as e:
    unreal.log_warning("Error: " + str(e))

# Check if the mesh asset exists
mesh_path = "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/SKM_Manny"
unreal.log_warning("")
unreal.log_warning("=== Checking Mesh Asset ===")
try:
    mesh = unreal.EditorAssetLibrary.load_asset(mesh_path)
    if mesh:
        unreal.log_warning("SKM_Manny found: " + mesh.get_name())
        unreal.log_warning("Path: " + mesh.get_path_name())
    else:
        unreal.log_warning("SKM_Manny NOT FOUND at: " + mesh_path)
except Exception as e:
    unreal.log_warning("Error loading mesh: " + str(e))
