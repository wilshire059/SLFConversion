"""
Test that the AnimBP correctly loads TwoHanded animations from DA_ExampleAnimSetup
"""
import unreal

def log(msg):
    print(f"[Test] {msg}")
    unreal.log_warning(f"[Test] {msg}")

def main():
    log("=== Testing TwoHanded Animation Loading ===")

    # Check DA_ExampleAnimSetup has the animations
    anim_data_path = "/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleAnimSetup"
    anim_data = unreal.load_asset(anim_data_path)

    if not anim_data:
        log(f"[ERROR] Could not load {anim_data_path}")
        return

    log(f"Loaded AnimData: {anim_data.get_name()}")
    log(f"Class: {anim_data.get_class().get_name()}")

    # Check TwoHanded animations
    try:
        twohanded_right = anim_data.get_editor_property("two_handed_weapon_right")
        twohanded_left = anim_data.get_editor_property("two_handed_weapon_left")

        log(f"TwoHandedWeapon_Right: {twohanded_right.get_name() if twohanded_right else 'NULL'}")
        log(f"TwoHandedWeapon_Left: {twohanded_left.get_name() if twohanded_left else 'NULL'}")

        if twohanded_right and twohanded_left:
            log("[SUCCESS] TwoHanded animations are set correctly!")
        else:
            log("[ERROR] TwoHanded animations are NULL!")
    except Exception as e:
        log(f"[ERROR] Exception reading animations: {e}")

    # Also check PDA_AnimData to compare
    log("\n=== Checking PDA_AnimData for comparison ===")
    pda_path = "/Game/SoulslikeFramework/Data/_AnimationData/PDA_AnimData"
    pda_data = unreal.load_asset(pda_path)

    if pda_data:
        log(f"Loaded PDA_AnimData: {pda_data.get_name()}")
        log(f"Class: {pda_data.get_class().get_name()}")

        # PDA_AnimData might be a Blueprint - get its GeneratedClass CDO
        if hasattr(pda_data, 'generated_class'):
            gen_class = pda_data.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        pda_right = cdo.get_editor_property("two_handed_weapon_right")
                        pda_left = cdo.get_editor_property("two_handed_weapon_left")
                        log(f"PDA_AnimData CDO - TwoHandedWeapon_Right: {pda_right.get_name() if pda_right else 'NULL'}")
                        log(f"PDA_AnimData CDO - TwoHandedWeapon_Left: {pda_left.get_name() if pda_left else 'NULL'}")
                    except Exception as e:
                        log(f"Could not read PDA_AnimData CDO properties: {e}")
    else:
        log("Could not load PDA_AnimData")

    log("\n=== Test Complete ===")

if __name__ == "__main__":
    main()
