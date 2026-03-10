import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

automation = unreal.SLFAutomationLibrary

log("=" * 70)
log("VERIFYING CUTSCENE SETUP")
log("=" * 70)

# 1. Check B_SequenceActor
log("\n1. B_SequenceActor Blueprint:")
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    parent = automation.get_blueprint_parent_class(bp)
    log(f"   Parent: {parent}")

    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                seq = cdo.get_editor_property("sequence_to_play")
                log(f"   Default sequence_to_play: {seq.get_name() if seq else 'None'}")
                can_skip = cdo.get_editor_property("can_be_skipped")
                log(f"   Default can_be_skipped: {can_skip}")
            except Exception as e:
                log(f"   Property access: {e}")

    errors = automation.get_blueprint_compile_errors(bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: {errors[:200]}")
else:
    log("   ERROR: Could not load Blueprint")

# 2. Check LS_ShowcaseRoom sequence
log("\n2. LS_ShowcaseRoom Level Sequence:")
seq_path = "/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom"
seq_asset = unreal.EditorAssetLibrary.load_asset(seq_path)
if seq_asset:
    log(f"   Found: {seq_asset.get_name()}")
    log(f"   Class: {seq_asset.get_class().get_name()}")
else:
    log("   ERROR: Could not load sequence")

# 3. Check GI_SoulslikeFramework
log("\n3. GI_SoulslikeFramework:")
gi_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"
gi_bp = unreal.EditorAssetLibrary.load_asset(gi_path)
if gi_bp:
    parent = automation.get_blueprint_parent_class(gi_bp)
    log(f"   Parent: {parent}")

    gen_class = gi_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                val = cdo.get_editor_property("first_time_on_demo_level")
                log(f"   first_time_on_demo_level (default): {val}")
            except:
                try:
                    val = cdo.get_editor_property("FirstTimeOnDemoLevel")
                    log(f"   FirstTimeOnDemoLevel (default): {val}")
                except Exception as e:
                    log(f"   Property access error: {e}")

    errors = automation.get_blueprint_compile_errors(gi_bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: {errors[:200]}")
else:
    log("   ERROR: Could not load Blueprint")

# 4. Check PC_SoulslikeFramework
log("\n4. PC_SoulslikeFramework:")
pc_path = "/Game/SoulslikeFramework/Blueprints/GameFramework/PC_SoulslikeFramework"
pc_bp = unreal.EditorAssetLibrary.load_asset(pc_path)
if pc_bp:
    parent = automation.get_blueprint_parent_class(pc_bp)
    log(f"   Parent: {parent}")
    errors = automation.get_blueprint_compile_errors(pc_bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: {errors[:200]}")
else:
    log("   ERROR: Could not load Blueprint")

log("\n" + "=" * 70)
log("CUTSCENE FLOW (C++ Implementation):")
log("=" * 70)
log("")
log("1. PIE starts -> PC_SoulslikeFramework::BeginPlay() runs")
log("2. C++ checks if level name contains 'Demo_Showcase'")
log("3. C++ gets USLFGameInstance and checks FirstTimeOnDemoLevel")
log("4. If true, C++ spawns B_SequenceActor with LS_ShowcaseRoom")
log("5. C++ sets FirstTimeOnDemoLevel = false")
log("")
log("This bypasses the broken Level Blueprint variable references.")
log("=" * 70)

# Write output
with open("C:/scripts/SLFConversion/migration_cache/cutscene_setup_verify.txt", 'w') as f:
    f.write('\n'.join(output))
