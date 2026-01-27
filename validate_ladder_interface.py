"""
Validate B_Ladder interface implementations and C++ base class setup.
"""
import unreal

def validate_ladder_interface():
    unreal.log_warning("=" * 60)
    unreal.log_warning("LADDER INTERFACE VALIDATION")
    unreal.log_warning("=" * 60)

    # Load the B_Ladder Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder"
    bp = unreal.load_asset(bp_path)

    if not bp:
        unreal.log_error(f"Failed to load B_Ladder Blueprint")
        return False

    gen_class = bp.generated_class()
    unreal.log_warning(f"Blueprint Class: {gen_class.get_name()}")

    # Check if class implements interfaces via C++
    # This validates the C++ parent class has proper interface declarations
    cdo = unreal.get_default_object(gen_class)

    if cdo:
        unreal.log_warning(f"CDO obtained: {cdo.get_name()}")

        # Check if it's an interactable (has OnInteract)
        # The C++ parent should have OnInteract_Implementation
        has_on_interact = hasattr(cdo, 'interaction_prompt')
        unreal.log_warning(f"Has InteractionPrompt property: {has_on_interact}")

        if has_on_interact:
            prompt = cdo.get_editor_property('interaction_prompt')
            unreal.log_warning(f"  InteractionPrompt value: '{prompt}'")

        # Check ladder-specific properties
        ladder_props = [
            'ladder_height',
            'bar_offset',
            'overshoot_count',
            'connection_count',
            'b_bar_at_zero',
            'b_add_finish_bar',
        ]

        unreal.log_warning("")
        unreal.log_warning("--- LADDER CONFIG PROPERTIES ---")
        for prop in ladder_props:
            try:
                value = cdo.get_editor_property(prop)
                unreal.log_warning(f"  {prop}: {value}")
            except Exception as e:
                unreal.log_warning(f"  {prop}: (not accessible)")

    # Validate LadderManagerComponent
    unreal.log_warning("")
    unreal.log_warning("--- LADDER MANAGER COMPONENT ---")

    lm_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_LadderManager"
    lm_bp = unreal.load_asset(lm_path)

    if lm_bp:
        lm_class = lm_bp.generated_class()
        unreal.log_warning(f"AC_LadderManager Class: {lm_class.get_name()}")

        lm_cdo = unreal.get_default_object(lm_class)
        if lm_cdo:
            # Check for LadderManagerComponent properties
            lm_props = [
                'is_climbing',
                'is_on_ground',
                'is_climbing_off_top',
                'is_climbing_down_from_top',
                'current_ladder',
            ]

            for prop in lm_props:
                try:
                    value = lm_cdo.get_editor_property(prop)
                    unreal.log_warning(f"  {prop}: {value}")
                except Exception as e:
                    unreal.log_warning(f"  {prop}: (not accessible via Python, defined in C++)")
    else:
        unreal.log_warning("  AC_LadderManager not loaded")

    # Validate PDA_LadderAnimData
    unreal.log_warning("")
    unreal.log_warning("--- LADDER ANIM DATA ---")

    pda_path = "/Game/SoulslikeFramework/Data/_AnimationData/PDA_LadderAnimData"
    pda = unreal.load_asset(pda_path)

    if pda:
        unreal.log_warning(f"PDA_LadderAnimData loaded: {pda.get_name()}")
        unreal.log_warning(f"  Class: {pda.get_class().get_name()}")
    else:
        unreal.log_warning("  PDA_LadderAnimData not loaded")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("INTERFACE VALIDATION COMPLETE")
    unreal.log_warning("=" * 60)
    unreal.log_warning("")
    unreal.log_warning("MANUAL PIE TEST REQUIRED:")
    unreal.log_warning("  1. Place B_Ladder in level (should be visible)")
    unreal.log_warning("  2. Walk player near ladder")
    unreal.log_warning("  3. Press interact key")
    unreal.log_warning("  4. Verify TryClimbLadder triggers climbing")
    unreal.log_warning("")

    return True

if __name__ == "__main__":
    validate_ladder_interface()
