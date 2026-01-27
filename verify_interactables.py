# verify_interactables.py
# Verify the state of B_Interactable and B_Container after migration

import unreal

BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
]


def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("VERIFYING INTERACTABLE BLUEPRINTS")
    unreal.log_warning("=" * 60)

    for bp_path in BLUEPRINTS:
        bp_name = bp_path.split('/')[-1]
        unreal.log_warning(f"\n=== {bp_name} ===")

        # Load Blueprint
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_error(f"  Could not load: {bp_path}")
            continue

        # Get generated class
        gen_class = bp.generated_class()
        if not gen_class:
            unreal.log_error(f"  No generated class")
            continue

        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check root component
            try:
                root = cdo.get_editor_property('root_component')
                if root:
                    root_name = root.get_name()
                    root_class = root.get_class().get_name()
                    unreal.log_warning(f"  Root component: {root_name} ({root_class})")
                else:
                    unreal.log_error(f"  Root component: None")
            except Exception as e:
                unreal.log_warning(f"  Root check error: {e}")

            # List all components
            try:
                all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"  Components ({len(all_comps)}):")
                for comp in all_comps:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()
                    unreal.log_warning(f"    - {comp_name} ({comp_class})")
            except Exception as e:
                unreal.log_warning(f"  Component list error: {e}")
        else:
            unreal.log_error(f"  No CDO")

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("VERIFICATION COMPLETE")
    unreal.log_warning("=" * 60)


if __name__ == "__main__":
    main()
