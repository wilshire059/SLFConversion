"""Check AC_EquipmentManager parent class and overlay state properties."""
import unreal

# Load the Blueprint
bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"=== AC_EquipmentManager Class Hierarchy ===")
        unreal.log_warning(f"Blueprint class: {gen_class.get_name()}")

        # Check for OverlayState properties by trying to access CDO
        unreal.log_warning(f"=== Checking OverlayState Properties via CDO ===")
        cdo = gen_class.get_default_object()
        if cdo:
            for prop_name in ['left_hand_overlay_state', 'right_hand_overlay_state', 'active_overlay_state']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    unreal.log_warning(f"  {prop_name}: EXISTS, value = {val}")
                except Exception as e:
                    unreal.log_warning(f"  {prop_name}: NOT ACCESSIBLE")
        else:
            unreal.log_warning("Could not get CDO")

        # Try using reflection to find parent
        try:
            # Use BlueprintEditorLibrary to get parent
            import unreal
            parent = unreal.BlueprintEditorLibrary.get_blueprint_parent(bp)
            if parent:
                unreal.log_warning(f"Parent blueprint: {parent.get_name() if hasattr(parent, 'get_name') else str(parent)}")
        except:
            pass
else:
    unreal.log_error("Failed to load AC_EquipmentManager blueprint")

# Also check what enum type is used
unreal.log_warning(f"=== Checking Enum Types ===")
bp_enum_path = "/Game/SoulslikeFramework/Enums/E_OverlayState"
bp_enum = unreal.EditorAssetLibrary.load_asset(bp_enum_path)
if bp_enum:
    unreal.log_warning(f"Blueprint enum E_OverlayState exists")
else:
    unreal.log_warning(f"Blueprint enum E_OverlayState NOT FOUND")
