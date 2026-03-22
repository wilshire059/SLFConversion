"""
Export complete door state from bp_only project.
"""
import unreal
import json
import os

def export_door_complete():
    """Export ALL door data for comparison."""
    
    output_dir = "C:/scripts/SLFConversion/migration_cache/door_comparison"
    os.makedirs(output_dir, exist_ok=True)
    
    result = {"B_Door": {}, "B_BossDoor": {}, "level_actors": []}
    
    # Export B_Door
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
    bp = unreal.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            result["B_Door"]["class_name"] = gen_class.get_name()
            result["B_Door"]["class_path"] = gen_class.get_path_name()
            
            # Get parent class hierarchy
            hierarchy = []
            current = gen_class
            for i in range(10):
                if current:
                    hierarchy.append({
                        "name": current.get_name(),
                        "path": current.get_path_name()
                    })
                    # Try to get super class
                    try:
                        if hasattr(current, 'get_super_class'):
                            current = current.get_super_class()
                        else:
                            break
                    except:
                        break
                else:
                    break
            result["B_Door"]["hierarchy"] = hierarchy
            
            # Spawn to get components
            try:
                actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                    gen_class,
                    unreal.Vector(0, 0, 10000),
                    unreal.Rotator(0, 0, 0)
                )
                if actor:
                    components = []
                    for comp in actor.get_components_by_class(unreal.ActorComponent):
                        if comp:
                            comp_data = {
                                "name": comp.get_name(),
                                "class": comp.get_class().get_name() if comp.get_class() else "Unknown"
                            }
                            if isinstance(comp, unreal.SceneComponent):
                                comp_data["location"] = {
                                    "x": comp.relative_location.x,
                                    "y": comp.relative_location.y,
                                    "z": comp.relative_location.z
                                }
                                comp_data["scale"] = {
                                    "x": comp.relative_scale3d.x,
                                    "y": comp.relative_scale3d.y,
                                    "z": comp.relative_scale3d.z
                                }
                            components.append(comp_data)
                    result["B_Door"]["components"] = components
                    actor.destroy_actor()
            except Exception as e:
                result["B_Door"]["spawn_error"] = str(e)
    
    # Export B_BossDoor
    bp_path2 = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    bp2 = unreal.load_asset(bp_path2)
    if bp2:
        gen_class2 = bp2.generated_class()
        if gen_class2:
            result["B_BossDoor"]["class_name"] = gen_class2.get_name()
            result["B_BossDoor"]["class_path"] = gen_class2.get_path_name()
            
            # Get parent class hierarchy
            hierarchy2 = []
            current2 = gen_class2
            for i in range(10):
                if current2:
                    hierarchy2.append({
                        "name": current2.get_name(),
                        "path": current2.get_path_name()
                    })
                    try:
                        if hasattr(current2, 'get_super_class'):
                            current2 = current2.get_super_class()
                        else:
                            break
                    except:
                        break
                else:
                    break
            result["B_BossDoor"]["hierarchy"] = hierarchy2
            
            # Spawn to get components
            try:
                actor2 = unreal.EditorLevelLibrary.spawn_actor_from_class(
                    gen_class2,
                    unreal.Vector(0, 0, 10000),
                    unreal.Rotator(0, 0, 0)
                )
                if actor2:
                    components2 = []
                    for comp in actor2.get_components_by_class(unreal.ActorComponent):
                        if comp:
                            comp_data = {
                                "name": comp.get_name(),
                                "class": comp.get_class().get_name() if comp.get_class() else "Unknown"
                            }
                            if isinstance(comp, unreal.SceneComponent):
                                comp_data["location"] = {
                                    "x": comp.relative_location.x,
                                    "y": comp.relative_location.y,
                                    "z": comp.relative_location.z
                                }
                                comp_data["scale"] = {
                                    "x": comp.relative_scale3d.x,
                                    "y": comp.relative_scale3d.y,
                                    "z": comp.relative_scale3d.z
                                }
                            components2.append(comp_data)
                    result["B_BossDoor"]["components"] = components2
                    actor2.destroy_actor()
            except Exception as e:
                result["B_BossDoor"]["spawn_error"] = str(e)
    
    # Save
    output_file = os.path.join(output_dir, "door_state_bp_only.json")
    with open(output_file, 'w') as f:
        json.dump(result, f, indent=2, default=str)
    
    unreal.log_warning(f"Saved to: {output_file}")
    return result

if __name__ == "__main__":
    export_door_complete()
