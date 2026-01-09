
import json
import os

# Pick a complex blueprint that likely has documented functions
# B_BaseCharacter is usually a good candidate
file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"

def print_recursive_search(data, target_keys, path=""):
    if isinstance(data, dict):
        for k, v in data.items():
            if k in target_keys:
                print(f"FOUND {k} at {path}/{k}: {str(v)[:100]}")
            
            # recursive step
            if isinstance(v, (dict, list)):
                print_recursive_search(v, target_keys, f"{path}/{k}")
    elif isinstance(data, list):
        for i, item in enumerate(data):
            print_recursive_search(item, target_keys, f"{path}[{i}]")

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    # 1. Inspect FunctionSignatures for MetaData or Description
    print("--- FunctionSignatures Inspection ---")
    if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
        for func in data['FunctionSignatures']['Functions']:
             name = func.get('Name')
             print(f"Function: {name}")
             print(f"  Keys: {list(func.keys())}")
             if 'MetaData' in func:
                 print(f"  MetaData: {func['MetaData']}")
             if 'Description' in func:
                 print(f"  Description: {func['Description']}")
             if 'Tooltip' in func:
                 print(f"  Tooltip: {func['Tooltip']}")
    
    # 2. Inspect Components
    print("\n--- Components Inspection ---")
    if 'Components' in data:
        print(f"Components Type: {type(data['Components'])}")
        # Usually it's a list or a list is inside 'List' or 'Nodes'
        # Let's just print keys/structure
        if isinstance(data['Components'], list):
             for comp in data['Components']:
                 print(f"  Component: {comp.get('Name')} Type: {comp.get('Type')}")
                 
    # 3. Interfaces
    print("\n--- Interfaces Inspection ---")
    if 'Interfaces' in data:
         print(f"Interfaces: {data['Interfaces']}")
         
    # 4. EventDispatchers
    print("\n--- EventDispatchers Inspection ---")
    if 'EventDispatchers' in data:
         print(f"EventDispatchers: {data['EventDispatchers']}")

except Exception as e:
    print(f"Error: {e}")
