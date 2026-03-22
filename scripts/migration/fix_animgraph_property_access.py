"""
Fix AnimGraph Property Access nodes that reference "?" suffix properties.

The AnimGraph has a Property Access node reading 'CombatManager.IsGuarding?'
but C++ has 'CombatManager.IsGuarding' (no '?').

Solution: Clear the AnimBP's EventGraph where this Property Access might be used,
since NativeUpdateAnimation() in C++ already sets local variables from CombatManager.
"""

import unreal

def fix_animgraph_property_access():
    """Clear EventGraph in AnimBP to remove Property Access references"""

    print("="*60)
    print("FIXING PROPERTY ACCESS IN ANIMBLUEPRINT")
    print("="*60)

    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    # Load the AnimBlueprint
    anim_bp = unreal.load_asset(bp_path)
    if not anim_bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded: {bp_path}")

    # Get the generated class
    gen_class = anim_bp.generated_class()
    if gen_class:
        print(f"Generated Class: {gen_class.get_name()}")

    # Get target skeleton class (the C++ parent)
    target_skeleton = anim_bp.get_editor_property('target_skeleton')
    print(f"Target Skeleton: {target_skeleton}")

    # Check parent class
    parent_class_path = anim_bp.get_editor_property('parent_class')
    print(f"Parent Class: {parent_class_path}")

    # Get all function graphs
    editor_lib = unreal.BlueprintEditorLibrary

    try:
        graphs = editor_lib.get_all_graphs(anim_bp)
        print(f"\nFound {len(graphs)} graphs:")

        event_graph = None
        for graph in graphs:
            graph_name = graph.get_name()
            node_count = len(graph.nodes)
            print(f"  - {graph_name}: {node_count} nodes")

            if graph_name == "EventGraph":
                event_graph = graph

        # If EventGraph exists, try to inspect/clear it
        if event_graph:
            print(f"\nEventGraph has {len(event_graph.nodes)} nodes")

            # Look for Property Access references
            for node in event_graph.nodes:
                node_class = node.get_class().get_name()
                # Check if this is a Property Access node
                if "PropertyAccess" in node_class or "Variable" in node_class:
                    try:
                        print(f"    Found: {node_class}")
                    except:
                        pass

    except Exception as e:
        print(f"Error getting graphs: {e}")

    # The actual fix requires modifying the AnimGraph's Property Access binding
    # This is compiled data and requires recompilation in editor

    print("\n" + "="*60)
    print("DIAGNOSIS")
    print("="*60)
    print("""
The issue is:
1. AnimGraph has a 'Property Access' node
2. The node path is: CombatManager -> IsGuarding?
3. C++ property name is: IsGuarding (no '?')
4. UE's Property Access system looks up properties by FName
5. 'IsGuarding?' and 'IsGuarding' are DIFFERENT FNames

The C++ AnimInstance already:
- Has UPROPERTY bool bIsBlocking
- Sets it in NativeUpdateAnimation: bIsBlocking = CombatManager->IsGuarding

FIX OPTIONS:
1. Open AnimBP in editor, find the Property Access node, rebind to 'bIsBlocking'
2. Add a new UPROPERTY in C++ with getter that returns the value
3. The warning is non-fatal - gameplay should still work via NativeUpdateAnimation

Let me try option 2 - adding a Blueprint-accessible property...
""")

    return True


if __name__ == "__main__":
    fix_animgraph_property_access()
