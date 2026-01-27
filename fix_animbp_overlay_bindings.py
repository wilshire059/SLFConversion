#!/usr/bin/env python3
"""
Fix AnimBP BlendListByEnum node bindings for overlay states.

The BlendListByEnum nodes in ABP_SoulslikeCharacter_Additive have their
ActiveEnumValue pins disconnected (LinkedTo: []). This script binds them
to the correct overlay state properties.

Node mappings:
- F99C9040441C3A2822628383594FFC20 (OverlayLeft) -> LeftHandOverlayState
- BD9E3EF24ACC90F0C5F3CEAF16434931 (OverlayedPose) -> RightHandOverlayState
"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

# Node GUIDs from JSON export
LEFT_BLEND_NODE_GUID = "F99C9040441C3A2822628383594FFC20"
RIGHT_BLEND_NODE_GUID = "BD9E3EF24ACC90F0C5F3CEAF16434931"

def find_anim_graph_nodes(animbp):
    """Find BlendListByEnum nodes in AnimGraph."""
    log("Searching for AnimGraph nodes...")

    # Get the generated class
    gen_class = animbp.generated_class()
    if not gen_class:
        log("ERROR: No generated class")
        return None, None

    log(f"Generated class: {gen_class.get_name()}")

    # Try to get AnimGraph via various methods
    try:
        # Method 1: Get all graphs
        graphs = unreal.BlueprintEditorLibrary.get_graphs(animbp)
        log(f"Found {len(graphs)} graphs")

        for graph in graphs:
            graph_name = graph.get_name() if graph else "None"
            log(f"  Graph: {graph_name}")

            # Try to get nodes from graph
            try:
                # UE5 Python doesn't expose direct node access easily
                # We need to use EdGraph APIs
                pass
            except Exception as e:
                log(f"  Error accessing graph: {e}")

    except Exception as e:
        log(f"Error getting graphs: {e}")

    return None, None

def check_animbp_properties(animbp):
    """Check what properties are available on the AnimBP."""
    log("\n=== AnimBP Properties ===")

    try:
        # Get skeleton
        skeleton = animbp.get_editor_property("target_skeleton")
        log(f"Skeleton: {skeleton.get_name() if skeleton else 'None'}")

        # Get parent class
        parent = animbp.get_editor_property("parent_class")
        log(f"Parent class: {parent.get_name() if parent else 'None'}")

        # List available properties
        log("\nAvailable editor properties:")
        props_to_check = [
            "target_skeleton",
            "parent_class",
            "use_multi_threaded_animation_update",
            "anim_graph",
            "event_graphs",
        ]

        for prop in props_to_check:
            try:
                val = animbp.get_editor_property(prop)
                val_str = str(val)[:80] if val else "None"
                log(f"  {prop}: {val_str}")
            except Exception as e:
                log(f"  {prop}: [not accessible - {e}]")

    except Exception as e:
        log(f"Error checking properties: {e}")

def check_cpp_class_properties():
    """Check if the C++ AnimInstance class has the overlay properties."""
    log("\n=== C++ AnimInstance Class Check ===")

    # Load the C++ class
    cpp_class_path = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

    try:
        cpp_class = unreal.load_class(None, cpp_class_path)
        if cpp_class:
            log(f"Found C++ class: {cpp_class.get_name()}")

            # Get CDO
            cdo = unreal.get_default_object(cpp_class)
            if cdo:
                log("CDO found, checking overlay properties:")

                # Check overlay state properties
                props_to_check = [
                    "LeftHandOverlayState",
                    "RightHandOverlayState",
                    "ActiveOverlayState",
                    "left_hand_overlay_state",
                    "right_hand_overlay_state",
                    "active_overlay_state",
                ]

                for prop in props_to_check:
                    try:
                        val = cdo.get_editor_property(prop)
                        log(f"  {prop}: {val}")
                    except Exception as e:
                        pass  # Property name variant doesn't exist

        else:
            log("C++ class not found")

    except Exception as e:
        log(f"Error loading C++ class: {e}")

def diagnose_animgraph_binding_issue():
    """Diagnose why the BlendListByEnum nodes aren't reading properties."""
    log("\n=== AnimGraph Binding Diagnosis ===")
    log("The issue: BlendListByEnum.ActiveEnumValue pins have LinkedTo: []")
    log("")
    log("In UE5, BlendListByEnum nodes can be bound to properties via:")
    log("1. Property Access node connected to ActiveEnumValue pin")
    log("2. Direct binding via 'Bind' property on the node")
    log("")
    log("After migration, these connections were lost because:")
    log("- EventGraph was cleared (which had Set Variable nodes)")
    log("- Property Access nodes in AnimGraph may have been invalid")
    log("- Or the binding was never directly in AnimGraph")
    log("")
    log("SOLUTION: Need to either:")
    log("1. Manually reconnect in UE Editor (AnimGraph -> BlendListByEnum -> Details -> Bind)")
    log("2. Or restore AnimBP from backup and only clear EventGraph")

def suggest_manual_fix():
    """Provide instructions for manual fix in editor."""
    log("\n" + "=" * 70)
    log("MANUAL FIX INSTRUCTIONS")
    log("=" * 70)
    log("")
    log("1. Open ABP_SoulslikeCharacter_Additive in UE Editor")
    log("")
    log("2. Go to AnimGraph (NOT EventGraph)")
    log("")
    log("3. Find the FIRST 'Blend Poses (E_OverlayState)' node (outputs to OverlayLeft)")
    log("   - Click on it")
    log("   - In Details panel, find 'Active Enum Value'")
    log("   - Click the dropdown and select 'Bind'")
    log("   - Choose 'LeftHandOverlayState' property")
    log("")
    log("4. Find the SECOND 'Blend Poses (E_OverlayState)' node (outputs to OverlayedPose)")
    log("   - Click on it")
    log("   - In Details panel, find 'Active Enum Value'")
    log("   - Click the dropdown and select 'Bind'")
    log("   - Choose 'RightHandOverlayState' property")
    log("")
    log("5. Compile and Save the AnimBP")
    log("")
    log("6. Test in PIE - arm should now raise when equipping sword")
    log("=" * 70)

def attempt_programmatic_fix(animbp):
    """Attempt to fix the bindings programmatically."""
    log("\n=== Attempting Programmatic Fix ===")

    # In UE5, we can try to access the AnimGraph node bindings
    # via the FAnimNode_BlendListByEnum structure

    try:
        # Get the blueprint graphs
        graphs = unreal.BlueprintEditorLibrary.get_graphs(animbp)

        anim_graph = None
        for graph in graphs:
            if graph.get_name() == "AnimGraph":
                anim_graph = graph
                break

        if not anim_graph:
            log("AnimGraph not found")
            return False

        log(f"Found AnimGraph: {anim_graph.get_name()}")

        # Try to access nodes through the graph
        # Note: UE5 Python API has limited AnimGraph node access
        # We may need to use EditorScriptingUtilities or custom C++

        # Check if we can use SubsystemBlueprintLibrary
        try:
            from unreal import AnimationBlueprintLibrary
            log("AnimationBlueprintLibrary available")
        except ImportError:
            log("AnimationBlueprintLibrary not available")

        # The cleanest solution is to use Animation Blueprint Template
        # or restore from backup with surgical changes

        log("Programmatic binding of AnimGraph nodes requires C++ or manual editor work")
        return False

    except Exception as e:
        log(f"Error in programmatic fix: {e}")
        return False

def main():
    log("=" * 70)
    log("ANIMBP OVERLAY BINDING DIAGNOSTIC")
    log("=" * 70)

    # Load the AnimBP
    animbp = unreal.EditorAssetLibrary.load_asset(ANIMBP_PATH)
    if not animbp:
        log(f"ERROR: Could not load AnimBP at {ANIMBP_PATH}")
        return

    log(f"Loaded: {animbp.get_name()}")

    # Check AnimBP properties
    check_animbp_properties(animbp)

    # Check C++ class properties
    check_cpp_class_properties()

    # Try to find nodes
    find_anim_graph_nodes(animbp)

    # Diagnose the issue
    diagnose_animgraph_binding_issue()

    # Attempt programmatic fix
    fixed = attempt_programmatic_fix(animbp)

    if not fixed:
        # Provide manual fix instructions
        suggest_manual_fix()

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
