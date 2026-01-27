# rename_vars_v3.py
# Try various methods to access and rename AnimBP variables

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/rename_v3_log.txt"
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
with open(OUTPUT_FILE, "w") as f:
    f.write("")

def log(msg):
    print(f"[V3] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

def explore_animblueprint():
    """Deep exploration of AnimBlueprint structure."""
    log("=" * 60)
    log("DEEP EXPLORATION OF ANIMBLUEPRINT")
    log("=" * 60)

    asset = unreal.EditorAssetLibrary.load_asset(
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    )

    if not asset:
        log("ERROR: Could not load asset")
        return

    bp = asset
    log(f"Asset type: {type(bp).__name__}")

    # List all attributes
    log("")
    log("All attributes of AnimBlueprint:")
    for attr in dir(bp):
        if not attr.startswith('_'):
            log(f"  {attr}")

    # Try to get skeleton class
    log("")
    log("Trying skeleton_generated_class...")
    if hasattr(bp, 'skeleton_generated_class'):
        try:
            skel = bp.get_editor_property('skeleton_generated_class')
            log(f"  skeleton_generated_class: {skel}")
        except Exception as e:
            log(f"  Error: {e}")

    # Try target_skeleton
    log("")
    log("Trying target_skeleton...")
    if hasattr(bp, 'target_skeleton'):
        try:
            skel = bp.get_editor_property('target_skeleton')
            log(f"  target_skeleton: {skel}")
        except Exception as e:
            log(f"  Error: {e}")

    # Try blueprint_generated_class
    log("")
    log("Trying get_blueprint_generated_class...")
    try:
        gen_class = unreal.BlueprintEditorLibrary.generated_class(bp)
        log(f"  generated_class: {gen_class}")
        if gen_class:
            log(f"  Class name: {gen_class.get_name()}")

            # Get CDO
            cdo = gen_class.get_default_object()
            log(f"  CDO: {cdo}")

            # List properties
            log("")
            log("  Properties with '?' in name:")
            for prop in gen_class.properties():
                name = str(prop.get_name())
                if "?" in name:
                    log(f"    {name} ({prop.get_class().get_name()})")
    except Exception as e:
        log(f"  Error: {e}")

    # Try accessing all editor properties
    log("")
    log("Editor properties (subset):")
    props_to_check = [
        'blueprint_category', 'blueprint_description', 'blueprint_display_name',
        'blueprint_namespace', 'parent_class', 'simple_construction_script',
        'uber_graph_pages', 'function_graphs', 'macro_graphs', 'event_graphs',
        'implemented_interfaces', 'component_template_class',
    ]
    for prop_name in props_to_check:
        try:
            val = bp.get_editor_property(prop_name)
            log(f"  {prop_name}: {val}")
        except:
            pass

    # Get graphs
    log("")
    log("Graphs in Blueprint:")
    try:
        graphs = unreal.BlueprintEditorLibrary.get_graphs(bp)
        for g in graphs:
            log(f"  Graph: {g.get_name()}")
    except Exception as e:
        log(f"  Error: {e}")

    # Now try AC_CombatManager (regular Blueprint)
    log("")
    log("=" * 60)
    log("EXPLORING AC_CombatManager (Regular Blueprint)")
    log("=" * 60)

    asset2 = unreal.EditorAssetLibrary.load_asset(
        "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager"
    )

    if asset2:
        log(f"Asset type: {type(asset2).__name__}")

        # Try different ways to get variables
        log("")
        log("Checking for variable access methods...")

        if hasattr(asset2, 'new_variables'):
            log("  Has new_variables attribute")
            try:
                nv = asset2.get_editor_property('new_variables')
                log(f"  new_variables: {nv}")
            except Exception as e:
                log(f"  get_editor_property failed: {e}")
        else:
            log("  No new_variables attribute")

        # Try generated class
        try:
            gen_class = unreal.BlueprintEditorLibrary.generated_class(asset2)
            if gen_class:
                log(f"  Generated class: {gen_class.get_name()}")
                for prop in gen_class.properties():
                    name = str(prop.get_name())
                    if "?" in name or "Guard" in name:
                        log(f"    Property: {name}")
        except Exception as e:
            log(f"  Error getting generated class: {e}")

    log("")
    log("=" * 60)
    log("EXPLORATION COMPLETE")
    log("=" * 60)

if __name__ == "__main__":
    explore_animblueprint()
