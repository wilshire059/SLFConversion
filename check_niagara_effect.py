import unreal

def check_resting_point_niagara():
    """Check the B_RestingPoint Niagara components"""
    
    output = []
    
    output.append("=== Starting B_RestingPoint check ===")
    
    # Load the Blueprint asset
    asset_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    output.append(f"Loading asset: {asset_path}")
    
    bp = unreal.EditorAssetLibrary.load_asset(asset_path)
    
    if not bp:
        output.append(f"ERROR: Could not load asset")
        return output
    
    output.append(f"Loaded asset: {bp.get_name()}")
    output.append(f"Asset type: {type(bp).__name__}")
    
    # Get generated class
    bp_class = None
    try:
        bp_class = bp.generated_class()
        output.append(f"Generated class: {bp_class.get_name() if bp_class else 'None'}")
    except Exception as e:
        output.append(f"Error getting generated class: {e}")
        return output
    
    if not bp_class:
        output.append("ERROR: No generated class")
        return output
    
    # Get parent class info (using class_default_object pattern)
    try:
        parent_class = bp.parent_class
        output.append(f"  Parent Class: {parent_class.get_name() if parent_class else 'None'}")
    except Exception as e:
        output.append(f"  Could not get parent class: {e}")
    
    # Also try to spawn and check a live instance
    output.append("")
    output.append("--- Spawning test instance ---")
    
    try:
        # Spawn the actor
        spawn_loc = unreal.Vector(0, 0, 5000)  # High up to not interfere
        spawn_rot = unreal.Rotator(0, 0, 0)
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_class, spawn_loc, spawn_rot)
        
        if actor:
            output.append(f"Spawned: {actor.get_name()}")
            
            # Get all components
            all_comps = actor.get_components_by_class(unreal.ActorComponent)
            output.append(f"")
            output.append(f"All Components ({len(all_comps)}):")
            
            niagara_comps = []
            mesh_comps = []
            
            for comp in all_comps:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                output.append(f"  - {comp_name} ({comp_class})")
                
                # Check Niagara components specifically
                if "Niagara" in comp_class:
                    niagara_comps.append(comp)
                
                # Check mesh components
                if "StaticMesh" in comp_class:
                    mesh_comps.append(comp)
            
            # Detailed Niagara info
            output.append("")
            output.append(f"Niagara Components Detail ({len(niagara_comps)}):")
            for nc in niagara_comps:
                output.append(f"  Component: {nc.get_name()}")
                try:
                    asset = nc.get_asset()
                    output.append(f"    NiagaraSystem: {asset.get_name() if asset else 'NONE'}")
                    if asset:
                        output.append(f"    Asset Path: {asset.get_path_name()}")
                except Exception as e:
                    output.append(f"    Error getting asset: {e}")
                try:
                    auto_activate = nc.get_editor_property('auto_activate')
                    output.append(f"    AutoActivate: {auto_activate}")
                except Exception as e:
                    output.append(f"    Error getting auto_activate: {e}")
                try:
                    is_active = nc.is_active()
                    output.append(f"    IsActive: {is_active}")
                except Exception as e:
                    output.append(f"    Error getting is_active: {e}")
            
            # Detailed StaticMesh info
            output.append("")
            output.append(f"StaticMesh Components Detail ({len(mesh_comps)}):")
            for mc in mesh_comps:
                output.append(f"  Component: {mc.get_name()}")
                try:
                    mesh = mc.get_static_mesh()
                    output.append(f"    Mesh: {mesh.get_name() if mesh else 'NONE'}")
                    if mesh:
                        output.append(f"    Mesh Path: {mesh.get_path_name()}")
                except Exception as e:
                    output.append(f"    Error getting mesh: {e}")
            
            # Destroy the test actor
            actor.destroy_actor()
            output.append("")
            output.append("Test actor destroyed")
        else:
            output.append("ERROR: Failed to spawn actor")
    except Exception as e:
        output.append(f"ERROR spawning: {e}")
        import traceback
        output.append(traceback.format_exc())
    
    return output

if __name__ == "__main__":
    results = check_resting_point_niagara()
    
    # Write to file
    with open("C:/scripts/SLFConversion/migration_cache/niagara_check.txt", "w") as f:
        f.write("\n".join(results))
    
    # Also log
    for line in results:
        unreal.log(line)
