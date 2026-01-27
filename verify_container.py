# verify_container.py
# Verify B_Container CDO values after migration

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"

def verify_container():
    """Verify B_Container CDO values"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("VERIFYING B_CONTAINER CDO VALUES")
    unreal.log_warning("=" * 60)

    # Load the Blueprint
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return

    # Get generated class and CDO
    gen_class = bp_asset.generated_class()
    cdo = unreal.get_default_object(gen_class)

    unreal.log_warning(f"Blueprint: {bp_asset.get_name()}")
    unreal.log_warning(f"CDO Type: {type(cdo).__name__}")

    # Verify all properties
    unreal.log_warning("\n--- VARIABLE PROPERTIES ---")

    props = [
        ("OpenMontage", "TSoftObjectPtr<UAnimMontage>"),
        ("OpenVFX", "TSoftObjectPtr<UNiagaraSystem>"),
        ("MoveDistance", "double"),
        ("SpeedMultiplier", "double"),
        ("CachedIntensity", "float"),
        ("InteractableDisplayName", "FText"),
        ("InteractionText", "FText"),
        ("CanBeTraced", "bool"),
        ("IsActivated", "bool"),
    ]

    for prop_name, prop_type in props:
        try:
            val = cdo.get_editor_property(prop_name)
            unreal.log_warning(f"  {prop_name} ({prop_type}) = {val}")
        except Exception as e:
            unreal.log_warning(f"  {prop_name} ERROR: {str(e)[:50]}")

    unreal.log_warning("\n--- COMPONENT PROPERTIES ---")

    components = ["Lid", "ItemSpawn", "PointLight", "AC_LootDropManager", "MoveToLocation", "NiagaraLocation"]
    for comp_name in components:
        try:
            comp = cdo.get_editor_property(comp_name)
            if comp:
                unreal.log_warning(f"  {comp_name}: {type(comp).__name__}")
            else:
                unreal.log_warning(f"  {comp_name}: None (created at runtime)")
        except Exception as e:
            unreal.log_warning(f"  {comp_name} ERROR: {str(e)[:50]}")

    # Spawn actor and verify runtime state
    unreal.log_warning("\n--- SPAWNED ACTOR TEST ---")
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, -10000))
        if actor:
            unreal.log_warning(f"Spawned: {actor.get_name()}")

            # Check components on spawned actor
            try:
                lid = actor.get_editor_property("Lid")
                unreal.log_warning(f"  Lid component: {type(lid).__name__ if lid else 'None'}")
                if lid:
                    mesh = lid.get_editor_property("static_mesh")
                    unreal.log_warning(f"  Lid mesh: {mesh.get_name() if mesh else 'None'}")
            except Exception as e:
                unreal.log_warning(f"  Lid ERROR: {e}")

            try:
                point_light = actor.get_editor_property("PointLight")
                unreal.log_warning(f"  PointLight component: {type(point_light).__name__ if point_light else 'None'}")
            except Exception as e:
                unreal.log_warning(f"  PointLight ERROR: {e}")

            actor.destroy_actor()

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("VERIFICATION COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    verify_container()
