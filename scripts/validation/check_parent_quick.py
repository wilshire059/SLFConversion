"""Quick parent class check after migration"""
import unreal

# Check a few key Blueprints to confirm reparenting worked
check_list = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
]

print("=" * 60)
print("PARENT CLASS CHECK")
print("=" * 60)

for bp_path in check_list:
    bp = unreal.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        parent_class = gen_class.get_super_class() if gen_class else None
        parent_name = parent_class.get_name() if parent_class else "NONE"
        print(f"  {bp_path.split('/')[-1]}: parent={parent_name}")
    else:
        print(f"  [FAIL] Could not load: {bp_path}")

print("=" * 60)
