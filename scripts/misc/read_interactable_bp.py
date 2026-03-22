
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\Blueprint\B_Interactable.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    print(f"Name: {data.get('Name')}")
    print("\n--- Components ---")
    if 'Components' in data and 'Hierarchy' in data['Components']:
        for c in data['Components']['Hierarchy']:
            print(f"- Component: {c.get('Name')} Class: {c.get('Class')}")
            
    print("\n--- Graphs/Functions ---")
    if 'Logic' in data and 'AllGraphs' in data['Logic']:
         for g in data['Logic']['AllGraphs']:
             print(f"- Graph: {g.get('GraphName')} Type: {g.get('GraphType')}")

except Exception as e:
    print(f"Error: {e}")
