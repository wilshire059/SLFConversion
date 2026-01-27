"""
Clear EventGraphs from chaos-related Blueprints after C++ migration.
"""
import unreal

def clear_eventgraph(bp_path):
    """Clear EventGraph from Blueprint using SLFAutomationLibrary."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False
    
    print(f"Clearing EventGraph for: {bp.get_name()}")
    
    try:
        result = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        print(f"  ClearEventGraphs returned: {result}")
        if result > 0:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  Saved")
            return True
        else:
            print(f"  No graphs cleared (may already be empty)")
            return True
    except Exception as e:
        print(f"  ERROR: {e}")
        return False

def main():
    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
    ]
    
    print("=== Clearing EventGraphs from Chaos Blueprints ===\n")
    
    for bp_path in blueprints:
        clear_eventgraph(bp_path)
        print()
    
    print("=== DONE ===")

if __name__ == "__main__":
    main()
