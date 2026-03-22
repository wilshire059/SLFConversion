"""
Force recompile Settings widgets and verify they compile
"""
import unreal

widgets = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
]

unreal.log_warning("=== FORCE RECOMPILE SETTINGS WIDGETS ===\n")

all_pass = True
for path in widgets:
    name = path.split('/')[-1]

    bp = unreal.load_asset(path)
    if not bp:
        unreal.log_warning(f"[FAIL] {name} - Could not load")
        all_pass = False
        continue

    # Force recompile
    unreal.log_warning(f"Recompiling {name}...")
    try:
        compile_result = unreal.KismetEditorUtilities.compile_blueprint(bp)
        unreal.log_warning(f"  Compile result: {compile_result}")
    except Exception as e:
        unreal.log_warning(f"  Compile exception: {e}")

    # Check compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)

    has_errors = "Error" in errors or "error" in errors
    if has_errors:
        unreal.log_warning(f"[FAIL] {name} - Has compile errors")
        unreal.log_warning(f"  {errors[:500]}")
        all_pass = False
    else:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        unreal.log_warning(f"[PASS] {name}")
        unreal.log_warning(f"  Parent: {parent}")

    # Save the blueprint
    unreal.EditorAssetLibrary.save_asset(path)
    unreal.log_warning(f"  Saved {name}")

if all_pass:
    unreal.log_warning("\n=== SUCCESS - ALL WIDGETS COMPILE ===")
else:
    unreal.log_warning("\n=== SOME WIDGETS HAVE ERRORS ===")
