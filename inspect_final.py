
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"
with open(file_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

print("--- FunctionSignatures ---")
if 'FunctionSignatures' in data:
    fs = data['FunctionSignatures']
    if isinstance(fs, dict):
        if 'List' in fs:
            print(f"FunctionSignatures['List'] found, len: {len(fs['List'])}")
            if len(fs['List']) > 0:
                print(f"Item 0 Name: {fs['List'][0].get('Name')}")
        else:
            print(f"FunctionSignatures keys: {list(fs.keys())[:5]}")

print("--- Graphs ---")
if 'Logic' in data and 'AllGraphs' in data['Logic']:
    for g in data['Logic']['AllGraphs']:
        print(f"GraphName: {g.get('GraphName')} Type: {g.get('GraphType')}")
