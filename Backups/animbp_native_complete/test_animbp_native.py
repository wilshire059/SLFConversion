# test_animbp_native.py
# Test AnimBP C++ native implementation after clearing Blueprint functions

import unreal

def log(msg):
    unreal.log_warning(f"[ANIM_TEST] {msg}")

def main():
    log("=" * 70)
    log("TESTING ANIMBP C++ NATIVE IMPLEMENTATION")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    # Load the AnimBP
    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"AnimBP loaded: {bp.get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if not gen_class:
        log("[ERROR] No generated class")
        return

    log(f"Generated class: {gen_class.get_name()}")

    # Check parent class - use Blueprint's parent class property
    try:
        parent_class_obj = bp.get_editor_property('parent_class')
        parent_name = parent_class_obj.get_name() if parent_class_obj else "None"
    except Exception:
        parent_name = "Unknown"

    log(f"Parent class: {parent_name}")

    # Check if it's a native C++ class (no _C suffix and starts with U/A)
    parent_is_native = parent_name and not parent_name.endswith("_C")
    log(f"Parent is native C++: {parent_is_native}")

    # Get CDO and check properties
    log("")
    log("=== CHECKING PROPERTIES ===")

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("[ERROR] Could not get CDO")
        return

    # List all properties
    properties_to_check = [
        # C++ UPROPERTY (should work)
        "Speed",
        "Direction",
        "Velocity",
        "IsCrouched",
        "bIsAccelerating",
        "bIsFalling",
        "bIsBlocking",
        "IsResting",
        "LeftHandOverlayState",
        "RightHandOverlayState",
        "ActiveGuardSequence",
        "IkWeight",
        "ActiveHitNormal",
        "GrantedTags",
        # Blueprint variables that might have _0 suffix
        "LeftHandOverlayState_0",
        "RightHandOverlayState_0",
        "Direction(Angle)",
    ]

    found_props = []
    missing_props = []

    for prop_name in properties_to_check:
        try:
            val = cdo.get_editor_property(prop_name.lower().replace("(", "_").replace(")", ""))
            found_props.append(prop_name)
            log(f"  [OK] {prop_name} = {val}")
        except Exception:
            try:
                # Try exact name
                val = cdo.get_editor_property(prop_name)
                found_props.append(prop_name)
                log(f"  [OK] {prop_name} = {val}")
            except Exception:
                missing_props.append(prop_name)
                log(f"  [--] {prop_name} (not accessible via get_editor_property)")

    # Check Blueprint compile status
    log("")
    log("=== BLUEPRINT COMPILE STATUS ===")

    if hasattr(unreal, 'SLFAutomationLibrary'):
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        if errors:
            log("[WARN] Compile errors:")
            for line in errors.split('\n')[:20]:
                log(f"  {line}")
        else:
            log("[OK] No compile errors!")

        # Get remaining functions
        log("")
        log("=== REMAINING FUNCTIONS ===")
        funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
        log(f"Total functions: {len(funcs)}")
        for f in funcs:
            log(f"  {f}")

        # Run diagnostic
        log("")
        log("=== ANIMGRAPH VARIABLE READS ===")
        diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Extract variable get nodes
        lines = diagnostic.split('\n')
        in_var_section = False
        for line in lines:
            if "VARIABLE GET:" in line:
                in_var_section = True
                log(line)
            elif in_var_section and line.strip().startswith("-"):
                log(line)
            elif in_var_section and not line.strip().startswith("-") and line.strip():
                in_var_section = False

        # Check BlendListByEnum bindings
        log("")
        log("=== BLENDLISTBYENUM BINDINGS ===")
        in_blend_section = False
        for line in lines:
            if "BLEND POSES BY ENUM" in line or "BlendListByEnum" in line:
                in_blend_section = True
            if in_blend_section:
                log(line)
                if line.strip() == '' and in_blend_section:
                    in_blend_section = False

    # Load demo level and check character
    log("")
    log("=== CHECKING DEMO LEVEL ===")

    demo_level = "/Game/SoulslikeFramework/Demo/DemoRoom/DemoRoom"
    try:
        success = unreal.EditorLevelLibrary.load_level(demo_level)
        log(f"Loaded demo level: {success}")
    except Exception as e:
        log(f"Could not load demo level: {e}")

    try:
        world = unreal.EditorLevelLibrary.get_editor_world()
        if world:
            # Find character with skeletal mesh
            all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)

            for actor in all_actors:
                actor_name = actor.get_name()
                if "Character" in actor_name or "Player" in actor_name:
                    log(f"Found character: {actor_name}")

                    # Try to find mesh component
                    components = actor.get_components_by_class(unreal.SkeletalMeshComponent)
                    for mesh_comp in components:
                        anim_inst = mesh_comp.get_anim_instance()
                        if anim_inst:
                            anim_class = anim_inst.get_class().get_name()
                            log(f"  AnimInstance class: {anim_class}")

                            # Check if it's our AnimBP
                            if "ABP_SoulslikeCharacter" in anim_class:
                                log("  [OK] Using our AnimBP!")

                                # Try reading some properties
                                try:
                                    speed = anim_inst.get_editor_property("speed")
                                    log(f"    Speed: {speed}")
                                except Exception:
                                    pass
                        else:
                            log("  No AnimInstance")
    except Exception as e:
        log(f"Error checking world: {e}")

    # Summary
    log("")
    log("=" * 70)
    log("SUMMARY")
    log("=" * 70)
    log(f"AnimBP: {bp.get_name()}")
    log(f"Parent class: {parent_name}")
    log(f"Properties found: {len(found_props)}")
    log(f"Functions remaining: {len(funcs) if 'funcs' in dir() else 'N/A'}")
    log(f"BlendListByEnum nodes should be bound to: LeftHandOverlayState_0, RightHandOverlayState_0")
    log("")
    log("To fully test animations, use PIE in editor or gameplay:")
    log("  1. Walk around - check locomotion blends")
    log("  2. Press crouch - check IsCrouched transition")
    log("  3. Equip weapon - check overlay state blends")
    log("  4. Enter guard - check bIsBlocking transitions")
    log("=" * 70)

if __name__ == "__main__":
    main()
