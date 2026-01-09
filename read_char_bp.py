
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\Blueprint\B_Soulslike_Character.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
        
    print(f"Name: {data.get('Name')}")
    print(f"Parent: {data.get('ParentClass')}")

    print("\n--- Components (First 10) ---")
    if 'Components' in data and 'Hierarchy' in data['Components']:
        for i, c in enumerate(data['Components']['Hierarchy']):
            if i < 10:
                print(f"- {c.get('Name')} ({c.get('Class')})")
            
    print("\n--- Functions (Searching for Interact/Trace) ---")
    if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
        for f in data['FunctionSignatures']['Functions']:
            name = f.get('Name')
            if 'Interact' in name or 'Trace' in name or 'Input' in name:
                print(f"- {name}")
                
    print("\n--- Logic Graphs (Searching for Input/Interact) ---")
    if 'Logic' in data and 'AllGraphs' in data['Logic']:
        for g in data['Logic']['AllGraphs']:
            name = g.get('GraphName')
            if name and ('Input' in name or 'Interact' in name):
                print(f"- Graph: {name}")

except Exception as e:
    print(f"Error: {e}")
