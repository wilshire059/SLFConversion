"""
Run C++ destructible diagnostics via USLFAutomationLibrary.
"""

import unreal

def run_diag():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"
    gc_path = "/Game/SoulslikeFramework/Meshes/SM/Barrel/Destructible/GC_Barrel"

    print("\n" + "="*80)
    print("Running C++ Destructible Diagnostics")
    print("="*80)

    # Run diagnosis
    result = unreal.SLFAutomationLibrary.diagnose_destructible(bp_path)
    print("\n--- DiagnoseDestructible Result ---")
    print(result)

    # Run comparison
    result2 = unreal.SLFAutomationLibrary.compare_destructible_settings(bp_path)
    print("\n--- CompareDestructibleSettings Result ---")
    print(result2)

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_cpp_diagnosis.txt"
    with open(output_path, "w") as f:
        f.write("=== DiagnoseDestructible ===\n")
        f.write(result)
        f.write("\n\n=== CompareDestructibleSettings ===\n")
        f.write(result2)
    print(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    run_diag()
