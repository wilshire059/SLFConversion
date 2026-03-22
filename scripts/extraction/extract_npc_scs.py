"""
Extract NPC SCS component configuration from bp_only Blueprints.
Looks at the SimpleConstructionScript to find AC_AI_InteractionManager settings.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/npc_scs_config.json"

NPC_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("EXTRACTING NPC SCS CONFIG FROM BP_ONLY")
    unreal.log_warning("="*70 + "\n")
    
    data = {"blueprints": {}}
    
    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")
        
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load")
            continue
        
        bp_data = {"scs_components": []}
        
        # Get SimpleConstructionScript
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            # Get all nodes
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                template = node.component_template
                if template:
                    comp_name = template.get_name()
                    comp_class = template.get_class().get_name()
                    
                    comp_data = {
                        "name": comp_name,
                        "class": comp_class,
                    }
                    
                    unreal.log_warning(f"  SCS Component: {comp_name} ({comp_class})")
                    
                    # If it's an InteractionManager, get its properties
                    if "InteractionManager" in comp_class or "InteractionManager" in comp_name:
                        try:
                            # Name
                            try:
                                n = template.get_editor_property('name')
                                if n:
                                    comp_data['npc_name'] = str(n)
                                    unreal.log_warning(f"    Name: {n}")
                            except:
                                pass
                            
                            # DialogAsset
                            try:
                                d = template.get_editor_property('dialog_asset')
                                if d:
                                    comp_data['dialog_asset'] = d.get_path_name()
                                    unreal.log_warning(f"    DialogAsset: {d.get_path_name()}")
                            except:
                                pass
                            
                            # VendorAsset
                            try:
                                v = template.get_editor_property('vendor_asset')
                                if v:
                                    comp_data['vendor_asset'] = v.get_path_name()
                                    unreal.log_warning(f"    VendorAsset: {v.get_path_name()}")
                            except:
                                pass
                                
                        except Exception as e:
                            unreal.log_warning(f"    Error: {e}")
                    
                    bp_data["scs_components"].append(comp_data)
        
        data["blueprints"][bp_name] = bp_data
    
    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(data, f, indent=2)
    
    unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    main()
