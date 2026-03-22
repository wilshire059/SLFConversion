"""
Diagnose Character - Inspect B_Soulslike_Character to find why it's not visible
"""

import unreal

RESULTS_FILE = "C:/scripts/SLFConversion/diagnose_results.txt"

def log(msg):
    unreal.log(str(msg))
    with open(RESULTS_FILE, "a") as f:
        f.write(str(msg) + "\n")

def run():
    with open(RESULTS_FILE, "w") as f:
        f.write("")

    log("=" * 70)
    log("DIAGNOSING B_Soulslike_Character")
    log("=" * 70)

    # Load the character Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        log(f"[ERROR] Could not load: {bp_path}")
        return

    log(f"[OK] Loaded: {bp.get_name()}")

    # Get the generated class
    bp_class = bp.generated_class()
    if bp_class:
        log(f"[OK] Generated class: {bp_class.get_name()}")
    else:
        log("[ERROR] No generated class!")
        return

    # Get the CDO (Class Default Object)
    cdo = unreal.get_default_object(bp_class)
    if not cdo:
        log("[ERROR] Could not get CDO")
        return

    log(f"[OK] CDO: {cdo.get_name()}")

    # Check for SimpleConstructionScript (where Blueprint components live)
    log("\n--- BLUEPRINT COMPONENTS (SimpleConstructionScript) ---")
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            log(f"[OK] SimpleConstructionScript found")

            # Get all nodes
            all_nodes = scs.get_all_nodes()
            log(f"     Total component nodes: {len(all_nodes)}")

            for node in all_nodes:
                comp_template = node.component_template
                if comp_template:
                    comp_name = comp_template.get_name()
                    comp_class = comp_template.get_class().get_name()

                    # Get parent
                    parent_node = node.get_editor_property('parent_component_or_variable_name')
                    parent_info = f" (parent: {parent_node})" if parent_node else ""

                    log(f"     - {comp_name} ({comp_class}){parent_info}")

                    # Check for mesh
                    if "SkeletalMesh" in comp_class:
                        try:
                            mesh = comp_template.get_editor_property('skeletal_mesh')
                            log(f"       Mesh: {mesh.get_name() if mesh else 'NONE'}")
                        except:
                            log(f"       Mesh: (could not read)")

                    # Check for camera
                    if "Camera" in comp_class:
                        log(f"       [CAMERA COMPONENT FOUND]")

                    # Check for spring arm
                    if "SpringArm" in comp_class:
                        try:
                            arm_length = comp_template.get_editor_property('target_arm_length')
                            log(f"       TargetArmLength: {arm_length}")
                        except:
                            pass
        else:
            log("[WARNING] No SimpleConstructionScript - no Blueprint components!")
    except Exception as e:
        log(f"[ERROR] Reading SCS: {e}")

    # Check inherited components from C++ parent
    log("\n--- C++ PARENT COMPONENTS ---")
    try:
        mesh_comp = cdo.get_editor_property('mesh')
        if mesh_comp:
            log(f"[OK] Mesh component exists: {mesh_comp.get_name()}")
            try:
                skel_mesh = mesh_comp.get_editor_property('skeletal_mesh')
                if skel_mesh:
                    log(f"     SkeletalMesh: {skel_mesh.get_name()}")
                else:
                    log(f"     SkeletalMesh: NONE - NO MESH ASSIGNED!")

                # Check visibility
                visible = mesh_comp.get_editor_property('visible')
                log(f"     Visible: {visible}")
            except Exception as e:
                log(f"     (could not read mesh properties: {e})")
        else:
            log("[ERROR] No mesh component!")

        capsule = cdo.get_editor_property('capsule_component')
        if capsule:
            log(f"[OK] Capsule component: {capsule.get_name()}")
            try:
                radius = capsule.get_editor_property('capsule_radius')
                half_height = capsule.get_editor_property('capsule_half_height')
                log(f"     Radius: {radius}, HalfHeight: {half_height}")
            except:
                pass
        else:
            log("[WARNING] No capsule component")

        movement = cdo.get_editor_property('character_movement')
        if movement:
            log(f"[OK] CharacterMovement: {movement.get_name()}")
        else:
            log("[WARNING] No movement component")
    except Exception as e:
        log(f"[ERROR] Reading C++ components: {e}")

    # Check GameMode
    log("\n--- GAMEMODE CHECK ---")
    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    gm = unreal.EditorAssetLibrary.load_asset(gm_path)
    if gm:
        gm_class = gm.generated_class()
        if gm_class:
            gm_cdo = unreal.get_default_object(gm_class)
            if gm_cdo:
                try:
                    default_pawn = gm_cdo.get_editor_property('default_pawn_class')
                    log(f"[OK] DefaultPawnClass: {default_pawn.get_name() if default_pawn else 'NONE'}")
                except Exception as e:
                    log(f"[WARNING] Could not read DefaultPawnClass: {e}")

    log("\n" + "=" * 70)
    log("DIAGNOSIS COMPLETE")
    log("=" * 70)

run()
