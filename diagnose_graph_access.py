"""
diagnose_graph_access.py

Follow-up diagnostic to test promising APIs for graph/node access.

Run in Unreal Editor Python console:
    exec(open(r"C:\scripts\SLFConversion\diagnose_graph_access.py").read())
"""

import unreal

print("=" * 70)
print("  Graph Access Diagnostic - Testing Promising APIs")
print("=" * 70)

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
bp = unreal.load_asset(BP_PATH)

# =============================================================================
# Test 1: BlueprintEditorLibrary.find_graph
# =============================================================================
print("\n[1] Testing BlueprintEditorLibrary.find_graph...")
try:
    bel = unreal.BlueprintEditorLibrary

    # Try common graph names
    graph_names = [
        "EventGraph",
        "UberGraph",
        "ConstructionScript",
        "GetStat",
        "AdjustStat",
        "GetAllStats"
    ]

    for name in graph_names:
        try:
            graph = bel.find_graph(bp, name)
            if graph:
                print(f"  Found '{name}': {graph}")
                print(f"    Type: {type(graph).__name__}")
                # Try to get graph attributes
                graph_attrs = [a for a in dir(graph) if not a.startswith('_')]
                print(f"    Attributes: {graph_attrs[:15]}...")
        except Exception as e:
            print(f"  '{name}': {e}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 2: BlueprintEditorLibrary.find_event_graph
# =============================================================================
print("\n[2] Testing BlueprintEditorLibrary.find_event_graph...")
try:
    event_graph = bel.find_event_graph(bp)
    if event_graph:
        print(f"  Found EventGraph: {event_graph}")
        print(f"  Type: {type(event_graph).__name__}")

        # Try to iterate nodes if possible
        if hasattr(event_graph, 'nodes'):
            print(f"  nodes attribute: {event_graph.nodes}")
        if hasattr(event_graph, 'get_nodes'):
            print(f"  get_nodes(): {event_graph.get_nodes()}")

        # List all attributes
        attrs = [a for a in dir(event_graph) if not a.startswith('_')]
        print(f"  All attributes: {attrs}")
    else:
        print("  No event graph found")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 3: BlueprintFixerLibrary.get_detailed_node_info
# =============================================================================
print("\n[3] Testing BlueprintFixerLibrary.get_detailed_node_info...")
try:
    bfl = unreal.BlueprintFixerLibrary

    # This might need a node reference - let's see the signature
    print(f"  Function exists: {hasattr(bfl, 'get_detailed_node_info')}")

    # Try calling with blueprint
    try:
        info = bfl.get_detailed_node_info(bp)
        print(f"  Result: {info}")
    except TypeError as te:
        print(f"  TypeError (wrong args): {te}")
    except Exception as e:
        print(f"  Exception: {e}")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 4: BlueprintFixerLibrary.get_blueprint_diagnostics
# =============================================================================
print("\n[4] Testing BlueprintFixerLibrary.get_blueprint_diagnostics...")
try:
    diag = bfl.get_blueprint_diagnostics(bp)
    print(f"  Diagnostics:\n{diag[:2000] if len(str(diag)) > 2000 else diag}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 5: BlueprintFixerLibrary.list_blueprint_functions
# =============================================================================
print("\n[5] Testing BlueprintFixerLibrary.list_blueprint_functions...")
try:
    funcs = bfl.list_blueprint_functions(bp)
    print(f"  Functions: {funcs}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 6: Try to access EdGraph nodes via get_editor_property
# =============================================================================
print("\n[6] Testing EdGraph node access...")
try:
    if event_graph:
        # Try various property access methods
        props_to_try = ['nodes', 'Nodes', 'graph_nodes', 'all_nodes', 'schema']
        for prop in props_to_try:
            try:
                val = event_graph.get_editor_property(prop)
                print(f"  {prop}: {val}")
            except:
                pass

        # Check if EdGraph has any useful methods
        if hasattr(event_graph, 'get_all_nodes'):
            nodes = event_graph.get_all_nodes()
            print(f"  get_all_nodes(): {nodes}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 7: Check if we can access via generated class UFunction
# =============================================================================
print("\n[7] Testing UFunction access for graph functions...")
try:
    gen_class = bp.generated_class()

    # Try to find a function by name
    func_names = ["GetStat", "AdjustStat", "GetAllStats"]
    for fname in func_names:
        try:
            # Try find_function
            if hasattr(gen_class, 'find_function'):
                func = gen_class.find_function(fname)
                print(f"  find_function('{fname}'): {func}")
        except Exception as e:
            print(f"  {fname}: {e}")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 8: Check EdGraph class methods
# =============================================================================
print("\n[8] Checking EdGraph class capabilities...")
try:
    ed_graph_class = unreal.EdGraph
    methods = [m for m in dir(ed_graph_class) if not m.startswith('_')]
    print(f"  EdGraph methods: {methods}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 9: Check K2Node class methods
# =============================================================================
print("\n[9] Checking K2Node class capabilities...")
try:
    k2_class = unreal.K2Node
    methods = [m for m in dir(k2_class) if not m.startswith('_')]
    print(f"  K2Node methods: {methods}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 10: Try to use Asset Registry to find enum references
# =============================================================================
print("\n[10] Testing Asset Registry for enum dependencies...")
try:
    ar = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get dependencies of E_ValueType
    enum_path = "/Game/SoulslikeFramework/Enums/E_ValueType"

    # Get referencers (what depends on the enum)
    deps = ar.get_referencers(enum_path)
    print(f"  Assets referencing E_ValueType: {len(deps) if deps else 0}")
    if deps:
        for d in deps[:10]:
            print(f"    - {d}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 11: Check if we can use ObjectLibrary for iteration
# =============================================================================
print("\n[11] Testing ObjectLibrary iteration...")
try:
    # Try to get all objects in the Blueprint package
    package = bp.get_outermost()
    print(f"  Package: {package}")

    # Check if we can iterate subobjects
    if hasattr(unreal, 'EditorAssetLibrary'):
        eal = unreal.EditorAssetLibrary
        # List assets in package
        print(f"  EditorAssetLibrary methods: {[m for m in dir(eal) if 'list' in m.lower() or 'get' in m.lower()][:15]}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 12: Check SystemLibrary for object iteration
# =============================================================================
print("\n[12] Testing object iteration methods...")
try:
    if hasattr(unreal, 'SystemLibrary'):
        sl = unreal.SystemLibrary
        print(f"  SystemLibrary exists")
        obj_methods = [m for m in dir(sl) if 'object' in m.lower()]
        print(f"  Object-related methods: {obj_methods}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  DIAGNOSTIC COMPLETE")
print("=" * 70)
print("""
Key questions answered:
1. Can we find graphs? -> Check find_graph/find_event_graph results
2. Can we access nodes? -> Check EdGraph attributes
3. Can we get node details? -> Check get_detailed_node_info results
4. Alternative: Asset Registry for finding affected assets

If direct node access fails, UAssetAPI is the fallback.
""")
