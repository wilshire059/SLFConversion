# check_bp_parent_v2.py
# Check what parent class B_RestingPoint actually has

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_RestingPoint PARENT CLASS")
    unreal.log_warning("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"
    bp = unreal.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Could not load Blueprint: {bp_path}")
        return

    # Get parent class directly from Blueprint
    try:
        parent = bp.get_editor_property('parent_class')
        if parent:
            unreal.log_warning(f"B_RestingPoint PARENT CLASS: {parent.get_name()}")
            unreal.log_warning(f"B_RestingPoint PARENT PATH: {parent.get_path_name()}")
        else:
            unreal.log_warning("B_RestingPoint PARENT CLASS: None")
    except Exception as e:
        unreal.log_error(f"Error getting parent_class: {e}")

    # Also check B_Container
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("CHECKING B_Container PARENT CLASS")
    unreal.log_warning("=" * 70)

    bp2_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp2 = unreal.load_asset(bp2_path)

    if bp2:
        try:
            parent2 = bp2.get_editor_property('parent_class')
            if parent2:
                unreal.log_warning(f"B_Container PARENT CLASS: {parent2.get_name()}")
                unreal.log_warning(f"B_Container PARENT PATH: {parent2.get_path_name()}")
            else:
                unreal.log_warning("B_Container PARENT CLASS: None")
        except Exception as e:
            unreal.log_error(f"Error getting parent_class: {e}")

    # Also check B_Interactable
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("CHECKING B_Interactable PARENT CLASS")
    unreal.log_warning("=" * 70)

    bp3_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    bp3 = unreal.load_asset(bp3_path)

    if bp3:
        try:
            parent3 = bp3.get_editor_property('parent_class')
            if parent3:
                unreal.log_warning(f"B_Interactable PARENT CLASS: {parent3.get_name()}")
                unreal.log_warning(f"B_Interactable PARENT PATH: {parent3.get_path_name()}")
            else:
                unreal.log_warning("B_Interactable PARENT CLASS: None")
        except Exception as e:
            unreal.log_error(f"Error getting parent_class: {e}")

if __name__ == "__main__":
    main()
