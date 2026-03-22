"""Check Niagara component details in B_RestingPoint SCS"""
import unreal

# First, find the NS_CampfireDiscovered asset
ns_path = "/Game/SoulslikeFramework"
all_assets = unreal.EditorAssetLibrary.list_assets(ns_path, recursive=True)
campfire_assets = [a for a in all_assets if "Campfire" in a or "campfire" in a]
unreal.log_warning(f"=== Campfire-related assets ({len(campfire_assets)}) ===")
for asset in campfire_assets[:10]:
    unreal.log_warning(f"  {asset}")

# Check B_RestingPoint Blueprint's SCS node templates
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    # Use C++ helper to get detailed SCS info
    unreal.log_warning("=== B_RestingPoint SCS Niagara Details ===")
    
    # Get generated class and spawn temp actor to check real component state
    gen_class = bp.generated_class()
    if gen_class:
        # Spawn in editor world
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            spawn_loc = unreal.Vector(0, 0, -10000)  # Far away
            spawn_rot = unreal.Rotator(0, 0, 0)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, spawn_rot)
            
            if actor:
                unreal.log_warning(f"Spawned test actor: {actor.get_name()}")
                
                # Get all components
                all_comps = actor.get_components_by_class(unreal.NiagaraComponent)
                unreal.log_warning(f"Niagara components on spawned actor: {len(all_comps)}")
                
                for comp in all_comps:
                    name = comp.get_name()
                    unreal.log_warning(f"  Component: {name}")
                    
                    try:
                        asset = comp.get_editor_property("asset")
                        unreal.log_warning(f"    Asset: {asset.get_path_name() if asset else 'NONE'}")
                    except Exception as e:
                        unreal.log_warning(f"    Asset error: {e}")
                    
                    try:
                        auto_act = comp.get_editor_property("auto_activate")
                        unreal.log_warning(f"    AutoActivate: {auto_act}")
                    except:
                        pass
                    
                    try:
                        visible = comp.is_visible()
                        unreal.log_warning(f"    Visible: {visible}")
                    except:
                        pass
                    
                    try:
                        active = comp.is_active()
                        unreal.log_warning(f"    Active: {active}")
                    except:
                        pass
                
                # Delete test actor
                actor.destroy_actor()
                unreal.log_warning("Test actor destroyed")
