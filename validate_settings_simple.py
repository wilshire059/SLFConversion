"""
Simple Settings Widget Validation using C++ functions
"""
import unreal
import json

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("SETTINGS WIDGET VALIDATION")
    unreal.log_warning("=" * 70)

    # 1. Run the C++ diagnostic
    unreal.log_warning("\n--- 1. C++ Diagnostic ---")
    result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
    unreal.log_warning(result)

    # 2. Check migration cache
    unreal.log_warning("\n--- 2. Migration Cache Status ---")
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
                unreal.log_warning(f"  Cache status: GOOD")
            else:
                unreal.log_warning(f"  Cache status: BAD")
    except FileNotFoundError:
        unreal.log_warning(f"  Cache file: NOT FOUND")
    except Exception as e:
        unreal.log_warning(f"  ERROR: {e}")

    # 3. Extract current state for comparison
    unreal.log_warning("\n--- 3. Current Embedded Widget State ---")
    current_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_current.json"
    extract_result = unreal.SLFAutomationLibrary.extract_embedded_settings_widgets(current_path)

    # Count entries in current state
    try:
        with open(current_path, 'r') as f:
            current_data = json.load(f)
            current_entries = current_data.get("Entries", [])
            correct_current = sum(1 for e in current_entries if e.get("EntryType") == 0)
            unreal.log_warning(f"  Current entries with EntryType=0 (DropDown): {correct_current}/{len(current_entries)}")

            # Show summary of entry types
            type_counts = {}
            for e in current_entries:
                t = e.get("EntryType", -1)
                type_counts[t] = type_counts.get(t, 0) + 1

            for t, count in sorted(type_counts.items()):
                type_name = {0: "DropDown", 1: "SingleButton", 2: "DoubleButton", 3: "Slider", 4: "InputKeySelector"}.get(t, f"Unknown({t})")
                unreal.log_warning(f"    Type {t} ({type_name}): {count} entries")
    except Exception as e:
        unreal.log_warning(f"  ERROR: {e}")

    # Summary
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("NEXT STEPS:")
    unreal.log_warning("=" * 70)
    unreal.log_warning("1. If EntryType is not 0 (DropDown) for all entries, run:")
    unreal.log_warning("   unreal.SLFAutomationLibrary.apply_embedded_settings_widgets('C:/scripts/SLFConversion/migration_cache/settings_embedded_bponly.json')")
    unreal.log_warning("2. RESTART the Editor after applying changes")
    unreal.log_warning("3. In PIE, open Settings menu and run: SLF.DiagnoseSettings")
    unreal.log_warning("4. Dropdown values should show:")
    unreal.log_warning("   - Quality: Low, Medium, High, Ultra, Epic")
    unreal.log_warning("   - Screen Mode: Fullscreen, Windowed Fullscreen, Windowed")
    unreal.log_warning("   - Resolution: Actual resolutions like 1920x1080")

main()
