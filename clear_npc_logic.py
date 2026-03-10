"""
Clear ALL Blueprint logic (variables, functions, graphs) from NPC Blueprints.
The C++ parent classes provide all the logic now.
"""
import unreal

NPC_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager",
]


def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("CLEARING ALL BLUEPRINT LOGIC FROM NPC BLUEPRINTS")
    unreal.log_warning("="*70 + "\n")

    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n{bp_name}:")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint")
            continue

        try:
            # Clear ALL Blueprint logic (variables, functions, graphs)
            result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
            unreal.log_warning(f"  ClearAllBlueprintLogic: {result}")

            # Compile and save
            unreal.SLFAutomationLibrary.compile_and_save(bp)
            unreal.log_warning(f"  [OK] Compiled and saved")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("COMPLETE")
    unreal.log_warning("="*70 + "\n")


if __name__ == "__main__":
    main()
