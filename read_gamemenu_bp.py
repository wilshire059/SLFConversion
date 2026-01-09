
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\WidgetBlueprint\W_GameMenu.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    print(f"Name: {data.get('Name')}")
    print(f"Parent: {data.get('ParentClass')}")

    print("\n--- Components (Widget Hierarchy) ---")
    if 'Components' in data and 'Hierarchy' in data['Components']:
        for i, c in enumerate(data['Components']['Hierarchy']):
             print(f"- {c.get('Name')} ({c.get('Class')})")
            
    print("\n--- Functions/Graphs ---")
    if 'Logic' in data and 'AllGraphs' in data['Logic']:
        for g in data['Logic']['AllGraphs']:
            print(f"- Graph: {g.get('GraphName')} Type: {g.get('GraphType')}")
            
    print("\n--- Bindings (Events) ---")
    if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
         for f in data['FunctionSignatures']['Functions']:
             print(f"- Function: {f.get('Name')}")

except Exception as e:
    print(f"Error: {e}")
