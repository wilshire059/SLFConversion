"""Check component hierarchy across parent Blueprints"""

import unreal

RESULTS_FILE = "C:/scripts/SLFConversion/parent_component_check.txt"

def log(msg):
    unreal.log(str(msg))
    with open(RESULTS_FILE, "a") as f:
        f.write(str(msg) + "\n")

def check_bp(bp_path, name):
    log(f"\n--- {name} ---")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load {bp_path}")
        return

    log(f"[OK] Loaded: {bp.get_name()}")

    # Check parent class
    try:
        parent = bp.get_editor_property('parent_class')
        log(f"Parent class: {parent.get_name() if parent else 'None'}")
    except Exception as e:
        log(f"[WARNING] Could not read parent_class: {e}")

    # Try to read SCS
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_all_nodes()
            log(f"SCS Components: {len(all_nodes)}")
            for node in all_nodes:
                comp_template = node.component_template
                if comp_template:
                    comp_name = comp_template.get_name()
                    comp_class = comp_template.get_class().get_name()
                    log(f"  - {comp_name} ({comp_class})")
        else:
            log("No SCS found")
    except Exception as e:
        log(f"[WARNING] Could not read SCS: {e}")

    # Check CDO for C++ components
    try:
        bp_class = bp.generated_class()
        if bp_class:
            cdo = unreal.get_default_object(bp_class)
            if cdo:
                # Try to get DefaultSceneRoot
                try:
                    dsr = cdo.get_editor_property('default_scene_root')
                    log(f"DefaultSceneRoot: {dsr.get_name() if dsr else 'None'}")
                except:
                    log("DefaultSceneRoot: Not found")

                # Check mesh
                try:
                    mesh = cdo.get_editor_property('mesh')
                    if mesh:
                        try:
                            skel = mesh.get_editor_property('skeletal_mesh')
                            log(f"Mesh: {mesh.get_name()}, SkeletalMesh: {skel.get_name() if skel else 'NONE'}")
                        except:
                            log(f"Mesh: {mesh.get_name()}, SkeletalMesh: (could not read)")
                except:
                    pass
    except Exception as e:
        log(f"[WARNING] CDO error: {e}")

def run():
    with open(RESULTS_FILE, "w") as f:
        f.write("")

    log("=" * 70)
    log("CHECKING PARENT COMPONENT HIERARCHY")
    log("=" * 70)

    # Check the chain: B_Soulslike_Character -> B_BaseCharacter -> C++ SLFSoulslikeCharacter
    check_bp("/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character", "B_Soulslike_Character")
    check_bp("/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter", "B_BaseCharacter")

    # Also check interactable hierarchy for the DefaultSceneRoot warnings
    check_bp("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable", "B_Interactable")
    check_bp("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door", "B_Door")

    log("\n" + "=" * 70)
    log("CHECK COMPLETE")
    log("=" * 70)

run()
