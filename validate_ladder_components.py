"""
Validate ladder components are preserved after migration.
Spawns B_Ladder in the editor world and checks for all expected components.
"""
import unreal

def validate_ladder_components():
    unreal.log_warning("=" * 60)
    unreal.log_warning("LADDER COMPONENT VALIDATION")
    unreal.log_warning("=" * 60)

    # Load the B_Ladder Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"
    bp = unreal.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Failed to load B_Ladder Blueprint at {bp_path}")
        return False

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("Blueprint has no generated class!")
        return False

    unreal.log_warning(f"Generated Class: {gen_class.get_name()}")

    # Get editor world
    editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = editor_subsystem.get_editor_world()

    if not world:
        unreal.log_error("Could not get editor world!")
        return False

    # Spawn the ladder actor using EditorActorSubsystem
    spawn_location = unreal.Vector(0, 0, 500)  # Spawn above ground
    spawn_rotation = unreal.Rotator(0, 0, 0)

    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    spawned_actor = actor_subsystem.spawn_actor_from_class(gen_class, spawn_location, spawn_rotation)

    if not spawned_actor:
        unreal.log_error("Failed to spawn B_Ladder actor!")
        return False

    unreal.log_warning(f"Spawned actor: {spawned_actor.get_name()}")

    # Get all components
    all_components = spawned_actor.get_components_by_class(unreal.ActorComponent)

    unreal.log_warning("")
    unreal.log_warning("--- COMPONENTS FOUND ---")

    component_names = []
    for comp in all_components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        component_names.append(comp_name)
        unreal.log_warning(f"  {comp_name} ({comp_class})")

    # Expected components from SCS
    expected_components = [
        "Barz",
        "Pole",
        "Connections",
        "ClimbingCollision",
        "BottomCollision",
        "TopCollision",
        "TopdownCollision"
    ]

    unreal.log_warning("")
    unreal.log_warning("--- VALIDATION RESULTS ---")

    all_found = True
    for expected in expected_components:
        found = any(expected in name for name in component_names)
        status = "FOUND" if found else "MISSING"
        if not found:
            all_found = False
            unreal.log_error(f"  {expected}: {status}")
        else:
            unreal.log_warning(f"  {expected}: {status}")

    # Check for mesh components specifically
    unreal.log_warning("")
    unreal.log_warning("--- MESH COMPONENT CHECK ---")

    ism_components = spawned_actor.get_components_by_class(unreal.InstancedStaticMeshComponent)
    for ism in ism_components:
        mesh = ism.get_editor_property("static_mesh")
        mesh_name = mesh.get_name() if mesh else "None"
        unreal.log_warning(f"  {ism.get_name()}: Mesh = {mesh_name}")

        if mesh:
            unreal.log_warning(f"    Mesh is assigned - GOOD (should be visible)")
        else:
            unreal.log_warning(f"    Mesh is NULL - May be invisible!")

    # Check box collision components
    unreal.log_warning("")
    unreal.log_warning("--- COLLISION COMPONENT CHECK ---")

    box_components = spawned_actor.get_components_by_class(unreal.BoxComponent)
    for box in box_components:
        extent = box.get_editor_property("box_extent")
        unreal.log_warning(f"  {box.get_name()}: Extent = {extent}")

    # Clean up - destroy spawned actor
    spawned_actor.destroy_actor()
    unreal.log_warning("")
    unreal.log_warning("Spawned actor destroyed after validation")

    # Final result
    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    if all_found:
        unreal.log_warning("VALIDATION: ALL COMPONENTS FOUND - SUCCESS")
    else:
        unreal.log_error("VALIDATION: SOME COMPONENTS MISSING - FAILED")
    unreal.log_warning("=" * 60)

    return all_found

if __name__ == "__main__":
    validate_ladder_components()
