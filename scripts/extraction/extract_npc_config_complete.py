"""
Extract complete NPC configuration from bp_only Blueprints.
Gets DialogAsset, Name, VendorAsset from the SCS component templates.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/npc_complete_config.json"

NPC_BLUEPRINTS = {
    "B_Soulslike_NPC_ShowcaseGuide": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "B_Soulslike_NPC_ShowcaseVendor": "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
}

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("EXTRACTING COMPLETE NPC CONFIG FROM BP_ONLY")
    unreal.log_warning("="*70 + "\n")

    data = {"npcs": {}}

    for bp_name, bp_path in NPC_BLUEPRINTS.items():
        unreal.log_warning(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            continue

        npc_data = {
            "path": bp_path,
            "interaction_manager": {}
        }

        # Get the Blueprint's generated class
        gen_class = bp.generated_class()
        if gen_class:
            unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

            # Get CDO to check default values
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Try to get components from CDO
                components = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning(f"  CDO has {len(components)} components")

                for comp in components:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()
                    unreal.log_warning(f"    Component: {comp_name} ({comp_class})")

                    if "InteractionManager" in comp_name or "InteractionManager" in comp_class:
                        unreal.log_warning(f"    >> Found InteractionManager component!")

                        # Try various property names
                        property_names = [
                            'dialog_asset', 'DialogAsset',
                            'name', 'Name', 'npc_name', 'NPCName',
                            'vendor_asset', 'VendorAsset'
                        ]

                        for prop_name in property_names:
                            try:
                                val = comp.get_editor_property(prop_name)
                                if val is not None:
                                    if hasattr(val, 'get_path_name'):
                                        npc_data["interaction_manager"][prop_name] = val.get_path_name()
                                        unreal.log_warning(f"      {prop_name}: {val.get_path_name()}")
                                    else:
                                        npc_data["interaction_manager"][prop_name] = str(val)
                                        unreal.log_warning(f"      {prop_name}: {val}")
                            except Exception as e:
                                pass

        # Also check SCS (SimpleConstructionScript)
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_all_nodes()
            unreal.log_warning(f"  SCS has {len(all_nodes)} nodes")

            for node in all_nodes:
                template = node.component_template
                if template:
                    comp_name = template.get_name()
                    comp_class = template.get_class().get_name()

                    if "InteractionManager" in comp_name or "InteractionManager" in comp_class:
                        unreal.log_warning(f"    >> SCS InteractionManager template: {comp_name}")

                        # List all properties available
                        for prop in template.get_class().properties():
                            prop_name = prop.get_name()
                            try:
                                val = template.get_editor_property(prop_name)
                                if val is not None and str(val) != "None" and str(val) != "":
                                    if hasattr(val, 'get_path_name'):
                                        val_str = val.get_path_name()
                                    else:
                                        val_str = str(val)

                                    if val_str and val_str != "None":
                                        npc_data["interaction_manager"][prop_name] = val_str
                                        unreal.log_warning(f"        {prop_name}: {val_str}")
                            except:
                                pass

        data["npcs"][bp_name] = npc_data

    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(data, f, indent=2)

    unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    main()
