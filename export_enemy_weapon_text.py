"""
Export enemy Blueprint text to analyze weapon configurations.
Uses export_text to get all component data including ChildActorClass.
"""
import unreal
import re

ENEMY_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
]

def main():
    # Determine project
    project_name = "unknown"
    try:
        project_path = unreal.Paths.get_project_file_path()
        if "bp_only" in project_path.lower():
            project_name = "bp_only"
        elif "slfconversion" in project_path.lower():
            project_name = "SLFConversion"
    except:
        pass

    print(f"\n{'='*70}")
    print(f"ENEMY WEAPON EXPORT - {project_name}")
    print(f"{'='*70}")

    output_lines = []
    output_lines.append(f"# Enemy Weapon Export - {project_name}")
    output_lines.append("=" * 70)

    for bp_path in ENEMY_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        print(f"\n--- {bp_name} ---")
        output_lines.append(f"\n## {bp_name}")
        output_lines.append("-" * 40)

        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"  ERROR: Could not load")
            output_lines.append("ERROR: Could not load")
            continue

        # Export the Blueprint text
        try:
            text_export = unreal.EditorAssetLibrary.export_text(bp_path)
            if not text_export:
                print(f"  ERROR: export_text returned None")
                output_lines.append("ERROR: export_text returned None")
                continue

            # Save full export to file
            output_lines.append(f"Full export length: {len(text_export)} chars")

            # Look for ChildActorComponent and ChildActorClass
            # Pattern: Find lines with ChildActorComponent and nearby ChildActorClass
            lines = text_export.split('\n')
            in_weapon_block = False
            weapon_info = []

            for i, line in enumerate(lines):
                # Check for Weapon component
                if 'Name="Weapon"' in line or '"Weapon"' in line or 'VariableName="Weapon"' in line:
                    in_weapon_block = True
                    weapon_info.append(f"  Line {i}: {line.strip()}")

                # Check for ChildActorClass
                if 'ChildActorClass' in line:
                    weapon_info.append(f"  Line {i}: {line.strip()}")

                # Check for socket attachment
                if 'AttachToName' in line or 'AttachSocketName' in line or 'ParentSocket' in line:
                    weapon_info.append(f"  Line {i}: {line.strip()}")

                # Check for relative transforms
                if ('RelativeLocation' in line or 'RelativeRotation' in line) and in_weapon_block:
                    weapon_info.append(f"  Line {i}: {line.strip()}")

                # Reset block detection
                if in_weapon_block and 'End Object' in line:
                    in_weapon_block = False

            print(f"  Found {len(weapon_info)} weapon-related lines")
            output_lines.append(f"Weapon-related lines ({len(weapon_info)}):")
            for info in weapon_info:
                print(f"  {info}")
                output_lines.append(info)

            # Also extract using regex patterns
            print(f"\n  Regex extraction:")
            output_lines.append("\nRegex extraction:")

            # Find all ChildActorClass references
            child_class_matches = re.findall(r'ChildActorClass["\s=:]+([^\s,"\)]+)', text_export)
            for match in child_class_matches:
                print(f"    ChildActorClass: {match}")
                output_lines.append(f"    ChildActorClass: {match}")

            # Find relative rotations
            rot_matches = re.findall(r'RelativeRotation["\s=:]+\(([^)]+)\)', text_export)
            for match in rot_matches[:5]:  # First 5 only
                print(f"    RelativeRotation: ({match})")
                output_lines.append(f"    RelativeRotation: ({match})")

        except Exception as e:
            print(f"  ERROR: {e}")
            output_lines.append(f"ERROR: {e}")

    # Save output
    output_file = f"C:/scripts/SLFConversion/migration_cache/enemy_weapon_text_{project_name}.txt"
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(output_lines))

    print(f"\n{'='*70}")
    print(f"Saved to: {output_file}")
    print(f"{'='*70}")

if __name__ == "__main__":
    main()
