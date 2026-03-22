# check_throwing_knife.py
# Check DA_ThrowingKnife configuration

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/item_config_check.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(str(msg))
    log_lines.append(str(msg))

paths = [
    "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife.DA_ThrowingKnife",
    "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell.DA_MagicSpell",
    "/Game/SoulslikeFramework/Data/Items/DA_HealthFlask.DA_HealthFlask",
    "/Game/SoulslikeFramework/Data/Items/Spells/DA_FireballSpell.DA_FireballSpell",
]

for path in paths:
    log(f"\n{'='*60}")
    log(f"Checking: {path}")
    log('='*60)

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log(f"  ERROR: Could not load")
        continue

    item_info = asset.get_editor_property('item_information')
    if not item_info:
        log(f"  ERROR: No item_information")
        continue

    # Category
    category_data = item_info.get_editor_property('category')
    if category_data:
        cat = category_data.get_editor_property('category')
        log(f"  Category: {cat}")

    # Usable
    usable = item_info.get_editor_property('usable')
    log(f"  Usable: {usable}")

    # Action tag
    action_tag = item_info.get_editor_property('action_to_trigger')
    log(f"  Action tag: {action_tag}")

    # Try to get tag name
    if action_tag:
        try:
            tag_name = action_tag.get_editor_property('tag_name')
            log(f"  Tag name: {tag_name}")
        except Exception as e:
            log(f"  Tag name error: {e}")

        # List all properties
        log(f"  Tag dir: {[x for x in dir(action_tag) if not x.startswith('_')]}")

    # Item class
    item_class = item_info.get_editor_property('item_class')
    log(f"  ItemClass: {item_class}")

# Write log
with open(LOG_FILE, 'w', encoding='utf-8') as f:
    f.write('\n'.join(log_lines))

log(f"\nLog saved to: {LOG_FILE}")
