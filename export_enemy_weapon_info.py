"""
Export enemy weapon ChildActorClass and skeleton mesh info
Uses export_text to get raw asset data
"""
import unreal
import re

ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/enemy_weapon_info.txt"

def main():
    result = []
    result.append("=" * 60)
    result.append("ENEMY WEAPON AND SKELETON EXPORT")
    result.append("=" * 60)

    for bp_path in ENEMY_BLUEPRINTS:
        bp = unreal.load_asset(bp_path)
        if not bp:
            result.append(f"\nERROR: Could not load {bp_path}")
            continue

        bp_name = bp_path.split("/")[-1]
        result.append(f"\n=== {bp_name} ===")

        # Export the Blueprint text
        export_text = unreal.EditorAssetLibrary.export_text(bp_path)
        if not export_text:
            result.append("  ERROR: Could not export text")
            continue

        # Find ChildActorClass references (weapon class)
        child_actor_matches = re.findall(r'ChildActorClass=Class\'([^\']+)\'', export_text)
        for match in child_actor_matches:
            result.append(f"  ChildActorClass: {match}")

        # If no ChildActorClass found, look for ChildActorTemplate
        if not child_actor_matches:
            child_template_matches = re.findall(r'ChildActorClass.*?=.*?Blueprint.*?"([^"]+)"', export_text)
            for match in child_template_matches:
                result.append(f"  ChildActorClass (Blueprint): {match}")

        # Also search for any B_Item_AI_Weapon references
        weapon_refs = re.findall(r'B_Item_AI_Weapon[A-Za-z_]*', export_text)
        unique_weapons = list(set(weapon_refs))
        if unique_weapons:
            result.append(f"  Weapon References: {unique_weapons}")

        # Find SkeletalMesh references
        mesh_matches = re.findall(r'SkeletalMesh=SkeletalMesh\'([^\']+)\'', export_text)
        for match in mesh_matches:
            result.append(f"  SkeletalMesh: {match}")

        # Find Skeleton references
        skeleton_matches = re.findall(r'Skeleton=Skeleton\'([^\']+)\'', export_text)
        for match in skeleton_matches:
            result.append(f"  Skeleton: {match}")

        # Find any SK_ or SKM_ mesh references
        sk_matches = re.findall(r'(SKM?_[A-Za-z0-9_]+)', export_text)
        unique_sk = list(set(sk_matches))[:10]  # Limit to 10
        if unique_sk:
            result.append(f"  SK Mesh References: {unique_sk}")

    result.append("\n" + "=" * 60)
    result.append("DONE")
    result.append("=" * 60)

    output_text = "\n".join(result)

    # Write to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output_text)

    print(output_text)
    print(f"\nSaved to: {OUTPUT_FILE}")

if __name__ == "__main__":
    main()
