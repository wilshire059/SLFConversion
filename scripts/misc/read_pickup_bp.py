
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\Blueprint\B_PickupItem.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    print(f"Name: {data.get('Name')}")
    print(f"Parent: {data.get('ParentClass')}")

    print("\n--- Properties/Components ---")
    if 'Components' in data and 'Hierarchy' in data['Components']:
        for c in data['Components']['Hierarchy']:
            print(f"- Component: {c.get('Name')} Class: {c.get('Class')}")

    print("\n--- Variables ---")
    if 'Variables' in data and 'List' in data['Variables']:
        for v in data['Variables']['List']:
            print(f"- {v.get('Name')} Type: {v.get('Type')}")
            
    print("\n--- Functions ---")
    if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
        for f in data['FunctionSignatures']['Functions']:
             print(f"- {f.get('Name')}")

except Exception as e:
    print(f"Error: {e}")
