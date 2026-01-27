"""Quick script to add BT functions to header"""

file_path = "C:/scripts/SLFConversion/Source/SLFConversion/SLFAutomationLibrary.h"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

old_text = '''	// Fix all montages with ANS_RegisterAttackSequence notifies
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Montage")
	static int32 FixAllMontageComboNotifies();

#endif // WITH_EDITOR
};'''

new_text = '''	// Fix all montages with ANS_RegisterAttackSequence notifies
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|Montage")
	static int32 FixAllMontageComboNotifies();

	// BEHAVIOR TREE DIAGNOSTICS
	// Export behavior tree structure including decorator IntValue configuration
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString ExportBehaviorTreeStructure(const FString& BehaviorTreePath);

	// Export all behavior tree subtrees and their decorator config
	UFUNCTION(BlueprintCallable, Category = "SLF Automation|BehaviorTree")
	static FString ExportAllBTSubtrees();

#endif // WITH_EDITOR
};'''

if old_text in content:
    content = content.replace(old_text, new_text)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Added BT functions to header")
else:
    print("ERROR: Old text not found")
