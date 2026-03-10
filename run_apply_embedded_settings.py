"""
Apply Embedded Settings Widgets from JSON cache
"""
import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("APPLYING EMBEDDED SETTINGS WIDGETS")
    unreal.log_warning("=" * 70)

    cache_path = "C:/scripts/SLFConversion/migration_cache/settings_embedded_bponly.json"

    result = unreal.SLFAutomationLibrary.apply_embedded_settings_widgets(cache_path)

    # Log each line
    for line in result.split('\n'):
        if line.strip():
            unreal.log_warning(line)

    unreal.log_warning("=" * 70)
    unreal.log_warning("DONE - RESTART EDITOR TO SEE CHANGES")
    unreal.log_warning("=" * 70)

main()
