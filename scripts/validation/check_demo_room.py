# Check B_DemoRoom and level loading
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/demo_room_check.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("DEMO ROOM CHECK")
    results.append("=" * 60)

    # Check B_DemoRoom Blueprint
    demo_room_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_DemoRoom"
    try:
        demo_room = unreal.EditorAssetLibrary.load_asset(demo_room_path)
        if demo_room:
            results.append(f"B_DemoRoom loaded: OK")
            gen_class = demo_room.generated_class()
            if gen_class:
                results.append(f"  Generated class: {gen_class.get_name()}")
                # Check CDO for Rooms property
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        rooms = cdo.get_editor_property("rooms")
                        results.append(f"  Rooms array: {len(rooms) if rooms else 0} entries")
                    except Exception as e:
                        results.append(f"  Rooms property error: {e}")
        else:
            results.append(f"B_DemoRoom: FAILED to load")
    except Exception as e:
        results.append(f"B_DemoRoom ERROR: {e}")

    # Check L_Demo_Showcase level
    demo_level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        level = unreal.EditorAssetLibrary.load_asset(demo_level_path)
        if level:
            results.append(f"L_Demo_Showcase loaded: OK")
            results.append(f"  Type: {type(level).__name__}")
        else:
            results.append(f"L_Demo_Showcase: FAILED to load")
    except Exception as e:
        results.append(f"L_Demo_Showcase ERROR: {e}")

    # Check if player character spawns correctly by loading GameMode
    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    try:
        gm = unreal.EditorAssetLibrary.load_asset(gm_path)
        if gm:
            results.append(f"GM_SoulslikeFramework loaded: OK")
            gen_class = gm.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    pawn_class = cdo.get_editor_property("default_pawn_class")
                    if pawn_class:
                        results.append(f"  DefaultPawnClass: {pawn_class.get_name()}")
                    else:
                        results.append(f"  DefaultPawnClass: None (CRITICAL!)")
    except Exception as e:
        results.append(f"GM ERROR: {e}")

    # Check B_Soulslike_Character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    try:
        char = unreal.EditorAssetLibrary.load_asset(char_path)
        if char:
            results.append(f"B_Soulslike_Character loaded: OK")
            gen_class = char.generated_class()
            if gen_class:
                results.append(f"  Generated class: {gen_class.get_name()}")
                # Check if mesh is set
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        mesh = cdo.get_editor_property("mesh")
                        if mesh:
                            skel_mesh = mesh.get_editor_property("skeletal_mesh")
                            results.append(f"  Mesh: {skel_mesh.get_name() if skel_mesh else 'None'}")
                        else:
                            results.append(f"  Mesh component: None (CRITICAL!)")
                    except Exception as e:
                        results.append(f"  Mesh check error: {e}")
        else:
            results.append(f"B_Soulslike_Character: FAILED to load")
    except Exception as e:
        results.append(f"B_Soulslike_Character ERROR: {e}")

    # Check AnimBP
    animBP_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    try:
        animBP = unreal.EditorAssetLibrary.load_asset(animBP_path)
        if animBP:
            results.append(f"ABP_SoulslikeCharacter_Additive: OK")
        else:
            results.append(f"ABP_SoulslikeCharacter_Additive: FAILED to load")
    except Exception as e:
        results.append(f"ABP ERROR: {e}")

    results.append("")
    results.append("=" * 60)

    # Write results
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))

    for line in results:
        unreal.log(line)

run()
