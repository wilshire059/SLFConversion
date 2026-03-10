import unreal

automation = unreal.SLFAutomationLibrary

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("CHECKING PC_SoulslikeFramework")
log("=" * 70)

pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
pc_bp = unreal.EditorAssetLibrary.load_asset(pc_path)
if pc_bp:
    parent = automation.get_blueprint_parent_class(pc_bp)
    log(f"Parent: {parent}")
    errors = automation.get_blueprint_compile_errors(pc_bp)
    if "Errors: 0" in errors:
        log("Compile: OK")
    else:
        log(f"Compile:\n{errors[:800]}")
else:
    log(f"ERROR: Could not load Blueprint at {pc_path}")

# Write output
with open("C:/scripts/SLFConversion/migration_cache/pc_bp_check.txt", 'w') as f:
    f.write('\n'.join(output))

log("\nDone. Output written to migration_cache/pc_bp_check.txt")
