# fix_guard_toggle.py
# Fix ToggleGuardReplicated to call SetGuardState instead of directly setting IsGuarding

FILE_PATH = "C:/scripts/slfconversion/Source/SLFConversion/Blueprints/SLFBaseCharacter.cpp"

OLD_CODE = '''void ASLFBaseCharacter::ToggleGuardReplicated_Implementation(bool bToggled, bool bIgnoreGracePeriod)
{
	// Delegate to CombatManager component (IMPLEMENTED)
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] ToggleGuardReplicated: %s, ignore grace: %s"),
		bToggled ? TEXT("true") : TEXT("false"),
		bIgnoreGracePeriod ? TEXT("true") : TEXT("false"));

	UAC_CombatManager* CombatManager = FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->WantsToGuard = bToggled;
		if (bToggled)
		{
			CombatManager->IsGuarding = true;
		}
		else if (bIgnoreGracePeriod)
		{
			CombatManager->IsGuarding = false;
		}
		// If not ignoring grace period, IsGuarding will be cleared by timer
	}
}'''

NEW_CODE = '''void ASLFBaseCharacter::ToggleGuardReplicated_Implementation(bool bToggled, bool bIgnoreGracePeriod)
{
	// Delegate to CombatManager component - call SetGuardState which handles grace period properly
	UE_LOG(LogTemp, Log, TEXT("[BaseCharacter] ToggleGuardReplicated: %s, ignore grace: %s"),
		bToggled ? TEXT("true") : TEXT("false"),
		bIgnoreGracePeriod ? TEXT("true") : TEXT("false"));

	UAC_CombatManager* CombatManager = FindComponentByClass<UAC_CombatManager>();
	if (CombatManager)
	{
		CombatManager->WantsToGuard = bToggled;
		// Call SetGuardState which handles grace period timer and proper state management
		CombatManager->SetGuardState(bToggled, bIgnoreGracePeriod);
	}
}'''

with open(FILE_PATH, 'r', encoding='utf-8') as f:
    content = f.read()

if OLD_CODE in content:
    content = content.replace(OLD_CODE, NEW_CODE)
    with open(FILE_PATH, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Patched ToggleGuardReplicated_Implementation to call SetGuardState")
else:
    print("ERROR: Could not find the old code block")
    # Check if already patched
    if "SetGuardState(bToggled, bIgnoreGracePeriod)" in content:
        print("NOTE: Code may already be patched - SetGuardState call found")
