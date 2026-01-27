#!/usr/bin/env python3
# patch_animbp_fix.py
# Add component reference initialization to UpdateAnimInstanceOverlayStates

import re

# Read the file
with open("C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFSoulslikeCharacter.cpp", "r", encoding="utf-8") as f:
    content = f.read()

# Find the function and insert the component reference code
old_code = """	// Get the AnimInstance class to find Blueprint properties
	UClass* AnimClass = AnimInstance->GetClass();

	// Find and set LeftHandOverlayState (Blueprint enum property)"""

new_code = """	// Get the AnimInstance class to find Blueprint properties
	UClass* AnimClass = AnimInstance->GetClass();

	// ═══════════════════════════════════════════════════════════════════════════
	// STEP 1: Set component references on AnimBP (required for Property Access nodes)
	// These were originally set in EventGraph, which was cleared during migration
	// ═══════════════════════════════════════════════════════════════════════════

	// Set ActionManager reference (on Character)
	if (FObjectProperty* ActionMgrProp = CastField<FObjectProperty>(AnimClass->FindPropertyByName(TEXT("ActionManager"))))
	{
		UObject* CurrentValue = ActionMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && CachedActionManager)
		{
			ActionMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, CachedActionManager);
		}
	}

	// Set CombatManager reference (on Character)
	if (FObjectProperty* CombatMgrProp = CastField<FObjectProperty>(AnimClass->FindPropertyByName(TEXT("CombatManager"))))
	{
		UObject* CurrentValue = CombatMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && CachedCombatManager)
		{
			CombatMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, CachedCombatManager);
		}
	}

	// Get EquipmentManager from controller (NOT character!)
	UAC_EquipmentManager* EquipMgr = nullptr;
	if (AController* PC = GetController())
	{
		EquipMgr = PC->FindComponentByClass<UAC_EquipmentManager>();
	}

	// Set EquipmentManager reference
	if (FObjectProperty* EquipMgrProp = CastField<FObjectProperty>(AnimClass->FindPropertyByName(TEXT("EquipmentManager"))))
	{
		UObject* CurrentValue = EquipMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && EquipMgr)
		{
			EquipMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, EquipMgr);
		}
	}

	// ═══════════════════════════════════════════════════════════════════════════
	// STEP 2: Update overlay state enum values (requires EquipmentManager)
	// ═══════════════════════════════════════════════════════════════════════════
	if (!EquipMgr)
	{
		return;
	}

	// Find and set LeftHandOverlayState (Blueprint enum property)"""

# Also need to remove the duplicate EquipmentManager fetch since we moved it up
old_equip_fetch = """	// Get EquipmentManager from controller (NOT character!)
	UAC_EquipmentManager* EquipMgr = nullptr;
	if (AController* PC = GetController())
	{
		EquipMgr = PC->FindComponentByClass<UAC_EquipmentManager>();
	}
	if (!EquipMgr)
	{
		return;
	}

	// Get the AnimInstance class to find Blueprint properties
	UClass* AnimClass = AnimInstance->GetClass();"""

new_equip_fetch = """	// Get the AnimInstance class to find Blueprint properties
	UClass* AnimClass = AnimInstance->GetClass();"""

# First, remove the old EquipmentManager fetch
if old_equip_fetch in content:
    content = content.replace(old_equip_fetch, new_equip_fetch)
    print("Removed duplicate EquipmentManager fetch")

# Then add the component reference code
if old_code in content:
    content = content.replace(old_code, new_code)
    print("Added component reference initialization code")
else:
    print("ERROR: Could not find target code block")
    exit(1)

# Write the file
with open("C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFSoulslikeCharacter.cpp", "w", encoding="utf-8") as f:
    f.write(content)

print("Patch applied successfully!")
