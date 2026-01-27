#!/usr/bin/env python3
"""Update SLFWeaponBase.cpp with interface implementations"""

import re

def main():
    cpp_path = "C:/scripts/slfconversion/Source/SLFConversion/Blueprints/SLFWeaponBase.cpp"

    with open(cpp_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Fix BeginPlay to not call AttachToOwnerSocket
    old_beginplay_end = '''	// NOTE: Damage is handled directly by CollisionManager, not via OnActorTraced event.
	// The weapon's OnActorTraced was sending 0 damage because GetAttackPowerStats returns empty.
	// CollisionManager applies correct damage values directly.
	if (CollisionManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Weapon] CollisionManager exists on %s"), *GetName());
		// Damage handled in CollisionManager::ProcessHit directly - no event binding needed
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] No CollisionManager found on %s!"), *GetName());
	}

	// ═══════════════════════════════════════════════════════════════════════════════
	// SOCKET ATTACHMENT - Attach weapon to character's skeletal mesh socket
	// ═══════════════════════════════════════════════════════════════════════════════
	AttachToOwnerSocket();
}'''

    new_beginplay_end = '''	// NOTE: CollisionManager handles damage directly
	if (CollisionManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Weapon] CollisionManager exists on %s"), *GetName());
	}

	// NOTE: DO NOT attach here - OnWeaponEquip handles attachment
	// OnWeaponEquip is called by AC_EquipmentManager after spawn
}'''

    content = content.replace(old_beginplay_end, new_beginplay_end)

    # 2. Update AttachToOwnerSocket signature and use bRightHand parameter
    old_attach_func = '''void ASLFWeaponBase::AttachToOwnerSocket()
{
	// Get the character that owns this weapon (set as Instigator during spawn)
	APawn* InstigatorPawn = GetInstigator();
	if (!IsValid(InstigatorPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] AttachToOwnerSocket - No Instigator, cannot attach"));
		return;
	}

	// Get the skeletal mesh component from the character
	USkeletalMeshComponent* MeshComponent = nullptr;

	// Try to get the mesh from Character first
	if (ACharacter* Character = Cast<ACharacter>(InstigatorPawn))
	{
		MeshComponent = Character->GetMesh();
	}
	else
	{
		// Fallback: try to find any skeletal mesh component
		MeshComponent = InstigatorPawn->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!IsValid(MeshComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] AttachToOwnerSocket - No SkeletalMeshComponent on Instigator"));
		return;
	}

	// Get socket names from ItemInfo
	const FSLFEquipmentSocketInfo& Sockets = ItemInfo.EquipmentDetails.AttachmentSockets;

	UE_LOG(LogTemp, Log, TEXT("[Weapon] AttachToOwnerSocket - Sockets L=%s R=%s"),
		*Sockets.LeftHandSocketName.ToString(),
		*Sockets.RightHandSocketName.ToString());

	// Select appropriate socket based on equipment slot (right or left hand)
	FName SocketName = IsRightHandWeapon() ? Sockets.RightHandSocketName : Sockets.LeftHandSocketName;'''

    new_attach_func = '''void ASLFWeaponBase::AttachToOwnerSocket(bool bRightHand)
{
	// Get the character that owns this weapon (set as Instigator during spawn)
	APawn* InstigatorPawn = GetInstigator();
	if (!IsValid(InstigatorPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] AttachToOwnerSocket - No Instigator, cannot attach"));
		return;
	}

	// Get the skeletal mesh component from the character
	USkeletalMeshComponent* MeshComponent = nullptr;

	if (ACharacter* Character = Cast<ACharacter>(InstigatorPawn))
	{
		MeshComponent = Character->GetMesh();
	}
	else
	{
		MeshComponent = InstigatorPawn->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (!IsValid(MeshComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Weapon] AttachToOwnerSocket - No SkeletalMeshComponent on Instigator"));
		return;
	}

	// Get socket names from ItemInfo
	const FSLFEquipmentSocketInfo& Sockets = ItemInfo.EquipmentDetails.AttachmentSockets;

	UE_LOG(LogTemp, Log, TEXT("[Weapon] AttachToOwnerSocket - bRightHand=%s, Sockets L=%s R=%s"),
		bRightHand ? TEXT("true") : TEXT("false"),
		*Sockets.LeftHandSocketName.ToString(),
		*Sockets.RightHandSocketName.ToString());

	// Select appropriate socket based on bRightHand parameter from AC_EquipmentManager
	FName SocketName = bRightHand ? Sockets.RightHandSocketName : Sockets.LeftHandSocketName;'''

    content = content.replace(old_attach_func, new_attach_func)

    # 3. Update the default socket logic to use bRightHand
    old_default_socket = '''	// Use default socket if none specified
	if (SocketName.IsNone())
	{
		SocketName = IsRightHandWeapon() ? FName("hand_r") : FName("hand_l");
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Using default socket: %s"), *SocketName.ToString());
	}'''

    new_default_socket = '''	// Use default socket if none specified
	if (SocketName.IsNone())
	{
		SocketName = bRightHand ? FName("hand_r") : FName("hand_l");
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Using default socket: %s"), *SocketName.ToString());
	}'''

    content = content.replace(old_default_socket, new_default_socket)

    # 4. Add interface implementations after BeginPlay (before AttachToOwnerSocket)
    interface_impl = '''

// ===================================================================
// IBPI_Item INTERFACE IMPLEMENTATION
// ===================================================================

void ASLFWeaponBase::OnWeaponEquip_Implementation(bool bRightHand)
{
	UE_LOG(LogTemp, Log, TEXT("[Weapon] OnWeaponEquip called: %s (RightHand: %s)"),
		*GetName(), bRightHand ? TEXT("Yes") : TEXT("No"));

	// Apply mesh settings first (mesh, location, rotation offsets)
	ApplyMeshSettings();

	// Now attach to the character socket
	AttachToOwnerSocket(bRightHand);

	bHasBeenEquipped = true;
}

void ASLFWeaponBase::OnWeaponUnequip_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[Weapon] OnWeaponUnequip called: %s"), *GetName());

	// Detach from parent
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	bHasBeenEquipped = false;
}

void ASLFWeaponBase::OnUse_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[Weapon] OnUse called: %s (not typically used for weapons)"), *GetName());
}

void ASLFWeaponBase::ApplyMeshSettings()
{
	UE_LOG(LogTemp, Log, TEXT("[Weapon] ApplyMeshSettings for %s"), *GetName());

	// Apply default weapon mesh if set (from Blueprint CDO)
	if (WeaponMesh && !DefaultWeaponMesh.IsNull())
	{
		UStaticMesh* MeshToApply = DefaultWeaponMesh.LoadSynchronous();
		if (MeshToApply)
		{
			WeaponMesh->SetStaticMesh(MeshToApply);
			UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied DefaultWeaponMesh: %s"), *MeshToApply->GetName());
		}
	}

	// Apply mesh transform offsets (from original Blueprint SCS component settings)
	if (WeaponMesh)
	{
		// Always apply location
		WeaponMesh->SetRelativeLocation(DefaultMeshRelativeLocation);
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh location: %s"), *DefaultMeshRelativeLocation.ToString());

		// Always apply rotation
		WeaponMesh->SetRelativeRotation(DefaultMeshRelativeRotation);
		UE_LOG(LogTemp, Log, TEXT("[Weapon] Applied mesh rotation: %s"), *DefaultMeshRelativeRotation.ToString());
	}
}

'''

    # Insert interface implementation before AttachToOwnerSocket
    insert_marker = "void ASLFWeaponBase::AttachToOwnerSocket(bool bRightHand)"
    content = content.replace(insert_marker, interface_impl + insert_marker)

    # 5. Add bHasBeenEquipped = false in constructor
    old_constructor_end = '''	// Initialize debug
	bDebugVisualizeTrace = false;
}'''

    new_constructor_end = '''	// Initialize debug
	bDebugVisualizeTrace = false;
	bHasBeenEquipped = false;
}'''

    content = content.replace(old_constructor_end, new_constructor_end)

    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(content)

    print("SLFWeaponBase.cpp updated successfully!")

if __name__ == "__main__":
    main()
