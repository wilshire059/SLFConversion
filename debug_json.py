import json
import sys

path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\Component\AC_LootDropManager.json"

try:
    with open(path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    if 'FunctionSignatures' in data:
        funcs = data['FunctionSignatures']
        print(f"FunctionSignatures type: {type(funcs)}")
        if isinstance(funcs, list) and len(funcs) > 0:
            print(f"First func element type: {type(funcs[0])}")
            print(f"First func content: {funcs[0]}")
        elif isinstance(funcs, dict):
            print(f"Function Keys: {list(funcs.keys())[:5]}")


except Exception as e:
    print(f"Error: {e}")
