"""
Extract NPC configuration from bp_only using text export.
"""
import unreal
import json
import os
import re

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/npc_text_config.json"

NPC_BLUEPRINTS = {
    "B_Soulslike_NPC_ShowcaseGuide": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
}

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("EXTRACTING NPC CONFIG VIA TEXT EXPORT")
    unreal.log_warning("="*70 + "\n")

    data = {"npcs": {}}

    for bp_name, bp_path in NPC_BLUEPRINTS.items():
        unreal.log_warning(f"\n=== {bp_name} ===")

        npc_data = {
            "path": bp_path,
            "dialog_asset": None,
            "vendor_asset": None,
            "npc_name": None
        }

        # Export Blueprint text
        export_text = unreal.EditorAssetLibrary.export_text(bp_path)
        if not export_text:
            unreal.log_warning(f"  [FAIL] Could not export text")
            data["npcs"][bp_name] = npc_data
            continue

        unreal.log_warning(f"  Export text length: {len(export_text)} chars")

        # Save export text to file for inspection
        text_path = f"C:/scripts/SLFConversion/migration_cache/npc_export_{bp_name}.txt"
        with open(text_path, 'w', encoding='utf-8') as f:
            f.write(export_text)
        unreal.log_warning(f"  Saved export to: {text_path}")

        # Parse for DialogAsset
        dialog_match = re.search(r'DialogAsset\s*=\s*(.*?)[\r\n]', export_text)
        if dialog_match:
            npc_data["dialog_asset"] = dialog_match.group(1).strip()
            unreal.log_warning(f"  DialogAsset: {npc_data['dialog_asset']}")

        # Parse for VendorAsset
        vendor_match = re.search(r'VendorAsset\s*=\s*(.*?)[\r\n]', export_text)
        if vendor_match:
            npc_data["vendor_asset"] = vendor_match.group(1).strip()
            unreal.log_warning(f"  VendorAsset: {npc_data['vendor_asset']}")

        # Parse for Name property (might be in InteractionManager component)
        name_match = re.search(r'AC_AI_InteractionManager.*?Name\s*=\s*"([^"]*)"', export_text, re.DOTALL)
        if name_match:
            npc_data["npc_name"] = name_match.group(1).strip()
            unreal.log_warning(f"  Name: {npc_data['npc_name']}")
        else:
            # Try alternative pattern
            name_match2 = re.search(r'NPCName\s*=\s*"([^"]*)"', export_text)
            if name_match2:
                npc_data["npc_name"] = name_match2.group(1).strip()
                unreal.log_warning(f"  NPCName: {npc_data['npc_name']}")

        # Look for InteractionManager component block
        im_match = re.search(r'Begin Object.*?Class=.*?InteractionManager.*?End Object', export_text, re.DOTALL)
        if im_match:
            im_block = im_match.group(0)
            unreal.log_warning(f"  Found InteractionManager block: {len(im_block)} chars")

            # Extract properties from block
            for prop_match in re.finditer(r'(\w+)\s*=\s*([^\r\n]+)', im_block):
                prop_name = prop_match.group(1)
                prop_value = prop_match.group(2).strip()
                if prop_name.lower() in ['dialogasset', 'vendorasset', 'name']:
                    unreal.log_warning(f"    {prop_name} = {prop_value}")

        data["npcs"][bp_name] = npc_data

    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(data, f, indent=2)

    unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    main()
