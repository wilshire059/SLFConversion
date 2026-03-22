import unreal
import os

def check_restingpoint():
    """Check current state of B_RestingPoint in SLFConversion"""

    output_path = "C:/scripts/slfconversion/migration_cache/restingpoint_current_state.txt"

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    lines = []
    lines.append("="*80)
    lines.append("B_RestingPoint Current State (SLFConversion)")
    lines.append("="*80)

    # Get the generated class
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)

        # Check parent class
        parent_class = gen_class.static_class()
        lines.append(f"\nGenerated Class: {gen_class.get_name()}")
        lines.append(f"Class Flags: {gen_class}")

        if cdo:
            lines.append(f"\nCDO Components:")
            # Try GetComponents equivalent
            try:
                components = cdo.get_components_by_class(unreal.ActorComponent)
                for comp in components:
                    lines.append(f"  - {comp.get_name()} ({comp.get_class().get_name()})")
                    if 'NiagaraComponent' in comp.get_class().get_name():
                        try:
                            asset = comp.get_editor_property('asset')
                            if asset:
                                lines.append(f"      Asset: {asset.get_path_name()}")
                            else:
                                lines.append(f"      Asset: NONE")
                        except:
                            lines.append(f"      Asset: ERROR getting")
            except Exception as e:
                lines.append(f"  ERROR getting components: {e}")

    # Write to file
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines))

    print('\n'.join(lines))
    print(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    check_restingpoint()
