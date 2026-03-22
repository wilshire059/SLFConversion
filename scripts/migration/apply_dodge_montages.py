# apply_dodge_montages.py - Set dodge montages on B_Action_Dodge Blueprint

import unreal

def apply_dodge_montages():
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING DODGE MONTAGES")
    unreal.log_warning("=" * 60)

    # Load the B_Action_Dodge Blueprint
    bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action_Dodge"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_warning(f"Could not load {bp_path}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()} (Class: {bp.get_class().get_name()})")

    # Get the generated class and CDO
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("Could not get generated class")
        return

    unreal.log_warning(f"Generated class: {gen_class.get_name()}")

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("Could not get CDO")
        return

    unreal.log_warning(f"CDO: {cdo.get_name()} (Class: {cdo.get_class().get_name()})")

    # Montage paths
    montage_base = "/Game/SoulslikeFramework/Demo/_Animations/Dodges"
    montage_map = {
        'forward': f'{montage_base}/AM_SLF_Dodge_F',
        'forward_left': f'{montage_base}/AM_SLF_Dodge_FL',
        'forward_right': f'{montage_base}/AM_SLF_Dodge_FR',
        'left': f'{montage_base}/AM_SLF_Dodge_L',
        'right': f'{montage_base}/AM_SLF_Dodge_R',
        'backward': f'{montage_base}/AM_SLF_Dodge_B',
        'backward_left': f'{montage_base}/AM_SLF_Dodge_BL',
        'backward_right': f'{montage_base}/AM_SLF_Dodge_BR',
    }

    # Try to get and modify DodgeMontages struct
    try:
        dodge_montages = cdo.get_editor_property('dodge_montages')
        unreal.log_warning(f"Got DodgeMontages: {dodge_montages}")

        if dodge_montages:
            for field, path in montage_map.items():
                montage = unreal.EditorAssetLibrary.load_asset(path)
                if montage:
                    try:
                        dodge_montages.set_editor_property(field, montage)
                        unreal.log_warning(f"  [OK] Set {field}: {montage.get_name()}")
                    except Exception as e:
                        unreal.log_warning(f"  [ERROR] {field}: {e}")
                else:
                    unreal.log_warning(f"  [SKIP] Could not load: {path}")

            # Set modified struct back
            cdo.set_editor_property('dodge_montages', dodge_montages)
            unreal.log_warning("Set DodgeMontages on CDO")

            # Mark Blueprint as modified and save
            bp.modify()
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            unreal.log_warning(f"Saved {bp_path}")

    except Exception as e:
        unreal.log_warning(f"Error accessing DodgeMontages: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    apply_dodge_montages()
