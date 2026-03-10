import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

automation = unreal.SLFAutomationLibrary

log("=" * 70)
log("FIXING GI_SoulslikeFramework AND LEVEL BLUEPRINT")
log("=" * 70)

# Fix GI_SoulslikeFramework Blueprint
gi_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"
gi_bp = unreal.EditorAssetLibrary.load_asset(gi_path)

if gi_bp:
    log(f"\nGI_SoulslikeFramework:")
    log(f"  Loaded: {gi_bp.get_name()}")

    parent = automation.get_blueprint_parent_class(gi_bp)
    log(f"  Parent: {parent}")

    variables = automation.get_blueprint_variables(gi_bp)
    log(f"  Variables: {variables}")

    # Check if FirstTimeOnDemoLevel is in variables
    has_var = any("FirstTimeOnDemoLevel" in v for v in variables)
    log(f"  Has FirstTimeOnDemoLevel variable: {has_var}")

    # Check CDO for the property
    gen_class = gi_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                val = cdo.get_editor_property("first_time_on_demo_level")
                log(f"  CDO first_time_on_demo_level: {val}")
            except Exception as e:
                log(f"  CDO error: {e}")

    # If Blueprint has variable that conflicts, remove it
    if has_var:
        log("  Removing Blueprint variable to use C++ property...")
        result = automation.remove_variable(gi_bp, "FirstTimeOnDemoLevel")
        log(f"  remove_variable result: {result}")
        # Also try with ? suffix
        result2 = automation.remove_variable(gi_bp, "FirstTimeOnDemoLevel?")
        log(f"  remove_variable (with ?) result: {result2}")

        automation.compile_and_save(gi_bp)
        log("  Compiled and saved")

    # Check compile errors
    errors = automation.get_blueprint_compile_errors(gi_bp)
    log(f"  Compile status: {errors[:200] if errors else 'OK'}")
else:
    log(f"ERROR: Could not load GI: {gi_path}")

# Now check Level Blueprint
log("\n" + "=" * 70)
log("LEVEL BLUEPRINT")
log("=" * 70)

level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"

# Load the level
editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if editor_subsystem:
    result = editor_subsystem.load_level(level_path)
    log(f"Level load result: {result}")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/gi_level_fix.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone!")
