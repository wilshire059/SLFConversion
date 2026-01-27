# diagnose_animgraph_errors.py
# Find nodes in AnimGraph that are causing "Anim Data Asset" errors

import unreal

def log(msg):
    unreal.log_warning(f"[DiagAnim] {msg}")

def main():
    log("=" * 60)
    log("Diagnosing AnimGraph Errors")
    log("=" * 60)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log(f"[ERROR] Failed to load AnimBP")
        return

    # Use our diagnostic function
    diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(bp)
    log("AnimGraph Diagnosis:")
    for line in diagnosis.split('\n')[:100]:
        if line.strip():
            log(f"  {line}")

    log("")
    log("=" * 60)
    log("Looking for Property Access nodes...")

    # Also get compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    log("")
    log("Compile Errors:")
    for line in errors.split('\n'):
        if line.strip() and ("Anim Data Asset" in line or "Error" in line):
            log(f"  {line}")

    log("=" * 60)

if __name__ == "__main__":
    main()
