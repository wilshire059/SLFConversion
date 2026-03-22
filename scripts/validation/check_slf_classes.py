# check_slf_classes.py
# Check which SLF C++ classes are visible to Python

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("CHECKING SLF CLASSES IN PYTHON")
lines.append("=" * 60)

# Check for SLF classes
lines.append("\n--- Classes containing 'SLF' in unreal module ---")
found = []
for name in sorted(dir(unreal)):
    if 'SLF' in name or 'slf' in name.lower():
        found.append(name)
lines.append(f"  Found {len(found)} SLF classes")
for f in found[:30]:
    lines.append(f"    {f}")

# Try loading some known SLF classes
lines.append("\n--- Trying to load known SLF classes ---")
test_classes = [
    "/Script/SLFConversion.SLFSoulslikeCharacter",
    "/Script/SLFConversion.StatManagerComponent",
    "/Script/SLFConversion.InputBufferComponent",
    "/Script/SLFConversion.UPDA_Base",
    "/Script/SLFConversion.UPDA_Item",
    "/Script/SLFConversion.UPDA_Action",
]

for cls_path in test_classes:
    try:
        cls = unreal.load_class(None, cls_path)
        lines.append(f"  {cls_path}: {cls}")
    except Exception as e:
        lines.append(f"  {cls_path}: ERROR - {e}")

# Try find_object instead
lines.append("\n--- Using find_object ---")
try:
    obj = unreal.find_object(None, "/Script/SLFConversion.UPDA_Item")
    lines.append(f"  find_object UPDA_Item: {obj}")
except Exception as e:
    lines.append(f"  find_object UPDA_Item: ERROR - {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
