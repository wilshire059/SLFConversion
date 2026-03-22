"""
Scan backup item data assets to extract ItemInformation values
"""
import os
import re
from pathlib import Path

def extract_strings(content_bytes):
    """Extract readable strings from uasset binary"""
    text = content_bytes.decode('utf-8', errors='ignore')
    return text

def main():
    backup_items = Path('C:/scripts/bp_only/Content/SoulslikeFramework/Data/Items')

    if not backup_items.exists():
        print(f"Backup path not found: {backup_items}")
        return

    print("="*80)
    print("SCANNING BACKUP ITEM DATA ASSETS")
    print("="*80)

    # Check a few item files
    items_to_check = [
        'DA_ThrowingKnife.uasset',
        'DA_HealthFlask.uasset',
        'DA_Sword01.uasset',
        'DA_Apple.uasset',
    ]

    for item_name in items_to_check:
        filepath = backup_items / item_name
        if not filepath.exists():
            print(f"\n{item_name}: NOT FOUND")
            continue

        print(f"\n{'='*60}")
        print(f"FILE: {item_name}")
        print(f"{'='*60}")

        with open(filepath, 'rb') as f:
            content = f.read()

        text = extract_strings(content)

        # Look for item-related patterns
        patterns = [
            r'DisplayName',
            r'ShortDescription',
            r'LongDescription',
            r'IconSmall',
            r'IconLarge',
            r'T_[A-Za-z0-9_]+',  # Texture names
            r'Throwing[A-Za-z]*',
            r'Health[A-Za-z]*',
            r'Sword[A-Za-z]*',
            r'Apple[A-Za-z]*',
        ]

        for pattern in patterns:
            matches = set(re.findall(pattern, text))
            if matches:
                for m in list(matches)[:5]:  # First 5 matches
                    print(f"  Found: {m}")

        # Find texture paths
        tex_pattern = r'/Game/SoulslikeFramework/Textures/[A-Za-z0-9_/]+'
        tex_matches = set(re.findall(tex_pattern, text))
        if tex_matches:
            print(f"\n  Texture paths:")
            for t in tex_matches:
                print(f"    {t}")

        # Look for readable text that might be item descriptions
        # Search for patterns that look like item descriptions (capitalized words)
        desc_pattern = r'[A-Z][a-z]+ [a-z]+ [a-z]+'
        desc_matches = set(re.findall(desc_pattern, text))
        if desc_matches:
            print(f"\n  Possible descriptions:")
            for d in list(desc_matches)[:5]:
                print(f"    '{d}'")

if __name__ == "__main__":
    main()
