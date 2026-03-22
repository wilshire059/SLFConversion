# migrate_enemy_animbps.py
# Properly migrate enemy AnimBPs using the multi-phase approach
# Based on run_migration.py but targeted at just the 3 enemy AnimBPs

import unreal
import gc
import time

# Enemy AnimBPs to migrate
ENEMY_ANIMBPS = {
    "ABP_SoulslikeEnemy": {
        "path": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "cpp_parent": "/Script/SLFConversion.ABP_SoulslikeEnemy",
    },
    "ABP_SoulslikeNPC": {
        "path": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
        "cpp_parent": "/Script/SLFConversion.ABP_SoulslikeNPC",
    },
    "ABP_SoulslikeBossNew": {
        "path": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "cpp_parent": "/Script/SLFConversion.ABP_SoulslikeBossNew",
    },
}


def main():
    print("=" * 60)
    print("ENEMY ANIMBP MIGRATION (Multi-Phase)")
    print("=" * 60)

    # =========================================================================
    # PHASE 1: LOAD ALL ANIMBPS
    # =========================================================================
    print("")
    print("=== PHASE 1: Load AnimBPs ===")
    loaded_bps = []
    for name, info in ENEMY_ANIMBPS.items():
        print(f"Loading {name}...")
        try:
            bp = unreal.load_asset(info["path"])
            if bp:
                loaded_bps.append((name, bp, info))
                print(f"  OK: Loaded")
            else:
                print(f"  FAILED: Could not load")
        except Exception as e:
            print(f"  ERROR: {e}")

    print(f"\nLoaded {len(loaded_bps)} AnimBPs")

    # =========================================================================
    # PHASE 2: CLEAR EVENTGRAPHS (NoCompile)
    # =========================================================================
    print("")
    print("=== PHASE 2: Clear EventGraphs (NoCompile) ===")
    for name, bp, info in loaded_bps:
        print(f"Clearing {name}...")
        try:
            # Use NoCompile version to avoid triggering broken compilation
            result = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            if result:
                print(f"  OK: EventGraph cleared")
            else:
                print(f"  WARNING: Clear returned False")
        except Exception as e:
            print(f"  ERROR: {e}")

    # =========================================================================
    # PHASE 3: REPARENT TO C++ (Try it despite warnings)
    # =========================================================================
    print("")
    print("=== PHASE 3: Reparent to C++ AnimInstance ===")
    for name, bp, info in loaded_bps:
        cpp_parent = info["cpp_parent"]
        print(f"Reparenting {name} to {cpp_parent.split('.')[-1]}...")
        try:
            result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
            if result:
                print(f"  OK: Reparented")
            else:
                print(f"  WARNING: Reparent returned False")
        except Exception as e:
            print(f"  ERROR: {e}")

    # =========================================================================
    # PHASE 4: SAVE ALL
    # =========================================================================
    print("")
    print("=== PHASE 4: Save All ===")
    gc.collect()
    unreal.SystemLibrary.collect_garbage()
    time.sleep(1.0)

    saved = 0
    for name, bp, info in loaded_bps:
        path = info["path"]
        print(f"Saving {name}...")
        try:
            result = unreal.EditorAssetLibrary.save_asset(path)
            if result:
                print(f"  OK: Saved")
                saved += 1
            else:
                print(f"  WARNING: Save returned False")
        except Exception as e:
            print(f"  ERROR: {e}")

    # =========================================================================
    # SUMMARY
    # =========================================================================
    print("")
    print("=" * 60)
    print(f"RESULT: {saved}/{len(loaded_bps)} saved successfully")
    print("=" * 60)
    print("")
    print("The C++ AnimInstance classes (UABP_SoulslikeEnemy, etc.)")
    print("now handle variable updates via NativeUpdateAnimation().")
    print("")
    print("Animation variables updated by C++:")
    print("  - GroundSpeed, Velocity, IsFalling, Direction")
    print("  - SoulslikeEnemy, SoulslikeCharacter")
    print("  - MovementComponent")
    print("")
    print("AnimGraph reads these variables for locomotion animations.")


if __name__ == "__main__":
    main()
