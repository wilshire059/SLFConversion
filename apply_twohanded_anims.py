"""
Apply Two-Handed Pose Animations to PDA_AnimData

This script assigns the two-handed pose animations to the AnimDataAsset
so the AnimBP can display the correct idle pose when in two-hand stance.
"""

import unreal

def apply_twohanded_anims():
    """Apply two-handed animations to PDA_AnimData"""

    # Load the AnimData asset - DA_ExampleAnimSetup is what the AnimBP actually uses!
    # (PDA_AnimData exists but isn't assigned to the AnimBP)
    anim_data_path = "/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleAnimSetup"
    anim_data = unreal.load_asset(anim_data_path)

    if not anim_data:
        print(f"ERROR: Could not load {anim_data_path}")
        return False

    print(f"Loaded AnimData: {anim_data.get_name()}")

    # Load the two-handed pose animations
    twohanded_right_path = "/Game/SoulslikeFramework/Demo/_Animations/Combat/Poses/AS_SLF_2h_Pose_R"
    twohanded_left_path = "/Game/SoulslikeFramework/Demo/_Animations/Combat/Poses/AS_SLF_2h_Pose_L"

    twohanded_right = unreal.load_asset(twohanded_right_path)
    twohanded_left = unreal.load_asset(twohanded_left_path)

    if not twohanded_right:
        print(f"ERROR: Could not load {twohanded_right_path}")
        return False

    if not twohanded_left:
        print(f"ERROR: Could not load {twohanded_left_path}")
        return False

    print(f"Loaded TwoHandedWeapon_Right: {twohanded_right.get_name()}")
    print(f"Loaded TwoHandedWeapon_Left: {twohanded_left.get_name()}")

    # PDA_AnimData is a Blueprint data asset - we need to get the CDO of its generated class
    # First, check if this is a Blueprint asset
    bp_class = anim_data.get_class()
    print(f"Asset class: {bp_class.get_name()}")

    # For Blueprint data assets, we need to work with the generated class CDO
    if bp_class.get_name() == "Blueprint":
        # This is a Blueprint asset - need to get its generated class
        bp = unreal.load_asset(anim_data_path)
        gen_class = unreal.EditorAssetLibrary.find_asset_data(anim_data_path).get_asset().generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            print(f"Generated class: {gen_class.get_name()}")
            print(f"CDO: {cdo.get_name() if cdo else 'None'}")
            anim_data = cdo  # Use the CDO for property setting
        else:
            print("ERROR: Could not get generated class from Blueprint!")
            return False
    else:
        print(f"Asset is native class: {bp_class.get_name()}")

    # Verify current values before setting
    try:
        current_right = anim_data.get_editor_property("two_handed_weapon_right")
        current_left = anim_data.get_editor_property("two_handed_weapon_left")
        print(f"Current two_handed_weapon_right: {current_right.get_name() if current_right else 'None'}")
        print(f"Current two_handed_weapon_left: {current_left.get_name() if current_left else 'None'}")
    except Exception as e:
        print(f"Could not read current values: {e}")

    # Set the animations using Python snake_case naming convention
    # C++ UPROPERTY TwoHandedWeapon_Right -> Python two_handed_weapon_right
    try:
        anim_data.set_editor_property("two_handed_weapon_right", twohanded_right)
        print(f"SUCCESS: Set two_handed_weapon_right = {twohanded_right.get_name()}")
    except Exception as e:
        print(f"FAILED to set two_handed_weapon_right: {e}")
        return False

    try:
        anim_data.set_editor_property("two_handed_weapon_left", twohanded_left)
        print(f"SUCCESS: Set two_handed_weapon_left = {twohanded_left.get_name()}")
    except Exception as e:
        print(f"FAILED to set two_handed_weapon_left: {e}")
        return False

    # Save the asset
    save_result = unreal.EditorAssetLibrary.save_asset(anim_data_path)
    print(f"Saved {anim_data_path}: {save_result}")

    return True

if __name__ == "__main__":
    import os
    output_file = "C:/scripts/SLFConversion/migration_cache/twohanded_anim_result.txt"

    # Redirect output to file
    original_print = print
    output_lines = []

    def capture_print(*args, **kwargs):
        line = ' '.join(str(a) for a in args)
        output_lines.append(line)
        original_print(*args, **kwargs)

    import builtins
    builtins.print = capture_print

    success = apply_twohanded_anims()

    builtins.print = original_print

    # Write results to file
    with open(output_file, 'w') as f:
        f.write('\n'.join(output_lines))
        f.write(f"\n\nFINAL RESULT: {'SUCCESS' if success else 'FAILED'}\n")

    print(f"\nResult: {'SUCCESS' if success else 'FAILED'}")
    print(f"Output written to: {output_file}")
