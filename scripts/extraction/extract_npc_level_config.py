"""
Extract NPC level instance configuration from bp_only.
Gets DialogAsset, Name, VendorAsset from NPC instances in the demo level.
"""
import unreal
import json
import os

OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/npc_level_config.json"

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("EXTRACTING NPC LEVEL INSTANCE CONFIG FROM BP_ONLY")
    unreal.log_warning("="*70 + "\n")
    
    data = {"npcs": []}
    
    # Get all actors of type that contain "NPC" in their class name
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(unreal.EditorLevelLibrary.get_editor_world(), unreal.Actor)
    
    for actor in all_actors:
        class_name = actor.get_class().get_name()
        
        # Check if it's an NPC
        if "NPC" in class_name or "Soulslike_NPC" in class_name:
            actor_name = actor.get_name()
            actor_label = actor.get_actor_label()
            
            npc_data = {
                "actor_name": actor_name,
                "actor_label": actor_label,
                "class_name": class_name,
                "location": str(actor.get_actor_location()),
                "components": {}
            }
            
            unreal.log_warning(f"\nFound NPC: {actor_label} ({class_name})")
            
            # Get all components
            components = actor.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                
                # Look for InteractionManager component
                if "InteractionManager" in comp_name or "InteractionManager" in comp_class:
                    unreal.log_warning(f"  Found component: {comp_name} ({comp_class})")
                    
                    comp_data = {}
                    
                    # Try to get properties
                    try:
                        # Name property
                        try:
                            name_val = comp.get_editor_property('name')
                            if name_val:
                                comp_data['name'] = str(name_val)
                                unreal.log_warning(f"    Name: {name_val}")
                        except:
                            unreal.log_warning(f"    Name: (not found)")
                            
                        # DialogAsset property
                        try:
                            dialog = comp.get_editor_property('dialog_asset')
                            if dialog:
                                comp_data['dialog_asset'] = dialog.get_path_name()
                                unreal.log_warning(f"    DialogAsset: {dialog.get_path_name()}")
                            else:
                                unreal.log_warning(f"    DialogAsset: None")
                        except Exception as e:
                            unreal.log_warning(f"    DialogAsset: (error: {e})")
                            
                        # VendorAsset property  
                        try:
                            vendor = comp.get_editor_property('vendor_asset')
                            if vendor:
                                comp_data['vendor_asset'] = vendor.get_path_name()
                                unreal.log_warning(f"    VendorAsset: {vendor.get_path_name()}")
                        except:
                            pass
                            
                    except Exception as e:
                        unreal.log_warning(f"    Error getting props: {e}")
                    
                    npc_data["components"]["interaction_manager"] = comp_data
            
            data["npcs"].append(npc_data)
    
    unreal.log_warning(f"\nFound {len(data['npcs'])} NPCs total")
    
    # Save
    os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
    with open(OUTPUT_PATH, 'w') as f:
        json.dump(data, f, indent=2)
    
    unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    unreal.log_warning("\n" + "="*70)

if __name__ == "__main__":
    main()
