# clear_weapon_graphs.py
# Clear graph logic from weapon Blueprints while preserving SCS components
# Run with UnrealEditor-Cmd.exe

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/clear_weapon_graphs_output.txt"

WEAPON_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_Item",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Usables/B_Item_Lantern",
]

def run():
    results = []

    def log(msg):
        results.append(msg)
        unreal.log_warning(msg)

    def section(title):
        log("")
        log("=" * 70)
        log(title)
        log("=" * 70)

    section("CLEARING WEAPON BLUEPRINT GRAPHS")
    log("This will remove graph logic while preserving SCS components")

    cleared = 0
    errors = 0

    for bp_path in WEAPON_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if not bp:
            log(f"  {bp_name}: NOT FOUND")
            errors += 1
            continue

        try:
            # Use clear_graphs_keep_variables to preserve variables AND SCS
            # This clears EventGraph and function graphs but keeps Blueprint structure
            result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            log(f"  {bp_name}: Graphs cleared (keep vars)")
            cleared += 1
        except Exception as e:
            log(f"  {bp_name}: ERROR - {e}")
            errors += 1

    # =========================================================================
    # SAVE ALL
    # =========================================================================
    section("SAVING BLUEPRINTS")

    saved = 0
    for bp_path in WEAPON_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if bp:
            try:
                unreal.EditorAssetLibrary.save_loaded_asset(bp)
                log(f"  {bp_name}: Saved")
                saved += 1
            except Exception as e:
                log(f"  {bp_name}: Save error - {e}")

    # =========================================================================
    # SUMMARY
    # =========================================================================
    section("SUMMARY")
    log(f"Cleared: {cleared}")
    log(f"Saved: {saved}")
    log(f"Errors: {errors}")
    log("")
    log("The SCS components (StaticMesh, Trail, etc.) should be preserved.")
    log("The graph logic (which had type errors) has been removed.")

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

run()
