import unreal

def log(msg):
    print(f"[BPOnlyAnimBP] {msg}")
    unreal.log_warning(f"[BPOnlyAnimBP] {msg}")

log("=" * 70)
log("BP_ONLY ABP_SOULSLIKEBOSSNEW CHECK")
log("=" * 70)

animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"
animbp = unreal.EditorAssetLibrary.load_asset(animbp_path)

if animbp:
    log(f"Loaded: {animbp.get_name()}")

    # Get parent class
    gen_class = animbp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")
        parent = gen_class.get_super_class()
        if parent:
            log(f"Parent class: {parent.get_name()}")
            log(f"Parent path: {parent.get_path_name()}")

    # Check skeleton
    try:
        skeleton = animbp.get_editor_property("target_skeleton")
        log(f"Target Skeleton: {skeleton.get_name() if skeleton else 'None'}")
    except Exception as e:
        log(f"Skeleton error: {e}")

    # Check variables
    log("")
    log("Blueprint Variables:")
    try:
        new_vars = animbp.get_editor_property("new_variables")
        if new_vars:
            for var in new_vars:
                var_name = var.get_editor_property("var_name")
                log(f"  - {var_name}")
        else:
            log("  (no variables or not accessible)")
    except Exception as e:
        log(f"  Variables error: {e}")

else:
    log(f"ERROR: Could not load {animbp_path}")

log("=" * 70)
