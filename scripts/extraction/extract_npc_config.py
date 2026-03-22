"""
Extract NPC configuration data from bp_only project.
This runs on bp_only to extract DialogAsset, Name, VendorAsset etc.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/npc_config.json"

NPC_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

COMPONENT_PATH = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager"

def extract_npc_data():
    data = {"npcs": {}, "component": {}}
    
    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\nExtracting: {bp_name}")
        
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load")
            continue
            
        npc_data = {}
        
        # Get generated class and CDO
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Try to get common NPC properties
                try:
                    # Check for AIInteractionManager component
                    components = cdo.get_components_by_class(unreal.ActorComponent)
                    for comp in components:
                        comp_name = comp.get_name()
                        unreal.log_warning(f"  Component: {comp_name} ({comp.get_class().get_name()})")
                        
                        # Look for AC_AI_InteractionManager
                        if "InteractionManager" in comp_name or "AI_Interaction" in comp_name:
                            npc_data["component_name"] = comp_name
                            
                            # Try to get properties from component
                            try:
                                if hasattr(comp, 'get_editor_property'):
                                    # Try Name property
                                    try:
                                        name_val = comp.get_editor_property('name')
                                        if name_val:
                                            npc_data["npc_name"] = str(name_val)
                                            unreal.log_warning(f"  Name: {name_val}")
                                    except:
                                        pass
                                    
                                    # Try DialogAsset property
                                    try:
                                        dialog = comp.get_editor_property('dialog_asset')
                                        if dialog:
                                            npc_data["dialog_asset"] = dialog.get_path_name()
                                            unreal.log_warning(f"  DialogAsset: {dialog.get_path_name()}")
                                    except:
                                        pass
                                    
                                    # Try VendorAsset property
                                    try:
                                        vendor = comp.get_editor_property('vendor_asset')
                                        if vendor:
                                            npc_data["vendor_asset"] = vendor.get_path_name()
                                            unreal.log_warning(f"  VendorAsset: {vendor.get_path_name()}")
                                    except:
                                        pass
                            except Exception as e:
                                unreal.log_warning(f"  Error getting component props: {e}")
                except Exception as e:
                    unreal.log_warning(f"  Error iterating components: {e}")
        
        data["npcs"][bp_name] = npc_data
    
    return data

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("EXTRACTING NPC CONFIGURATION FROM BP_ONLY")
    unreal.log_warning("="*70 + "\n")
    
    data = extract_npc_data()
    
    # Save to JSON
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(data, f, indent=2)
    
    unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    main()
