# check_bponly_container_delay.py
# Check the OpenDelayTime or similar delay setting in bp_only B_Container

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING BP_ONLY B_CONTAINER DELAY SETTINGS")
    unreal.log_warning("=" * 70)

    # Load the B_Container Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Failed to load: {bp_path}")
        return

    # Get the generated class and CDO
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("No CDO")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")
    unreal.log_warning(f"Class: {gen_class.get_name()}")

    # Try to find delay-related properties
    delay_props = ["OpenDelayTime", "OpenDelay", "DelayTime", "Delay", "AnimationDelay",
                   "open_delay_time", "open_delay", "delay_time", "delay"]

    for prop_name in delay_props:
        try:
            value = cdo.get_editor_property(prop_name)
            unreal.log_warning(f"  {prop_name} = {value}")
        except:
            pass

    # Also check via export_text for any delay values
    unreal.log_warning("\n--- Export Text Search ---")
    try:
        export_text = bp.export_text()
        # Look for delay-related lines
        for line in export_text.split('\n'):
            line_lower = line.lower()
            if 'delay' in line_lower:
                unreal.log_warning(f"  {line.strip()}")
    except Exception as e:
        unreal.log_warning(f"Could not export: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
