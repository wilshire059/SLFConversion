#!/usr/bin/env python3
"""Add InitializeDefaultActions to AC_ActionManager"""

# ====== HEADER UPDATE ======
header_path = 'C:/scripts/SLFConversion/Source/SLFConversion/Components/AC_ActionManager.h'

with open(header_path, 'r', encoding='utf-8') as f:
    header_content = f.read()

old_async_section = '''	// ═══════════════════════════════════════════════════════════════════════
	// ASYNC LOADING
	// ═══════════════════════════════════════════════════════════════════════

	/** Recursively load action data assets */
	UFUNCTION()
	void RecursiveLoadActions();

	/** Callback when an action asset finishes loading */
	UFUNCTION()
	void OnActionLoaded();
};'''

new_async_section = '''	// ═══════════════════════════════════════════════════════════════════════
	// ACTION INITIALIZATION (Migration fallback)
	// ═══════════════════════════════════════════════════════════════════════

	/**
	 * InitializeDefaultActions - Populate Actions TMap with default action data assets
	 * Called from BeginPlay if Actions is empty (migration fallback)
	 */
	void InitializeDefaultActions();

	/**
	 * BuildAvailableActionsFromActionsMap - Build AvailableActions TMap from Actions
	 * Creates action class instances from action data assets
	 */
	void BuildAvailableActionsFromActionsMap();

	// ═══════════════════════════════════════════════════════════════════════
	// ASYNC LOADING
	// ═══════════════════════════════════════════════════════════════════════

	/** Recursively load action data assets */
	UFUNCTION()
	void RecursiveLoadActions();

	/** Callback when an action asset finishes loading */
	UFUNCTION()
	void OnActionLoaded();
};'''

if old_async_section in header_content:
    header_content = header_content.replace(old_async_section, new_async_section)
    with open(header_path, 'w', encoding='utf-8') as f:
        f.write(header_content)
    print('Header updated: Added InitializeDefaultActions and BuildAvailableActionsFromActionsMap')
else:
    print('Header: Could not find ASYNC LOADING section - may already be updated')

# ====== CPP UPDATE ======
cpp_path = 'C:/scripts/SLFConversion/Source/SLFConversion/Components/AC_ActionManager.cpp'

with open(cpp_path, 'r', encoding='utf-8') as f:
    cpp_content = f.read()

# Add new functions before the closing of the file
initialize_actions_impl = '''

// ═══════════════════════════════════════════════════════════════════════════════
// ACTION INITIALIZATION (Migration fallback)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * InitializeDefaultActions - Populate Actions TMap with default action data assets
 * This is a migration fallback - loads all 26 standard actions
 */
void UAC_ActionManager::InitializeDefaultActions()
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
}

/**
 * BuildAvailableActionsFromActionsMap - Build AvailableActions from Actions map
 * For each action data asset, get its ActionClass and add to AvailableActions
 */
void UAC_ActionManager::BuildAvailableActionsFromActionsMap()
{
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::BuildAvailableActionsFromActionsMap - %d actions to process"), Actions.Num());

	AvailableActions.Empty();

	for (const auto& Pair : Actions)
	{
		const FGameplayTag& Tag = Pair.Key;
		UPDA_Action* ActionData = Pair.Value;

		if (!ActionData)
		{
			UE_LOG(LogTemp, Warning, TEXT("  Null action data for tag: %s"), *Tag.ToString());
			continue;
		}

		// Get the ActionClass from the data asset using reflection
		// PDA_Action has an ActionClass property that's a TSubclassOf<UB_Action>
		FProperty* ActionClassProp = ActionData->GetClass()->FindPropertyByName(FName("ActionClass"));
		if (ActionClassProp)
		{
			FClassProperty* ClassProp = CastField<FClassProperty>(ActionClassProp);
			if (ClassProp)
			{
				UClass* ActionClass = Cast<UClass>(ClassProp->GetPropertyValue_InContainer(ActionData));
				if (ActionClass && ActionClass->IsChildOf(UB_Action::StaticClass()))
				{
					AvailableActions.Add(Tag, ActionClass);
					UE_LOG(LogTemp, Verbose, TEXT("  Added: %s -> %s"), *Tag.ToString(), *ActionClass->GetName());
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("  ActionClass property not found on %s"), *ActionData->GetName());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("  Built %d available actions"), AvailableActions.Num());
}
'''

# Check if already added
if 'void UAC_ActionManager::InitializeDefaultActions()' not in cpp_content:
    # Add before the final closing of file (find last function)
    # We'll add it at the end of the file
    cpp_content = cpp_content.rstrip() + initialize_actions_impl

    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(cpp_content)
    print('CPP updated: Added InitializeDefaultActions and BuildAvailableActionsFromActionsMap')
else:
    print('CPP: InitializeDefaultActions already exists')

# ====== UPDATE BEGINPLAY ======
# Now update BeginPlay to call these functions

old_beginplay = '''void UAC_ActionManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::BeginPlay"));
}'''

new_beginplay = '''void UAC_ActionManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::BeginPlay"));

	// Migration fallback: If Actions map is empty, load default actions
	if (Actions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAC_ActionManager::BeginPlay - Actions map empty, loading defaults"));
		InitializeDefaultActions();
	}

	// Build AvailableActions from Actions map
	if (AvailableActions.Num() == 0 && Actions.Num() > 0)
	{
		BuildAvailableActionsFromActionsMap();
	}

	UE_LOG(LogTemp, Log, TEXT("UAC_ActionManager::BeginPlay - Actions: %d, AvailableActions: %d"), Actions.Num(), AvailableActions.Num());
}'''

# Re-read since we just modified
with open(cpp_path, 'r', encoding='utf-8') as f:
    cpp_content = f.read()

if old_beginplay in cpp_content:
    cpp_content = cpp_content.replace(old_beginplay, new_beginplay)
    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(cpp_content)
    print('CPP updated: Modified BeginPlay to call InitializeDefaultActions')
else:
    print('CPP: BeginPlay not in expected format - may need manual update')

print('Done!')
