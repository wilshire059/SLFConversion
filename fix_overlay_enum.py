# fix_overlay_enum.py
# Fix ESLFOverlayState enum to match Blueprint enum values
# Blueprint: Unarmed=0, Shield=1, OneHanded=2, TwoHanded=3, DualWield=4
# C++ (WRONG): Default=0, Unarmed=1, Shield=2, OneHanded=3, TwoHanded=4, DualWield=5

import os

# File 1: SLFEnums.h
ENUMS_FILE = "C:/scripts/slfconversion/Source/SLFConversion/SLFEnums.h"

OLD_ENUM = '''//////////////////////////////////////////////////////////////////////////
// ESLFOverlayState
// Replaces: /Game/SoulslikeFramework/Enums/E_OverlayState
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFOverlayState : uint8
{
	Default = 0		UMETA(DisplayName = "Default"),
	Unarmed = 1		UMETA(DisplayName = "Unarmed"),
	Shield = 2		UMETA(DisplayName = "Shield"),
	OneHanded = 3	UMETA(DisplayName = "OneHanded"),
	TwoHanded = 4	UMETA(DisplayName = "TwoHanded"),
	DualWield = 5	UMETA(DisplayName = "DualWield")
};'''

NEW_ENUM = '''//////////////////////////////////////////////////////////////////////////
// ESLFOverlayState
// Replaces: /Game/SoulslikeFramework/Enums/E_OverlayState
// CRITICAL: Values MUST match Blueprint enum for AnimGraph BlendListByEnum
//////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class ESLFOverlayState : uint8
{
	Unarmed = 0		UMETA(DisplayName = "Unarmed"),
	Shield = 1		UMETA(DisplayName = "Shield"),
	OneHanded = 2	UMETA(DisplayName = "OneHanded"),
	TwoHanded = 3	UMETA(DisplayName = "TwoHanded"),
	DualWield = 4	UMETA(DisplayName = "DualWield")
};'''

# Files that need Default -> Unarmed replacement
DEFAULT_USAGES = [
    "C:/scripts/slfconversion/Source/SLFConversion/Components/AC_EquipmentManager.cpp",
    "C:/scripts/slfconversion/Source/SLFConversion/Components/EquipmentManagerComponent.cpp",
    "C:/scripts/slfconversion/Source/SLFConversion/Animation/ABP_SoulslikeCharacter_Additive.h",
]

def fix_file(filepath, old_str, new_str):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        if old_str in content:
            content = content.replace(old_str, new_str)
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"FIXED: {os.path.basename(filepath)}")
            return True
        else:
            print(f"NOT FOUND: {os.path.basename(filepath)} - pattern not found")
            return False
    except Exception as e:
        print(f"ERROR: {os.path.basename(filepath)} - {e}")
        return False

def main():
    print("=" * 60)
    print("FIXING ESLFOverlayState enum to match Blueprint values")
    print("=" * 60)

    # Fix enum definition
    print("\n1. Fixing SLFEnums.h...")
    fix_file(ENUMS_FILE, OLD_ENUM, NEW_ENUM)

    # Fix all usages of ESLFOverlayState::Default
    print("\n2. Replacing ESLFOverlayState::Default with Unarmed...")
    for filepath in DEFAULT_USAGES:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()

        if "ESLFOverlayState::Default" in content:
            content = content.replace("ESLFOverlayState::Default", "ESLFOverlayState::Unarmed")
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"  UPDATED: {os.path.basename(filepath)}")
        else:
            print(f"  SKIP: {os.path.basename(filepath)} - no usages found")

    print("\n" + "=" * 60)
    print("DONE! Rebuild C++ to apply changes.")
    print("=" * 60)

if __name__ == "__main__":
    main()
