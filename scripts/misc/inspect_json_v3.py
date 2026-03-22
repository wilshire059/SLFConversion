
import json
import os

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"

def find_key(data, target_key, path=""):
    if isinstance(data, dict):
        for k, v in data.items():
            if k == target_key:
                print(f"Found {target_key} at {path}/{k}: {str(v)[:100]}...")
            find_key(v, target_key, f"{path}/{k}")
    elif isinstance(data, list):
        for i, item in enumerate(data):
            find_key(item, target_key, f"{path}[{i}]")

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
        
    print(f"Variables type: {type(data.get('Variables'))}")
    if isinstance(data.get('Variables'), list):
         print(f"Variables count: {len(data['Variables'])}")
         if len(data['Variables']) > 0:
             print(f"First var: {data['Variables'][0].get('Name')}")
             
    logic = data.get('Logic', {})
    all_graphs = logic.get('AllGraphs', [])
    print(f"AllGraphs count: {len(all_graphs)}")
    for g in all_graphs[:5]:
        print(f"Graph Name: {g.get('Name')} Type: {g.get('Type')}")
        
    # Search for Description
    print("Searching for Description...")
    find_key(data, "Description")
    find_key(data, "ToolTip")
    
except Exception as e:
    print(f"Error: {e}")
