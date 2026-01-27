"""
Parse weapon T3D files and extract stat data
"""
import os
import re
import json

def parse_t3d_file(filepath):
    """Parse a T3D file and extract weapon stat data"""
    # Try different encodings
    content = None
    for encoding in ['utf-8', 'utf-16', 'latin-1']:
        try:
            with open(filepath, 'r', encoding=encoding) as f:
                content = f.read()
            break
        except:
            continue

    if not content:
        print(f"Could not read {filepath}")
        return None

    # Remove null bytes if present
    content = content.replace('\x00', '')

    weapon_data = {}

    # Extract weapon name
    name_match = re.search(r'Name="([^"]+)"', content)
    if name_match:
        weapon_data['name'] = name_match.group(1)

    # Extract HasStatScaling
    has_scaling_match = re.search(r'HasStatScaling\?_[0-9A-F_]+=(True|False)', content)
    if has_scaling_match:
        weapon_data['has_stat_scaling'] = has_scaling_match.group(1) == 'True'

    # Extract HasStatRequirement
    has_req_match = re.search(r'HasStatRequirement\?_[0-9A-F_]+=(True|False)', content)
    if has_req_match:
        weapon_data['has_stat_requirement'] = has_req_match.group(1) == 'True'

    # Extract ScalingInfo - directly look for the pattern in content
    # Format: ScalingInfo_...=(...entries...)
    # Use a simpler approach - just extract entries after ScalingInfo pattern
    scaling_entries = re.findall(r'ScalingInfo_[0-9A-F_]+=\([^)]*\(\(TagName="([^"]+)"\),\s*(\w+)\)', content)
    if scaling_entries:
        weapon_data['scaling_info'] = {}
        for tag, enum_val in scaling_entries:
            short_name = tag.split('.')[-1]
            scaling_map = {
                'NewEnumerator0': 'S',
                'NewEnumerator1': 'A',
                'NewEnumerator2': 'B',
                'NewEnumerator3': 'C',
                'NewEnumerator4': 'D',
                'NewEnumerator5': 'E',
            }
            grade = scaling_map.get(enum_val, enum_val)
            weapon_data['scaling_info'][short_name] = grade
    else:
        # Try alternative pattern - find all entries between ScalingInfo and next field
        match = re.search(r'ScalingInfo_[0-9A-F_]+=\(([^=]+)\)', content)
        if match:
            scaling_str = match.group(0)
            entries = re.findall(r'\(\(TagName="([^"]+)"\),\s*(\w+)\)', scaling_str)
            if entries:
                weapon_data['scaling_info'] = {}
                for tag, enum_val in entries:
                    short_name = tag.split('.')[-1]
                    scaling_map = {
                        'NewEnumerator0': 'S',
                        'NewEnumerator1': 'A',
                        'NewEnumerator2': 'B',
                        'NewEnumerator3': 'C',
                        'NewEnumerator4': 'D',
                        'NewEnumerator5': 'E',
                    }
                    grade = scaling_map.get(enum_val, enum_val)
                    weapon_data['scaling_info'][short_name] = grade

    # Extract StatRequirementInfo
    req_entries = re.findall(r'StatRequirementInfo_[0-9A-F_]+=\([^)]*\(\(TagName="([^"]+)"\),\s*(\d+)\)', content)
    if req_entries:
        weapon_data['stat_requirement_info'] = {}
        for tag, value in req_entries:
            short_name = tag.split('.')[-1]
            weapon_data['stat_requirement_info'][short_name] = int(value)
    else:
        match = re.search(r'StatRequirementInfo_[0-9A-F_]+=\(([^=]+)\)', content)
        if match:
            req_str = match.group(0)
            entries = re.findall(r'\(\(TagName="([^"]+)"\),\s*(\d+)\)', req_str)
            if entries:
                weapon_data['stat_requirement_info'] = {}
                for tag, value in entries:
                    short_name = tag.split('.')[-1]
                    weapon_data['stat_requirement_info'][short_name] = int(value)

    # Extract StatChanges
    changes_entries = re.findall(r'StatChanges_[0-9A-F_]+=\([^)]*\(\(TagName="([^"]+)"\),\s*(\d+)\)', content)
    if changes_entries:
        weapon_data['stat_changes'] = {}
        for tag, value in changes_entries:
            parts = tag.split('.')
            if 'AttackPower' in parts:
                idx = parts.index('AttackPower')
                if idx + 1 < len(parts):
                    damage_type = parts[idx + 1]
                    weapon_data['stat_changes'][damage_type] = int(value)
            else:
                short_name = parts[-1]
                weapon_data['stat_changes'][short_name] = int(value)
    else:
        match = re.search(r'StatChanges_[0-9A-F_]+=\(([^=]+)\)', content)
        if match:
            changes_str = match.group(0)
            entries = re.findall(r'\(\(TagName="([^"]+)"\),\s*(\d+)\)', changes_str)
            if entries:
                weapon_data['stat_changes'] = {}
                for tag, value in entries:
                    parts = tag.split('.')
                    if 'AttackPower' in parts:
                        idx = parts.index('AttackPower')
                        if idx + 1 < len(parts):
                            damage_type = parts[idx + 1]
                            weapon_data['stat_changes'][damage_type] = int(value)
                    else:
                        short_name = parts[-1]
                        weapon_data['stat_changes'][short_name] = int(value)

    # Extract MinPoiseDamage and MaxPoiseDamage
    min_poise_match = re.search(r'MinPoiseDamage_[0-9A-F_]+=([\d.]+)', content)
    if min_poise_match:
        weapon_data['min_poise_damage'] = float(min_poise_match.group(1))

    max_poise_match = re.search(r'MaxPoiseDamage_[0-9A-F_]+=([\d.]+)', content)
    if max_poise_match:
        weapon_data['max_poise_damage'] = float(max_poise_match.group(1))

    # Extract StaminaMultiplier
    stamina_match = re.search(r'StaminaMultiplier_[0-9A-F_]+=([\d.]+)', content)
    if stamina_match:
        weapon_data['stamina_multiplier'] = float(stamina_match.group(1))

    return weapon_data


def main():
    t3d_dir = "C:/scripts/SLFConversion/migration_cache/weapon_t3d/"
    output_file = "C:/scripts/SLFConversion/migration_cache/weapon_stats_parsed.json"

    all_weapons = {}

    for filename in os.listdir(t3d_dir):
        if filename.endswith('.t3d'):
            filepath = os.path.join(t3d_dir, filename)
            print(f"\nParsing {filename}...")
            weapon_data = parse_t3d_file(filepath)
            if weapon_data:
                weapon_name = weapon_data.get('name', filename.replace('.t3d', ''))
                all_weapons[weapon_name] = weapon_data
                print(f"  Name: {weapon_name}")
                if 'has_stat_scaling' in weapon_data:
                    print(f"  Has Scaling: {weapon_data['has_stat_scaling']}")
                if 'scaling_info' in weapon_data:
                    print(f"  Scaling: {weapon_data['scaling_info']}")
                if 'has_stat_requirement' in weapon_data:
                    print(f"  Has Requirement: {weapon_data['has_stat_requirement']}")
                if 'stat_requirement_info' in weapon_data:
                    print(f"  Requirements: {weapon_data['stat_requirement_info']}")
                if 'stat_changes' in weapon_data:
                    print(f"  Stat Changes: {weapon_data['stat_changes']}")

    # Save to JSON
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(all_weapons, f, indent=2)
    print(f"\n\nSaved {len(all_weapons)} weapons to {output_file}")


if __name__ == "__main__":
    main()
