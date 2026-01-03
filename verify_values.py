# verify_values.py
# Quick verification script to check if CDO values were restored

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/verify_output.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("VERIFYING CDO VALUES")
    results.append("=" * 60)

    # Load PC_SoulslikeFramework
    bp_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        results.append(f"ERROR: Could not load {bp_path}")
        with open(OUTPUT_FILE, 'w') as f:
            f.write("\n".join(results))
        return

    # Get CDO
    gen_class = bp.generated_class()
    if not gen_class:
        results.append("ERROR: No generated class")
        with open(OUTPUT_FILE, 'w') as f:
            f.write("\n".join(results))
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        results.append("ERROR: No CDO")
        with open(OUTPUT_FILE, 'w') as f:
            f.write("\n".join(results))
        return

    # Check GameplayMappingContext
    results.append("PC_SoulslikeFramework CDO:")

    # Try to get the property
    try:
        # Try CamelCase (UPROPERTY name)
        val = cdo.get_editor_property("GameplayMappingContext")
        if val:
            results.append(f"  GameplayMappingContext = {val.get_path_name()}")
        else:
            results.append("  GameplayMappingContext = None (NOT SET)")
    except Exception as e:
        results.append(f"  GameplayMappingContext ERROR (CamelCase): {e}")

        # Try snake_case (Python convention)
        try:
            val = cdo.get_editor_property("gameplay_mapping_context")
            if val:
                results.append(f"  gameplay_mapping_context = {val.get_path_name()}")
            else:
                results.append("  gameplay_mapping_context = None (NOT SET)")
        except Exception as e2:
            results.append(f"  gameplay_mapping_context ERROR: {e2}")

    # Also check the parent C++ class CDO
    results.append("Parent C++ class CDO (APC_SoulslikeFramework):")
    try:
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.PC_SoulslikeFramework")
        if cpp_class:
            cpp_cdo = unreal.get_default_object(cpp_class)
            if cpp_cdo:
                try:
                    val = cpp_cdo.get_editor_property("GameplayMappingContext")
                    if val:
                        results.append(f"  GameplayMappingContext = {val.get_path_name()}")
                    else:
                        results.append("  GameplayMappingContext = None")
                except Exception as e:
                    results.append(f"  Error: {e}")
        else:
            results.append("  ERROR: Could not find C++ class")
    except Exception as e:
        results.append(f"  ERROR loading class: {e}")

    # Write results to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))

run()
