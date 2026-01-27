import unreal
import os
import json

output_file = "C:/scripts/SLFConversion/migration_cache/bp_only_icons.json"
os.makedirs(os.path.dirname(output_file), exist_ok=True)

results = {}

# Export W_Settings to text to see per-instance values
settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"

try:
    text = unreal.EditorAssetLibrary.export_text(settings_path)
    if text:
        # Save the full export
        with open("C:/scripts/SLFConversion/migration_cache/bp_only_w_settings_export.txt", "w", encoding='utf-8') as f:
            f.write(text)

        # Look for Icon references in the export
        lines = text.split('\n')
        icon_lines = []
        for i, line in enumerate(lines):
            # Look for Icon property or texture references related to category entries
            if 'CategoryEntry' in line or ('Icon' in line and ('T_' in line or 'Texture' in line or 'SoftObjectPath' in line)):
                icon_lines.append(f"Line {i}: {line.strip()[:300]}")

        results["export_lines"] = len(lines)
        results["icon_related_lines"] = icon_lines[:50]  # First 50 relevant lines

        print(f"Exported W_Settings ({len(lines)} lines)")
        print(f"Found {len(icon_lines)} icon-related lines")
except Exception as e:
    print(f"Export error: {e}")
    results["error"] = str(e)

with open(output_file, "w") as f:
    json.dump(results, f, indent=2)

print(f"Written to {output_file}")
