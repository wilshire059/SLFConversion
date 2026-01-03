import shutil
import os

# Files to restore from bp_only
files_to_copy = [
    # AnimBPs (already done but let's ensure)
    ("Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC.uasset", None),
    ("Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy.uasset", None),
    ("Content/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew.uasset", None),
    # NPCs
    ("Content/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC.uasset", None),
    ("Content/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide.uasset", None),
    ("Content/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor.uasset", None),
    # Enemies
    ("Content/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard.uasset", None),
    ("Content/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase.uasset", None),
    ("Content/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth.uasset", None),
]

src_base = r"C:\scripts\bp_only"
dst_base = r"C:\scripts\SLFConversion"

for rel_path, _ in files_to_copy:
    src = os.path.join(src_base, rel_path)
    dst = os.path.join(dst_base, rel_path)

    if os.path.exists(src):
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        shutil.copy2(src, dst)
        print(f"Restored: {os.path.basename(rel_path)}")
    else:
        print(f"NOT FOUND: {src}")

print("\nDone!")
