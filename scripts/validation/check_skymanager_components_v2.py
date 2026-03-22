"""
Check B_SkyManager components using alternative methods
"""
import unreal
import json
import os

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/skymanager_detailed.json"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("B_SkyManager COMPONENT CHECK v2")
    unreal.log_warning("=" * 70)

    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    result = {'path': BP_PATH}

    # Get generated class and parent chain
    try:
        gen_class = bp.generated_class()
        if gen_class:
            result['generated_class'] = gen_class.get_name()
            unreal.log_warning(f"Generated class: {gen_class.get_name()}")

            # Walk parent chain
            parent_chain = []
            current = gen_class
            while current:
                class_name = current.get_name()
                parent_chain.append(class_name)
                unreal.log_warning(f"  Parent: {class_name}")
                try:
                    parent = current.get_super_class()
                    if parent and parent != current:
                        current = parent
                    else:
                        break
                except:
                    break
            result['parent_chain'] = parent_chain
    except Exception as e:
        unreal.log_warning(f"Parent chain error: {e}")
        result['parent_error'] = str(e)

    # Get CDO and its components
    unreal.log_warning("\nCDO COMPONENTS (via get_components_by_class):")
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log_warning(f"  CDO: {cdo.get_name()}")

                # Get all components
                try:
                    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                    result['cdo_components'] = []
                    unreal.log_warning(f"  Total components: {len(all_comps)}")

                    for comp in all_comps:
                        comp_data = {
                            'name': comp.get_name(),
                            'class': comp.get_class().get_name() if comp.get_class() else 'Unknown'
                        }
                        unreal.log_warning(f"    {comp_data['name']} ({comp_data['class']})")
                        result['cdo_components'].append(comp_data)

                    if len(all_comps) == 0:
                        unreal.log_warning("    (no components found)")
                except Exception as e:
                    unreal.log_warning(f"    Component error: {e}")
                    result['component_error'] = str(e)

                # Try to get specific light components by name
                unreal.log_warning("\nTrying to find DirectionalLight components by name:")
                comp_names = ['DirectionalLight_Sun', 'DirectionalLight_Moon', 'SunLight', 'MoonLight',
                              'DirectionalLightComponent', 'DefaultSceneRoot']
                for name in comp_names:
                    try:
                        comp = cdo.get_component_by_class(unreal.DirectionalLightComponent)
                        if comp:
                            unreal.log_warning(f"    Found DirectionalLightComponent: {comp.get_name()}")
                    except:
                        pass

                # Get CDO properties
                unreal.log_warning("\nCDO PROPERTIES:")
                result['cdo_props'] = {}
                props = ['time', 'current_time_of_day', 'time_dilation', 'day_length_minutes',
                         'sunrise_time', 'sunset_time', 'time_paused',
                         'sun_light', 'cached_sun_light', 'cached_moon_light',
                         'directional_light_sun', 'directional_light_moon']
                for prop in props:
                    try:
                        val = cdo.get_editor_property(prop)
                        val_str = str(val) if val is not None else "None"
                        unreal.log_warning(f"    {prop}: {val_str}")
                        result['cdo_props'][prop] = val_str
                    except Exception as e:
                        pass
    except Exception as e:
        unreal.log_warning(f"CDO error: {e}")
        result['cdo_error'] = str(e)

    # Try using BlueprintEditorLibrary
    unreal.log_warning("\nBLUEPRINT EDITOR LIBRARY:")
    try:
        # Get the BlueprintGeneratedClass
        bgc = bp.generated_class()
        if bgc:
            unreal.log_warning(f"  BlueprintGeneratedClass: {bgc.get_name()}")

            # Check if there's a native parent
            native_class = bgc.get_super_class()
            if native_class:
                unreal.log_warning(f"  Native parent: {native_class.get_name()}")
                result['native_parent'] = native_class.get_name()

                # Check if native parent is SLFSkyManager
                if 'SLFSkyManager' in native_class.get_name():
                    unreal.log_warning("  ** B_SkyManager IS reparented to SLFSkyManager **")
                    result['is_reparented'] = True
                else:
                    unreal.log_warning("  ** B_SkyManager is NOT reparented to SLFSkyManager **")
                    result['is_reparented'] = False
    except Exception as e:
        unreal.log_warning(f"  Error: {e}")

    # Save result
    try:
        with open(OUTPUT_PATH, 'w') as f:
            json.dump(result, f, indent=2)
        unreal.log_warning(f"\nSaved to: {OUTPUT_PATH}")
    except Exception as e:
        unreal.log_warning(f"Save error: {e}")

    unreal.log_warning("\n" + "=" * 70)

main()
