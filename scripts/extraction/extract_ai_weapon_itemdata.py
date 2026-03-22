"""
Extract Item Data references from AI weapon Blueprints.
"""

import unreal
import json
import re

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/ai_weapon_itemdata.json"
log_lines = []

def log(msg):
    print(msg)
    log_lines.append(str(msg))

log("=" * 70)
log("EXTRACTING AI WEAPON ITEM DATA REFERENCES")
log("=" * 70)

ai_weapon_paths = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
]

results = {}

for bp_path in ai_weapon_paths:
    bp_name = bp_path.split('/')[-1]
    log(f"\n--- {bp_name} ---")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"  NOT FOUND")
        continue

    try:
        # Export as text to find ItemData reference
        export_path = f"C:/scripts/SLFConversion/migration_cache/{bp_name}_export.txt"
        unreal.BlueprintEditorLibrary.export_blueprint_as_text(bp, export_path)

        # Read and search for ItemData / Item Data reference
        with open(export_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        # Look for Item Data or ItemData property with ObjectProperty reference
        patterns = [
            r'Item Data.*?ObjectProperty.*?(/Game/[^"\']+)',
            r'ItemData.*?ObjectProperty.*?(/Game/[^"\']+)',
            r'"Item Data"[^}]*?"DefaultValue":\s*"([^"]+)"',
            r'PDA_Item.*?(/Game/SoulslikeFramework/Data/Items/[^"\'\.]+)',
        ]

        found = False
        for pattern in patterns:
            matches = re.findall(pattern, content, re.IGNORECASE | re.DOTALL)
            for match in matches:
                if 'DA_' in match or 'PDA_' in match:
                    log(f"  Item Data: {match}")
                    results[bp_name] = match
                    found = True
                    break
            if found:
                break

        if not found:
            # Try to find any DA_ reference
            da_matches = re.findall(r'/Game/SoulslikeFramework/Data/Items/(DA_[^"\'\.]+)', content)
            if da_matches:
                log(f"  Item Data (from content): {da_matches[0]}")
                results[bp_name] = f"/Game/SoulslikeFramework/Data/Items/{da_matches[0]}"
            else:
                log(f"  No Item Data reference found")

    except Exception as e:
        log(f"  Error: {e}")

# Save results
with open(OUTPUT_FILE, 'w') as f:
    json.dump(results, f, indent=2)

log(f"\nResults saved to {OUTPUT_FILE}")
log(f"\nSummary:")
for bp, item_data in results.items():
    log(f"  {bp} -> {item_data}")
