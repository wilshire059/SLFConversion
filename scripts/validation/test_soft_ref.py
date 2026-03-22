import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/test_soft_ref_result.txt"
lines = []

asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F"

lines.append("=== Test Soft Reference ===\n")

asset = unreal.EditorAssetLibrary.load_asset(asset_path)
dm = asset.get_editor_property('DodgeMontages')

lines.append(f"DodgeMontages type: {type(dm)}")
lines.append(f"DodgeMontages dir: {[x for x in dir(dm) if not x.startswith('_')]}")

# Check what type 'forward' is
lines.append(f"\nChecking 'forward' property:")
try:
    forward_val = getattr(dm, 'forward', None)
    lines.append(f"  Type: {type(forward_val)}")
    lines.append(f"  Value: {forward_val}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Try to set using different approaches
lines.append("\n--- Testing different set approaches ---")

# Approach 1: Direct object assignment (what we've been doing)
lines.append("\nApproach 1: Direct UAnimMontage object")
try:
    montage = unreal.load_object(None, montage_path)
    lines.append(f"  Montage type: {type(montage)}")
    setattr(dm, 'forward', montage)
    lines.append(f"  After setattr: {getattr(dm, 'forward')}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Approach 2: Try using SoftObjectPath
lines.append("\nApproach 2: SoftObjectPath")
try:
    soft_path = unreal.SoftObjectPath(montage_path)
    lines.append(f"  SoftObjectPath type: {type(soft_path)}")
    lines.append(f"  SoftObjectPath value: {soft_path}")
    # Can we set the property with this?
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Approach 3: Check if there's a special setter
lines.append("\nApproach 3: Check struct methods")
try:
    methods = [x for x in dir(dm) if 'set' in x.lower()]
    lines.append(f"  Set methods: {methods}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Approach 4: Try using set_editor_property on the struct
lines.append("\nApproach 4: set_editor_property on struct")
try:
    montage = unreal.load_object(None, montage_path)
    dm.set_editor_property('forward', montage)
    lines.append(f"  After set_editor_property: {getattr(dm, 'forward')}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print("Results written to test_soft_ref_result.txt")
