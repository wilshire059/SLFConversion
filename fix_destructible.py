"""
Fix B_Destructible Blueprint.
- Reparent to C++ AB_Destructible
- Set GeometryCollection to GC_Barrel
- Clear Blueprint logic (C++ handles it now)
"""

import unreal
import os

def fix_destructible():
    """Fix B_Destructible Blueprint."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    cpp_class_path = "/Script/SLFConversion.B_Destructible"
    gc_barrel_path = "/Game/SoulslikeFramework/Meshes/SM/Barrel/Destructible/GC_Barrel"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("Fixing B_Destructible")
    log("="*80)

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return output_lines

    log(f"\nLoaded Blueprint: {bp_path}")

    # Check current parent
    gen_class = bp_asset.generated_class()
    if gen_class:
        try:
            parent = gen_class.get_super_class()
            if parent:
                log(f"Current parent: {parent.get_name()}")
        except:
            pass

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        log(f"ERROR: Could not load C++ class: {cpp_class_path}")
        return output_lines

    log(f"Target C++ parent: {cpp_class.get_name()}")

    # Reparent if needed
    try:
        # Get the current parent class properly
        current_parent = None
        if gen_class:
            for parent_class in [gen_class]:
                try:
                    current_parent = gen_class.get_super_class()
                except:
                    pass

        if current_parent and current_parent.get_name() != "B_Destructible":
            log(f"Blueprint already reparented to: {current_parent.get_name()}")
        else:
            # Reparent using BlueprintEditorLibrary
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
            log(f"Reparenting result: {result}")
    except Exception as e:
        log(f"Reparenting note: {e}")

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

    # Clear Blueprint logic (C++ handles it)
    log(f"\nClearing Blueprint logic...")
    try:
        # Clear the event graph
        graphs = bp_asset.uber_graph_pages
        cleared_count = 0
        for graph in graphs:
            if graph and "EventGraph" in graph.get_name():
                nodes = list(graph.nodes)
                for node in nodes:
                    try:
                        graph.remove_node(node)
                        cleared_count += 1
                    except:
                        pass
        log(f"  Cleared {cleared_count} nodes from EventGraph")

        # Clear function graphs
        func_graphs = bp_asset.function_graphs
        for fg in func_graphs:
            if fg and "UserConstructionScript" in fg.get_name():
                nodes = list(fg.nodes)
                entry_count = 0
                for node in nodes:
                    node_name = node.get_name() if node else ""
                    # Keep function entry node
                    if "K2Node_FunctionEntry" in node_name:
                        entry_count += 1
                        continue
                    try:
                        fg.remove_node(node)
                    except:
                        pass
                log(f"  Cleared UserConstructionScript (kept {entry_count} entry nodes)")
    except Exception as e:
        log(f"  Note clearing graphs: {e}")

    # Mark as structurally modified and compile
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

    log("\n" + "="*80)
    log("Fix Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_fix.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

    return output_lines

if __name__ == "__main__":
    fix_destructible()
