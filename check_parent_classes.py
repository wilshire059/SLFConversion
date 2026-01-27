# check_parent_classes.py
# Check what parent classes the Blueprints have using SLFAutomationLibrary

import unreal

BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
]

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("PARENT CLASS CHECK")
    unreal.log_warning("=" * 70)

    for bp_path in BLUEPRINTS_TO_CHECK:
        bp_name = bp_path.split('/')[-1]
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_error(f"{bp_name}: Could not load")
            continue

        # Get generated class
        gen_class = bp.generated_class()
        if not gen_class:
            unreal.log_error(f"{bp_name}: No generated class")
            continue

        # Export the Blueprint to see parent class
        export_text = unreal.SLFAutomationLibrary.export_blueprint_to_text(bp)

        # Find parent class in export
        parent_line = ""
        for line in export_text.split('\n')[:50]:
            if 'ParentClass=' in line or 'Parent=' in line:
                parent_line = line.strip()
                break

        unreal.log_warning(f"\n{bp_name}:")
        unreal.log_warning(f"  Generated: {gen_class.get_name()}")
        unreal.log_warning(f"  Parent info: {parent_line[:200] if parent_line else 'NOT FOUND'}")

        # Get CDO and check components
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            components = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning(f"  CDO components: {len(components)}")
            for c in components:
                unreal.log_warning(f"    - {c.get_name()} ({c.get_class().get_name()})")

            # Check root
            try:
                root = cdo.get_editor_property('root_component')
                unreal.log_warning(f"  Root: {root.get_name() if root else 'None'}")
            except:
                unreal.log_warning(f"  Root: Error getting")

if __name__ == "__main__":
    main()
