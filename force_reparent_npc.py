"""
Force reparent problematic NPC Blueprints using ForceReparentAndValidate.
"""
import unreal

# Blueprints that need forced reparenting
FORCE_REPARENT_MAP = {
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager": "/Script/SLFConversion.AIInteractionManagerComponent",
}


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("FORCE REPARENTING PROBLEMATIC NPC BLUEPRINTS")
    unreal.log_warning("="*70 + "\n")

    for bp_path, cpp_path in FORCE_REPARENT_MAP.items():
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")
        unreal.log_warning(f"  Blueprint: {bp_path}")
        unreal.log_warning(f"  Target C++: {cpp_path}")

        try:
            result = unreal.SLFAutomationLibrary.force_reparent_and_validate(bp_path, cpp_path)
            unreal.log_warning(f"  Result:\n{result}")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")

    unreal.log_warning("\n" + "="*70)


if __name__ == "__main__":
    main()
