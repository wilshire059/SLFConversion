"""
Extract readable strings from uasset files to find mesh references.
Run this with standard Python, not Unreal Python.
"""
import re
import os

def extract_strings(filepath, min_length=8):
    """Extract printable strings from a binary file."""
    with open(filepath, 'rb') as f:
        data = f.read()

    # Find sequences of printable ASCII characters
    pattern = rb'[\x20-\x7E]{' + str(min_length).encode() + rb',}'
    matches = re.findall(pattern, data)

    return [m.decode('ascii', errors='ignore') for m in matches]

def main():
    backup_dir = "C:/scripts/bp_only/Content/SoulslikeFramework"

    files_to_check = [
        f"{backup_dir}/Data/BaseCharacters/DA_QuinnMeshDefault.uasset",
        f"{backup_dir}/Data/PDA_DefaultMeshData.uasset",
        f"{backup_dir}/Blueprints/_Characters/B_Soulslike_Character.uasset",
    ]

    keywords = ['Mesh', 'Quinn', 'Manny', 'SKM', 'SK_', 'Head', 'Body', 'Arms', 'Lower', 'Upper', 'Game/', 'Content/', 'SkeletalMesh']

    for filepath in files_to_check:
        if os.path.exists(filepath):
            print(f"\n{'='*60}")
            print(f"FILE: {os.path.basename(filepath)}")
            print(f"{'='*60}")

            strings = extract_strings(filepath)
            relevant = [s for s in strings if any(kw.lower() in s.lower() for kw in keywords)]

            if relevant:
                for s in relevant[:30]:
                    print(f"  {s}")
            else:
                # Show some strings anyway to understand the format
                print("  No keyword matches, showing first 20 strings:")
                for s in strings[:20]:
                    print(f"  {s}")
        else:
            print(f"NOT FOUND: {filepath}")

if __name__ == "__main__":
    main()
