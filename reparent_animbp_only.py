"""
Reparent ABP_SoulslikeCharacter_Additive to C++ AnimInstance
While preserving the AnimGraph and all its Linked Anim Layer implementations
"""

import unreal

def reparent_animbp():
    """Reparent AnimBP to C++ parent"""

    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    new_parent = "/Script/SLFConversion.ABP_SoulslikeCharacter_Additive"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded AnimBP: {bp.get_name()}")
    # AnimBlueprint uses different API for parent class
    try:
        gen_class = bp.generated_class()
        parent = gen_class.get_super_class() if gen_class else None
        print(f"Current parent: {parent.get_name() if parent else 'None'}")
    except Exception as e:
        print(f"Could not get parent: {e}")

    # Load new parent class
    new_parent_class = unreal.load_class(None, new_parent)
    if not new_parent_class:
        print(f"ERROR: Could not load parent class {new_parent}")
        return False

    print(f"New parent class: {new_parent_class.get_name()}")

    # Reparent the Blueprint - this preserves AnimGraph
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
        print(f"Reparent result: {result}")
    except Exception as e:
        print(f"ERROR during reparent: {e}")
        return False

    # Compile the Blueprint
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("Compiled successfully")
    except Exception as e:
        print(f"Warning during compile: {e}")

    # Save the asset
    save_result = unreal.EditorAssetLibrary.save_asset(bp_path)
    print(f"Saved: {save_result}")

    return True

if __name__ == "__main__":
    import os
    output_file = "C:/scripts/SLFConversion/migration_cache/reparent_animbp_result.txt"

    # Redirect output to file
    original_print = print
    output_lines = []

    def capture_print(*args, **kwargs):
        line = ' '.join(str(a) for a in args)
        output_lines.append(line)
        original_print(*args, **kwargs)

    import builtins
    builtins.print = capture_print

    success = reparent_animbp()

    builtins.print = original_print

    # Write results to file
    with open(output_file, 'w') as f:
        f.write('\n'.join(output_lines))
        f.write(f"\n\nFINAL RESULT: {'SUCCESS' if success else 'FAILED'}\n")

    print(f"\nResult: {'SUCCESS' if success else 'FAILED'}")
    print(f"Output written to: {output_file}")
