"""
Export item wheel slot configuration from W_HUD to understand slot organization.
"""
import unreal

OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/item_wheel_config.txt"

def export_config():
    with open(OUTPUT_FILE, 'w') as f:
        # Load W_HUD widget
        hud_path = "/Game/SoulslikeFramework/Widgets/HUD/W_HUD"
        hud_class = unreal.EditorAssetLibrary.load_asset(hud_path)

        if not hud_class:
            f.write(f"ERROR: Could not load {hud_path}\n")
            return

        f.write(f"Loaded: {hud_class.get_name()}\n\n")

        # Try to get the widget tree
        try:
            # Get the widget blueprint generated class
            bp_gen_class = hud_class.generated_class()
            if bp_gen_class:
                f.write(f"Generated class: {bp_gen_class.get_name()}\n\n")

                # Get CDO
                cdo = bp_gen_class.get_default_object()
                if cdo:
                    f.write("CDO Properties:\n")

                    # Try to find ItemWheel related properties
                    for prop_name in ['ItemWheel_Tools', 'ItemWheel_RightHand', 'ItemWheel_LeftHand', 'ItemWheel_Pouch']:
                        try:
                            prop = cdo.get_editor_property(prop_name.lower().replace('_', ''))
                            f.write(f"  {prop_name}: {prop}\n")
                        except:
                            pass
        except Exception as e:
            f.write(f"Error getting generated class: {e}\n")

        # Also check W_ItemWheelSlot directly
        f.write("\n\n=== W_ItemWheelSlot Default ===\n")
        slot_path = "/Game/SoulslikeFramework/Widgets/HUD/W_ItemWheelSlot"
        slot_class = unreal.EditorAssetLibrary.load_asset(slot_path)
        if slot_class:
            f.write(f"Loaded: {slot_class.get_name()}\n")
            try:
                bp_gen = slot_class.generated_class()
                if bp_gen:
                    cdo = bp_gen.get_default_object()
                    if cdo:
                        try:
                            slots_to_track = cdo.get_editor_property("slots_to_track")
                            f.write(f"Default SlotsToTrack: {slots_to_track}\n")
                        except Exception as e:
                            f.write(f"Could not get slots_to_track: {e}\n")
            except Exception as e:
                f.write(f"Error: {e}\n")

        f.write("\nDone.\n")

if __name__ == "__main__":
    export_config()
