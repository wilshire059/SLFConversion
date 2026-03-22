"""
Parse backup .uasset files to extract widget instance property values.
This is a standalone Python script (not Unreal Python) that parses the binary.

Run this BEFORE migration to capture the original data.
"""
import os
import re
import json
from pathlib import Path

def extract_texture_refs(content_bytes: bytes) -> list:
    """Extract texture path references from uasset binary"""
    # Try to decode as UTF-16LE (UE's internal string format)
    results = []

    # Look for /Game/ paths followed by texture names
    # UE stores strings as null-terminated UTF-16LE in some places
    text_utf16 = content_bytes.decode('utf-16-le', errors='ignore')
    text_utf8 = content_bytes.decode('utf-8', errors='ignore')

    # Pattern for game asset paths
    patterns = [
        r'/Game/SoulslikeFramework/Widgets/_Textures/[A-Za-z0-9_]+',
        r'/Game/SoulslikeFramework/[^/\x00]+/[^/\x00]+/T_[A-Za-z0-9_]+',
    ]

    for text in [text_utf8, text_utf16]:
        for pattern in patterns:
            matches = re.findall(pattern, text)
            for m in matches:
                if m not in results and 'T_' in m:
                    results.append(m)

    return results

def extract_category_data(content_bytes: bytes) -> dict:
    """Extract category-related data from W_Inventory or W_Inventory_CategoryEntry"""
    data = {}

    # Find texture references
    textures = extract_texture_refs(content_bytes)
    data['textures'] = textures

    # Look for enum values (E_ItemCategory)
    text_utf8 = content_bytes.decode('utf-8', errors='ignore')

    # Common category enum values
    category_patterns = [
        'E_ItemCategory',
        'AllItems',
        'Weapons',
        'Armor',
        'Consumables',
        'KeyItems',
        'Materials',
    ]

    found_categories = []
    for cat in category_patterns:
        if cat in text_utf8:
            found_categories.append(cat)
    data['categories'] = found_categories

    return data

def analyze_widget_uasset(filepath: str) -> dict:
    """Analyze a widget .uasset file"""
    print(f"\nAnalyzing: {filepath}")

    with open(filepath, 'rb') as f:
        content = f.read()

    print(f"  File size: {len(content)} bytes")

    result = {
        'path': filepath,
        'size': len(content),
        'data': extract_category_data(content)
    }

    print(f"  Textures found: {result['data']['textures']}")
    print(f"  Categories found: {result['data']['categories']}")

    return result

def main():
    backup_base = Path('C:/scripts/bp_only/Content/SoulslikeFramework/Widgets')

    # Files to analyze
    widget_files = [
        backup_base / 'Inventory/W_Inventory.uasset',
        backup_base / 'Inventory/W_Inventory_CategoryEntry.uasset',
    ]

    all_data = {}

    for filepath in widget_files:
        if filepath.exists():
            name = filepath.stem
            all_data[name] = analyze_widget_uasset(str(filepath))
        else:
            print(f"WARNING: {filepath} not found")

    # Save to JSON for use by migration script
    output_path = 'C:/scripts/slfconversion/backup_widget_data.json'
    with open(output_path, 'w') as f:
        json.dump(all_data, f, indent=2)
    print(f"\nSaved extracted data to: {output_path}")

    # Print summary
    print("\n" + "="*80)
    print("SUMMARY - Widget Instance Data from Backup")
    print("="*80)

    for name, data in all_data.items():
        print(f"\n{name}:")
        for tex in data['data']['textures']:
            print(f"  Texture: {tex}")

if __name__ == "__main__":
    main()
