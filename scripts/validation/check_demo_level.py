# Check demo level status
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/demo_level_check.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("DEMO LEVEL CHECK")
    results.append("=" * 60)
    
    # Check the demo room Blueprint
    demo_room_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_DemoRoom"
    try:
        demo_room = unreal.EditorAssetLibrary.load_asset(demo_room_path)
        if demo_room:
            results.append(f"B_DemoRoom loaded: OK")
            gen_class = demo_room.generated_class()
            if gen_class:
                results.append(f"  Generated class: {gen_class.get_name()}")
                # Check parent class
                parent = gen_class.get_super_class()
                if parent:
                    results.append(f"  Parent class: {parent.get_name()}")
        else:
            results.append(f"B_DemoRoom: FAILED to load")
    except Exception as e:
        results.append(f"B_DemoRoom ERROR: {e}")
        
    # Check level
    demo_level_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/L_Demo_Showcase"
    try:
        level = unreal.EditorAssetLibrary.load_asset(demo_level_path)
        if level:
            results.append(f"L_Demo_Showcase level: OK")
        else:
            results.append(f"L_Demo_Showcase: FAILED to load")
    except Exception as e:
        results.append(f"L_Demo_Showcase ERROR: {e}")
        
    # Check character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    try:
        char = unreal.EditorAssetLibrary.load_asset(char_path)
        if char:
            results.append(f"B_Soulslike_Character: OK")
            gen_class = char.generated_class()
            if gen_class:
                results.append(f"  Generated class: {gen_class.get_name()}")
                parent = gen_class.get_super_class()
                if parent:
                    results.append(f"  Parent class: {parent.get_name()}")
        else:
            results.append(f"B_Soulslike_Character: FAILED to load")
    except Exception as e:
        results.append(f"B_Soulslike_Character ERROR: {e}")
        
    # Check GameMode's default pawn
    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    try:
        gm = unreal.EditorAssetLibrary.load_asset(gm_path)
        if gm:
            results.append(f"GM_SoulslikeFramework: OK")
            gen_class = gm.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    try:
                        pawn_class = cdo.get_editor_property("default_pawn_class")
                        if pawn_class:
                            results.append(f"  DefaultPawnClass: {pawn_class.get_name()}")
                        else:
                            results.append(f"  DefaultPawnClass: None (CRITICAL)")
                    except Exception as e:
                        results.append(f"  DefaultPawnClass error: {e}")
    except Exception as e:
        results.append(f"GM_SoulslikeFramework ERROR: {e}")
    
    # Write results
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))
    
    for line in results:
        unreal.log(line)
    
run()
