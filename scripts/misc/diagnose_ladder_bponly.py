"""
Diagnostic for bp_only B_Ladder ISM components
"""
import unreal

output_file = "C:/scripts/slfconversion/ladder_diagnostic_bponly.txt"

def log(msg):
    """Log to file"""
    with open(output_file, "a") as f:
        f.write(msg + "\n")
    unreal.log(msg)

def diagnose_ladder_visibility():
    """Check ISM components on bp_only B_Ladder"""
    # Clear the output file
    with open(output_file, "w") as f:
        f.write("")

    log("="*80)
    log("BP_ONLY B_LADDER DIAGNOSTIC")
    log("="*80)

    b_ladder_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"

    bp = unreal.load_asset(b_ladder_path)
    if not bp:
        log("ERROR: Could not load B_Ladder")
        return

    log(f"Loaded: {b_ladder_path}")

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("ERROR: No CDO")
        return

    log(f"CDO: {cdo.get_name()}")
    log(f"CDO Class: {cdo.get_class().get_name()}")

    # Get parent class from Blueprint
    try:
        parent_class = bp.get_editor_property('parent_class')
        log(f"Blueprint ParentClass: {parent_class.get_name() if parent_class else 'None'}")
    except Exception as e:
        log(f"Error getting parent class: {e}")

    # Get SCS components from blueprint
    log("")
    log("-"*60)
    log("BLUEPRINT SCS COMPONENTS:")
    log("-"*60)

    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            log(f"SCS: {scs.get_name()}")
            nodes = scs.get_editor_property('all_nodes')
            log(f"SCS Nodes: {len(nodes)}")
            for node in nodes:
                template = node.get_editor_property('component_template')
                if template:
                    log(f"  - {template.get_name()} ({template.get_class().get_name()})")
    except Exception as e:
        log(f"Error getting SCS: {e}")

    # Get all components from CDO
    log("")
    log("-"*60)
    log("CDO COMPONENTS (via get_components_by_class):")
    log("-"*60)

    ism_components = cdo.get_components_by_class(unreal.InstancedStaticMeshComponent)
    log(f"ISM Components: {len(ism_components)}")
    for ism in ism_components:
        log(f"  - {ism.get_name()}")
        mesh = ism.get_editor_property('static_mesh')
        log(f"      Mesh: {mesh.get_name() if mesh else 'NULL'}")

    # Get all primitive components
    all_prims = cdo.get_components_by_class(unreal.PrimitiveComponent)
    log(f"\nAll Primitive Components: {len(all_prims)}")
    for prim in all_prims:
        log(f"  - {prim.get_name()} ({prim.get_class().get_name()})")

    # Get ALL components
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"\nAll Actor Components: {len(all_comps)}")
    for comp in all_comps:
        log(f"  - {comp.get_name()} ({comp.get_class().get_name()})")

    log("")
    log("="*80)
    log("DIAGNOSTIC COMPLETE")
    log("="*80)

if __name__ == "__main__":
    diagnose_ladder_visibility()
