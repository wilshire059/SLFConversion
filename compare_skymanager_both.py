"""
Compare B_SkyManager between bp_only and SLFConversion
Run this on bp_only FIRST to get baseline, then on SLFConversion
"""
import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_comparison.json"

def get_scs_components(bp):
    """Get all SCS components from a Blueprint"""
    components = []
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                comp_template = node.get_editor_property('component_template')
                if comp_template:
                    comp_info = {
                        'name': comp_template.get_name(),
                        'class': comp_template.get_class().get_name() if comp_template.get_class() else 'Unknown'
                    }

                    # Get component-specific properties
                    try:
                        rel_loc = comp_template.get_editor_property('relative_location')
                        comp_info['relative_location'] = f"X={rel_loc.x}, Y={rel_loc.y}, Z={rel_loc.z}"
                    except:
                        pass

                    try:
                        rel_rot = comp_template.get_editor_property('relative_rotation')
                        comp_info['relative_rotation'] = f"P={rel_rot.pitch}, Y={rel_rot.yaw}, R={rel_rot.roll}"
                    except:
                        pass

                    if 'Light' in comp_info['class']:
                        try:
                            comp_info['intensity'] = float(comp_template.get_editor_property('intensity'))
                        except:
                            pass
                        try:
                            lc = comp_template.get_editor_property('light_color')
                            comp_info['light_color'] = f"R={lc.r}, G={lc.g}, B={lc.b}"
                        except:
                            pass
                        try:
                            comp_info['mobility'] = str(comp_template.get_editor_property('mobility'))
                        except:
                            pass

                    components.append(comp_info)
    except Exception as e:
        components.append({'error': str(e)})

    return components

def get_cdo_properties(bp):
    """Get CDO properties"""
    props = {}
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                prop_names = [
                    'time', 'current_time_of_day', 'time_dilation', 'day_length_minutes',
                    'sunrise_time', 'sunset_time', 'time_paused', 'time_info_asset',
                    'current_weather', 'weather_intensity'
                ]
                for prop in prop_names:
                    try:
                        val = cdo.get_editor_property(prop)
                        if val is not None:
                            if hasattr(val, 'get_path_name'):
                                props[prop] = val.get_path_name()
                            elif hasattr(val, 'get_name'):
                                props[prop] = val.get_name()
                            else:
                                props[prop] = str(val)
                        else:
                            props[prop] = "None"
                    except:
                        pass
    except Exception as e:
        props['error'] = str(e)
    return props

def main():
    # Determine which project we're in
    project_name = "Unknown"
    bp_path = "/Game/SoulslikeFramework/Blueprints/Gameplay/B_SkyManager"

    # Try to load the blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        # Try alternate path
        bp_path = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        unreal.log_warning("Could not find B_SkyManager in any expected location")
        return

    # Check parent class to determine project
    try:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            parent_name = parent.get_name() if parent else "None"
            if "SLF" in parent_name:
                project_name = "SLFConversion"
            else:
                project_name = "bp_only"
    except:
        pass

    unreal.log_warning("=" * 70)
    unreal.log_warning(f"B_SkyManager Analysis - {project_name}")
    unreal.log_warning("=" * 70)

    result = {
        'project': project_name,
        'path': bp_path,
    }

    # Get parent chain
    try:
        gen_class = bp.generated_class()
        parent_chain = []
        current = gen_class
        while current:
            parent_chain.append(current.get_name())
            try:
                current = current.get_super_class()
            except:
                break
        result['parent_chain'] = parent_chain
        unreal.log_warning(f"Parent chain: {' -> '.join(parent_chain)}")
    except Exception as e:
        result['parent_error'] = str(e)

    # Get SCS components
    unreal.log_warning("\nSCS COMPONENTS:")
    scs_comps = get_scs_components(bp)
    result['scs_components'] = scs_comps
    for comp in scs_comps:
        if 'error' in comp:
            unreal.log_warning(f"  ERROR: {comp['error']}")
        else:
            unreal.log_warning(f"  {comp['name']} ({comp['class']})")
            for k, v in comp.items():
                if k not in ['name', 'class']:
                    unreal.log_warning(f"    {k}: {v}")

    # Get CDO properties
    unreal.log_warning("\nCDO PROPERTIES:")
    cdo_props = get_cdo_properties(bp)
    result['cdo_properties'] = cdo_props
    for k, v in sorted(cdo_props.items()):
        unreal.log_warning(f"  {k}: {v}")

    # Save to file (append if exists)
    try:
        existing = {}
        if os.path.exists(OUTPUT_FILE):
            with open(OUTPUT_FILE, 'r') as f:
                existing = json.load(f)

        existing[project_name] = result

        with open(OUTPUT_FILE, 'w') as f:
            json.dump(existing, f, indent=2)
        unreal.log_warning(f"\nSaved to: {OUTPUT_FILE}")
    except Exception as e:
        unreal.log_warning(f"Could not save: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
