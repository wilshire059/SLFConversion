# Full demo level test
import unreal

def run():
    results = []
    results.append("=" * 60)
    results.append("DEMO LEVEL FULL TEST")
    results.append("=" * 60)
    
    # List all errors we encounter
    errors = []
    warnings = []
    
    # 1. Check demo level
    demo_level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        level = unreal.EditorAssetLibrary.load_asset(demo_level_path)
        if level:
            results.append(f"L_Demo_Showcase: OK")
        else:
            errors.append("L_Demo_Showcase failed to load")
    except Exception as e:
        errors.append(f"L_Demo_Showcase error: {e}")
    
    # 2. Check B_DemoRoom (floor and walls)
    demo_room_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_DemoRoom"
    try:
        demo_room = unreal.EditorAssetLibrary.load_asset(demo_room_path)
        if demo_room:
            results.append(f"B_DemoRoom: OK")
            gen = demo_room.generated_class()
            if gen:
                results.append(f"  Class: {gen.get_name()}")
        else:
            errors.append("B_DemoRoom failed to load")
    except Exception as e:
        errors.append(f"B_DemoRoom error: {e}")
    
    # 3. Check player character
    char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    try:
        char = unreal.EditorAssetLibrary.load_asset(char_path)
        if char:
            results.append(f"B_Soulslike_Character: OK")
        else:
            errors.append("B_Soulslike_Character failed to load")
    except Exception as e:
        errors.append(f"B_Soulslike_Character error: {e}")
    
    # 4. Check AnimBP
    animBP_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    try:
        animBP = unreal.EditorAssetLibrary.load_asset(animBP_path)
        if animBP:
            results.append(f"ABP_SoulslikeCharacter_Additive: OK")
        else:
            errors.append("ABP_SoulslikeCharacter_Additive failed to load")
    except Exception as e:
        errors.append(f"ABP_SoulslikeCharacter_Additive error: {e}")
    
    # 5. Check GameMode
    gm_path = "/Game/SoulslikeFramework/Blueprints/Global/GM_SoulslikeFramework"
    try:
        gm = unreal.EditorAssetLibrary.load_asset(gm_path)
        if gm:
            results.append(f"GM_SoulslikeFramework: OK")
        else:
            errors.append("GM_SoulslikeFramework failed to load")
    except Exception as e:
        errors.append(f"GM_SoulslikeFramework error: {e}")
    
    # 6. Check PlayerController
    pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    try:
        pc = unreal.EditorAssetLibrary.load_asset(pc_path)
        if pc:
            results.append(f"PC_SoulslikeFramework: OK")
        else:
            errors.append("PC_SoulslikeFramework failed to load")
    except Exception as e:
        errors.append(f"PC_SoulslikeFramework error: {e}")
    
    # Summary
    results.append("")
    results.append("=" * 60)
    if errors:
        results.append(f"ERRORS ({len(errors)}):")
        for e in errors:
            results.append(f"  - {e}")
    else:
        results.append("No errors found - all assets loaded")
    
    if warnings:
        results.append(f"WARNINGS ({len(warnings)}):")
        for w in warnings:
            results.append(f"  - {w}")
    results.append("=" * 60)
    
    # Write and log
    with open("C:/scripts/SLFConversion/demo_full_test.txt", 'w') as f:
        f.write("\n".join(results))
    
    for line in results:
        unreal.log(line)

run()
