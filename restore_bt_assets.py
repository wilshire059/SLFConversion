# restore_bt_assets.py
# Surgically restore BT assets from bp_only backup
# This restores the original working BT structure without affecting other assets

import os
import shutil

# Source and destination paths
BP_ONLY = "C:/scripts/bp_only/Content/SoulslikeFramework/Blueprints/_AI"
SLF_CONVERSION = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Blueprints/_AI"

# BT assets to restore (these have different decorators in SLFConversion)
BT_ASSETS = [
    "BT_Enemy.uasset",
    "Subtrees/BT_Combat.uasset",
    "Subtrees/BT_Idle.uasset",
    "Subtrees/BT_Investigating.uasset",
    "Subtrees/BT_OutOfBounds.uasset",
    "Subtrees/BT_PatrolPath.uasset",
    "Subtrees/BT_PoiseBroken.uasset",
    "Subtrees/BT_RandomRoam.uasset",
    "Subtrees/BT_Uninterruptable.uasset",
]

# Also restore the blackboard (uses Blueprint enum which matches original structure)
BB_ASSETS = [
    "BB_Standard.uasset",
]


def main():
    print("="*60)
    print("SURGICAL BT ASSET RESTORE")
    print("From: bp_only")
    print("To: SLFConversion")
    print("="*60)

    restored = 0
    failed = 0

    # Restore BT assets
    for asset in BT_ASSETS:
        src = os.path.join(BP_ONLY, asset)
        dst = os.path.join(SLF_CONVERSION, asset)

        if os.path.exists(src):
            # Ensure destination directory exists
            os.makedirs(os.path.dirname(dst), exist_ok=True)

            # Copy
            shutil.copy2(src, dst)
            print(f"OK: {asset}")
            restored += 1
        else:
            print(f"MISSING: {asset}")
            failed += 1

    # Restore blackboard
    for asset in BB_ASSETS:
        src = os.path.join(BP_ONLY, asset)
        dst = os.path.join(SLF_CONVERSION, asset)

        if os.path.exists(src):
            shutil.copy2(src, dst)
            print(f"OK: {asset}")
            restored += 1
        else:
            print(f"MISSING: {asset}")
            failed += 1

    print("="*60)
    print(f"RESULT: {restored} restored, {failed} failed")
    print("="*60)
    print()
    print("NOTE: The original BTs use InCombat boolean for combat gating.")
    print("The C++ code now sets InCombat=true when entering combat.")
    print("BTS_ChaseBounds clears InCombat when out of bounds.")


if __name__ == "__main__":
    main()
