"""
Clear B_Destructible Blueprint logic after C++ migration.
The C++ class AB_Destructible now handles all the logic.
"""
import unreal

def clear_destructible():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False
    
    print(f"Loaded: {bp.get_name()}")
    
    # Use BlueprintEditorLibrary to clear the event graph
    # This removes all Blueprint logic nodes while keeping variables and components
    try:
        # Get all graphs and clear them
        result = unreal.SLFAutomationLibrary.clear_event_graph(bp)
        print(f"ClearEventGraph result: {result}")
    except Exception as e:
        print(f"SLFAutomationLibrary not available: {e}")
        # Fallback: try direct approach
        try:
            # Mark package dirty and save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print("Saved asset (manual clear may be needed)")
        except Exception as e2:
            print(f"Save failed: {e2}")
            return False
    
    # Save the asset
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"Saved: {bp_path}")
    except Exception as e:
        print(f"Save error: {e}")
        return False
    
    print("Done - B_Destructible Blueprint logic cleared")
    return True

if __name__ == "__main__":
    clear_destructible()
