"""
Scan backup widgets for texture/data that needs migration
"""
import os
import re
from pathlib import Path

def extract_textures(content_bytes):
    """Extract texture refs from uasset binary"""
    text = content_bytes.decode('utf-8', errors='ignore')

    patterns = [
        r'/Game/SoulslikeFramework/Widgets/_Textures/T_[A-Za-z0-9_]+',
        r'/Game/SoulslikeFramework/Textures/[A-Za-z0-9_/]+/T_[A-Za-z0-9_]+',
    ]

    results = set()
    for pattern in patterns:
        matches = re.findall(pattern, text)
        results.update(matches)

    return list(results)

def main():
    backup_widgets = Path('C:/scripts/bp_only/Content/SoulslikeFramework/Widgets')

    if not backup_widgets.exists():
        print(f"Backup path not found: {backup_widgets}")
        return

    print("="*80)
    print("SCANNING BACKUP WIDGETS FOR DATA MIGRATION")
    print("="*80)

    # Find all uasset files
    widget_files = list(backup_widgets.rglob('*.uasset'))
    print(f"\nFound {len(widget_files)} widget files")

    # Check each widget for textures
    widgets_with_textures = {}

    for filepath in widget_files:
        with open(filepath, 'rb') as f:
            content = f.read()

        textures = extract_textures(content)

        # Filter for category/icon textures (not default UI textures)
        special_textures = [t for t in textures if 'T_Category' in t or 'T_Icon' in t]

        if special_textures:
            rel_path = filepath.relative_to(backup_widgets)
            widgets_with_textures[str(rel_path)] = special_textures

    print("\n" + "="*80)
    print("WIDGETS WITH CATEGORY/ICON TEXTURES")
    print("="*80)

    for widget, textures in sorted(widgets_with_textures.items()):
        print(f"\n{widget}:")
        for t in textures:
            print(f"  {t}")

    # Count unique textures
    all_textures = set()
    for textures in widgets_with_textures.values():
        all_textures.update(textures)

    print("\n" + "="*80)
    print(f"UNIQUE CATEGORY/ICON TEXTURES ({len(all_textures)})")
    print("="*80)
    for t in sorted(all_textures):
        print(f"  {t}")

if __name__ == "__main__":
    main()
