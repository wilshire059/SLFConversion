import unreal

def check_resting_point():
    """Check the bp_only B_RestingPoint components"""
    
    output = []
    
    output.append("=== bp_only B_RestingPoint check ===")
    
    # Load the Blueprint asset
    asset_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    output.append(f"Loading asset: {asset_path}")
    
    bp = unreal.EditorAssetLibrary.load_asset(asset_path)
    
    if not bp:
        output.append(f"ERROR: Could not load asset")
        return output
    
    output.append(f"Loaded asset: {bp.get_name()}")
    
    # Get generated class
    bp_class = bp.generated_class()
    
    # Spawn
    spawn_loc = unreal.Vector(0, 0, 5000)
    spawn_rot = unreal.Rotator(0, 0, 0)
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_class, spawn_loc, spawn_rot)
    
    if actor:
        output.append(f"Spawned: {actor.get_name()}")
        
        # Get all components
        all_comps = actor.get_components_by_class(unreal.ActorComponent)
        output.append(f"")
        output.append(f"All Components ({len(all_comps)}):")
        
        for comp in all_comps:
            comp_name = comp.get_name()
            comp_class = comp.get_class().get_name()
            output.append(f"  - {comp_name} ({comp_class})")
            
            # Check StaticMesh components specifically
            if "StaticMesh" in comp_class:
                try:
                    mesh = comp.get_static_mesh()
                    output.append(f"      Mesh: {mesh.get_name() if mesh else 'NONE'}")
                except:
                    pass
            
            # Check Niagara components
            if "Niagara" in comp_class:
                try:
                    asset = comp.get_asset()
                    output.append(f"      NiagaraSystem: {asset.get_name() if asset else 'NONE'}")
                except:
                    pass
        
        actor.destroy_actor()
    
    return output

if __name__ == "__main__":
    results = check_resting_point()
    
    with open("C:/scripts/SLFConversion/migration_cache/bp_only_niagara_check.txt", "w") as f:
        f.write("\n".join(results))
    
    for line in results:
        unreal.log(line)
