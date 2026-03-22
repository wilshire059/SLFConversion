# check_copy9_container.py
# Check what components B_Container in Copy (9) has

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING COPY 9 B_CONTAINER")
    unreal.log_warning("=" * 70)

    # Try loading from bp_only backup
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Check the generated class
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"CDO Type: {type(cdo).__name__}")

            # Get all actor components
            try:
                components = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"\nComponents: {len(components) if components else 0}")
                if components:
                    for comp in components:
                        comp_name = comp.get_name()
                        comp_type = type(comp).__name__
                        unreal.log_warning(f"  - {comp_name} ({comp_type})")

                        # If it's a mesh component, try to get the mesh
                        if isinstance(comp, unreal.StaticMeshComponent):
                            try:
                                mesh = comp.get_editor_property("static_mesh")
                                if mesh:
                                    unreal.log_warning(f"      Mesh: {mesh.get_name()}")
                            except:
                                pass
            except Exception as e:
                unreal.log_error(f"Components error: {e}")

    # Export the Blueprint to see its raw structure
    try:
        export = bp.export_text()
        # Look for component lines
        unreal.log_warning("\n--- EXPORT COMPONENT LINES ---")
        for line in export.split('\n'):
            if 'Begin Object' in line and ('Component' in line or 'Lid' in line or 'Light' in line or 'Spawn' in line):
                unreal.log_warning(f"  {line.strip()}")
    except Exception as e:
        unreal.log_warning(f"Export error: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
