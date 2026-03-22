# diagnose_enemy_animbp.py
# Deep diagnosis of enemy AnimBP to understand why animations aren't working

import unreal

def log(msg):
    unreal.log_warning(f"[DIAG] {msg}")

def main():
    log("=" * 80)
    log("ENEMY ANIMBP DEEP DIAGNOSIS")
    log("=" * 80)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("ERROR: Could not load AnimBP")
        return

    # 1. Check parent class
    log("")
    log("=== PARENT CLASS ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent: {parent}")
    else:
        log("SLFAutomationLibrary not available")

    # 2. List all Blueprint variables
    log("")
    log("=== BLUEPRINT VARIABLES ===")
    log("(These are what AnimGraph K2Node_VariableGet nodes look for)")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        bp_vars = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        if bp_vars:
            for v in bp_vars:
                log(f"  - {v}")
        else:
            log("  (No Blueprint variables - AnimGraph should read C++ properties)")
    else:
        log("  (Cannot check - SLFAutomationLibrary not available)")

    # 3. Check compile status
    log("")
    log("=== COMPILE STATUS ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            for line in errors.split('\n')[:30]:
                if line.strip():
                    log(f"  {line.strip()[:100]}")
        else:
            log("  No compile errors")

    # 4. Deep AnimBP diagnosis
    log("")
    log("=== ANIMGRAPH NODE DIAGNOSIS ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        diagnosis = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
        if diagnosis:
            # Look for key sections
            lines = diagnosis.split('\n')
            for i, line in enumerate(lines):
                # Print variable get nodes
                if 'K2Node_VariableGet' in line or 'VARIABLE GET' in line:
                    log(line)
                    # Print next few lines for context
                    for j in range(1, 5):
                        if i + j < len(lines):
                            log(lines[i + j])
                # Print blend nodes
                if 'BlendSpace' in line or 'BLEND' in line:
                    log(line)

    # 5. Compare C++ properties vs Blueprint variables
    log("")
    log("=== C++ VS BLUEPRINT VARIABLE COMPARISON ===")
    log("")
    log("C++ UPROPERTY in UABP_SoulslikeEnemy:")
    cpp_props = [
        "SoulslikeCharacter (AActor*)",
        "MovementComponent (UCharacterMovementComponent*)",
        "Velocity (FVector)",
        "GroundSpeed (float)",
        "IsFalling (bool)",
        "EquipmentComponent (UActorComponent*)",
        "LocomotionType (ESLFOverlayState)",
        "PhysicsWeight (float)",
        "SoulslikeEnemy (AActor*)",
        "ACAICombatManager (UActorComponent*)",
        "HitLocation (FVector)",
        "Direction (float)",
        "PoiseBroken (bool)",
        "PoiseBreakAsset (UPrimaryDataAsset*)",
    ]
    for p in cpp_props:
        log(f"  - {p}")

    log("")
    log("Blueprint variables (above) should be EMPTY or match C++ names exactly")
    log("If Blueprint has 'GroundSpeed_0' but C++ has 'GroundSpeed' => MISMATCH!")

    # 6. Check what AnimGraph is reading
    log("")
    log("=== EXPECTED BEHAVIOR ===")
    log("")
    log("AnimGraph K2Node_VariableGet nodes look for variables by EXACT FName:")
    log("  - Node looking for 'GroundSpeed' will find C++ UPROPERTY 'GroundSpeed'")
    log("  - Node looking for 'GroundSpeed_0' will NOT find C++ property!")
    log("")
    log("SOLUTION: Delete Blueprint variables so AnimGraph falls back to C++ properties")

    # 7. Check if we can see what names the AnimGraph nodes are looking for
    log("")
    log("=== CHECKING ANIMGRAPH VARIABLE REFERENCES ===")
    if hasattr(unreal, 'SLFAutomationLibrary'):
        full_diag = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
        # Look for VariableName references
        for line in full_diag.split('\n'):
            if 'VariableName' in line or 'Variable:' in line or 'GroundSpeed' in line:
                log(f"  {line.strip()[:100]}")

    log("")
    log("=" * 80)
    log("END DIAGNOSIS")
    log("=" * 80)


if __name__ == "__main__":
    main()
