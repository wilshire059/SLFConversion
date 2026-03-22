# find_reparent_method.py
# Find methods for reparenting Blueprint data assets

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("FINDING REPARENT METHODS")
lines.append("=" * 60)

# Check BlueprintEditorLibrary
lines.append("\n--- BlueprintEditorLibrary methods ---")
if hasattr(unreal, 'BlueprintEditorLibrary'):
    lib = unreal.BlueprintEditorLibrary
    # Check all methods
    all_methods = [m for m in dir(lib) if not m.startswith('_')]
    lines.append(f"  All methods ({len(all_methods)}): {all_methods[:30]}")

    # Look for reparent-related methods
    reparent_methods = [m for m in dir(lib) if 'parent' in m.lower() or 'repar' in m.lower() or 'base' in m.lower()]
    lines.append(f"  Reparent-related: {reparent_methods}")
else:
    lines.append("  BlueprintEditorLibrary NOT FOUND")

# Load the PDA_Item Blueprint and check if we can access its parent
lines.append("\n--- Load PDA_Item Blueprint ---")
try:
    # Load the Blueprint asset itself (not an instance)
    bp_path = "/Game/SoulslikeFramework/Data/PDA_Item"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    lines.append(f"  Blueprint: {bp}")
    lines.append(f"  Type: {type(bp)}")
    lines.append(f"  Class Name: {bp.get_class().get_name()}")

    # Check all methods on the Blueprint
    all_methods = [m for m in dir(bp) if not m.startswith('_')]
    lines.append(f"  All methods ({len(all_methods)}): {all_methods[:40]}")

    # Check set_ methods
    set_methods = [m for m in dir(bp) if m.startswith('set_')]
    lines.append(f"  set_ methods: {set_methods}")

    # Check get_ methods for parent info
    get_methods = [m for m in dir(bp) if 'parent' in m.lower() or 'super' in m.lower()]
    lines.append(f"  parent/super methods: {get_methods}")

except Exception as e:
    lines.append(f"  ERROR: {e}")

# Check if Blueprint class is different from data asset class
lines.append("\n--- Check Blueprint type ---")
try:
    bp = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/PDA_Item")
    lines.append(f"  Is Blueprint: {isinstance(bp, unreal.Blueprint) if hasattr(unreal, 'Blueprint') else 'N/A'}")
    lines.append(f"  Class hierarchy: {type(bp).__mro__}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Try using EditorUtilityLibrary
lines.append("\n--- EditorUtilityLibrary ---")
if hasattr(unreal, 'EditorUtilityLibrary'):
    lib = unreal.EditorUtilityLibrary
    reparent_methods = [m for m in dir(lib) if 'parent' in m.lower() or 'repar' in m.lower()]
    lines.append(f"  Reparent-related: {reparent_methods}")
else:
    lines.append("  EditorUtilityLibrary NOT FOUND")

# Check if there's a specific method for setting parent class
lines.append("\n--- Try reparent_blueprint ---")
if hasattr(unreal, 'BlueprintEditorLibrary'):
    lib = unreal.BlueprintEditorLibrary
    if hasattr(lib, 'reparent_blueprint'):
        lines.append("  reparent_blueprint EXISTS")
        # Check method signature
        lines.append(f"  Method: {lib.reparent_blueprint}")
    else:
        lines.append("  reparent_blueprint NOT FOUND on BlueprintEditorLibrary")

# Check what libraries exist
lines.append("\n--- Available Editor libraries ---")
editor_libs = [name for name in dir(unreal) if 'Editor' in name and 'Library' in name]
lines.append(f"  {editor_libs[:15]}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
