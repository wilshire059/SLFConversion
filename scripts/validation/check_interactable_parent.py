# check_interactable_parent.py
# Check parent class of B_Interactable

import unreal

def check_parent():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    try:
        # Check parent class property
        parent_bp = bp.get_editor_property('parent_class')
        unreal.log_warning(f"  Parent class: {parent_bp.get_name() if parent_bp else 'None'}")
    except Exception as e:
        unreal.log_warning(f"  Parent check error: {e}")

    # Also check B_Container
    container_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp_c = unreal.load_asset(container_path)
    if bp_c:
        unreal.log_warning(f"\nB_Container:")
        try:
            parent_bp = bp_c.get_editor_property('parent_class')
            unreal.log_warning(f"  Parent class: {parent_bp.get_name() if parent_bp else 'None'}")
        except Exception as e:
            unreal.log_warning(f"  Parent check error: {e}")


if __name__ == "__main__":
    check_parent()
