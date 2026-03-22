#!/usr/bin/env python3
"""Fix InitializeDefaultActions to use correct tag names and asset loading"""

cpp_path = 'C:/scripts/SLFConversion/Source/SLFConversion/Components/AC_ActionManager.cpp'

with open(cpp_path, 'r', encoding='utf-8') as f:
    content = f.read()

old_impl = '''void UAC_ActionManager::InitializeDefaultActions()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::InitializeDefaultActions - Populating 26 default actions"));

	static const TCHAR* ActionMappings[] = {
		TEXT("Jump"), TEXT("Dodge"), TEXT("Crouch"),
		TEXT("StartSprinting"), TEXT("StopSprinting"), TEXT("SprintAttack"),
		TEXT("ComboLight_L"), TEXT("ComboLight_R"), TEXT("ComboHeavy"),
		TEXT("GuardStart"), TEXT("GuardEnd"), TEXT("GuardCancel"),
		TEXT("JumpAttack"), TEXT("Backstab"), TEXT("Execute"),
		TEXT("DrinkFlask_HP"), TEXT("Projectile"), TEXT("WeaponAbility"),
		TEXT("DualWieldAttack"), TEXT("PickupItemMontage"), TEXT("UseEquippedTool"),
		TEXT("TwoHandedStance_L"), TEXT("TwoHandedStance_R"),
		TEXT("ScrollWheel_LeftHand"), TEXT("ScrollWheel_RightHand"), TEXT("ScrollWheel_Tools")
	};

	int32 LoadedCount = 0;
	for (const TCHAR* ActionName : ActionMappings)
	{
		FString TagString = FString::Printf(TEXT("SoulslikeFramework.Action.%s"), ActionName);
		FString AssetPath = FString::Printf(TEXT("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_%s.DA_Action_%s"), ActionName, ActionName);

		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);
		if (!Tag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("  Tag not found: %s"), *TagString);
			continue;
		}

		UPDA_Action* Asset = LoadObject<UPDA_Action>(nullptr, *AssetPath);
		if (Asset)
		{
			Actions.Add(Tag, Asset);
			LoadedCount++;
			UE_LOG(LogTemp, Verbose, TEXT("  Loaded: %s -> %s"), *TagString, *Asset->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Asset not found: %s"), *AssetPath);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Loaded %d/%d default actions"), LoadedCount, (int32)(sizeof(ActionMappings) / sizeof(ActionMappings[0])));
}'''

new_impl = '''void UAC_ActionManager::InitializeDefaultActions()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::InitializeDefaultActions - Populating default actions"));

	// Mapping: {TagName, AssetFileName} - Tag names from DefaultGameplayTags.ini, asset names from /Data/Actions/ActionData/
	struct FActionMapping { const TCHAR* TagName; const TCHAR* AssetName; };
	static const FActionMapping ActionMappings[] = {
		// Basic movement
		{TEXT("Jump"), TEXT("Jump")},
		{TEXT("Dodge"), TEXT("Dodge")},
		{TEXT("Crouch"), TEXT("Crouch")},
		{TEXT("StartSprinting"), TEXT("StartSprinting")},
		{TEXT("StopSprinting"), TEXT("StopSprinting")},
		{TEXT("SprintAttack"), TEXT("SprintAttack")},
		// Combat - Tags use different names than assets!
		{TEXT("LightAttackLeft"), TEXT("ComboLight_L")},
		{TEXT("LightAttackRight"), TEXT("ComboLight_R")},
		{TEXT("HeavyAttack"), TEXT("ComboHeavy")},
		// Guard
		{TEXT("GuardStart"), TEXT("GuardStart")},
		{TEXT("GuardEnd"), TEXT("GuardEnd")},
		{TEXT("GuardCancel"), TEXT("GuardCancel")},
		// Special attacks
		{TEXT("JumpAttack"), TEXT("JumpAttack")},
		{TEXT("Backstab"), TEXT("Backstab")},
		{TEXT("Execute"), TEXT("Execute")},
		// Items and abilities
		{TEXT("DrinkFlask.HP"), TEXT("DrinkFlask_HP")},
		{TEXT("ThrowKnife"), TEXT("Projectile")},
		{TEXT("SpecialAttack"), TEXT("WeaponAbility")},
		{TEXT("DualWieldAttack"), TEXT("DualWieldAttack")},
		{TEXT("PickupItem"), TEXT("PickupItemMontage")},
		{TEXT("UseEquippedTool"), TEXT("UseEquippedTool")},
		// Stances
		{TEXT("TwoHandStanceLeft"), TEXT("TwoHandedStance_L")},
		{TEXT("TwoHandStanceRight"), TEXT("TwoHandedStance_R")},
		// Scroll wheel
		{TEXT("ScrollWheel.Left"), TEXT("ScrollWheel_LeftHand")},
		{TEXT("ScrollWheel.Right"), TEXT("ScrollWheel_RightHand")},
		{TEXT("ScrollWheel.Bottom"), TEXT("ScrollWheel_Tools")},
	};

	int32 LoadedCount = 0;
	const int32 NumMappings = sizeof(ActionMappings) / sizeof(ActionMappings[0]);

	for (const FActionMapping& Mapping : ActionMappings)
	{
		FString TagString = FString::Printf(TEXT("SoulslikeFramework.Action.%s"), Mapping.TagName);
		FString AssetPath = FString::Printf(TEXT("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_%s.DA_Action_%s"), Mapping.AssetName, Mapping.AssetName);

		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagString), false);
		if (!Tag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("  Tag not found: %s"), *TagString);
			continue;
		}

		// Load as UPrimaryDataAsset since PDA_Action is a Blueprint class, then cast
		UPrimaryDataAsset* Asset = LoadObject<UPrimaryDataAsset>(nullptr, *AssetPath);
		if (Asset)
		{
			// Cast to UPDA_Action - works because PDA_Action_C inherits from UPrimaryDataAsset
			UPDA_Action* ActionAsset = Cast<UPDA_Action>(Asset);
			if (ActionAsset)
			{
				Actions.Add(Tag, ActionAsset);
				LoadedCount++;
				UE_LOG(LogTemp, Log, TEXT("  Loaded: %s -> %s"), *TagString, *Asset->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("  Cast failed for: %s"), *AssetPath);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  Asset not found: %s"), *AssetPath);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Loaded %d/%d default actions"), LoadedCount, NumMappings);
}'''

if old_impl in content:
    content = content.replace(old_impl, new_impl)
    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print('Successfully updated InitializeDefaultActions with correct tag/asset mappings')
else:
    print('Could not find old InitializeDefaultActions implementation')
    print('Searching for function...')
    if 'void UAC_ActionManager::InitializeDefaultActions()' in content:
        print('Function exists but content differs - manual update needed')
    else:
        print('Function not found in file')
