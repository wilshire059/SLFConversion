"""Compare Niagara settings between bp_only and SLFConversion B_RestingPoint"""
import unreal

# Check bp_only version
bp_only_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"

# We can't load from bp_only project, but let's check the current SLFConversion one
# and also check if there's a StaticMesh assigned for the campfire

bp = unreal.EditorAssetLibrary.load_asset(bp_only_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        # Spawn actor
        world = unreal.UnrealEditorSubsystem().get_editor_world()
        if world:
            spawn_loc = unreal.Vector(0, 0, -10000)
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_loc, unreal.Rotator())
            
            if actor:
                # Check Niagara
                niagara_comps = actor.get_components_by_class(unreal.NiagaraComponent)
                unreal.log_warning(f"=== SLFConversion B_RestingPoint Niagara ===")
                for comp in niagara_comps:
                    name = comp.get_name()
                    asset = comp.get_editor_property("asset")
                    auto_act = comp.get_editor_property("auto_activate")
                    unreal.log_warning(f"{name}: AutoActivate={auto_act}, Asset={asset.get_name() if asset else 'None'}")
                
                # Check StaticMesh
                static_mesh_comps = actor.get_components_by_class(unreal.StaticMeshComponent)
                unreal.log_warning(f"=== StaticMeshComponents ({len(static_mesh_comps)}) ===")
                for comp in static_mesh_comps:
                    name = comp.get_name()
                    mesh = comp.get_editor_property("static_mesh")
                    unreal.log_warning(f"{name}: Mesh={mesh.get_name() if mesh else 'None'}")
                
                # Check all scene components for hierarchy
                unreal.log_warning(f"=== Component Hierarchy ===")
                all_comps = actor.get_components_by_class(unreal.SceneComponent)
                for comp in all_comps:
                    parent = comp.get_attach_parent()
                    parent_name = parent.get_name() if parent else "None"
                    unreal.log_warning(f"  {comp.get_name()} -> parent: {parent_name}")
                
                actor.destroy_actor()
