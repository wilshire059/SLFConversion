import unreal
import os

def export_restingpoint_details():
    """Export complete B_RestingPoint Blueprint details"""

    output_path = "C:/scripts/slfconversion/migration_cache/restingpoint_bp_only_export.txt"

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    lines = []
    lines.append("="*80)
    lines.append("B_RestingPoint Blueprint Analysis (bp_only)")
    lines.append("="*80)

    # Use AssetExportTask
    export_task = unreal.AssetExportTask()
    export_task.object = bp
    export_task.filename = "C:/scripts/slfconversion/migration_cache/B_RestingPoint_export.t3d"
    export_task.automated = True
    export_task.prompt = False
    export_task.replace_identical = True

    result = unreal.Exporter.run_asset_export_task(export_task)
    print(f"Export task result: {result}")

    # Also get CDO properties
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            lines.append(f"\nGenerated Class: {gen_class.get_name()}")
            lines.append(f"\nCDO PROPERTIES:")

            props = [
                'is_activated', 'can_be_traced', 'interaction_text', 'interactable_display_name',
                'location_name', 'sitting_angle', 'force_face_sitting_actor',
                'camera_distance', 'camera_offset', 'camera_rotation',
                'discover_interaction_montage'
            ]

            for prop in props:
                try:
                    val = cdo.get_editor_property(prop)
                    lines.append(f"  {prop}: {val}")
                except:
                    pass

            # Try to get component references
            comp_names = ['unlock_effect', 'effect', 'effect_light', 'environment_light',
                         'sitting_zone', 'scene', 'camera_arm', 'lookat_camera']
            lines.append(f"\nCOMPONENT REFERENCES:")
            for comp_name in comp_names:
                try:
                    comp = cdo.get_editor_property(comp_name)
                    if comp:
                        lines.append(f"  {comp_name}: {comp.get_class().get_name()}")
                        # For NiagaraComponent, try to get the asset
                        if 'NiagaraComponent' in comp.get_class().get_name():
                            try:
                                asset = comp.get_editor_property('asset')
                                if asset:
                                    lines.append(f"    Asset: {asset.get_path_name()}")
                                else:
                                    lines.append(f"    Asset: NONE")
                            except Exception as e:
                                lines.append(f"    Asset: ERROR - {e}")
                    else:
                        lines.append(f"  {comp_name}: None")
                except Exception as e:
                    lines.append(f"  {comp_name}: ERROR - {e}")

    # Write to file
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))

    print('\n'.join(lines))
    print(f"\nSaved to: {output_path}")
    print(f"T3D export at: C:/scripts/slfconversion/migration_cache/B_RestingPoint_export.t3d")

if __name__ == "__main__":
    export_restingpoint_details()
