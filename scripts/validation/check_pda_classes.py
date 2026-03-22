# check_pda_classes.py
# Check if PDA_* classes are accessible (without U prefix)

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("CHECKING PDA_* CLASS ACCESSIBILITY")
lines.append("=" * 60)

# Check Python module for PDA_ prefixed classes (Unreal strips U prefix)
lines.append("\n--- Classes containing 'PDA' in unreal module ---")
pda_classes = []
for name in sorted(dir(unreal)):
    if 'PDA' in name:
        pda_classes.append(name)
lines.append(f"  Found {len(pda_classes)} PDA classes:")
for cls in pda_classes[:20]:
    lines.append(f"    {cls}")

# Try accessing classes directly from unreal module
lines.append("\n--- Direct module access ---")
class_names = ['PDA_Item', 'PDA_Base', 'PDA_Action', 'PDA_Buff',
               'UPDA_Item', 'UPDA_Base', 'SLFPrimaryDataAsset',
               'PrimaryDataAsset', 'DataAsset']
for name in class_names:
    if hasattr(unreal, name):
        cls = getattr(unreal, name)
        lines.append(f"  unreal.{name}: {cls}")
    else:
        lines.append(f"  unreal.{name}: NOT FOUND")

# Try different load_class paths
lines.append("\n--- load_class with different paths ---")
paths_to_try = [
    "/Script/SLFConversion.PDA_Item",
    "/Script/SLFConversion.PDA_Base",
    "/Script/SLFConversion.UPDA_Item",
    "/Script/Engine.PrimaryDataAsset",
    "/Script/Engine.DataAsset",
]
for path in paths_to_try:
    try:
        cls = unreal.load_class(None, path)
        lines.append(f"  {path}: {cls}")
    except Exception as e:
        lines.append(f"  {path}: ERROR - {type(e).__name__}")

# Check if we can use EditorAssetSubsystem for reparenting
lines.append("\n--- EditorAssetSubsystem check ---")
try:
    subsys = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
    lines.append(f"  EditorAssetSubsystem: {subsys}")

    # Check available methods for reparenting
    reparent_methods = [m for m in dir(subsys) if 'parent' in m.lower() or 'repar' in m.lower()]
    lines.append(f"  Reparent methods: {reparent_methods}")
except Exception as e:
    lines.append(f"  EditorAssetSubsystem: ERROR - {e}")

# Try loading a data asset Blueprint to check its parent
lines.append("\n--- Load existing PDA_Item Blueprint ---")
try:
    pda_item_bp = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_Apple")
    if pda_item_bp:
        lines.append(f"  DA_Apple: {pda_item_bp}")
        lines.append(f"  Class: {pda_item_bp.get_class()}")

        # Get parent class info
        bp_class = pda_item_bp.get_class()
        if hasattr(bp_class, 'get_super_struct'):
            parent = bp_class.get_super_struct()
            lines.append(f"  Parent class: {parent}")
except Exception as e:
    lines.append(f"  DA_Apple: ERROR - {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
