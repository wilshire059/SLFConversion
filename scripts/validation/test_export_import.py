import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/test_export_import_result.txt"
lines = []

asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"

lines.append("=== Test Export/Import ===\n")

asset = unreal.EditorAssetLibrary.load_asset(asset_path)
dm = asset.get_editor_property('DodgeMontages')

# Export current state
lines.append("Current struct export_text:")
try:
    export = dm.export_text()
    lines.append(f"  {export}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Set all montages using setattr
lines.append("\nSetting montages with setattr...")
MONTAGES = {
    "forward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F",
    "forward_left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FL.AM_SLF_Dodge_FL",
    "forward_right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FR.AM_SLF_Dodge_FR",
    "left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_L.AM_SLF_Dodge_L",
    "right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_R.AM_SLF_Dodge_R",
    "backward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_B.AM_SLF_Dodge_B",
    "backward_left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BL.AM_SLF_Dodge_BL",
    "backward_right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BR.AM_SLF_Dodge_BR",
    "backstep": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Backstep.AM_SLF_Backstep",
}

for prop, path in MONTAGES.items():
    montage = unreal.load_object(None, path)
    setattr(dm, prop, montage)
    lines.append(f"  Set {prop}")

# Export after setting
lines.append("\nAfter setting, struct export_text:")
try:
    export2 = dm.export_text()
    lines.append(f"  {export2}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Apply back and save
asset.set_editor_property('DodgeMontages', dm)
result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
lines.append(f"\nSave result: {result}")

# Export from asset after save
lines.append("\nAfter save, re-get and export_text:")
try:
    dm3 = asset.get_editor_property('DodgeMontages')
    export3 = dm3.export_text()
    lines.append(f"  {export3}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Try import_text approach
lines.append("\n--- Try import_text approach ---")

# Create a proper UE text representation
import_str = '''(Forward="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F",ForwardLeft="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FL.AM_SLF_Dodge_FL",ForwardRight="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FR.AM_SLF_Dodge_FR",Left="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_L.AM_SLF_Dodge_L",Right="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_R.AM_SLF_Dodge_R",Backward="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_B.AM_SLF_Dodge_B",BackwardLeft="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BL.AM_SLF_Dodge_BL",BackwardRight="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BR.AM_SLF_Dodge_BR",Backstep="/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Backstep.AM_SLF_Backstep")'''

lines.append(f"Import string: {import_str[:100]}...")

try:
    dm4 = asset.get_editor_property('DodgeMontages')
    result = dm4.import_text(import_str)
    lines.append(f"import_text result: {result}")

    # Check if it worked
    forward = getattr(dm4, 'forward', None)
    lines.append(f"After import_text, forward: {forward.get_name() if forward else 'NULL'}")

    # Apply and save
    asset.set_editor_property('DodgeMontages', dm4)
    save_result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
    lines.append(f"Save result: {save_result}")
except Exception as e:
    lines.append(f"ERROR: {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print("Results written to test_export_import_result.txt")
