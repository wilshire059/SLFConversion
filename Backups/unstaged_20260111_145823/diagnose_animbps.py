# diagnose_animbps.py
# Use SLFAutomationLibrary to diagnose AnimBPs

import unreal

ANIMBP_PATHS = {
    "Player": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "Enemy": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "Boss": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "NPC": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
}

results = []

for name, path in ANIMBP_PATHS.items():
    results.append(f"\n=== {name} ===")
    
    bp = unreal.load_asset(path)
    if not bp:
        results.append(f"  ERROR: Cannot load")
        continue
    
    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    results.append(f"  Parent: {parent}")
    
    # Check if C++
    if "/Script/" in parent:
        results.append(f"  [OK] C++ parent")
    else:
        results.append(f"  [WARN] Blueprint parent - NativeUpdateAnimation won't run!")
    
    # Diagnose AnimGraph nodes
    diag = unreal.SLFAutomationLibrary.diagnose_anim_graph_nodes(bp)
    results.append(f"  AnimGraph Diagnosis:\n{diag}")
    
    # Get compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        results.append(f"  Compile Errors: {errors}")
    else:
        results.append(f"  Compile Errors: None")

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/animbp_diagnosis.txt", "w") as f:
    f.write("\n".join(results))

unreal.log_warning("Diagnosis written to migration_cache/animbp_diagnosis.txt")
