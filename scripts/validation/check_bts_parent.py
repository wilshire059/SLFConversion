# check_bts_parent.py
# Check if BTS_TryGetAbility is parented to C++ class

import unreal

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility"
    bp = unreal.load_asset(bp_path)

    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    print(f"Blueprint: {bp.get_name()}")
    print(f"Class: {bp.get_class().get_name()}")

    # Use export_text to get parent info
    export = unreal.EditorAssetLibrary.export_text(bp_path) if hasattr(unreal.EditorAssetLibrary, 'export_text') else None
    if export:
        # Look for ParentClass in export
        for line in export.split('\n')[:50]:
            if 'Parent' in line or 'Super' in line or 'Class' in line:
                print(f"Export: {line.strip()}")

    # Try to use automation library
    if hasattr(unreal, 'SLFAutomationLibrary'):
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        print(f"ParentFromLib: {parent}")

    # Try generated_class
    gen = bp.generated_class()
    if gen:
        print(f"GeneratedClass: {gen.get_name()}")
        print(f"GeneratedClassPath: {gen.get_path_name()}")

if __name__ == "__main__":
    main()
