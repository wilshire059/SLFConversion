"""Fix BT export functions to use public API"""

file_path = "C:/scripts/SLFConversion/Source/SLFConversion/SLFAutomationLibrary.cpp"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Replace the broken implementation with a working one
old_impl = '''// ============================================================================
// BEHAVIOR TREE DIAGNOSTICS
// ============================================================================

FString USLFAutomationLibrary::ExportBehaviorTreeStructure(const FString& BehaviorTreePath)
{
	FString Result;

	UBehaviorTree* BT = LoadObject<UBehaviorTree>(nullptr, *BehaviorTreePath);
	if (!BT)
	{
		return FString::Printf(TEXT("ERROR: Could not load BehaviorTree: %s"), *BehaviorTreePath);
	}

	Result += FString::Printf(TEXT("BehaviorTree: %s\\n"), *BT->GetName());
	Result += FString::Printf(TEXT("BlackboardAsset: %s\\n"), BT->BlackboardAsset ? *BT->BlackboardAsset->GetName() : TEXT("None"));

	// Check root decorators
	Result += FString::Printf(TEXT("RootDecorators: %d\\n"), BT->RootDecorators.Num());

	for (int32 i = 0; i < BT->RootDecorators.Num(); ++i)
	{
		UBTDecorator* Decorator = BT->RootDecorators[i];
		if (!Decorator)
		{
			Result += FString::Printf(TEXT("  [%d] NULL\\n"), i);
			continue;
		}

		Result += FString::Printf(TEXT("  [%d] %s - %s\\n"), i, *Decorator->GetClass()->GetName(), *Decorator->GetNodeName());

		// Check if it's a Blackboard decorator
		UBTDecorator_Blackboard* BBDecorator = Cast<UBTDecorator_Blackboard>(Decorator);
		if (BBDecorator)
		{
			// Get the blackboard key
			FName KeyName = BBDecorator->GetSelectedBlackboardKey();
			Result += FString::Printf(TEXT("      BlackboardKey: %s\\n"), *KeyName.ToString());

			// Get IntValue - this is what we compare against
			int32 IntVal = BBDecorator->GetIntValue();
			Result += FString::Printf(TEXT("      IntValue: %d\\n"), IntVal);

			// Get notify observer mode
			Result += FString::Printf(TEXT("      NotifyObserver: %d\\n"), (int32)BBDecorator->GetNotifyObserver());

			// Get flow abort mode
			Result += FString::Printf(TEXT("      FlowAbortMode: %d\\n"), (int32)BBDecorator->GetFlowAbortMode());
		}
	}

	// Check root node
	if (BT->RootNode)
	{
		Result += FString::Printf(TEXT("RootNode: %s\\n"), *BT->RootNode->GetClass()->GetName());

		// If it's a composite node, check children
		UBTCompositeNode* CompositeNode = Cast<UBTCompositeNode>(BT->RootNode);
		if (CompositeNode)
		{
			Result += FString::Printf(TEXT("  Children: %d\\n"), CompositeNode->Children.Num());

			for (int32 i = 0; i < CompositeNode->Children.Num(); ++i)
			{
				const FBTCompositeChild& Child = CompositeNode->Children[i];

				if (Child.ChildComposite)
				{
					Result += FString::Printf(TEXT("    [%d] Composite: %s\\n"), i, *Child.ChildComposite->GetClass()->GetName());
				}
				else if (Child.ChildTask)
				{
					Result += FString::Printf(TEXT("    [%d] Task: %s - %s\\n"), i,
						*Child.ChildTask->GetClass()->GetName(),
						*Child.ChildTask->GetNodeName());

					// Check if it's a RunBehavior task (subtree)
					UBTTask_RunBehavior* RunBehavior = Cast<UBTTask_RunBehavior>(Child.ChildTask);
					if (RunBehavior && RunBehavior->GetSubtreeAsset())
					{
						Result += FString::Printf(TEXT("        SubtreeAsset: %s\\n"), *RunBehavior->GetSubtreeAsset()->GetName());
					}
				}

				// Check decorators on this child
				for (UBTDecorator* ChildDec : Child.Decorators)
				{
					if (ChildDec)
					{
						Result += FString::Printf(TEXT("      [Decorator] %s\\n"), *ChildDec->GetClass()->GetName());

						UBTDecorator_Blackboard* BBDec = Cast<UBTDecorator_Blackboard>(ChildDec);
						if (BBDec)
						{
							Result += FString::Printf(TEXT("        Key: %s, IntValue: %d\\n"),
								*BBDec->GetSelectedBlackboardKey().ToString(),
								BBDec->GetIntValue());
						}
					}
				}
			}
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}'''

new_impl = '''// ============================================================================
// BEHAVIOR TREE DIAGNOSTICS
// ============================================================================

FString USLFAutomationLibrary::ExportBehaviorTreeStructure(const FString& BehaviorTreePath)
{
	FString Result;

	UBehaviorTree* BT = LoadObject<UBehaviorTree>(nullptr, *BehaviorTreePath);
	if (!BT)
	{
		return FString::Printf(TEXT("ERROR: Could not load BehaviorTree: %s"), *BehaviorTreePath);
	}

	Result += FString::Printf(TEXT("BehaviorTree: %s\\n"), *BT->GetName());
	Result += FString::Printf(TEXT("BlackboardAsset: %s\\n"), BT->BlackboardAsset ? *BT->BlackboardAsset->GetName() : TEXT("None"));

	// Check root decorators
	Result += FString::Printf(TEXT("RootDecorators: %d\\n"), BT->RootDecorators.Num());

	for (int32 i = 0; i < BT->RootDecorators.Num(); ++i)
	{
		UBTDecorator* Decorator = BT->RootDecorators[i];
		if (!Decorator)
		{
			Result += FString::Printf(TEXT("  [%d] NULL\\n"), i);
			continue;
		}

		Result += FString::Printf(TEXT("  [%d] %s - %s\\n"), i, *Decorator->GetClass()->GetName(), *Decorator->GetNodeName());

		// Use GetStaticDescription which includes key, value, and operation info
		FString Desc = Decorator->GetStaticDescription();
		Result += FString::Printf(TEXT("      Description: %s\\n"), *Desc);

		// Check if it's a Blackboard decorator and get key name
		UBTDecorator_Blackboard* BBDecorator = Cast<UBTDecorator_Blackboard>(Decorator);
		if (BBDecorator)
		{
			FName KeyName = BBDecorator->GetSelectedBlackboardKey();
			Result += FString::Printf(TEXT("      BlackboardKey: %s\\n"), *KeyName.ToString());
			Result += FString::Printf(TEXT("      FlowAbortMode: %d\\n"), (int32)BBDecorator->GetFlowAbortMode());
		}
	}

	// Check root node
	if (BT->RootNode)
	{
		Result += FString::Printf(TEXT("RootNode: %s\\n"), *BT->RootNode->GetClass()->GetName());

		// If it's a composite node, check children
		UBTCompositeNode* CompositeNode = Cast<UBTCompositeNode>(BT->RootNode);
		if (CompositeNode)
		{
			Result += FString::Printf(TEXT("  Children: %d\\n"), CompositeNode->Children.Num());

			for (int32 i = 0; i < CompositeNode->Children.Num(); ++i)
			{
				const FBTCompositeChild& Child = CompositeNode->Children[i];

				if (Child.ChildComposite)
				{
					Result += FString::Printf(TEXT("    [%d] Composite: %s\\n"), i, *Child.ChildComposite->GetClass()->GetName());
				}
				else if (Child.ChildTask)
				{
					Result += FString::Printf(TEXT("    [%d] Task: %s - %s\\n"), i,
						*Child.ChildTask->GetClass()->GetName(),
						*Child.ChildTask->GetNodeName());

					// Check if it's a RunBehavior task (subtree)
					UBTTask_RunBehavior* RunBehavior = Cast<UBTTask_RunBehavior>(Child.ChildTask);
					if (RunBehavior && RunBehavior->GetSubtreeAsset())
					{
						Result += FString::Printf(TEXT("        SubtreeAsset: %s\\n"), *RunBehavior->GetSubtreeAsset()->GetName());
					}
				}

				// Check decorators on this child
				for (UBTDecorator* ChildDec : Child.Decorators)
				{
					if (ChildDec)
					{
						FString ChildDesc = ChildDec->GetStaticDescription();
						Result += FString::Printf(TEXT("      [Decorator] %s\\n"), *ChildDec->GetClass()->GetName());
						Result += FString::Printf(TEXT("        Description: %s\\n"), *ChildDesc);

						UBTDecorator_Blackboard* BBDec = Cast<UBTDecorator_Blackboard>(ChildDec);
						if (BBDec)
						{
							Result += FString::Printf(TEXT("        Key: %s\\n"), *BBDec->GetSelectedBlackboardKey().ToString());
						}
					}
				}
			}
		}
	}

	UE_LOG(LogSLFAutomation, Warning, TEXT("%s"), *Result);
	return Result;
}'''

content = content.replace(old_impl, new_impl)

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("SUCCESS: Fixed BT export implementation")
