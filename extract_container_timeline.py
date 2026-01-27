# extract_container_timeline.py
# Extract Timeline data from B_Container in bp_only

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING B_CONTAINER TIMELINE DATA")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Failed to load: {bp_path}")
        return

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

    # Get all components
    unreal.log_warning("\n--- COMPONENTS ---")
    components = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in components:
        unreal.log_warning(f"  {comp.get_class().get_name()}: {comp.get_name()}")

    # Look for Timeline components
    unreal.log_warning("\n--- TIMELINE COMPONENTS ---")
    timeline_comps = cdo.get_components_by_class(unreal.TimelineComponent)
    for tc in timeline_comps:
        unreal.log_warning(f"\n  Timeline: {tc.get_name()}")
        try:
            length = tc.get_timeline_length()
            unreal.log_warning(f"    Length: {length}s")
        except Exception as e:
            unreal.log_warning(f"    Length: (error) {e}")

        try:
            play_rate = tc.get_play_rate()
            unreal.log_warning(f"    Play Rate: {play_rate}")
        except Exception as e:
            pass

    # Also check via export_text for timeline info
    unreal.log_warning("\n--- EXPORT TEXT (timeline/track/key lines) ---")
    try:
        export_text = bp.export_text()
        lines = export_text.split('\n')
        for i, line in enumerate(lines):
            line_lower = line.lower()
            if any(kw in line_lower for kw in ['timeline', 'openchest', 'floattrack', 'eventtrack', 'keys=', 'roty', 'spawnvfx', 'itemspawn']):
                unreal.log_warning(f"  [{i}] {line.strip()}")
    except Exception as e:
        unreal.log_warning(f"  Could not export: {e}")

    # Check variables
    unreal.log_warning("\n--- VARIABLES ---")
    try:
        speed_mult = cdo.get_editor_property("speed_multiplier")
        unreal.log_warning(f"  SpeedMultiplier = {speed_mult}")
    except Exception as e:
        unreal.log_warning(f"  SpeedMultiplier: (error) {e}")

    try:
        lid_angle = cdo.get_editor_property("lid_open_angle")
        unreal.log_warning(f"  LidOpenAngle = {lid_angle}")
    except Exception as e:
        unreal.log_warning(f"  LidOpenAngle: (error) {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
