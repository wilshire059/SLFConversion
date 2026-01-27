"""
Diagnose and fix B_Destructible visibility issues.
"""

import unreal
import os

def diagnose_destructible():
    """Diagnose B_Destructible Blueprint issues."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    gc_barrel_path = "/Game/SoulslikeFramework/Meshes/SM/Barrel/Destructible/GC_Barrel"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible Diagnosis")
    log("="*80)

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return output_lines

    log(f"\nBlueprint: {bp_path}")

    # Get the generated class
    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: No generated class!")
        return output_lines

    log(f"Generated Class: {gen_class.get_name()}")

    # Get the CDO (Class Default Object)
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        log(f"\nCDO (Default Object): {cdo.get_name()}")

        # Check GeometryCollection property
        try:
            gc = cdo.get_editor_property("geometry_collection")
            if gc:
                log(f"  GeometryCollection: {gc.get_path_name()}")
            else:
                log(f"  GeometryCollection: None (NOT SET) <-- PROBLEM!")
        except Exception as e:
            log(f"  GeometryCollection: ERROR - {e}")

        # Check DestructionSound property
        try:
            ds = cdo.get_editor_property("destruction_sound")
            log(f"  DestructionSound: {ds}")
        except Exception as e:
            log(f"  DestructionSound: ERROR - {e}")

        # Try to get components from CDO
        log(f"\nCDO Components:")
        try:
            components = cdo.get_components_by_class(unreal.GeometryCollectionComponent)
            log(f"  GeometryCollectionComponents: {len(components)}")
            for comp in components:
                log(f"    - {comp.get_name()}")
                try:
                    rest_coll = comp.get_editor_property("rest_collection")
                    if rest_coll:
                        log(f"        RestCollection: {rest_coll.get_path_name()}")
                    else:
                        log(f"        RestCollection: None (NOT SET) <-- PROBLEM!")
                except Exception as e:
                    log(f"        RestCollection: ERROR - {e}")
                try:
                    visible = comp.is_visible()
                    log(f"        Visible: {visible}")
                except:
                    pass
        except Exception as e:
            log(f"  Components error: {e}")

    # Check GC_Barrel asset
    log("\n" + "-"*40)
    log("Checking GC_Barrel asset...")
    gc_barrel = unreal.load_asset(gc_barrel_path)
    if gc_barrel:
        log(f"  GC_Barrel found: {gc_barrel.get_path_name()}")
        log(f"  Class: {type(gc_barrel).__name__}")
    else:
        log(f"  ERROR: Could not load GC_Barrel at {gc_barrel_path}")

    log("\n" + "="*80)
    log("Diagnosis Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_diagnosis.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

    return output_lines

if __name__ == "__main__":
    diagnose_destructible()
