"""
Reparent NPC system Blueprints to C++ classes.

Uses C++ SLFAutomationLibrary.ReparentBlueprint for reliable reparenting.
Python is only a thin wrapper to call the C++ function.
"""
import unreal

# NPC system reparenting map
NPC_REPARENT_MAP = {
    # NPC Characters
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC": "/Script/SLFConversion.SLFSoulslikeNPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide": "/Script/SLFConversion.SLFNPCShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor": "/Script/SLFConversion.SLFNPCShowcaseVendor",

    # NPC Component
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager": "/Script/SLFConversion.AIInteractionManagerComponent",
}


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("REPARENTING NPC SYSTEM BLUEPRINTS TO C++ CLASSES (via C++ Automation)")
    unreal.log_warning("="*70 + "\n")

    succeeded = 0
    failed = 0

    for bp_path, cpp_path in NPC_REPARENT_MAP.items():
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")
        unreal.log_warning(f"  Blueprint: {bp_path}")
        unreal.log_warning(f"  Target C++: {cpp_path}")

        # Load the Blueprint asset
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            failed += 1
            continue

        # Call C++ ReparentBlueprint function
        try:
            result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
            if result:
                unreal.log_warning(f"  [OK] Reparented via C++")

                # Compile and save
                unreal.SLFAutomationLibrary.compile_and_save(bp)
                unreal.log_warning(f"  [OK] Compiled and saved")
                succeeded += 1
            else:
                unreal.log_warning(f"  [FAIL] C++ reparent returned false")
                failed += 1
        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")
            failed += 1

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning(f"COMPLETE: {succeeded} succeeded, {failed} failed")
    unreal.log_warning("="*70 + "\n")


if __name__ == "__main__":
    main()
