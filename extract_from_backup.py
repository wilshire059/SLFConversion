"""
Extract widget instance properties from BACKUP W_Inventory Blueprint
Uses strings/text search on the backup .uasset file
"""
import unreal
import os
import re

def log(msg):
    unreal.log_warning(str(msg))

def extract_from_backup():
    log("=" * 80)
    log("EXTRACTING FROM BACKUP CONTENT")
    log("=" * 80)

    # Check backup path
    backup_path = 'C:/scripts/bp_only/Content/SoulslikeFramework/Widgets/Inventory/W_Inventory.uasset'

    if not os.path.exists(backup_path):
        log(f"ERROR: Backup not found at {backup_path}")
        return

    log(f"Reading backup: {backup_path}")

    # Read the binary file and look for text patterns
    with open(backup_path, 'rb') as f:
        content = f.read()

    log(f"File size: {len(content)} bytes")

    # Convert to string for pattern searching (may have some garbled chars)
    try:
        text = content.decode('utf-8', errors='ignore')
    except:
        text = content.decode('latin-1', errors='ignore')

    # Search for key patterns
    patterns = [
        'T_Category',
        'AllItemsCategoryEntry',
        'InventoryCategoryData',
        'CategoryIcon',
        'AllCategoriesButton',
    ]

    log("Searching for patterns...")
    for pattern in patterns:
        matches = [(m.start(), m.end()) for m in re.finditer(pattern, text)]
        log(f"  '{pattern}': {len(matches)} matches")

        # Show context around first few matches
        for i, (start, end) in enumerate(matches[:3]):
            context_start = max(0, start - 50)
            context_end = min(len(text), end + 100)
            context = text[context_start:context_end]
            # Clean up non-printable chars
            context = ''.join(c if c.isprintable() or c in '\n\t' else '.' for c in context)
            log(f"    Match {i}: ...{context}...")

    # Also look for texture paths
    log("=" * 80)
    log("SEARCHING FOR TEXTURE PATHS")
    log("=" * 80)

    texture_pattern = r'/Game/[^\x00]+?\.T_[^\x00]+?'
    texture_matches = re.findall(texture_pattern, text)
    for t in set(texture_matches):
        if 'Category' in t or 'Controller' in t:
            log(f"  Texture: {t}")

    # Look for the AllItems category specifically
    log("=" * 80)
    log("LOOKING FOR AllItems CATEGORY DATA")
    log("=" * 80)

    # Search for pattern near AllItemsCategoryEntry
    all_items_pos = text.find('AllItemsCategoryEntry')
    if all_items_pos >= 0:
        log(f"Found AllItemsCategoryEntry at position {all_items_pos}")
        # Get surrounding context (500 chars each way)
        context = text[max(0, all_items_pos-200):all_items_pos+500]
        context = ''.join(c if c.isprintable() or c in '\n\t' else '.' for c in context)
        log(f"Context around AllItemsCategoryEntry:")
        log(context)

    # Also check the W_Inventory_CategoryEntry backup
    log("=" * 80)
    log("CHECKING W_Inventory_CategoryEntry BACKUP")
    log("=" * 80)

    cat_backup_path = 'C:/scripts/bp_only/Content/SoulslikeFramework/Widgets/Inventory/W_Inventory_CategoryEntry.uasset'
    if os.path.exists(cat_backup_path):
        with open(cat_backup_path, 'rb') as f:
            cat_content = f.read()

        try:
            cat_text = cat_content.decode('utf-8', errors='ignore')
        except:
            cat_text = cat_content.decode('latin-1', errors='ignore')

        log(f"W_Inventory_CategoryEntry size: {len(cat_content)} bytes")

        # Look for texture references
        for pattern in ['T_Category', 'CategoryIcon', 'InventoryCategoryData']:
            matches = list(re.finditer(pattern, cat_text))
            log(f"  '{pattern}': {len(matches)} matches")

if __name__ == "__main__":
    extract_from_backup()
