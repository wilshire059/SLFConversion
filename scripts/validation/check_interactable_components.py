# check_interactable_components.py
# Check B_Interactable's components

import unreal

def check_blueprint(name, path):
    unreal.log_warning(f"\n--- {name} ---")
    bp = unreal.load_asset(path)
    if not bp:
        unreal.log_error(f"Could not load: {path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log_warning(f"CDO Type: {type(cdo).__name__}")

        # Spawn instance
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, -5000))
            if actor:
                components = actor.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"Components: {len(components) if components else 0}")
                if components:
                    for comp in components:
                        unreal.log_warning(f"  - {comp.get_name()} ({type(comp).__name__})")
                actor.destroy_actor()

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING INTERACTABLE HIERARCHY COMPONENTS")
    unreal.log_warning("=" * 70)

    blueprints = [
        ("B_Interactable", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"),
        ("B_Container", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"),
        ("B_RestingPoint", "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"),
    ]

    for name, path in blueprints:
        check_blueprint(name, path)

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
