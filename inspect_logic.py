
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"
with open(file_path, 'r', encoding='utf-8') as f:
    data = json.load(f)

print("--- FunctionSignatures ---")
if 'FunctionSignatures' in data:
    fs = data['FunctionSignatures']
    print(f"Type: {type(fs)}")
    if isinstance(fs, list) and len(fs) > 0:
        print(f"Item 0 keys: {fs[0].keys()}")
        print(f"Item 0 Name: {fs[0].get('Name')}")

print("--- Logic ---")
if 'Logic' in data:
    logic = data['Logic']
    print(f"Logic keys: {logic.keys()}")
    if 'AllGraphs' in logic:
        graphs = logic['AllGraphs']
        print(f"AllGraphs len: {len(graphs)}")
        if len(graphs) > 0:
            print(f"Graph 0 keys: {graphs[0].keys()}")
            print(f"Graph 0 Name: {graphs[0].get('Name')}")
