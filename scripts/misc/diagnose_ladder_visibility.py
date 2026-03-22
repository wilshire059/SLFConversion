"""
Comprehensive diagnostic for B_Ladder ISM component visibility
Writes results to a file for review
"""
import unreal

output_file = "C:/scripts/slfconversion/ladder_diagnostic.txt"

def log(msg):
    """Log to file"""
    with open(output_file, "a") as f:
        f.write(msg + "\n")
    unreal.log(msg)

def diagnose_ladder_visibility():
    """Check all properties that could affect ladder visibility"""
    # Clear the output file
    with open(output_file, "w") as f:
        f.write("")

    log("="*80)
    log("B_LADDER VISIBILITY DIAGNOSTIC")
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

    # Get parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent Class: {parent}")
    except:
        log("Parent Class: (error getting)")

    # Get all components from CDO
    log("")
    log("-"*60)
    log("ISM COMPONENTS ON CDO:")
    log("-"*60)

    ism_components = cdo.get_components_by_class(unreal.InstancedStaticMeshComponent)
    log(f"Total ISM Components: {len(ism_components)}")

    for ism in ism_components:
        name = ism.get_name()
        log(f"\n=== {name} ===")

        # Mesh assignment
        mesh = ism.get_editor_property('static_mesh')
        mesh_name = mesh.get_name() if mesh else "NULL"
        log(f"  StaticMesh: {mesh_name}")

        if mesh:
            # Check mesh materials
            try:
                num_mats = mesh.get_num_sections(0)
                log(f"  Mesh Material Slots: {num_mats}")
                for i in range(num_mats):
                    mat = mesh.get_material(i)
                    mat_name = mat.get_name() if mat else "NULL"
                    log(f"    Material[{i}]: {mat_name}")
            except Exception as e:
                log(f"  Error getting mesh materials: {e}")

        # Instance count
        num_instances = ism.get_instance_count()
        log(f"  Instance Count: {num_instances}")

        # Visibility settings
        try:
            is_visible = ism.is_visible()
            log(f"  IsVisible(): {is_visible}")
        except Exception as e:
            log(f"  IsVisible(): error - {e}")

        try:
            hidden_in_game = ism.get_editor_property('hidden_in_game')
            log(f"  HiddenInGame: {hidden_in_game}")
        except Exception as e:
            log(f"  HiddenInGame: error - {e}")

        try:
            visible = ism.get_editor_property('visible')
            log(f"  Visible property: {visible}")
        except Exception as e:
            log(f"  Visible property: error - {e}")

        # Rendering settings
        try:
            render_in_main = ism.get_editor_property('render_in_main_pass')
            log(f"  RenderInMainPass: {render_in_main}")
        except Exception as e:
            log(f"  RenderInMainPass: error - {e}")

        # Mobility
        try:
            mobility = ism.get_editor_property('mobility')
            log(f"  Mobility: {mobility}")
        except Exception as e:
            log(f"  Mobility: error - {e}")

        # Culling
        try:
            cull_distance = ism.get_editor_property('ld_max_draw_distance')
            log(f"  LDMaxDrawDistance (culling): {cull_distance}")
        except Exception as e:
            log(f"  LDMaxDrawDistance: error - {e}")

        # Cast shadow
        try:
            casts_shadow = ism.get_editor_property('cast_shadow')
            log(f"  CastShadow: {casts_shadow}")
        except Exception as e:
            log(f"  CastShadow: error - {e}")

    # Check for the static meshes themselves
    log("")
    log("-"*60)
    log("STATIC MESH ASSETS:")
    log("-"*60)

    meshes_to_check = [
        "/Game/SoulslikeFramework/Meshes/SM/Ladder/SM_LadderBar",
        "/Game/SoulslikeFramework/Meshes/SM/Ladder/SM_LadderPole"
    ]

    for mesh_path in meshes_to_check:
        mesh = unreal.load_asset(mesh_path)
        if mesh:
            log(f"\n{mesh_path}")
            log(f"  Name: {mesh.get_name()}")
            log(f"  NumLODs: {mesh.get_num_lods()}")
            try:
                bounds = mesh.get_bounds()
                if bounds:
                    log(f"  Bounds - Origin: {bounds.origin}, BoxExtent: {bounds.box_extent}")
            except Exception as e:
                log(f"  Bounds: error - {e}")
            # Materials
            try:
                num_mats = mesh.get_num_sections(0)
                for i in range(num_mats):
                    mat = mesh.get_material(i)
                    log(f"  Material[{i}]: {mat.get_name() if mat else 'NULL'}")
            except Exception as e:
                log(f"  Materials: error - {e}")
        else:
            log(f"\n{mesh_path} - NOT FOUND!")

    log("")
    log("="*80)
    log("DIAGNOSTIC COMPLETE - Results saved to: " + output_file)
    log("="*80)

if __name__ == "__main__":
    diagnose_ladder_visibility()
