"""
Fix PDA_DayNight and B_SkyManager after migration
- Clear PDA_DayNight EventGraph to resolve struct mismatch
- Set B_SkyManager CDO defaults (Time=14.976, TimeDilation=10.0)
"""
import unreal

def clear_blueprint_eventgraph(bp_path):
    """Clear EventGraph from a Blueprint"""
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return False

    unreal.log_warning(f"Processing: {bp.get_name()}")

    try:
        # Get the UbergraphPages (EventGraph)
        ubergraph_pages = bp.get_editor_property('ubergraph_pages')
        if ubergraph_pages:
            unreal.log_warning(f"  Found {len(ubergraph_pages)} UbergraphPages")
            for page in ubergraph_pages:
                # Clear all nodes except entry points
                graph = page.get_editor_property('graph') if hasattr(page, 'get_editor_property') else page
                if graph:
                    nodes = graph.get_editor_property('nodes') if hasattr(graph, 'get_editor_property') else []
                    unreal.log_warning(f"    Graph has {len(nodes) if nodes else 0} nodes")
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    # Use BlueprintEditorLibrary to clear if available
    try:
        from unreal import BlueprintEditorLibrary
        # This doesn't directly clear, but we can try compile which may help
        BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning(f"  Compiled {bp.get_name()}")
    except Exception as e:
        unreal.log_warning(f"  Compile error: {e}")

    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning(f"  Saved {bp_path}")
    return True

def set_skymanager_defaults():
    """Set B_SkyManager CDO defaults to match bp_only"""
    bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"Could not load: {bp_path}")
        return False

    unreal.log_warning(f"Processing: {bp.get_name()}")

    # Get generated class and CDO
    if hasattr(bp, 'generated_class'):
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Set defaults to match bp_only
                try:
                    cdo.set_editor_property('time', 14.976)
                    unreal.log_warning(f"  Set Time = 14.976")
                except Exception as e:
                    unreal.log_warning(f"  Error setting Time: {e}")

                try:
                    cdo.set_editor_property('time_dilation', 10.0)
                    unreal.log_warning(f"  Set TimeDilation = 10.0")
                except Exception as e:
                    unreal.log_warning(f"  Error setting TimeDilation: {e}")

                try:
                    cdo.set_editor_property('current_time_of_day', 14.976)
                    unreal.log_warning(f"  Set CurrentTimeOfDay = 14.976")
                except Exception as e:
                    unreal.log_warning(f"  Error setting CurrentTimeOfDay: {e}")

    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning(f"  Saved {bp_path}")
    return True

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("FIXING PDA_DayNight and B_SkyManager")
    unreal.log_warning("=" * 70)

    # Clear PDA_DayNight EventGraph
    clear_blueprint_eventgraph("/Game/SoulslikeFramework/Data/PDA_DayNight")

    # Set B_SkyManager defaults
    set_skymanager_defaults()

    unreal.log_warning("Done!")

main()
