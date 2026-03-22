"""
diagnose_enum_pins.py

Diagnostic script specifically for testing enum pin modification capabilities.

Run in Unreal Editor Python console:
    exec(open(r"C:\scripts\SLFConversion\diagnose_enum_pins.py").read())
"""

import unreal

print("=" * 70)
print("  Enum Pin Modification Diagnostic")
print("=" * 70)

# =============================================================================
# Test 1: Load the E_ValueType Blueprint enum
# =============================================================================
print("\n[1] Loading E_ValueType Blueprint enum...")
ENUM_PATH = "/Game/SoulslikeFramework/Enums/E_ValueType"

try:
    bp_enum = unreal.load_asset(ENUM_PATH)
    if bp_enum:
        print(f"  Loaded: {bp_enum.get_name()}")
        print(f"  Type: {type(bp_enum).__name__}")

        # Try to get enum values
        if hasattr(bp_enum, 'get_display_name_text_by_value'):
            for i in range(10):  # Try first 10 values
                try:
                    name = bp_enum.get_display_name_text_by_value(i)
                    print(f"  Value {i}: {name}")
                except:
                    break

        # List attributes
        attrs = [a for a in dir(bp_enum) if not a.startswith('_')]
        print(f"  Attributes: {attrs[:15]}...")
    else:
        print("  FAILED to load")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 2: Check if we can access enum entries via Python
# =============================================================================
print("\n[2] Testing enum value access...")
try:
    # Try to get enum names
    if hasattr(unreal, 'StringLibrary'):
        # Try using engine string functions
        print("  StringLibrary available")

    # Try EnumHelpers if available
    enum_helpers = [n for n in dir(unreal) if 'enum' in n.lower()]
    print(f"  Enum-related classes: {enum_helpers}")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 3: Load AC_StatManager and try to access function graphs
# =============================================================================
print("\n[3] Loading AC_StatManager Blueprint...")
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"

try:
    bp = unreal.load_asset(BP_PATH)
    if bp:
        print(f"  Loaded: {bp.get_name()}")

        # Try to get SimpleConstructionScript (component template)
        try:
            scs = bp.get_editor_property('simple_construct_script')
            print(f"  SimpleConstructionScript: {scs}")
        except:
            print("  SimpleConstructionScript: Not accessible")

        # Try to iterate through subobjects
        print("\n  Attempting to find graph objects...")

        # Check if there's a way to get all objects in the package
        # This is a long shot but worth trying

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 4: Check RigVMController availability (for SetPinDefaultValue)
# =============================================================================
print("\n[4] Checking RigVMController...")
try:
    if hasattr(unreal, 'RigVMController'):
        print("  RigVMController: EXISTS")
        funcs = [f for f in dir(unreal.RigVMController) if 'pin' in f.lower() or 'default' in f.lower()]
        print(f"  Pin/Default functions: {funcs}")
    else:
        print("  RigVMController: NOT EXPOSED")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 5: Check EditorScriptingLibrary
# =============================================================================
print("\n[5] Checking scripting utilities...")
try:
    # Check for any SetPin functions
    set_funcs = [n for n in dir(unreal) if 'set' in n.lower() and 'pin' in n.lower()]
    print(f"  SetPin functions: {set_funcs}")

    # Check for graph-related functions
    graph_funcs = [n for n in dir(unreal) if 'graph' in n.lower()]
    print(f"  Graph functions: {graph_funcs[:20]}...")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 6: Try to use BlueprintEditorLibrary for graph access
# =============================================================================
print("\n[6] Testing BlueprintEditorLibrary graph functions...")
try:
    bel = unreal.BlueprintEditorLibrary

    # Get all graphs from Blueprint
    if hasattr(bel, 'get_all_graphs_of_blueprint'):
        graphs = bel.get_all_graphs_of_blueprint(bp)
        print(f"  Graphs found: {len(graphs) if graphs else 0}")
        if graphs:
            for i, g in enumerate(graphs[:5]):
                print(f"    Graph {i}: {g.get_name() if hasattr(g, 'get_name') else g}")
    else:
        print("  get_all_graphs_of_blueprint: NOT AVAILABLE")

    # Try find_graph_by_name
    if hasattr(bel, 'find_graph_by_name'):
        print("  find_graph_by_name: EXISTS")
    else:
        print("  find_graph_by_name: NOT AVAILABLE")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 7: Check if PropertyAccessLibrary exists for deep property access
# =============================================================================
print("\n[7] Checking PropertyAccessLibrary...")
try:
    if hasattr(unreal, 'PropertyAccessLibrary'):
        print("  PropertyAccessLibrary: EXISTS")
        funcs = [f for f in dir(unreal.PropertyAccessLibrary) if not f.startswith('_')]
        print(f"  Functions: {funcs[:10]}...")
    else:
        print("  PropertyAccessLibrary: NOT EXPOSED")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 8: Try EditorUtilityLibrary
# =============================================================================
print("\n[8] Checking EditorUtilityLibrary...")
try:
    if hasattr(unreal, 'EditorUtilityLibrary'):
        eul = unreal.EditorUtilityLibrary
        funcs = [f for f in dir(eul) if not f.startswith('_')]
        print(f"  Functions: {funcs}")
    else:
        print("  EditorUtilityLibrary: NOT EXPOSED")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 9: Check for FBlueprintEditorUtils-like functionality
# =============================================================================
print("\n[9] Checking for Blueprint editor utilities...")
try:
    # Look for anything that could help modify blueprints
    blueprint_utils = [n for n in dir(unreal) if 'blueprint' in n.lower() and ('util' in n.lower() or 'library' in n.lower() or 'helper' in n.lower())]
    print(f"  Blueprint utilities: {blueprint_utils}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 10: Try to access Blueprint subobjects via reflection
# =============================================================================
print("\n[10] Attempting Blueprint subobject access...")
try:
    # Get class of Blueprint
    bp_class = bp.get_class()
    print(f"  Blueprint class: {bp_class.get_name() if bp_class else 'None'}")

    # Try to find EdGraph subobjects
    if hasattr(bp, 'get_outer'):
        outer = bp.get_outer()
        print(f"  Outer: {outer}")

except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 11: Check PCH subsystems
# =============================================================================
print("\n[11] Checking Editor Subsystems...")
try:
    # List all subsystems
    subsystem_types = [n for n in dir(unreal) if 'subsystem' in n.lower()]
    print(f"  Subsystem types: {subsystem_types[:15]}...")

    # Try to get AssetEditorSubsystem
    if hasattr(unreal, 'AssetEditorSubsystem'):
        aes = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)
        if aes:
            print(f"  AssetEditorSubsystem: Available")
            funcs = [f for f in dir(aes) if not f.startswith('_')]
            print(f"    Functions: {funcs[:10]}...")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Test 12: Check if we can access via generated_class properties
# =============================================================================
print("\n[12] Checking generated class for functions...")
try:
    gen_class = bp.generated_class()
    if gen_class:
        # Try to iterate over functions
        funcs = gen_class.get_functions() if hasattr(gen_class, 'get_functions') else None
        if funcs:
            print(f"  Functions: {funcs}")
        else:
            print("  get_functions: Not available")

        # Try call_function to see what's callable
        print(f"  Generated class methods: {[m for m in dir(gen_class) if 'func' in m.lower()]}")
except Exception as e:
    print(f"  ERROR: {e}")

# =============================================================================
# Summary
# =============================================================================
print("\n" + "=" * 70)
print("  DIAGNOSTIC COMPLETE")
print("=" * 70)
print("""
Next steps based on findings:
1. If BlueprintEditorLibrary.get_all_graphs_of_blueprint works, we may be able
   to iterate over nodes and pins
2. If not, we may need to use binary file manipulation or C++ plugin
3. Alternative: Create new nodes with correct values and delete old ones
""")
