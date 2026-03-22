
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"
with open(file_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

print("--- FunctionSignatures Value ---")
if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
    funcs = data['FunctionSignatures']['Functions']
    print(f"Type: {type(funcs)}")
    if isinstance(funcs, list) and len(funcs) > 0:
        print(f"Item 0: {funcs[0].get('Name')}")
