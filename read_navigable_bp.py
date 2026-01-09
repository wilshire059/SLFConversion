
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\WidgetBlueprint\W_Navigable_InputReader.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    print(f"Name: {data.get('Name')}")
    print(f"Parent: {data.get('ParentClass')}")

    print("\n--- Functions/Graphs ---")
    if 'Logic' in data and 'AllGraphs' in data['Logic']:
        for g in data['Logic']['AllGraphs']:
            print(f"- Graph: {g.get('GraphName')} Type: {g.get('GraphType')}")

except Exception as e:
    print(f"Error: {e}")
