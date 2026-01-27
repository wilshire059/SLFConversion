"""
Full diagnosis of B_SkyManager Blueprint state after migration.
Checks for:
1. Duplicate components (Blueprint SCS vs C++ CreateDefaultSubobject)
2. Compilation errors
3. EventGraph/Function remnants that need clearing
"""

import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_full_diagnosis.txt"

def log(msg):
    unreal.log(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def diagnose_skymanager():
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("B_SkyManager Full Diagnosis")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

    # Load Blueprint
    log(f"\nLoading Blueprint: {bp_path}")
    bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp_asset:
        log("ERROR: Could not load Blueprint")
        return

    # Get generated class
    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return

    log(f"Generated Class: {gen_class.get_name()}")

    # Check parent class
    parent_class = gen_class.get_super_class()
    log(f"Parent Class: {parent_class.get_name() if parent_class else 'None'}")

    # Check CDO components
    log("\n" + "-" * 50)
    log("CDO COMPONENTS")
    log("-" * 50)

    cdo = unreal.get_default_object(gen_class)
    if cdo:
        all_components = cdo.get_components_by_class(unreal.ActorComponent)
        log(f"Total CDO Components: {len(all_components)}")

        # Group by type
        comp_by_type = {}
        for comp in all_components:
            comp_class = comp.get_class().get_name()
            comp_name = comp.get_name()
            if comp_class not in comp_by_type:
                comp_by_type[comp_class] = []
            comp_by_type[comp_class].append(comp_name)

        for comp_class, names in comp_by_type.items():
            log(f"\n  {comp_class}: {len(names)} instance(s)")
            for name in names:
                log(f"    - {name}")

        # Check for duplicates
        log("\n" + "-" * 50)
        log("DUPLICATE CHECK")
        log("-" * 50)

        all_names = [comp.get_name() for comp in all_components]
        seen = set()
        duplicates = []
        for name in all_names:
            if name in seen:
                duplicates.append(name)
            seen.add(name)

        if duplicates:
            log(f"DUPLICATES FOUND: {duplicates}")
        else:
            log("No exact name duplicates found")

        # Check for similar names (e.g., DirectionalLight vs DirectionalLight_Sun)
        log("\nComponent names that might be duplicates:")
        directional_lights = [n for n in all_names if "Directional" in n or "Light" in n]
        log(f"  Lights: {directional_lights}")

        sky_comps = [n for n in all_names if "Sky" in n]
        log(f"  Sky-related: {sky_comps}")

    # Use EditorScriptingLibrary to get more info
    log("\n" + "-" * 50)
    log("BLUEPRINT STRUCTURE (via export)")
    log("-" * 50)

    # Export to text to analyze
    try:
        export_path = "C:/scripts/SLFConversion/migration_cache/B_SkyManager_export.txt"
        # Use unreal's export functionality
        result = unreal.EditorAssetLibrary.export_text(bp_path, export_path)
        if result:
            log(f"Exported to: {export_path}")

            # Read and analyze
            with open(export_path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()

            # Count SimpleConstructionScript entries (Blueprint SCS)
            scs_count = content.count("SimpleConstructionScript")
            log(f"SimpleConstructionScript references: {scs_count}")

            # Count SCS_Node entries
            scs_node_count = content.count("SCS_Node")
            log(f"SCS_Node entries: {scs_node_count}")

            # Look for EventGraph
            if "EventGraph" in content:
                log("EventGraph: PRESENT (needs clearing)")
            else:
                log("EventGraph: Not found or empty")

            # Look for function graphs
            func_graph_count = content.count("FunctionGraph")
            log(f"FunctionGraph entries: {func_graph_count}")

            # Look for UberGraphFrame (indicates Blueprint logic)
            uber_count = content.count("UberGraphFrame")
            log(f"UberGraphFrame entries: {uber_count}")

        else:
            log("Export failed")
    except Exception as e:
        log(f"Export error: {e}")

    # Check for compilation status
    log("\n" + "-" * 50)
    log("COMPILATION STATUS")
    log("-" * 50)

    try:
        # Try to compile and check for errors
        from unreal import BlueprintEditorLibrary
        compile_result = BlueprintEditorLibrary.compile_blueprint(bp_asset)
        log(f"Compile result: {compile_result}")
    except Exception as e:
        log(f"Compile check error: {e}")

    # Summary
    log("\n" + "=" * 70)
    log("DIAGNOSIS SUMMARY")
    log("=" * 70)
    log(f"Blueprint: {bp_path}")
    log(f"Parent: {parent_class.get_name() if parent_class else 'Unknown'}")
    log(f"CDO Components: {len(all_components) if cdo else 'Unknown'}")

    log("\nRECOMMENDED ACTIONS:")
    if len(all_components) > 8:
        log("1. CLEAR Blueprint SCS - duplicate components detected")
    log("2. CLEAR EventGraph - remove all Blueprint logic")
    log("3. CLEAR Functions - let C++ parent handle everything")
    log("4. RESAVE Blueprint")

    log(f"\nResults written to: {OUTPUT_FILE}")

if __name__ == "__main__":
    diagnose_skymanager()
