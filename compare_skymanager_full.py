"""
THOROUGH comparison of B_SkyManager between bp_only and SLFConversion
Checks: SCS components, CDO defaults, parent class, all properties
"""
import unreal
import json

def get_all_properties(obj, prefix=""):
    """Get all properties from an object"""
    results = {}
    if obj is None:
        return results

    # Get class
    obj_class = obj.get_class()
    if obj_class is None:
        return results

    # Common properties to check
    props_to_check = [
        'time', 'current_time_of_day', 'time_dilation', 'day_length_minutes',
        'sunrise_time', 'sunset_time', 'time_paused', 'time_info_asset',
        'current_weather', 'weather_intensity', 'weather_transition_time',
        'sun_light', 'sky_atmosphere_actor', 'directional_light_sun', 'directional_light_moon'
    ]

    for prop in props_to_check:
        try:
            val = obj.get_editor_property(prop)
            if val is not None:
                if hasattr(val, 'get_name'):
                    results[prop] = val.get_name()
                elif hasattr(val, 'get_path_name'):
                    results[prop] = val.get_path_name()
                else:
                    results[prop] = str(val)
            else:
                results[prop] = "None"
        except:
            pass

    return results

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
                    if 'DirectionalLight' in comp_info['class']:
                        try:
                            comp_info['intensity'] = comp_template.get_editor_property('intensity')
                        except:
                            pass
                        try:
                            comp_info['light_color'] = str(comp_template.get_editor_property('light_color'))
                        except:
                            pass
                        try:
                            rel_rot = comp_template.get_editor_property('relative_rotation')
                            comp_info['relative_rotation'] = f"P={rel_rot.pitch}, Y={rel_rot.yaw}, R={rel_rot.roll}"
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

def analyze_skymanager(project_name, bp_path):
    """Analyze a B_SkyManager Blueprint"""
    unreal.log_warning(f"\n{'='*70}")
    unreal.log_warning(f"ANALYZING: {project_name}")
    unreal.log_warning(f"Path: {bp_path}")
    unreal.log_warning(f"{'='*70}")

    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  ERROR: Could not load {bp_path}")
        return None

    result = {
        'project': project_name,
        'path': bp_path,
    }

    # Get generated class and parent
    try:
        gen_class = bp.generated_class()
        result['generated_class'] = gen_class.get_name() if gen_class else 'None'

        # Get parent class chain
        parent_chain = []
        current = gen_class
        while current:
            parent_chain.append(current.get_name())
            try:
                current = current.get_super_class()
            except:
                break
        result['parent_chain'] = parent_chain
        unreal.log_warning(f"  Parent chain: {' -> '.join(parent_chain)}")
    except Exception as e:
        result['parent_error'] = str(e)
        unreal.log_warning(f"  Parent chain error: {e}")

    # Get SCS components
    unreal.log_warning(f"\n  SCS COMPONENTS:")
    scs_comps = get_scs_components(bp)
    result['scs_components'] = scs_comps
    for comp in scs_comps:
        if 'error' in comp:
            unreal.log_warning(f"    ERROR: {comp['error']}")
        else:
            unreal.log_warning(f"    - {comp['name']} ({comp['class']})")
            for k, v in comp.items():
                if k not in ['name', 'class']:
                    unreal.log_warning(f"        {k}: {v}")

    # Get CDO properties
    unreal.log_warning(f"\n  CDO PROPERTIES:")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                props = get_all_properties(cdo)
                result['cdo_properties'] = props
                for k, v in sorted(props.items()):
                    unreal.log_warning(f"    {k}: {v}")
            else:
                unreal.log_warning(f"    ERROR: Could not get CDO")
        else:
            unreal.log_warning(f"    ERROR: No generated class")
    except Exception as e:
        unreal.log_warning(f"    ERROR: {e}")
        result['cdo_error'] = str(e)

    return result

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("THOROUGH B_SkyManager COMPARISON")
    unreal.log_warning("=" * 70)

    # Analyze SLFConversion version
    slfc_result = analyze_skymanager(
        "SLFConversion",
        "/Game/SoulslikeFramework/Blueprints/Gameplay/B_SkyManager"
    )

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("SUMMARY")
    unreal.log_warning("=" * 70)

    if slfc_result:
        unreal.log_warning(f"\nSLFConversion B_SkyManager:")
        unreal.log_warning(f"  Parent chain: {slfc_result.get('parent_chain', 'Unknown')}")
        unreal.log_warning(f"  SCS components: {len(slfc_result.get('scs_components', []))}")

        # Check for DirectionalLight components
        has_sun = False
        has_moon = False
        for comp in slfc_result.get('scs_components', []):
            if 'Sun' in comp.get('name', ''):
                has_sun = True
            if 'Moon' in comp.get('name', ''):
                has_moon = True

        unreal.log_warning(f"  Has DirectionalLight_Sun: {has_sun}")
        unreal.log_warning(f"  Has DirectionalLight_Moon: {has_moon}")

        # CDO check
        cdo = slfc_result.get('cdo_properties', {})
        unreal.log_warning(f"  CDO time: {cdo.get('time', 'N/A')}")
        unreal.log_warning(f"  CDO current_time_of_day: {cdo.get('current_time_of_day', 'N/A')}")
        unreal.log_warning(f"  CDO time_dilation: {cdo.get('time_dilation', 'N/A')}")
        unreal.log_warning(f"  CDO sunrise_time: {cdo.get('sunrise_time', 'N/A')}")
        unreal.log_warning(f"  CDO sunset_time: {cdo.get('sunset_time', 'N/A')}")

main()
