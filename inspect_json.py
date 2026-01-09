
import json
import os

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_Action.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
        
    print(f"Top level keys: {list(data.keys())}")
    
    if 'Functions' in data:
        print(f"Functions found: {len(data['Functions'])}")
        if len(data['Functions']) > 0:
            print(f"First function keys: {list(data['Functions'][0].keys())}")
            print(f"First function name: {data['Functions'][0].get('Name')}")
            
    if 'Variables' in data:
        print(f"Variables found: {len(data['Variables'])}")
        if len(data['Variables']) > 0:
            print(f"First variable keys: {list(data['Variables'][0].keys())}")
            print(f"First variable name: {data['Variables'][0].get('Name')}")
            
    if 'Properties' in data: # sometimes variables are properties
         print(f"Properties found: {len(data['Properties'])}")

except Exception as e:
    print(f"Error: {e}")
