# fix_guard_sequence.py
# Patch AC_EquipmentManager.cpp to fix RefreshActiveGuardSequence

import re

FILE_PATH = "C:/scripts/slfconversion/Source/SLFConversion/Components/AC_EquipmentManager.cpp"

OLD_CODE = '''			// Check if item has block animation via moveset data
			if (ItemData.EquipmentDetails.MovesetWeapons)
			{
				// Item has a moveset, check for block sequence
				// For now, assume any left-hand weapon/shield can block
				UE_LOG(LogTemp, Log, TEXT("  Left hand item supports guarding"));
				// ActiveBlockSequence would be set from the item's animation data
				break;
			}
		}
	}
}

/**
 * DoesEquipmentSupportGuard'''

NEW_CODE = '''			// Check if item has block animation via moveset data
			if (ItemData.EquipmentDetails.MovesetWeapons)
			{
				// Cast to UPDA_WeaponAnimset to get Guard_L sequence
				UPDA_WeaponAnimset* Animset = Cast<UPDA_WeaponAnimset>(ItemData.EquipmentDetails.MovesetWeapons);
				if (Animset && Animset->Guard_L)
				{
					ActiveBlockSequence = Animset->Guard_L;
					UE_LOG(LogTemp, Log, TEXT("  Left hand Guard_L sequence set: %s"), *Animset->Guard_L->GetName());
					return;
				}
				UE_LOG(LogTemp, Log, TEXT("  Left hand item has moveset but no Guard_L"));
			}
		}
	}

	// If no left hand guard, check right hand slots for two-handed weapons
	for (const FGameplayTag& SlotTag : RightHandSlots)
	{
		if (IsSlotOccupied(SlotTag))
		{
			FSLFItemInfo ItemData;
			UPrimaryDataAsset* ItemAsset = nullptr;
			FGuid ItemId;
			FSLFItemInfo ItemData_1;
			UPrimaryDataAsset* ItemAsset_1 = nullptr;
			FGuid ItemId_1;
			GetItemAtSlot(SlotTag, ItemData, ItemAsset, ItemId, ItemData_1, ItemAsset_1, ItemId_1);

			if (ItemData.EquipmentDetails.MovesetWeapons)
			{
				UPDA_WeaponAnimset* Animset = Cast<UPDA_WeaponAnimset>(ItemData.EquipmentDetails.MovesetWeapons);
				if (Animset && Animset->Guard_R)
				{
					ActiveBlockSequence = Animset->Guard_R;
					UE_LOG(LogTemp, Log, TEXT("  Right hand Guard_R sequence set: %s"), *Animset->Guard_R->GetName());
					return;
				}
				UE_LOG(LogTemp, Log, TEXT("  Right hand item has moveset but no Guard_R"));
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  No guard sequence found in any equipped weapon"));
}

/**
 * DoesEquipmentSupportGuard'''

with open(FILE_PATH, 'r', encoding='utf-8') as f:
    content = f.read()

if OLD_CODE in content:
    content = content.replace(OLD_CODE, NEW_CODE)
    with open(FILE_PATH, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Patched RefreshActiveGuardSequence_Implementation")
else:
    print("ERROR: Could not find the old code block")
    # Print what we found around line 1000-1020
    lines = content.split('\n')
    print("Lines 1000-1020:")
    for i, line in enumerate(lines[999:1020], start=1000):
        print(f"{i}: {line}")
