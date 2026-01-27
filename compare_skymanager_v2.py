"""
Compare B_SkyManager - comprehensive check
"""
import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_comparison.json"
BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("B_SkyManager FULL ANALYSIS")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    result = {'path': BP_PATH}

    # Get generated class and parent chain
    try:
        gen_class = bp.generated_class()
        if gen_class:
            result['generated_class'] = gen_class.get_name()

            parent_chain = []
            current = gen_class
            while current:
                parent_chain.append(current.get_name())
                try:
                    parent = current.get_super_class()
                    if parent:
                        current = parent
                    else:
                        break
                except:
                    break

            result['parent_chain'] = parent_chain
            unreal.log_warning(f"\nPARENT CHAIN:")
            for i, p in enumerate(parent_chain):
                unreal.log_warning(f"  {'  ' * i}{p}")

            # Determine project
            if 'SLFSkyManager' in parent_chain:
                result['project'] = 'SLFConversion'
            elif 'Actor' in parent_chain and len(parent_chain) <= 3:
                result['project'] = 'bp_only'
            else:
                result['project'] = 'Unknown'
    except Exception as e:
        unreal.log_warning(f"Error getting parent chain: {e}")

    # Get all components from the actor
    unreal.log_warning(f"\nCOMPONENTS (via GetComponents on CDO):")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Use reflection to get components
                components = cdo.get_components_by_class(unreal.ActorComponent)
                result['components'] = []
                for comp in components:
                    comp_info = {
                        'name': comp.get_name(),
                        'class': comp.get_class().get_name() if comp.get_class() else 'Unknown'
                    }

                    # Light-specific properties
                    if hasattr(comp, 'intensity'):
                        try:
                            comp_info['intensity'] = float(comp.intensity)
                        except:
                            pass

                    result['components'].append(comp_info)
                    unreal.log_warning(f"  {comp_info['name']} ({comp_info['class']})")
                    for k, v in comp_info.items():
                        if k not in ['name', 'class']:
                            unreal.log_warning(f"    {k}: {v}")

                if len(result['components']) == 0:
                    unreal.log_warning("  (no components found)")
    except Exception as e:
        unreal.log_warning(f"Error getting components: {e}")

    # Get CDO properties
    unreal.log_warning(f"\nCDO PROPERTIES:")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                result['cdo'] = {}

                # Time properties
                for prop in ['time', 'current_time_of_day', 'time_dilation', 'day_length_minutes', 'sunrise_time', 'sunset_time', 'time_paused']:
                    try:
                        val = cdo.get_editor_property(prop)
                        result['cdo'][prop] = str(val)
                        unreal.log_warning(f"  {prop}: {val}")
                    except:
                        pass

                # TimeInfoAsset
                try:
                    tia = cdo.get_editor_property('time_info_asset')
                    if tia:
                        result['cdo']['time_info_asset'] = tia.get_path_name()
                        unreal.log_warning(f"  time_info_asset: {tia.get_path_name()}")
                    else:
                        result['cdo']['time_info_asset'] = 'None'
                        unreal.log_warning(f"  time_info_asset: None")
                except:
                    pass
    except Exception as e:
        unreal.log_warning(f"Error getting CDO: {e}")

    # Save result
    try:
        existing = {}
        if os.path.exists(OUTPUT_FILE):
            with open(OUTPUT_FILE, 'r') as f:
                existing = json.load(f)

        project = result.get('project', 'Unknown')
        existing[project] = result

        with open(OUTPUT_FILE, 'w') as f:
            json.dump(existing, f, indent=2)
        unreal.log_warning(f"\nSaved to: {OUTPUT_FILE}")
    except Exception as e:
        unreal.log_warning(f"Save error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
