# Patch SLFWeaponBase.cpp to add world position logging
import re

FILE_PATH = "C:/scripts/slfconversion/Source/SLFConversion/Blueprints/SLFWeaponBase.cpp"

with open(FILE_PATH, 'r', encoding='utf-8') as f:
    content = f.read()

old_code = '''		// Debug: Log WeaponMesh transform AFTER attachment to verify it is preserved
		if (WeaponMesh)
		{
			FVector PostAttachLoc = WeaponMesh->GetRelativeLocation();
			FRotator PostAttachRot = WeaponMesh->GetRelativeRotation();
			UE_LOG(LogTemp, Log, TEXT("[Weapon] POST-ATTACH WeaponMesh transform - Location: %s, Rotation: %s"),
				*PostAttachLoc.ToString(), *PostAttachRot.ToString());
		}

		// Apply rotation offset'''

new_code = '''		// Debug: Log WeaponMesh transform AFTER attachment to verify it is preserved
		if (WeaponMesh)
		{
			FVector PostAttachLoc = WeaponMesh->GetRelativeLocation();
			FRotator PostAttachRot = WeaponMesh->GetRelativeRotation();
			UE_LOG(LogTemp, Log, TEXT("[Weapon] POST-ATTACH WeaponMesh Relative - Location: %s, Rotation: %s"),
				*PostAttachLoc.ToString(), *PostAttachRot.ToString());

			// Log WORLD position to verify the offset is actually visible
			FVector WorldLoc = WeaponMesh->GetComponentLocation();
			FRotator WorldRot = WeaponMesh->GetComponentRotation();
			UE_LOG(LogTemp, Log, TEXT("[Weapon] POST-ATTACH WeaponMesh WORLD - Location: %s, Rotation: %s"),
				*WorldLoc.ToString(), *WorldRot.ToString());

			// Log socket world position for comparison
			FVector SocketLoc = MeshComponent->GetSocketLocation(SocketName);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Socket '%s' WORLD Location: %s"),
				*SocketName.ToString(), *SocketLoc.ToString());

			// Calculate actual offset from socket
			FVector Offset = WorldLoc - SocketLoc;
			UE_LOG(LogTemp, Log, TEXT("[Weapon] WeaponMesh offset from socket: %s (Expected Z: %.2f)"),
				*Offset.ToString(), PostAttachLoc.Z);
		}

		// Apply rotation offset'''

if old_code in content:
    content = content.replace(old_code, new_code)
    with open(FILE_PATH, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Patched SLFWeaponBase.cpp with world position logging")
else:
    print("ERROR: Could not find target code block")
    # Show what we're looking for
    print("\nLooking for:")
    print(old_code[:200])
