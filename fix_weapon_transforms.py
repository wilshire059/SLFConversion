#!/usr/bin/env python3
"""Remove IsZero() guards from weapon mesh transform application in BeginPlay"""

def main():
    cpp_path = "C:/scripts/slfconversion/Source/SLFConversion/Blueprints/SLFWeaponBase.cpp"

    with open(cpp_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Replace conditional transform application with unconditional
    old_code = '''	// Apply mesh transform offsets (from original Blueprint SCS component settings)
	if (WeaponMesh)
	{
		if (!DefaultMeshRelativeLocation.IsZero())
		{
			WeaponMesh->SetRelativeLocation(DefaultMeshRelativeLocation);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh location offset: %s"), *DefaultMeshRelativeLocation.ToString());
		}
		if (!DefaultMeshRelativeRotation.IsZero())
		{
			WeaponMesh->SetRelativeRotation(DefaultMeshRelativeRotation);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh rotation offset: %s"), *DefaultMeshRelativeRotation.ToString());
		}
	}'''

    new_code = '''	// Apply mesh transform offsets (from original Blueprint SCS component settings)
	// NOTE: Always apply transforms unconditionally - matches Blueprint SCS behavior
	// The IsZero() check was skipping valid transforms like (0,0,0) location with non-zero rotation
	if (WeaponMesh)
	{
		WeaponMesh->SetRelativeLocation(DefaultMeshRelativeLocation);
		WeaponMesh->SetRelativeRotation(DefaultMeshRelativeRotation);
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh transform - Location: %s, Rotation: %s"),
			*DefaultMeshRelativeLocation.ToString(),
			*DefaultMeshRelativeRotation.ToString());
	}'''

    if old_code not in content:
        print("ERROR: Could not find the old code pattern to replace!")
        return

    content = content.replace(old_code, new_code)

    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(content)

    print("Successfully updated SLFWeaponBase.cpp - removed IsZero() guards from mesh transform application")

if __name__ == "__main__":
    main()
