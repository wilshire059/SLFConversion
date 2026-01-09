"""
Fix Property Access nodes in AnimBP that reference "?" suffix properties.

The AnimGraph uses Property Access nodes which store FName paths.
When a Blueprint variable is named "IsGuarding?" but C++ uses "IsGuarding",
the Property Access fails to resolve.

This script:
1. Loads the AnimBP
2. Finds Property Access nodes with "?" suffix paths
3. Updates them to use the C++ property names (without "?")
"""

import unreal

def fix_animblueprint_property_access(bp_path):
    """Fix Property Access nodes in an AnimBP"""

    # Load the AnimBlueprint
    anim_bp = unreal.load_asset(bp_path)
    if not anim_bp:
        print(f"ERROR: Could not load {bp_path}")
        return False

    print(f"Loaded: {bp_path}")
    print(f"  Class: {anim_bp.get_class().get_name()}")

    # Get the generated class to find property bindings
    gen_class = anim_bp.get_class()

    # AnimBlueprints store property access in the AnimGraph
    # We need to use the editor subsystem to modify these

    # Export current state
    export_text = unreal.EditorAssetLibrary.export_text(bp_path)

    # Check if IsGuarding? is in the export
    if "IsGuarding?" in export_text:
        print("  Found 'IsGuarding?' in export text")

        # Replace the property path
        fixed_text = export_text.replace("IsGuarding?", "IsGuarding")

        # Also fix bIsBlocking? if present
        if "bIsBlocking?" in fixed_text:
            fixed_text = fixed_text.replace("bIsBlocking?", "bIsBlocking")
            print("  Fixed 'bIsBlocking?' -> 'bIsBlocking'")

        # Unfortunately we can't re-import the text directly
        # We need to use a different approach
        print("  NOTE: Property Access paths are compiled into the AnimBP")
        print("  The fix requires recompiling the AnimBP with correct property names")

    else:
        print("  'IsGuarding?' not found in export text")

    return True


def fix_property_bindings():
    """
    Alternative approach: Use Property Redirect in DefaultEngine.ini

    This tells UE to redirect old property names to new ones.
    """

    print("=" * 60)
    print("PROPERTY ACCESS FIX")
    print("=" * 60)

    # The AnimBPs with issues
    animblueprints = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    ]

    for bp_path in animblueprints:
        fix_animblueprint_property_access(bp_path)

    print()
    print("RECOMMENDED FIX:")
    print("-" * 60)
    print("Add CoreRedirects to DefaultEngine.ini:")
    print()
    print('[CoreRedirects]')
    print('+PropertyRedirects=(OldName="AC_CombatManager.IsGuarding?",NewName="AC_CombatManager.IsGuarding")')
    print('+PropertyRedirects=(OldName="ABP_SoulslikeCharacter_Additive.bIsBlocking?",NewName="ABP_SoulslikeCharacter_Additive.bIsBlocking")')
    print()
    print("Or recompile the AnimBP in editor after fixing the source properties.")


if __name__ == "__main__":
    fix_property_bindings()
