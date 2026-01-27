# check_copy9.py
# Check what's in the working Copy 9 backup

import unreal

# Paths in Copy 9 (mounted at /Game_Copy9 or we'll check directly)
# We need to check the actual files - let's use export_text to see structure

BLUEPRINTS_TO_CHECK = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
]

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING CURRENT B_RestingPoint")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error("Could not load B_RestingPoint")
        return

    gen_class = bp.generated_class()
    if gen_class:
        # Get class hierarchy
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        unreal.log_warning(f"Path: {gen_class.get_path_name()}")

        # Export to text to see structure
        exporter = unreal.Exporter()
        export_text = unreal.EditorAssetLibrary.get_metadata_tag(bp_path, "Parent")
        unreal.log_warning(f"Metadata Parent tag: {export_text}")

        # Try to get parent via reflection
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            cdo_class = cdo.get_class()
            unreal.log_warning(f"\nCDO Info:")
            unreal.log_warning(f"  CDO Class: {cdo_class.get_name()}")
            unreal.log_warning(f"  CDO Path: {cdo_class.get_path_name()}")

            # Check if it implements interfaces
            # Try casting to see what we have
            unreal.log_warning(f"\n  Actor type check:")
            unreal.log_warning(f"    Is Actor: {isinstance(cdo, unreal.Actor)}")

            components = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning(f"\n  Components ({len(components)}):")
            for c in components:
                unreal.log_warning(f"    - {c.get_name()} ({c.get_class().get_name()})")

    # Also check the SCS from Blueprint
    unreal.log_warning(f"\nBlueprint SCS check:")
    try:
        # Access SCS via the Blueprint asset's attribute
        scs_nodes = unreal.SLFAutomationLibrary.get_blueprint_scs_nodes(bp)
        unreal.log_warning(f"  SCS node count: {len(scs_nodes) if scs_nodes else 0}")
    except Exception as e:
        unreal.log_warning(f"  SCS access error: {e}")

if __name__ == "__main__":
    main()
