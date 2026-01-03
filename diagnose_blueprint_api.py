"""
diagnose_blueprint_api.py

Diagnostic script to determine what Unreal Python APIs are available
for Blueprint graph manipulation.

Run in Unreal Editor Python console:
    exec(open(r"C:\scripts\SLFConversion\diagnose_blueprint_api.py").read())
"""

import unreal

print("=" * 70)
print("  Blueprint Graph API Diagnostic")
print("=" * 70)

# Test Blueprint to analyze
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"

# =============================================================================
# Step 1: Load Blueprint and examine structure
# =============================================================================
print("\n[1] Loading Blueprint...")
bp = unreal.load_asset(BP_PATH)
if not bp:
    print("  FAILED to load Blueprint")
else:
    print(f"  Loaded: {bp.get_name()}")
    print(f"  Class: {type(bp).__name__}")

    # List all methods/attributes
    print("\n  Available methods/attributes on Blueprint:")
    attrs = [a for a in dir(bp) if not a.startswith('_')]
    for attr in attrs[:30]:  # First 30
        print(f"    - {attr}")
    if len(attrs) > 30:
        print(f"    ... and {len(attrs) - 30} more")

# =============================================================================
# Step 2: Check BlueprintEditorLibrary functions
# =============================================================================
print("\n[2] BlueprintEditorLibrary functions...")
try:
    bel = unreal.BlueprintEditorLibrary
    funcs = [f for f in dir(bel) if not f.startswith('_')]
    print("  Available functions:")
    for func in funcs:
        print(f"    - {func}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 3: Try to access Blueprint graphs
# =============================================================================
print("\n[3] Attempting to access Blueprint graphs...")
try:
    # Try different methods to get graphs
    if hasattr(bp, 'get_all_graphs'):
        graphs = bp.get_all_graphs()
        print(f"  get_all_graphs(): {graphs}")
    else:
        print("  No get_all_graphs() method")

    if hasattr(bp, 'function_graphs'):
        print(f"  function_graphs: {bp.function_graphs}")
    else:
        print("  No function_graphs attribute")

    if hasattr(bp, 'ubergraph_pages'):
        print(f"  ubergraph_pages: {bp.ubergraph_pages}")
    else:
        print("  No ubergraph_pages attribute")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 4: Check if we can get generated class and its properties
# =============================================================================
print("\n[4] Checking generated class...")
try:
    gen_class = bp.generated_class()
    if gen_class:
        print(f"  Generated class: {gen_class.get_name()}")

        # Try to get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            print(f"  CDO: {cdo.get_name()}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 5: Check EdGraphSchema and node types
# =============================================================================
print("\n[5] Checking for graph-related classes...")
graph_classes = [
    'EdGraph',
    'EdGraphNode',
    'EdGraphPin',
    'K2Node',
    'K2Node_SwitchEnum',
    'BlueprintGraph',
    'EdGraphSchema',
    'EdGraphSchema_K2'
]
for cls_name in graph_classes:
    if hasattr(unreal, cls_name):
        print(f"  unreal.{cls_name}: EXISTS")
    else:
        print(f"  unreal.{cls_name}: NOT EXPOSED")

# =============================================================================
# Step 6: Check SubobjectDataSubsystem for deep access
# =============================================================================
print("\n[6] Checking SubobjectDataSubsystem...")
try:
    subsystem = unreal.get_editor_subsystem(unreal.SubobjectDataSubsystem)
    if subsystem:
        print(f"  SubobjectDataSubsystem: Available")
    else:
        print("  SubobjectDataSubsystem: Not available")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 7: Try to use AssetTools to get more info
# =============================================================================
print("\n[7] Checking AssetTools...")
try:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    print(f"  AssetTools: {asset_tools}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 8: Check if we can access via BlueprintFunctionLibrary
# =============================================================================
print("\n[8] Checking BlueprintFunctionLibrary methods...")
try:
    # Check for any enum-related functions
    enum_funcs = []
    for name in dir(unreal):
        if 'enum' in name.lower() or 'switch' in name.lower():
            enum_funcs.append(name)
    print(f"  Enum/Switch related: {enum_funcs}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 9: Try to access the Blueprint's internal data
# =============================================================================
print("\n[9] Trying to access Blueprint internal data...")
try:
    # Check all editor properties
    props = bp.get_editor_property_names() if hasattr(bp, 'get_editor_property_names') else []
    print(f"  Editor properties: {props[:20] if props else 'Unable to get'}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Step 10: Check K2 Node specific APIs
# =============================================================================
print("\n[10] Checking K2 specific APIs...")
k2_related = [name for name in dir(unreal) if 'K2' in name or 'k2' in name]
print(f"  K2-related classes: {k2_related}")

# =============================================================================
# Step 11: Check if BlueprintFixerLibrary exists (used in migration script)
# =============================================================================
print("\n[11] Checking BlueprintFixerLibrary...")
if hasattr(unreal, 'BlueprintFixerLibrary'):
    print("  BlueprintFixerLibrary: EXISTS")
    funcs = [f for f in dir(unreal.BlueprintFixerLibrary) if not f.startswith('_')]
    print(f"  Functions: {funcs}")
else:
    print("  BlueprintFixerLibrary: NOT EXPOSED")

# =============================================================================
# Step 12: Try accessing via get_editor_property on different paths
# =============================================================================
print("\n[12] Trying get_editor_property on various paths...")
test_props = [
    'simple_construct_script',
    'component_templates',
    'timelines',
    'inherited_graphs',
    'macro_graphs',
    'event_graphs',
    'intermediate_generated_graphs',
    'function_graphs'
]
for prop in test_props:
    try:
        val = bp.get_editor_property(prop)
        print(f"  {prop}: {val}")
    except:
        print(f"  {prop}: NOT ACCESSIBLE")

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  DIAGNOSTIC COMPLETE")
print("=" * 70)
print("""
Based on this diagnostic, we need to determine:
1. Can we access Blueprint graph nodes via Python?
2. Can we modify pin default values?
3. What alternative approaches exist?

If direct graph access is not available, alternatives:
- Use text-based modification of .uasset files (risky)
- Use C++ plugin to expose necessary APIs
- Manual fixing in editor
""")
