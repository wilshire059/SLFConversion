#!/usr/bin/env python3
"""Check IK Rig configuration for hand IK"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

IK_RIG_PATHS = [
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Rigs/IK_Mannequin",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/IK_HitReact",
    "/Game/SoulslikeFramework/Demo/SKM/Mannequin_UE4/Rigs/IK_UE4_Mannequin",
]

def check_ik_rig(path):
    """Check IK Rig asset configuration"""
    log(f"\n=== {path.split('/')[-1]} ===")

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        log(f"ERROR: Could not load {path}")
        return

    log(f"Asset type: {asset.get_class().get_name()}")

    # Try to get IK Rig definition properties
    try:
        # Get skeleton
        skeleton = asset.get_editor_property("skeleton")
        if skeleton:
            log(f"Skeleton: {skeleton.get_name()}")

        # Try to get solvers/goals
        try:
            solvers = asset.get_editor_property("solvers")
            if solvers:
                log(f"Solvers: {len(solvers)}")
                for solver in solvers:
                    solver_name = solver.get_name() if solver else "None"
                    log(f"  - {solver_name}")
        except Exception as e:
            log(f"  Could not get solvers: {e}")

        # Try to get goals
        try:
            goals = asset.get_editor_property("goals")
            if goals:
                log(f"Goals: {len(goals)}")
                for goal in goals:
                    goal_name = goal.get_name() if goal else "None"
                    log(f"  - {goal_name}")
        except Exception as e:
            log(f"  Could not get goals: {e}")

    except Exception as e:
        log(f"Error: {e}")

    # List all available properties
    log("\nAvailable editor properties:")
    try:
        cls = asset.get_class()
        for prop in dir(asset):
            if not prop.startswith('_') and not callable(getattr(asset, prop)):
                try:
                    val = asset.get_editor_property(prop)
                    val_str = str(val)[:100] if val is not None else "None"
                    log(f"  {prop}: {val_str}")
                except:
                    pass
    except Exception as e:
        log(f"  Error listing properties: {e}")

def main():
    log("=" * 70)
    log("IK RIG CHECK")
    log("=" * 70)

    for path in IK_RIG_PATHS:
        check_ik_rig(path)

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
