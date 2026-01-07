import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/test_dodge_save_result.txt"
lines = []

asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"

lines.append("=== Test Dodge Save ===\n")

# Step 1: Load and check initial state
lines.append("Step 1: Initial state")
asset = unreal.EditorAssetLibrary.load_asset(asset_path)
lines.append(f"  Class: {asset.get_class().get_name()}")

try:
    dm = asset.get_editor_property('DodgeMontages')
    lines.append(f"  DodgeMontages type: {type(dm)}")
    forward = getattr(dm, 'forward', None)
    lines.append(f"  Initial forward: {forward.get_name() if forward else 'NULL'}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Step 2: Set montages
lines.append("\nStep 2: Setting montages")
montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F"
montage = unreal.load_object(None, montage_path)
lines.append(f"  Loaded montage: {montage.get_name() if montage else 'NULL'}")

try:
    dm = asset.get_editor_property('DodgeMontages')
    setattr(dm, 'forward', montage)

    # Verify it was set on the struct
    check1 = getattr(dm, 'forward', None)
    lines.append(f"  After setattr, forward: {check1.get_name() if check1 else 'NULL'}")

    # Apply back to asset
    asset.set_editor_property('DodgeMontages', dm)
    lines.append("  Called set_editor_property")

    # Verify on asset (without reload)
    dm2 = asset.get_editor_property('DodgeMontages')
    check2 = getattr(dm2, 'forward', None)
    lines.append(f"  After set_editor_property, forward: {check2.get_name() if check2 else 'NULL'}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Step 3: Save
lines.append("\nStep 3: Saving")
try:
    result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
    lines.append(f"  save_asset result: {result}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Step 4: Verify (same session, no reload)
lines.append("\nStep 4: Verify (same session)")
try:
    dm3 = asset.get_editor_property('DodgeMontages')
    check3 = getattr(dm3, 'forward', None)
    lines.append(f"  forward: {check3.get_name() if check3 else 'NULL'}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

# Step 5: Force reload and verify
lines.append("\nStep 5: Force reload")
try:
    # Unload and reload
    unreal.EditorAssetLibrary.unload_asset(asset_path)
    asset2 = unreal.EditorAssetLibrary.load_asset(asset_path)
    dm4 = asset2.get_editor_property('DodgeMontages')
    check4 = getattr(dm4, 'forward', None)
    lines.append(f"  After reload, forward: {check4.get_name() if check4 else 'NULL'}")
except Exception as e:
    lines.append(f"  ERROR: {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print("Results written to test_dodge_save_result.txt")
