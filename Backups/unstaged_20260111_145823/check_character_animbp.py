# check_character_animbp.py
# Check if the character has AnimBP properly assigned

import unreal

def log(msg):
    unreal.log_warning(f"[CharCheck] {msg}")

def main():
    log("=" * 70)
    log("Checking Character AnimBP Assignment")
    log("=" * 70)

    # Check main player character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    char_bp = unreal.load_asset(char_path)
    if not char_bp:
        log(f"[ERROR] Failed to load character Blueprint")
        return

    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(char_bp)
    log(f"Character Blueprint Parent: {parent}")

    # Check compile status
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(char_bp)
    log("Compile Status:")
    for line in errors.split('\n')[:20]:
        if line.strip():
            log(f"  {line}")

    # Get the generated class and check its CDO
    gen_class = char_bp.generated_class()
    if gen_class:
        log(f"")
        log(f"Generated Class: {gen_class.get_name()}")

        # Get CDO
        cdo = gen_class.get_default_object()
        if cdo:
            log(f"CDO: {cdo.get_name()}")

            # Try to find Mesh component
            mesh_comp = cdo.get_component_by_class(unreal.SkeletalMeshComponent)
            if mesh_comp:
                log(f"")
                log(f"SkeletalMeshComponent found:")
                log(f"  SkeletalMesh: {mesh_comp.skeletal_mesh}")
                log(f"  AnimationMode: {mesh_comp.animation_mode}")
                log(f"  AnimClass: {mesh_comp.anim_class}")
            else:
                log("[WARNING] No SkeletalMeshComponent found in CDO")
    else:
        log("[ERROR] No generated class")

    log("=" * 70)

if __name__ == "__main__":
    main()
