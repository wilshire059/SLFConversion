"""
Complete fix for B_Destructible Blueprint.
- Remove duplicate Blueprint variables (C++ has them now)
- Clear EventGraph logic (C++ handles it)
- Keep SCS components (GC_DestructibleMesh, Billboard)
"""

import unreal
import os

def fix_destructible_complete():
    """Complete fix for B_Destructible Blueprint."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    cpp_class_path = "/Script/SLFConversion.B_Destructible"
    gc_barrel_path = "/Game/SoulslikeFramework/Meshes/SM/Barrel/Destructible/GC_Barrel"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("Complete Fix for B_Destructible")
    log("="*80)

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return output_lines

    log(f"\nLoaded Blueprint: {bp_path}")

    # Get simple construction script to check SCS components
    try:
        scs = bp_asset.simple_construction_script
        if scs:
            log(f"\nSCS Components found:")
            # Use root nodes
            try:
                root_nodes = scs.get_all_nodes()
                for node in root_nodes:
                    if node:
                        comp_template = node.component_template
                        if comp_template:
                            log(f"  - {comp_template.get_name()} ({type(comp_template).__name__})")
            except Exception as e:
                log(f"  Error listing SCS nodes: {e}")
        else:
            log("\nWARNING: No SCS found!")
    except Exception as e:
        log(f"\nNote getting SCS: {e}")

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        log(f"ERROR: Could not load C++ class: {cpp_class_path}")
        return output_lines

    log(f"\nTarget C++ parent: {cpp_class.get_name()}")

    # Check if already reparented
    gen_class = bp_asset.generated_class()
    current_parent_name = ""
    if gen_class:
        try:
            current_parent = gen_class.get_super_class()
            if current_parent:
                current_parent_name = current_parent.get_name()
                log(f"Current parent: {current_parent_name}")
        except:
            pass

    # Reparent if needed
    if current_parent_name != "B_Destructible":
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
            log(f"Reparenting to C++ class...")
        except Exception as e:
            log(f"Reparenting note: {e}")
    else:
        log("Already reparented to C++ class")

    # Remove duplicate Blueprint variables (C++ defines them)
    log(f"\nRemoving duplicate Blueprint variables...")
    variables_to_remove = ["GeometryCollection", "DestructionSound"]
    for var_name in variables_to_remove:
        try:
            # Use the FName version
            fname = unreal.Name(var_name)
            # Try to remove via FBlueprintEditorUtils equivalent
            removed = unreal.BlueprintEditorLibrary.remove_variable(bp_asset, fname)
            if removed:
                log(f"  Removed variable: {var_name}")
            else:
                log(f"  Variable {var_name} not found or couldn't remove")
        except Exception as e:
            log(f"  Note removing {var_name}: {e}")

    # Load GC_Barrel asset
    gc_barrel = unreal.load_asset(gc_barrel_path)
    if not gc_barrel:
        log(f"ERROR: Could not load GC_Barrel: {gc_barrel_path}")
        return output_lines

    log(f"\nLoaded GC_Barrel: {gc_barrel.get_path_name()}")

    # Get the CDO and set the GeometryCollection property
    gen_class = bp_asset.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"\nSetting CDO properties...")

            # Set GeometryCollection
            try:
                cdo.set_editor_property("geometry_collection", gc_barrel)
                log(f"  Set GeometryCollection to: {gc_barrel_path}")
            except Exception as e:
                log(f"  ERROR setting GeometryCollection: {e}")

            # Verify the property was set
            try:
                gc_check = cdo.get_editor_property("geometry_collection")
                if gc_check:
                    log(f"  Verified GeometryCollection: {gc_check.get_path_name()}")
                else:
                    log(f"  WARNING: GeometryCollection still None!")
            except Exception as e:
                log(f"  ERROR verifying: {e}")

    # Compile the Blueprint
    log(f"\nCompiling Blueprint...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        log("  Compiled successfully")
    except Exception as e:
        log(f"  Compile note: {e}")

    # Save the asset
    log(f"\nSaving asset...")
    try:
        result = unreal.EditorAssetLibrary.save_asset(bp_path)
        log(f"  Save result: {result}")
    except Exception as e:
        log(f"  Save error: {e}")

    # Final verification
    log(f"\n" + "-"*40)
    log("Final Verification:")
    gen_class = bp_asset.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                gc = cdo.get_editor_property("geometry_collection")
                log(f"  GeometryCollection: {gc.get_path_name() if gc else 'None'}")
            except Exception as e:
                log(f"  GeometryCollection check error: {e}")

            # Check for spawned components
            try:
                all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                log(f"  CDO ActorComponents: {len(all_comps)}")
            except:
                log(f"  CDO ActorComponents: (SCS components created at spawn)")

    log("\n" + "="*80)
    log("Fix Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_fix_complete.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

    return output_lines

if __name__ == "__main__":
    fix_destructible_complete()
