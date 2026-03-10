"""
Complete Settings Widget Validation
Validates all aspects of settings widget migration:
1. Embedded widget properties (SettingTag, EntryType, etc.)
2. Blueprint parent class reparenting
3. C++ class properties
4. Widget tree structure
"""
import unreal
import json

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("COMPLETE SETTINGS WIDGET VALIDATION")
    unreal.log_warning("=" * 70)

    # 1. Check W_Settings_Entry Blueprint
    unreal.log_warning("\n--- 1. W_Settings_Entry Blueprint State ---")
    entry_bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    entry_bp = unreal.load_asset(entry_bp_path)
    if entry_bp:
        unreal.log_warning(f"  Loaded: {entry_bp.get_name()}")
        # WidgetBlueprint uses generated_class to get the UClass
        gen_class = entry_bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"  Parent class: {parent.get_name() if parent else 'None'}")

            # Check if reparented to C++
            is_reparented = "W_Settings_Entry" in str(parent) if parent else False
            unreal.log_warning(f"  Reparented to C++: {'YES' if is_reparented else 'NO (needs reparent)'}")
        else:
            unreal.log_warning(f"  Could not get generated class")
    else:
        unreal.log_warning(f"  ERROR: Failed to load {entry_bp_path}")

    # 2. Check W_Settings_CenteredText Blueprint
    unreal.log_warning("\n--- 2. W_Settings_CenteredText Blueprint State ---")
    centered_bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText"
    centered_bp = unreal.load_asset(centered_bp_path)
    if centered_bp:
        unreal.log_warning(f"  Loaded: {centered_bp.get_name()}")
        gen_class = centered_bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            unreal.log_warning(f"  Parent class: {parent.get_name() if parent else 'None'}")
        else:
            unreal.log_warning(f"  Could not get generated class")
    else:
        unreal.log_warning(f"  ERROR: Failed to load {centered_bp_path}")

    # 3. Check W_Settings (parent container) embedded widgets
    unreal.log_warning("\n--- 3. W_Settings Embedded Widgets ---")
    settings_bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    settings_bp = unreal.load_asset(settings_bp_path)
    if settings_bp:
        unreal.log_warning(f"  Loaded: {settings_bp.get_name()}")

        # Get widget tree
        widget_tree = settings_bp.widget_tree
        if widget_tree:
            unreal.log_warning(f"  WidgetTree: FOUND")

            # Get all widgets
            all_widgets = widget_tree.get_all_widgets()
            entry_count = 0
            correct_entry_count = 0

            for widget in all_widgets:
                widget_class_name = widget.get_class().get_name()
                if "W_Settings_Entry" in widget_class_name:
                    entry_count += 1
                    widget_name = widget.get_name()

                    # Check EntryType
                    entry_type = -1
                    setting_tag = ""
                    try:
                        entry_type = widget.get_editor_property("entry_type")
                        setting_tag_obj = widget.get_editor_property("setting_tag")
                        if setting_tag_obj:
                            setting_tag = str(setting_tag_obj)
                    except:
                        pass

                    # EntryType 0 = DropDown (correct), EntryType 3 = Slider (wrong)
                    is_correct = (entry_type == 0)  # ESLFSettingEntry::DropDown
                    if is_correct:
                        correct_entry_count += 1
                        status = "OK"
                    else:
                        status = f"WRONG (Type={entry_type}, expected 0)"

                    if entry_count <= 5:  # Show first 5
                        unreal.log_warning(f"    {widget_name}: Type={entry_type}, Tag={setting_tag[:50]}... [{status}]")

            if entry_count > 5:
                unreal.log_warning(f"    ... and {entry_count - 5} more entries")

            unreal.log_warning(f"\n  Summary: {correct_entry_count}/{entry_count} entries have correct EntryType (DropDown)")

            if correct_entry_count < entry_count:
                unreal.log_warning(f"  ACTION NEEDED: Run 'apply_embedded_settings_widgets' to fix embedded widget data")
        else:
            unreal.log_warning(f"  WidgetTree: NOT FOUND")
    else:
        unreal.log_warning(f"  ERROR: Failed to load {settings_bp_path}")

    # 4. Verify bp_only cache exists and is complete
    unreal.log_warning("\n--- 4. Migration Cache Status ---")
    cache_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_bponly.json"
    try:
        with open(cache_path, 'r') as f:
            cache_data = json.load(f)
            entries = cache_data.get("Entries", [])
            categories = cache_data.get("Categories", [])
            unreal.log_warning(f"  Cache file: FOUND")
            unreal.log_warning(f"  Entries in cache: {len(entries)}")
            unreal.log_warning(f"  Categories in cache: {len(categories)}")

            # Check if all entries have correct EntryType
            correct_in_cache = sum(1 for e in entries if e.get("EntryType") == 0)
            unreal.log_warning(f"  Entries with EntryType=0 (DropDown): {correct_in_cache}/{len(entries)}")

            if correct_in_cache == len(entries):
                unreal.log_warning(f"  Cache status: GOOD (all entries have correct EntryType)")
            else:
                unreal.log_warning(f"  Cache status: BAD (need to re-extract from bp_only)")
    except FileNotFoundError:
        unreal.log_warning(f"  Cache file: NOT FOUND at {cache_path}")
        unreal.log_warning(f"  ACTION NEEDED: Extract from bp_only project first")
    except Exception as e:
        unreal.log_warning(f"  ERROR reading cache: {e}")

    # 5. Test C++ CDO values
    unreal.log_warning("\n--- 5. C++ CDO Test ---")
    result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
    for line in result.split('\n'):
        if line.strip():
            unreal.log_warning(f"  {line}")

    # Summary and next steps
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("VALIDATION COMPLETE")
    unreal.log_warning("=" * 70)
    unreal.log_warning("\nTo fix settings displaying wrong values:")
    unreal.log_warning("1. Ensure C++ build is up to date")
    unreal.log_warning("2. Run: unreal.SLFAutomationLibrary.apply_embedded_settings_widgets('C:/scripts/SLFConversion/migration_cache/settings_embedded_bponly.json')")
    unreal.log_warning("3. Restart the Editor to ensure all changes take effect")
    unreal.log_warning("4. In PIE, open Settings menu and verify dropdown values show:")
    unreal.log_warning("   - Quality settings: Low, Medium, High, Ultra, Epic")
    unreal.log_warning("   - Screen Mode: Fullscreen, Windowed Fullscreen, Windowed")
    unreal.log_warning("   - Resolution: Actual screen resolutions like 1920x1080")
    unreal.log_warning("   - Camera invert: Yes/No")
    unreal.log_warning("   - Camera speed: Slow, Normal, Fast")
    unreal.log_warning("\n5. Run console command 'SLF.DiagnoseSettings' during PIE for runtime validation")

main()
