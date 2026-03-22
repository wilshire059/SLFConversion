"""
Check SCS component templates directly in both projects
"""
import unreal

output_file = "C:/scripts/slfconversion/scs_templates_check.txt"

def log(msg):
    with open(output_file, "a") as f:
        f.write(msg + "\n")

def check_scs_templates():
    with open(output_file, "w") as f:
        f.write("")

    log("="*80)
    log("SCS COMPONENT TEMPLATE CHECK")
    log("="*80)

    b_ladder_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"
    bp = unreal.load_asset(b_ladder_path)
    if not bp:
        log("ERROR: Could not load B_Ladder")
        return

    log(f"Loaded: {b_ladder_path}")

    # Try different ways to access SCS
    log("\n--- Accessing SCS via SLFAutomationLibrary ---")
    try:
        comp_names = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log(f"Component names: {list(comp_names)}")
    except Exception as e:
        log(f"Error: {e}")

    # Try to get the SimpleConstructionScript directly
    log("\n--- Accessing SCS directly ---")
    try:
        # The Blueprint class in UE5 might have different property names
        # Try to iterate through properties
        for prop_name in ['simple_construction_script', 'SimpleConstructionScript', 'scs']:
            try:
                scs = bp.get_editor_property(prop_name)
                log(f"Found via '{prop_name}': {scs}")
                break
            except:
                pass
    except Exception as e:
        log(f"Error: {e}")

    # Try exporting Blueprint to text
    log("\n--- Checking via export_text ---")
    try:
        export_text = unreal.EditorAssetLibrary.export_text(b_ladder_path)
        if export_text:
            # Look for ISM component references
            lines = export_text.split('\n')
            ism_lines = [l for l in lines if 'InstancedStaticMesh' in l or 'Barz' in l or 'Pole' in l]
            log(f"Found {len(ism_lines)} ISM-related lines in export")
            for line in ism_lines[:30]:  # First 30 lines
                log(f"  {line.strip()[:120]}")
    except Exception as e:
        log(f"Error: {e}")

    # Spawn a ladder actor and check its components
    log("\n--- Spawning a test ladder ---")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            # Can't spawn without a world in commandlet
            log("Cannot spawn actor in headless mode")
        else:
            log("No generated class")
    except Exception as e:
        log(f"Error: {e}")

    log("\n" + "="*80)
    log("DONE")
    log("="*80)

if __name__ == "__main__":
    check_scs_templates()
