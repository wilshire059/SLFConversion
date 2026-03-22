"""
Verify all Settings widgets compile correctly after migration.
"""
import unreal

WIDGETS = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
]

EXPECTED_PARENTS = {
    "W_Settings_Entry": "/Script/SLFConversion.W_Settings_Entry",
    "W_Settings_CategoryEntry": "/Script/SLFConversion.W_Settings_CategoryEntry",
    "PDA_CustomSettings": "/Script/SLFConversion.PDA_CustomSettings",
    "W_Settings": "/Script/SLFConversion.W_Settings",
}

unreal.log_warning("=== VERIFYING SETTINGS WIDGET MIGRATION ===\n")

all_pass = True
for path in WIDGETS:
    name = path.split('/')[-1]

    bp = unreal.load_asset(path)
    if not bp:
        unreal.log_warning(f"[FAIL] {name} - Could not load")
        all_pass = False
        continue

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    expected = EXPECTED_PARENTS.get(name, "")

    # Check parent matches expected C++ class
    if expected in parent:
        unreal.log_warning(f"[PASS] {name}")
        unreal.log_warning(f"  Parent: {parent}")
    else:
        unreal.log_warning(f"[FAIL] {name} - Wrong parent")
        unreal.log_warning(f"  Expected: {expected}")
        unreal.log_warning(f"  Got: {parent}")
        all_pass = False

if all_pass:
    unreal.log_warning("\n=== SUCCESS - ALL WIDGETS MIGRATED ===")
else:
    unreal.log_warning("\n=== SOME WIDGETS HAVE ERRORS ===")
