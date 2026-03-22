#!/usr/bin/env python3
"""
Fix duplicate type definitions in component headers.
Adds includes to shared type headers and removes local definitions.
"""

import os
import re

SOURCE_DIR = r"C:\scripts\SLFConversion\Source\SLFConversion"

# Types defined in SLFEnums.h
ENUM_TYPES = [
    "ESLFValueType", "ESLFDirection", "ESLFItemCategory", "ESLFAIBossEncounterType",
    "ESLFAIStates", "ESLFHitReactType", "ESLFOverlayState", "ESLFWeaponAbilityHandle",
    "ESLFLadderClimbState", "ESLFProgress", "ESLFSaveBehavior", "ESLFActionWeaponSlot",
    "ESLFAISenses", "ESLFAIStateHandle", "ESLFAIStrafeLocations", "ESLFAttackPower"
]

# Types defined in SLFGameTypes.h
STRUCT_TYPES = [
    "FSLFDodgeMontages", "FSLFAiBossPhase", "FSLFCurrentEquipment",
    "FSLFEquipmentItemsSaveInfo", "FSLFLootItem", "FSLFWeightedLoot",
    "FSLFProgressSaveInfo", "FSLFSaveGameInfo", "FSLFCardinalData",
    "FSLFItemInfo", "FSLFWeaponAttackPower"
]

# Types defined in SLFStatTypes.h
STAT_TYPES = [
    "FSLFRegen", "FSLFStatInfo", "FSLFStatBehavior", "FSLFStatOverride"
]

def fix_header(filepath):
    """Fix a single header file."""
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    original = content
    modified = False

    # Check if file has local type definitions that need removal
    has_enum = bool(re.search(r'^UENUM\(BlueprintType\)\s*\nenum class (E\w+)', content, re.MULTILINE))
    has_struct = bool(re.search(r'^USTRUCT\(BlueprintType\)\s*\nstruct (F\w+)', content, re.MULTILINE))

    if not has_enum and not has_struct:
        return False

    # Find the #pragma once line and add includes after it if needed
    pragma_match = re.search(r'#pragma once\n', content)
    if not pragma_match:
        return False

    # Check which includes are needed
    needs_enums = any(t in content for t in ENUM_TYPES)
    needs_types = any(t in content for t in STRUCT_TYPES)
    needs_stats = any(t in content for t in STAT_TYPES)

    # Build new includes
    new_includes = []
    if needs_enums and '#include "SLFEnums.h"' not in content:
        new_includes.append('#include "SLFEnums.h"')
    if needs_types and '#include "SLFGameTypes.h"' not in content:
        new_includes.append('#include "SLFGameTypes.h"')
    if needs_stats and '#include "SLFStatTypes.h"' not in content:
        new_includes.append('#include "SLFStatTypes.h"')

    # Find insertion point (after #include "...Component.generated.h" or similar)
    generated_match = re.search(r'(#include "[^"]+\.generated\.h")\n', content)
    if generated_match and new_includes:
        insert_pos = generated_match.start()
        # Insert before the .generated.h include
        new_include_text = '\n'.join(new_includes) + '\n'
        content = content[:insert_pos] + new_include_text + content[insert_pos:]
        modified = True

    # Remove local enum definitions that duplicate shared ones
    for enum_type in ENUM_TYPES:
        # Pattern to match full enum definition
        pattern = rf'(/\*\*[^*]*\*/\s*)?UENUM\(BlueprintType\)\s*\nenum class {enum_type}\s*:\s*uint8\s*\n\{{[^}}]+\}};?\n?'
        if re.search(pattern, content, re.MULTILINE | re.DOTALL):
            content = re.sub(pattern, '', content, flags=re.MULTILINE | re.DOTALL)
            modified = True
            print(f"  Removed duplicate enum: {enum_type}")

    # Remove local struct definitions that duplicate shared ones
    for struct_type in STRUCT_TYPES + STAT_TYPES:
        # Pattern to match full struct definition
        pattern = rf'(/\*\*[^*]*\*/\s*)?USTRUCT\(BlueprintType\)\s*\nstruct {struct_type}\s*\n\{{\s*\n\s*GENERATED_BODY\(\)[^}}]+\}};?\n?'
        if re.search(pattern, content, re.MULTILINE | re.DOTALL):
            content = re.sub(pattern, '', content, flags=re.MULTILINE | re.DOTALL)
            modified = True
            print(f"  Removed duplicate struct: {struct_type}")

    if modified and content != original:
        # Clean up multiple blank lines
        content = re.sub(r'\n{3,}', '\n\n', content)
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        return True
    return False

def main():
    print("Fixing duplicate type definitions in headers...")

    # Files to process
    dirs_to_check = [
        os.path.join(SOURCE_DIR, "Components"),
        os.path.join(SOURCE_DIR, "Interfaces"),
        os.path.join(SOURCE_DIR, "Blueprints"),
        os.path.join(SOURCE_DIR, "Blueprints", "Actions"),
    ]

    fixed_count = 0
    for dir_path in dirs_to_check:
        if not os.path.exists(dir_path):
            continue
        for filename in os.listdir(dir_path):
            if filename.endswith('.h'):
                filepath = os.path.join(dir_path, filename)
                print(f"Checking: {filename}")
                if fix_header(filepath):
                    fixed_count += 1
                    print(f"  Fixed!")

    print(f"\nDone! Fixed {fixed_count} files.")

if __name__ == "__main__":
    main()
