# migrate_dodge_montages.py
# Migrates dodge montages from Blueprint FInstancedStruct to C++ DodgeMontages property
# Following the PDA_Item pattern - direct property access, no reflection

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/dodge_migration_result.txt"
lines = []

# The montage paths extracted from the DA_Action_Dodge.uasset binary
DODGE_MONTAGES = {
    "Forward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_F.AM_SLF_Dodge_F",
    "ForwardLeft": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FL.AM_SLF_Dodge_FL",
    "ForwardRight": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_FR.AM_SLF_Dodge_FR",
    "Left": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_L.AM_SLF_Dodge_L",
    "Right": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_R.AM_SLF_Dodge_R",
    "Backward": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_B.AM_SLF_Dodge_B",
    "BackwardLeft": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BL.AM_SLF_Dodge_BL",
    "BackwardRight": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Dodge_BR.AM_SLF_Dodge_BR",
    "Backstep": "/Game/SoulslikeFramework/Demo/_Animations/Dodges/AM_SLF_Backstep.AM_SLF_Backstep",
}

def migrate_dodge_montages():
    lines.append("=== Dodge Montages Migration ===\n")

    # Load the DA_Action_Dodge asset
    asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
    lines.append(f"Loading {asset_path}...")

    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        lines.append("ERROR: Could not load asset!")
        return False

    lines.append(f"  Loaded: {asset}")
    lines.append(f"  Class: {asset.get_class().get_name()}")

    # Get current DodgeMontages property
    try:
        dodge_montages = asset.get_editor_property('DodgeMontages')
        lines.append(f"\nCurrent DodgeMontages: {type(dodge_montages)}")
    except Exception as e:
        lines.append(f"WARNING: Could not get DodgeMontages property: {e}")
        lines.append("This may be expected if C++ needs rebuild first")
        return False

    # Load each montage and set the property
    lines.append("\nSetting montage properties...")

    # Python property names use snake_case with underscores
    PYTHON_PROP_NAMES = {
        "Forward": "forward",
        "ForwardLeft": "forward_left",
        "ForwardRight": "forward_right",
        "Left": "left",
        "Right": "right",
        "Backward": "backward",
        "BackwardLeft": "backward_left",
        "BackwardRight": "backward_right",
        "Backstep": "backstep",
    }

    for prop_name, montage_path in DODGE_MONTAGES.items():
        montage = unreal.load_object(None, montage_path)
        if montage:
            lines.append(f"  {prop_name}: {montage.get_name()}")
            try:
                # Set property on DodgeMontages struct (snake_case with underscores)
                python_prop = PYTHON_PROP_NAMES[prop_name]
                setattr(dodge_montages, python_prop, montage)
            except Exception as e:
                lines.append(f"    ERROR setting {prop_name}: {e}")
        else:
            lines.append(f"  {prop_name}: FAILED to load {montage_path}")

    # Apply the modified struct back
    try:
        asset.set_editor_property('DodgeMontages', dodge_montages)
        lines.append("\nApplied DodgeMontages to asset")
    except Exception as e:
        lines.append(f"\nERROR applying DodgeMontages: {e}")
        return False

    # Save the asset
    lines.append("\nSaving asset...")
    result = unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False)
    lines.append(f"  Save result: {result}")

    # Verify
    lines.append("\nVerifying...")
    verify = unreal.EditorAssetLibrary.load_asset(asset_path)
    try:
        verify_dm = verify.get_editor_property('DodgeMontages')
        forward = getattr(verify_dm, 'forward', None)
        if forward:
            lines.append(f"  Forward montage: {forward.get_name()}")
            lines.append("\n*** SUCCESS: Dodge montages migrated! ***")
        else:
            lines.append("  Forward montage: NULL")
            lines.append("\n*** WARNING: Verification failed ***")
    except Exception as e:
        lines.append(f"  Verification error: {e}")

    return True

# Run migration
migrate_dodge_montages()

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))
