#!/usr/bin/env python3
# patch_animbp_diagnostics.py
# Add comprehensive diagnostic logging to UpdateAnimInstanceOverlayStates

# Read the file
with open("C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFSoulslikeCharacter.cpp", "r", encoding="utf-8") as f:
    content = f.read()

old_code = '''void ASLFSoulslikeCharacter::UpdateAnimInstanceOverlayStates()
{
	// Get AnimInstance from skeletal mesh
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	// Get the AnimInstance class to find Blueprint properties
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
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Set AnimBP ActionManager reference"));
		}
	}

	// Set CombatManager reference (on Character)
	if (FObjectProperty* CombatMgrProp = CastField<FObjectProperty>(AnimClass->FindPropertyByName(TEXT("CombatManager"))))
	{
		UObject* CurrentValue = CombatMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && CachedCombatManager)
		{
			CombatMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, CachedCombatManager);
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Set AnimBP CombatManager reference"));
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
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Set AnimBP EquipmentManager reference"));
		}
	}'''

new_code = '''void ASLFSoulslikeCharacter::UpdateAnimInstanceOverlayStates()
{
	// Get AnimInstance from skeletal mesh
	UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		return;
	}

	// Get the AnimInstance class to find Blueprint properties
	UClass* AnimClass = AnimInstance->GetClass();

	// ═══════════════════════════════════════════════════════════════════════════
	// STEP 1: Set component references on AnimBP (required for Property Access nodes)
	// These were originally set in EventGraph, which was cleared during migration
	// ═══════════════════════════════════════════════════════════════════════════

	// One-time diagnostic log to understand AnimBP property structure
	static bool bLoggedDiagnostics = false;
	if (!bLoggedDiagnostics)
	{
		bLoggedDiagnostics = true;
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] === DIAGNOSTIC START ==="));
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] AnimInstance Class: %s"), *AnimInstance->GetClass()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] CachedActionManager: %p"), CachedActionManager);
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] CachedCombatManager: %p"), CachedCombatManager);

		// List all properties on the AnimInstance class to find correct names
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Listing AnimBP properties:"));
		for (TFieldIterator<FProperty> PropIt(AnimClass); PropIt; ++PropIt)
		{
			FProperty* Prop = *PropIt;
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX]   - %s (%s)"), *Prop->GetName(), *Prop->GetClass()->GetName());
		}
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] === DIAGNOSTIC END ==="));
	}

	// Set ActionManager reference (on Character)
	FProperty* ActionMgrRawProp = AnimClass->FindPropertyByName(TEXT("ActionManager"));
	if (FObjectProperty* ActionMgrProp = CastField<FObjectProperty>(ActionMgrRawProp))
	{
		UObject* CurrentValue = ActionMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && CachedActionManager)
		{
			ActionMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, CachedActionManager);
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Set AnimBP ActionManager reference to %p"), CachedActionManager);
		}
	}
	else if (ActionMgrRawProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] ActionManager property found but not FObjectProperty: %s"), *ActionMgrRawProp->GetClass()->GetName());
	}

	// Set CombatManager reference (on Character)
	FProperty* CombatMgrRawProp = AnimClass->FindPropertyByName(TEXT("CombatManager"));
	if (FObjectProperty* CombatMgrProp = CastField<FObjectProperty>(CombatMgrRawProp))
	{
		UObject* CurrentValue = CombatMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && CachedCombatManager)
		{
			CombatMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, CachedCombatManager);
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Set AnimBP CombatManager reference to %p"), CachedCombatManager);
		}
	}
	else if (CombatMgrRawProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] CombatManager property found but not FObjectProperty: %s"), *CombatMgrRawProp->GetClass()->GetName());
	}

	// Get EquipmentManager from controller (NOT character!)
	UAC_EquipmentManager* EquipMgr = nullptr;
	if (AController* PC = GetController())
	{
		EquipMgr = PC->FindComponentByClass<UAC_EquipmentManager>();
	}

	// Set EquipmentManager reference
	FProperty* EquipMgrRawProp = AnimClass->FindPropertyByName(TEXT("EquipmentManager"));
	if (FObjectProperty* EquipMgrProp = CastField<FObjectProperty>(EquipMgrRawProp))
	{
		UObject* CurrentValue = EquipMgrProp->GetObjectPropertyValue_InContainer(AnimInstance);
		if (!CurrentValue && EquipMgr)
		{
			EquipMgrProp->SetObjectPropertyValue_InContainer(AnimInstance, EquipMgr);
			UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] Set AnimBP EquipmentManager reference to %p"), EquipMgr);
		}
	}
	else if (EquipMgrRawProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ANIMFIX] EquipmentManager property found but not FObjectProperty: %s"), *EquipMgrRawProp->GetClass()->GetName());
	}'''

if old_code in content:
    content = content.replace(old_code, new_code)
    print("Patch applied successfully!")
else:
    print("ERROR: Could not find target code block")
    print("Looking for alternate patterns...")

    # Try to find the function start
    if "void ASLFSoulslikeCharacter::UpdateAnimInstanceOverlayStates()" in content:
        print("Function found, but code block doesn't match exactly")
    else:
        print("Function not found at all!")
    exit(1)

# Write the file
with open("C:/scripts/SLFConversion/Source/SLFConversion/Blueprints/SLFSoulslikeCharacter.cpp", "w", encoding="utf-8") as f:
    f.write(content)

print("File saved successfully!")
