# restore_animbp_assets.py
# Surgically restore AnimBP assets from bp_only backup
# This restores the original working animations

import os
import shutil

# Source and destination paths
BP_ONLY = "C:/scripts/bp_only/Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP"
SLF_CONVERSION = "C:/scripts/SLFConversion/Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP"

# AnimBP assets to restore
ANIMBP_ASSETS = [
    "ABP_SoulslikeEnemy.uasset",
    "ABP_SoulslikeNPC.uasset",
    "ABP_SoulslikeBossNew.uasset",
]


def main():
    print("=" * 60)
    print("SURGICAL ANIMBP ASSET RESTORE")
    print("From: bp_only")
    print("To: SLFConversion")
    print("=" * 60)

    restored = 0
    failed = 0

    for asset in ANIMBP_ASSETS:
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

    print("=" * 60)
    print(f"RESULT: {restored} restored, {failed} failed")
    print("=" * 60)
    print()
    print("NOTE: Original AnimBPs have Blueprint EventGraph logic")
    print("that updates animation variables. This should fix animations.")


if __name__ == "__main__":
    main()
