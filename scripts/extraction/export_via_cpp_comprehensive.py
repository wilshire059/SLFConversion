# export_via_cpp_comprehensive.py
# Use C++ SLFAutomationLibrary for comprehensive AnimBP export

import unreal
import json
import os

def log(msg):
    print(f"[CppExport] {msg}")
    unreal.log_warning(f"[CppExport] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/animbp_comprehensive_current.txt"

def main():
    log("=" * 80)
    log("COMPREHENSIVE ANIMBP EXPORT VIA C++ AUTOMATION")
    log("=" * 80)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    output = ""

    # 1. Full diagnosis
    log("1. Getting DiagnoseAnimBP...")
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
    output += "=" * 80 + "\n"
    output += "SECTION 1: DIAGNOSE ANIMBP\n"
    output += "=" * 80 + "\n"
    output += diagnosis + "\n\n"

    # 2. All state machines
    log("2. Getting all state machine structures...")
    for sm_name in ["Locomotion", "States", "Falling"]:
        sm_structure = unreal.SLFAutomationLibrary.get_state_machine_structure(bp, sm_name)
        if sm_structure and len(sm_structure) > 50:
            output += "=" * 80 + "\n"
            output += f"SECTION 2: STATE MACHINE - {sm_name}\n"
            output += "=" * 80 + "\n"
            output += sm_structure + "\n\n"

    # 3. All transitions (empty keyword = all)
    log("3. Getting ALL transition graphs...")
    all_transitions = unreal.SLFAutomationLibrary.inspect_transition_graph(bp, "")
    output += "=" * 80 + "\n"
    output += "SECTION 3: ALL TRANSITION GRAPHS\n"
    output += "=" * 80 + "\n"
    output += all_transitions + "\n\n"

    # 4. Export AnimGraph state
    log("4. Getting AnimGraph state...")
    animgraph_state = unreal.SLFAutomationLibrary.export_anim_graph_state(bp, "")
    output += "=" * 80 + "\n"
    output += "SECTION 4: ANIMGRAPH STATE\n"
    output += "=" * 80 + "\n"
    output += animgraph_state + "\n\n"

    # 5. Blueprint variables
    log("5. Getting Blueprint variables...")
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    output += "=" * 80 + "\n"
    output += "SECTION 5: BLUEPRINT VARIABLES\n"
    output += "=" * 80 + "\n"
    for var in variables:
        output += f"  - {var}\n"
    output += f"\nTotal variables: {len(variables)}\n\n"

    # 6. Blueprint functions
    log("6. Getting Blueprint functions...")
    functions = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    output += "=" * 80 + "\n"
    output += "SECTION 6: BLUEPRINT FUNCTIONS\n"
    output += "=" * 80 + "\n"
    for func in functions:
        output += f"  - {func}\n"
    output += f"\nTotal functions: {len(functions)}\n\n"

    # 7. Parent class
    log("7. Getting parent class...")
    parent_class = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    output += "=" * 80 + "\n"
    output += "SECTION 7: PARENT CLASS\n"
    output += "=" * 80 + "\n"
    output += f"Parent: {parent_class}\n\n"

    # 8. Compile errors
    log("8. Getting compile errors...")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    output += "=" * 80 + "\n"
    output += "SECTION 8: COMPILE ERRORS\n"
    output += "=" * 80 + "\n"
    output += errors + "\n\n"

    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w', encoding='utf-8') as f:
        f.write(output)

    log(f"Total output length: {len(output)} chars")
    log(f"Saved to {OUTPUT_PATH}")
    log("[SUCCESS] Comprehensive export complete")

if __name__ == "__main__":
    main()
