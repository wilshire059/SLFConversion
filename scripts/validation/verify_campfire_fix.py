"""Verify B_RestingPoint now has the campfire mesh"""
import unreal

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    gen_class = bp.generated_class()
    if gen_class:
        # Spawn actor
        world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
        spawn_loc = unreal.Vector(0, 0, -10000)
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, unreal.Rotator())
        
        if actor:
            unreal.log_warning("=== B_RestingPoint After Fix ===")
            
            # Check StaticMesh components
            static_mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
            unreal.log_warning(f"StaticMeshComponents: {len(static_mesh_comps)}")
            for comp in static_mesh_comps:
                name = comp.get_name()
                mesh = comp.get_editor_property("static_mesh")
                unreal.log_warning(f"  {name}: {mesh.get_name() if mesh else 'None'}")
            
            # Check Niagara components
            niagara_comps = actor.get_components_by_class(unreal.NiagaraComponent)
            unreal.log_warning(f"NiagaraComponents: {len(niagara_comps)}")
            for comp in niagara_comps:
                name = comp.get_name()
                asset = comp.get_editor_property("asset")
                auto_act = comp.get_editor_property("auto_activate")
                unreal.log_warning(f"  {name}: {asset.get_name() if asset else 'None'}, AutoActivate={auto_act}")
            
            # Clean up
            actor.destroy_actor()
            unreal.log_warning("Test complete!")
