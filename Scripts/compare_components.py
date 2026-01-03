"""
Compare Blueprint Components Before/After Migration
Run with mode=before to capture, mode=after to compare
"""

import unreal
import json
import os

RESULTS_DIR = "C:/scripts/SLFConversion/component_snapshots"
RESULTS_FILE = "C:/scripts/SLFConversion/component_comparison.txt"

# Blueprint to analyze
TEST_BP = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

def log(msg):
    unreal.log(str(msg))
    with open(RESULTS_FILE, "a") as f:
        f.write(str(msg) + "\n")

def get_component_info(bp_path):
    """Extract component hierarchy from a Blueprint"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        return None

    info = {
        "name": bp.get_name(),
        "path": bp_path,
        "blueprint_components": [],
        "inherited_components": [],
    }

    # Get Blueprint-defined components (SimpleConstructionScript)
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                comp_template = node.component_template
                if comp_template:
                    comp_info = {
                        "name": comp_template.get_name(),
                        "class": comp_template.get_class().get_name(),
                    }
                    # Try to get parent
                    try:
                        parent_name = node.get_editor_property('parent_component_or_variable_name')
                        if parent_name:
                            comp_info["parent"] = str(parent_name)
                    except:
                        pass

                    # Try to get attached parent component name
                    try:
                        attach_parent = node.get_editor_property('attach_parent_name')
                        if attach_parent:
                            comp_info["attach_parent"] = str(attach_parent)
                    except:
                        pass

                    info["blueprint_components"].append(comp_info)
    except Exception as e:
        info["scs_error"] = str(e)

    # Get CDO components (inherited from C++)
    try:
        bp_class = bp.generated_class()
        if bp_class:
            cdo = unreal.get_default_object(bp_class)
            if cdo:
                # Check standard ACharacter components
                try:
                    mesh = cdo.get_editor_property('mesh')
                    if mesh:
                        mesh_info = {"name": mesh.get_name(), "class": "SkeletalMeshComponent"}
                        try:
                            skel_mesh = mesh.get_editor_property('skeletal_mesh')
                            mesh_info["skeletal_mesh"] = skel_mesh.get_name() if skel_mesh else "NONE"
                        except:
                            mesh_info["skeletal_mesh"] = "ERROR"
                        info["inherited_components"].append(mesh_info)
                except:
                    pass

                try:
                    capsule = cdo.get_editor_property('capsule_component')
                    if capsule:
                        info["inherited_components"].append({
                            "name": capsule.get_name(),
                            "class": "CapsuleComponent"
                        })
                except:
                    pass

                try:
                    movement = cdo.get_editor_property('character_movement')
                    if movement:
                        info["inherited_components"].append({
                            "name": movement.get_name(),
                            "class": "CharacterMovementComponent"
                        })
                except:
                    pass

                # Check for DefaultSceneRoot
                try:
                    scene_root = cdo.get_editor_property('default_scene_root')
                    if scene_root:
                        info["inherited_components"].append({
                            "name": scene_root.get_name(),
                            "class": "SceneComponent",
                            "note": "DefaultSceneRoot"
                        })
                except:
                    pass
    except Exception as e:
        info["cdo_error"] = str(e)

    return info

def capture_before():
    """Capture component state before migration"""
    os.makedirs(RESULTS_DIR, exist_ok=True)

    log("=" * 70)
    log("CAPTURING COMPONENT STATE - BEFORE MIGRATION")
    log("=" * 70)

    info = get_component_info(TEST_BP)
    if info:
        snapshot_file = os.path.join(RESULTS_DIR, "before.json")
        with open(snapshot_file, 'w') as f:
            json.dump(info, f, indent=2)

        log(f"Blueprint: {info['name']}")
        log(f"\nBlueprint-defined components: {len(info['blueprint_components'])}")
        for comp in info['blueprint_components']:
            parent_info = f" (parent: {comp.get('parent', 'N/A')})" if 'parent' in comp else ""
            log(f"  - {comp['name']} ({comp['class']}){parent_info}")

        log(f"\nInherited components: {len(info['inherited_components'])}")
        for comp in info['inherited_components']:
            extra = f" - {comp.get('skeletal_mesh', '')}" if 'skeletal_mesh' in comp else ""
            log(f"  - {comp['name']} ({comp['class']}){extra}")

        if 'scs_error' in info:
            log(f"\n[WARNING] SCS Error: {info['scs_error']}")

        log(f"\nSnapshot saved to: {snapshot_file}")
    else:
        log(f"[ERROR] Could not load {TEST_BP}")

def capture_after():
    """Capture component state after migration and compare"""
    log("=" * 70)
    log("CAPTURING COMPONENT STATE - AFTER MIGRATION")
    log("=" * 70)

    info = get_component_info(TEST_BP)
    if not info:
        log(f"[ERROR] Could not load {TEST_BP}")
        return

    # Save after snapshot
    snapshot_file = os.path.join(RESULTS_DIR, "after.json")
    with open(snapshot_file, 'w') as f:
        json.dump(info, f, indent=2)

    log(f"Blueprint: {info['name']}")
    log(f"\nBlueprint-defined components: {len(info['blueprint_components'])}")
    for comp in info['blueprint_components']:
        parent_info = f" (parent: {comp.get('parent', 'N/A')})" if 'parent' in comp else ""
        log(f"  - {comp['name']} ({comp['class']}){parent_info}")

    log(f"\nInherited components: {len(info['inherited_components'])}")
    for comp in info['inherited_components']:
        extra = f" - {comp.get('skeletal_mesh', '')}" if 'skeletal_mesh' in comp else ""
        log(f"  - {comp['name']} ({comp['class']}){extra}")

    # Compare with before
    before_file = os.path.join(RESULTS_DIR, "before.json")
    if os.path.exists(before_file):
        with open(before_file, 'r') as f:
            before_info = json.load(f)

        log("\n" + "=" * 70)
        log("COMPARISON: BEFORE vs AFTER")
        log("=" * 70)

        before_bp_comps = {c['name']: c for c in before_info.get('blueprint_components', [])}
        after_bp_comps = {c['name']: c for c in info.get('blueprint_components', [])}

        # Check for lost components
        lost = set(before_bp_comps.keys()) - set(after_bp_comps.keys())
        if lost:
            log(f"\n[LOST] Components that existed BEFORE but not AFTER:")
            for name in lost:
                comp = before_bp_comps[name]
                log(f"  - {name} ({comp['class']})")

        # Check for new components
        added = set(after_bp_comps.keys()) - set(before_bp_comps.keys())
        if added:
            log(f"\n[ADDED] Components that exist AFTER but not BEFORE:")
            for name in added:
                comp = after_bp_comps[name]
                log(f"  - {name} ({comp['class']})")

        # Check for parent changes
        log(f"\n[PARENT CHANGES]:")
        for name in before_bp_comps:
            if name in after_bp_comps:
                before_parent = before_bp_comps[name].get('parent', 'N/A')
                after_parent = after_bp_comps[name].get('parent', 'N/A')
                if before_parent != after_parent:
                    log(f"  - {name}: {before_parent} -> {after_parent}")

        if not lost and not added:
            log("\n[OK] All Blueprint components preserved!")
    else:
        log(f"\n[INFO] No 'before' snapshot found at {before_file}")
        log("Run with mode=before first to capture initial state")

    log("\n" + "=" * 70)
    log("COMPARISON COMPLETE")
    log("=" * 70)

def run():
    # Clear results file
    with open(RESULTS_FILE, "w") as f:
        f.write("")

    # Check for mode argument - default to 'after' for typical usage
    # To capture before: run this before migration
    # To compare after: run this after migration

    # For simplicity, we'll detect if before.json exists
    before_file = os.path.join(RESULTS_DIR, "before.json")
    if os.path.exists(before_file):
        capture_after()
    else:
        capture_before()

run()
