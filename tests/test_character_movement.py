# test_character_movement.py
# Test character movement setup after migration to C++

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/test_character_movement_output.txt"

def run():
    results = []
    results.append("=" * 70)
    results.append("CHARACTER MOVEMENT TEST - Post Migration")
    results.append("=" * 70)
    results.append("")

    try:
        # Test 1: Load B_Soulslike_Character
        results.append("[TEST 1] Loading B_Soulslike_Character...")
        char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
        char_bp = unreal.EditorAssetLibrary.load_asset(char_path)
        if char_bp:
            results.append("  OK: B_Soulslike_Character loaded")
            gen_class = char_bp.generated_class()
            if gen_class:
                results.append(f"  Class: {gen_class.get_name()}")

                # Get parent via SLFAutomationLibrary
                parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(char_bp)
                results.append(f"  Parent: {parent}")

                if parent and "SLF" in str(parent):
                    results.append("  OK: Parent is C++ class")
                else:
                    results.append(f"  Note: Check parent class name: {parent}")
            else:
                results.append("  ISSUE: Could not get generated class")
        else:
            results.append("  FAILED: Could not load B_Soulslike_Character")

        # Test 2: Check CDO for input actions
        results.append("")
        results.append("[TEST 2] Checking Input Action configuration...")
        if char_bp:
            gen_class = char_bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    # Check for movement input action
                    try:
                        ia_move = cdo.get_editor_property("IA_Move")
                        if ia_move:
                            results.append(f"  OK: IA_Move = {ia_move.get_path_name()}")
                        else:
                            results.append("  ISSUE: IA_Move is None (movement input not configured)")
                    except Exception as e:
                        results.append(f"  ISSUE: Could not check IA_Move: {e}")

                    # Check for look input action
                    try:
                        ia_look = cdo.get_editor_property("IA_Look")
                        if ia_look:
                            results.append(f"  OK: IA_Look = {ia_look.get_path_name()}")
                        else:
                            results.append("  ISSUE: IA_Look is None")
                    except Exception as e:
                        results.append(f"  ISSUE: Could not check IA_Look: {e}")

                    # Check for mapping context
                    try:
                        mapping_ctx = cdo.get_editor_property("PlayerMappingContext")
                        if mapping_ctx:
                            results.append(f"  OK: PlayerMappingContext = {mapping_ctx.get_path_name()}")
                        else:
                            results.append("  ISSUE: PlayerMappingContext is None")
                    except Exception as e:
                        results.append(f"  ISSUE: Could not check PlayerMappingContext: {e}")

        # Test 3: Check CharacterMovementComponent exists
        results.append("")
        results.append("[TEST 3] Checking CharacterMovementComponent...")
        if char_bp:
            gen_class = char_bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        cmc = cdo.get_character_movement()
                        if cmc:
                            results.append("  OK: CharacterMovementComponent exists")
                            max_speed = cmc.max_walk_speed
                            results.append(f"  MaxWalkSpeed: {max_speed}")
                        else:
                            results.append("  ISSUE: CharacterMovementComponent is None")
                    except Exception as e:
                        results.append(f"  ISSUE: Could not check movement component: {e}")

        # Test 4: Check GameMode spawns this character
        results.append("")
        results.append("[TEST 4] Checking GameMode default pawn class...")
        gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
        gm = unreal.EditorAssetLibrary.load_asset(gm_path)
        if gm:
            results.append("  OK: GM_SoulslikeFramework loaded")
            gen_class = gm.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        pawn_class = cdo.get_editor_property("DefaultPawnClass")
                        if pawn_class:
                            results.append(f"  DefaultPawnClass: {pawn_class.get_name()}")
                        else:
                            results.append("  ISSUE: DefaultPawnClass is None")
                    except Exception as e:
                        results.append(f"  ISSUE: Could not check DefaultPawnClass: {e}")

        # Test 5: Check PlayerController
        results.append("")
        results.append("[TEST 5] Checking PlayerController configuration...")
        pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
        pc = unreal.EditorAssetLibrary.load_asset(pc_path)
        if pc:
            results.append("  OK: PC_SoulslikeFramework loaded")
            gen_class = pc.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        mapping_ctx = cdo.get_editor_property("GameplayMappingContext")
                        if mapping_ctx:
                            results.append(f"  GameplayMappingContext: {mapping_ctx.get_path_name()}")
                        else:
                            results.append("  ISSUE: GameplayMappingContext is None")
                    except Exception as e:
                        results.append(f"  Note: GameplayMappingContext check: {e}")
        else:
            results.append("  ISSUE: Could not load PC_SoulslikeFramework")

        # Test 6: Check B_BaseCharacter parent
        results.append("")
        results.append("[TEST 6] Checking B_BaseCharacter parent class...")
        base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
        base_bp = unreal.EditorAssetLibrary.load_asset(base_path)
        if base_bp:
            results.append("  OK: B_BaseCharacter loaded")
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(base_bp)
            results.append(f"  Parent: {parent}")
            if parent and "SLF" in str(parent):
                results.append("  OK: Parent is C++ class")
        else:
            results.append("  ISSUE: Could not load B_BaseCharacter")

        # Test 7: Check for common component issues
        results.append("")
        results.append("[TEST 7] Checking component configuration...")
        if char_bp:
            gen_class = char_bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    # List all components
                    try:
                        comps = cdo.get_components_by_class(unreal.ActorComponent)
                        results.append(f"  Total components: {len(comps)}")

                        # Check for key components
                        comp_names = [c.get_name() for c in comps]
                        if any("Capsule" in n for n in comp_names):
                            results.append("  OK: Has CapsuleComponent")
                        if any("CharacterMovement" in n for n in comp_names):
                            results.append("  OK: Has CharacterMovementComponent")
                        if any("Mesh" in n or "SkeletalMesh" in n for n in comp_names):
                            results.append("  OK: Has SkeletalMeshComponent")
                        if any("Camera" in n for n in comp_names):
                            results.append("  OK: Has Camera-related component")

                    except Exception as e:
                        results.append(f"  Could not list components: {e}")

        # Summary
        results.append("")
        results.append("=" * 70)
        results.append("SUMMARY")
        results.append("=" * 70)
        results.append("For character movement to work in PIE:")
        results.append("1. B_Soulslike_Character must have C++ parent (SLFSoulslikeCharacter)")
        results.append("2. PlayerMappingContext and Input Actions must be configured in Blueprint")
        results.append("3. CharacterMovementComponent must exist with valid MaxWalkSpeed")
        results.append("4. GameMode must spawn the correct character class")
        results.append("")
        results.append("NOTE: If Input Actions are None, configure them in the Blueprint defaults!")

    except Exception as e:
        results.append(f"ERROR: {e}")
        import traceback
        results.append(traceback.format_exc())

    # Write results
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

    unreal.log_warning(output)

run()
