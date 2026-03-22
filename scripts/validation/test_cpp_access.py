# test_cpp_access.py
# Test that C++ UPDA_Item properties are accessible and contain data

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("TESTING C++ UPDA_Item PROPERTY ACCESS")
lines.append("=" * 60)

test_items = [
    "DA_Apple",
    "DA_BossMace",
    "DA_Katana",
    "DA_HealthFlask",
    "DA_ThrowingKnife"
]

for item_name in test_items:
    item_path = f"/Game/SoulslikeFramework/Data/Items/{item_name}"
    lines.append(f"\n{item_name}:")

    try:
        item = unreal.EditorAssetLibrary.load_asset(item_path)
        if not item:
            lines.append("  SKIP - Asset not found")
            continue

        # Check class hierarchy
        item_class = item.get_class()
        lines.append(f"  Class: {item_class.get_name()}")

        # Try to access C++ property
        try:
            niagara = item.get_editor_property('world_niagara_system')
            if niagara:
                lines.append(f"  WorldNiagaraSystem: {niagara.get_name()}")
            else:
                lines.append(f"  WorldNiagaraSystem: None (not set)")
        except Exception as e:
            lines.append(f"  WorldNiagaraSystem: ERROR - {e}")

        # Also check if ItemInformation still exists (Blueprint property)
        try:
            item_info = item.get_editor_property('ItemInformation')
            lines.append(f"  ItemInformation: EXISTS (Blueprint struct)")
        except:
            lines.append(f"  ItemInformation: NOT FOUND (might be expected)")

    except Exception as e:
        lines.append(f"  ERROR: {e}")

# Summary
lines.append("\n" + "=" * 60)
lines.append("SUMMARY")
lines.append("=" * 60)
lines.append("\nIf WorldNiagaraSystem shows a valid NiagaraSystem name,")
lines.append("the migration was successful and Niagara effects should")
lines.append("be visible in PIE.")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
