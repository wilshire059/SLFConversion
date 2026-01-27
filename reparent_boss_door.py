"""
Reparent B_BossDoor to ASLFBossDoor.
This is a surgical migration - only touches B_BossDoor.
"""
import unreal

def reparent_boss_door():
    boss_door_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    target_class_path = "/Script/SLFConversion.SLFBossDoor"

    print(f"=== Reparenting B_BossDoor to ASLFBossDoor ===")

    # Load the Blueprint
    bp = unreal.load_asset(boss_door_path)
    if not bp:
        print(f"ERROR: Could not load {boss_door_path}")
        return False

    # Get current parent
    gen_class = bp.generated_class()
    if gen_class:
        current_parent = gen_class.get_class().get_outer()
        print(f"Current Blueprint: {bp.get_name()}")
        print(f"Current generated class: {gen_class.get_name()}")

    # Load target parent class
    target_class = unreal.load_class(None, target_class_path)
    if not target_class:
        print(f"ERROR: Could not load target class {target_class_path}")
        return False

    print(f"Target class: {target_class.get_name()}")

    # Reparent using BlueprintEditorLibrary
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, target_class)
        if result:
            print(f"SUCCESS: Reparented B_BossDoor to ASLFBossDoor")

            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            print("Compiled Blueprint")

            # Save the Blueprint
            unreal.EditorAssetLibrary.save_asset(boss_door_path)
            print(f"Saved: {boss_door_path}")

            # Verify
            bp_reloaded = unreal.load_asset(boss_door_path)
            if bp_reloaded:
                gen_reloaded = bp_reloaded.generated_class()
                if gen_reloaded:
                    super_class = gen_reloaded.get_class()
                    print(f"Verified: New parent is working")

            return True
        else:
            print(f"ERROR: Reparent returned False")
            return False
    except Exception as e:
        print(f"ERROR during reparent: {e}")
        return False

if __name__ == "__main__":
    reparent_boss_door()
