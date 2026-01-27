# rename_vars_v4.py
# Try raw property access and modification

import unreal
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/rename_v4_log.txt"
os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
with open(OUTPUT_FILE, "w") as f:
    f.write("")

def log(msg):
    print(f"[V4] {msg}")
    with open(OUTPUT_FILE, "a") as f:
        f.write(f"{msg}\n")

def try_raw_modification():
    """Try to modify the Blueprint using raw UObject property access."""
    log("=" * 60)
    log("RAW PROPERTY MODIFICATION ATTEMPT")
    log("=" * 60)

    # Load the AnimBP
    bp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)

    if not bp:
        log("ERROR: Could not load asset")
        return

    log(f"Loaded: {bp.get_name()}")

    # Get all editor properties
    log("")
    log("Checking all accessible editor properties:")

    # Try to find properties that might contain variable definitions
    property_names_to_try = [
        'new_variables',
        'variable_descriptions',
        'variables',
        'blueprint_system_variables',
        'category_sorting',
        'categories',
        'inherited_variables',
        'property_guids',
    ]

    for prop_name in property_names_to_try:
        try:
            val = bp.get_editor_property(prop_name)
            log(f"  {prop_name}: {val} (type: {type(val).__name__})")
            if val is not None and hasattr(val, '__len__'):
                log(f"    Length: {len(val)}")
                if len(val) > 0:
                    log(f"    First item type: {type(val[0]).__name__}")
        except Exception as e:
            log(f"  {prop_name}: [not accessible] {str(e)[:50]}")

    # Try to get the UBlueprint's native properties using the class reflection
    log("")
    log("Trying to enumerate class properties:")
    bp_class = bp.get_class()
    log(f"  Blueprint class: {bp_class.get_name()}")

    # Check what functions/properties the class exposes
    log("")
    log("Class attributes that might help:")
    class_attrs = [attr for attr in dir(bp_class) if not attr.startswith('_')]
    for attr in class_attrs[:30]:
        log(f"    {attr}")

    # Let's try to use unreal.find_object or reflection to access NewVariables
    log("")
    log("Trying to access via reflection...")

    # The UBlueprint has a TArray<FBPVariableDescription> NewVariables
    # Let's try to access it via Python struct

    # Try using SystemLibrary or other utilities
    log("")
    log("Checking if EditorScriptingUtilities has anything useful:")
    if hasattr(unreal, 'EditorScriptingUtilities'):
        for attr in dir(unreal.EditorScriptingUtilities):
            if 'variable' in attr.lower() or 'blueprint' in attr.lower():
                log(f"  {attr}")

    # Try getting raw property data
    log("")
    log("Attempting raw struct access...")

    # AnimBP specific: try accessing animation-related properties
    anim_properties = [
        'anim_notify_data',
        'groups',
        'pose_watches',
        'anim_layers',
        'layer_groups',
    ]
    for prop_name in anim_properties:
        try:
            val = bp.get_editor_property(prop_name)
            log(f"  {prop_name}: {val}")
        except:
            pass

    log("")
    log("=" * 60)
    log("CONCLUSION:")
    log("The Python API does not expose NewVariables for modification.")
    log("Manual renaming in the editor is required.")
    log("=" * 60)

if __name__ == "__main__":
    try_raw_modification()
