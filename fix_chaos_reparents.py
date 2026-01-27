"""
Fix ANS_ToggleChaosField and B_Destructible Blueprint reparenting.
These need to be reparented to their C++ classes for the chaos destruction to work.
"""
import unreal

def reparent_blueprint(bp_path, new_parent_path):
    """Reparent a Blueprint to a new parent class."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False
    
    # Load new parent class
    new_parent = unreal.load_class(None, new_parent_path)
    if not new_parent:
        print(f"ERROR: Could not load parent class {new_parent_path}")
        return False
    
    print(f"Reparenting {bp.get_name()} to {new_parent_path}")
    
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
        if result:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  SUCCESS: Reparented and saved")
            return True
        else:
            print(f"  FAILED: Reparent returned false")
            return False
    except Exception as e:
        print(f"  ERROR: {e}")
        return False

def clear_blueprint_logic(bp_path):
    """Clear EventGraph logic from Blueprint."""
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False
    
    try:
        result = unreal.SLFAutomationLibrary.clear_event_graph(bp)
        if result:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  Cleared EventGraph for {bp.get_name()}")
            return True
    except Exception as e:
        print(f"  SLFAutomationLibrary not available: {e}")
    
    return False

def main():
    success_count = 0
    fail_count = 0
    
    # Reparent ANS_ToggleChaosField to C++ class
    print("\n=== Reparenting ANS_ToggleChaosField ===")
    if reparent_blueprint(
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField",
        "/Script/SLFConversion.ANS_ToggleChaosField"
    ):
        # Clear Blueprint logic since C++ now handles it
        clear_blueprint_logic("/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField")
        success_count += 1
    else:
        fail_count += 1
    
    # Reparent B_Destructible to C++ class
    print("\n=== Reparenting B_Destructible ===")
    if reparent_blueprint(
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible",
        "/Script/SLFConversion.B_Destructible"
    ):
        # Clear Blueprint logic since C++ now handles it
        clear_blueprint_logic("/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible")
        success_count += 1
    else:
        fail_count += 1
    
    # Reparent B_Chaos_ForceField to C++ class (if not already)
    print("\n=== Checking B_Chaos_ForceField ===")
    if reparent_blueprint(
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField",
        "/Script/SLFConversion.B_Chaos_ForceField"
    ):
        clear_blueprint_logic("/Game/SoulslikeFramework/Blueprints/_WorldActors/B_Chaos_ForceField")
        success_count += 1
    else:
        fail_count += 1
    
    print(f"\n=== DONE: {success_count} succeeded, {fail_count} failed ===")

if __name__ == "__main__":
    main()
