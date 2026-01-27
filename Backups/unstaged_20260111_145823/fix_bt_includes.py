"""Fix BT includes"""

file_path = "C:/scripts/SLFConversion/Source/SLFConversion/SLFAutomationLibrary.cpp"

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add missing include
old_includes = '''#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"'''
new_includes = '''#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"
#include "BehaviorTree/BlackboardData.h"'''

content = content.replace(old_includes, new_includes)

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("SUCCESS: Added BlackboardData include")
