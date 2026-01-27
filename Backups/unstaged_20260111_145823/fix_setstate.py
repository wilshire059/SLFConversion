"""Fix SetState to also set InCombat"""

file_path = "C:/scripts/SLFConversion/Source/SLFConversion/Components/AIBehaviorManagerComponent.cpp"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

old_text = '''void UAIBehaviorManagerComponent::SetState_Implementation(ESLFAIStates NewState)
{
	if (CurrentState != NewState)
	{
		PreviousState = CurrentState;
		CurrentState = NewState;

		UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] SetState: %d -> %d"),
			static_cast<int32>(PreviousState), static_cast<int32>(CurrentState));

		// Update blackboard state key - use "State" which is the standard key name
		if (UBlackboardComponent* BB = GetBlackboard())
		{
			BB->SetValueAsEnum(FName("State"), static_cast<uint8>(CurrentState));
			UE_LOG(LogTemp, Log, TEXT("[AIBehaviorManager] Updated blackboard State to %d"), static_cast<int32>(CurrentState));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] GetBlackboard() returned null!"));
		}
	}
}'''

new_text = '''void UAIBehaviorManagerComponent::SetState_Implementation(ESLFAIStates NewState)
{
	if (CurrentState != NewState)
	{
		PreviousState = CurrentState;
		CurrentState = NewState;

		UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] SetState: %d -> %d"),
			static_cast<int32>(PreviousState), static_cast<int32>(CurrentState));

		// Update blackboard state key - use "State" which is the standard key name
		if (UBlackboardComponent* BB = GetBlackboard())
		{
			BB->SetValueAsEnum(FName("State"), static_cast<uint8>(CurrentState));
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] Updated blackboard State to %d"), static_cast<int32>(CurrentState));

			// Also update InCombat bool when entering/leaving Combat state
			bool bInCombat = (CurrentState == ESLFAIStates::Combat);
			BB->SetValueAsBool(FName("InCombat"), bInCombat);
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] Updated blackboard InCombat to %s"), bInCombat ? TEXT("TRUE") : TEXT("FALSE"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[AIBehaviorManager] GetBlackboard() returned null!"));
		}
	}
}'''

if old_text in content:
    content = content.replace(old_text, new_text)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Updated SetState to set InCombat")
else:
    print("ERROR: Old text not found - checking for variations...")
    # Check if already updated
    if "InCombat" in content and "SetState_Implementation" in content:
        print("Already updated!")
    else:
        print("Content sample around SetState:")
        idx = content.find("SetState_Implementation")
        if idx != -1:
            print(content[idx:idx+500])
