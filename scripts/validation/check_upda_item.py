# check_upda_item.py
# Check if UPDA_Item class is accessible - outputs to file

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("CHECKING UPDA_Item CLASS")
lines.append("=" * 60)

# Try to find the class
class_path = "/Script/SLFConversion.UPDA_Item"
lines.append(f"\nTrying to load: {class_path}")

try:
    cls = unreal.load_class(None, class_path)
    lines.append(f"  Found: {cls}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Try finding via Python module
lines.append("\n--- Checking unreal module ---")
try:
    if hasattr(unreal, 'UPDA_Item'):
        lines.append(f"unreal.UPDA_Item: {unreal.UPDA_Item}")
    else:
        lines.append("unreal.UPDA_Item: NOT FOUND in unreal module")
except Exception as e:
    lines.append(f"unreal.UPDA_Item: {type(e).__name__}")

# List some related classes
lines.append("\n--- Classes containing 'pda' or 'upda' ---")
found = []
for name in sorted(dir(unreal)):
    if 'pda' in name.lower():
        found.append(name)
lines.append(f"  {found[:20]}")

# Check if UPDA_Base exists
lines.append("\n--- Checking UPDA_Base ---")
if hasattr(unreal, 'UPDA_Base'):
    lines.append(f"unreal.UPDA_Base: {unreal.UPDA_Base}")
else:
    lines.append("unreal.UPDA_Base: NOT FOUND")

# Check if PrimaryDataAsset exists
lines.append("\n--- Checking PrimaryDataAsset ---")
if hasattr(unreal, 'PrimaryDataAsset'):
    lines.append(f"unreal.PrimaryDataAsset: {unreal.PrimaryDataAsset}")
else:
    lines.append("unreal.PrimaryDataAsset: NOT FOUND")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
