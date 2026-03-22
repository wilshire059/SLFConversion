"""
Check mesh assignments on SCS component templates
"""
import unreal

output_file = "C:/scripts/slfconversion/template_meshes_check.txt"

def log(msg):
    with open(output_file, "a") as f:
        f.write(msg + "\n")

def check_template_meshes():
    with open(output_file, "w") as f:
        f.write("")

    log("="*80)
    log("SCS TEMPLATE MESH CHECK")
    log("="*80)

    b_ladder_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"
    bp = unreal.load_asset(b_ladder_path)
    if not bp:
        log("ERROR: Could not load B_Ladder")
        return

    log(f"Loaded: {b_ladder_path}")

    # Get SCS and iterate through nodes
    try:
        scs_data = unreal.SLFAutomationLibrary.get_blueprint_scs_data(bp)
        log(f"\nSCS Data: {scs_data}")
    except Exception as e:
        log(f"get_blueprint_scs_data error: {e}")

    # Try to get component templates via C++ function
    log("\n--- Checking ISM mesh via C++ function ---")
    try:
        result = unreal.SLFAutomationLibrary.check_ism_component_meshes(bp)
        log(f"ISM mesh check result: {result}")
    except Exception as e:
        log(f"Error: {e}")

    # Try to access the Blueprint's SCS directly
    log("\n--- Raw Blueprint properties ---")
    try:
        # List all properties on the blueprint object
        import sys
        props = dir(bp)
        scs_related = [p for p in props if 'scs' in p.lower() or 'construction' in p.lower() or 'component' in p.lower()]
        log(f"SCS-related properties: {scs_related}")
    except Exception as e:
        log(f"Error: {e}")

    log("\n" + "="*80)
    log("DONE")
    log("="*80)

if __name__ == "__main__":
    check_template_meshes()
