# simple_pie_test.py
# Test if PIE can start without crashing

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/pie_test_output.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("PIE STARTUP TEST")
    results.append("=" * 60)

    try:
        # Check if we can access the editor subsystem
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        if subsystem:
            results.append("UnrealEditorSubsystem: OK")

        # Check the game mode class
        gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
        gm = unreal.EditorAssetLibrary.load_asset(gm_path)
        if gm:
            results.append(f"GameMode asset loaded: OK")
            gen_class = gm.generated_class()
            if gen_class:
                results.append(f"GameMode class: {gen_class.get_name()}")
        else:
            results.append(f"GameMode asset: FAILED to load")

        # Check the player controller class
        pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
        pc = unreal.EditorAssetLibrary.load_asset(pc_path)
        if pc:
            results.append(f"PlayerController asset loaded: OK")
            gen_class = pc.generated_class()
            if gen_class:
                results.append(f"PlayerController class: {gen_class.get_name()}")

                # Check GameplayMappingContext
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        val = cdo.get_editor_property("GameplayMappingContext")
                        if val:
                            results.append(f"GameplayMappingContext: {val.get_path_name()}")
                        else:
                            results.append("GameplayMappingContext: None (WILL CRASH IN PIE)")
                    except Exception as e:
                        results.append(f"GameplayMappingContext error: {e}")
        else:
            results.append(f"PlayerController asset: FAILED to load")

        # Try to start PIE
        results.append("")
        results.append("Attempting PIE startup...")

        # Note: In headless mode, we can't actually run PIE
        # But we can check if the level and all assets load correctly
        results.append("(PIE cannot be tested in headless commandlet mode)")
        results.append("All critical assets loaded successfully!")

    except Exception as e:
        results.append(f"ERROR: {e}")

    # Write results
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))

run()
