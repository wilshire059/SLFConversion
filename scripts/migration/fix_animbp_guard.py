# fix_animbp_guard.py
# Fix AnimBP to use GetIsGuarding() instead of directly reading IsGuarding

FILE_PATH = "C:/scripts/slfconversion/Source/SLFConversion/Animation/ABP_SoulslikeCharacter_Additive.cpp"

OLD_CODE = '''	if (CombatManager)
	{
		bIsBlocking = CombatManager->IsGuarding;
		IkWeight = CombatManager->IKWeight;
		ActiveHitNormal = CombatManager->CurrentHitNormal;
	}'''

NEW_CODE = '''	if (CombatManager)
	{
		// Use GetIsGuarding() to include grace period (not direct IsGuarding access)
		bIsBlocking = CombatManager->GetIsGuarding();
		IkWeight = CombatManager->IKWeight;
		ActiveHitNormal = CombatManager->CurrentHitNormal;
	}'''

with open(FILE_PATH, 'r', encoding='utf-8') as f:
    content = f.read()

if OLD_CODE in content:
    content = content.replace(OLD_CODE, NEW_CODE)
    with open(FILE_PATH, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Patched AnimBP to use GetIsGuarding()")
else:
    print("ERROR: Could not find the old code block")
    if "GetIsGuarding()" in content:
        print("NOTE: Code may already be patched - GetIsGuarding() call found")
