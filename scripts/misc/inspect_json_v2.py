
import json
import os

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_Action.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
        
    print(f"Logic keys: {list(data.get('Logic', {}).keys())}")
    
    if 'Logic' in data:
        logic = data['Logic']
        if 'Functions' in logic:
             print(f"Functions in Logic found: {len(logic['Functions'])}")
             # Print first function
             if len(logic['Functions']) > 0:
                 print(f"First function: {logic['Functions'][0].get('Name')}")

        if 'Graphs' in logic:
            print(f"Graphs in Logic found: {len(logic['Graphs'])}")
            if len(logic['Graphs']) > 0:
                 print(f"First graph: {logic['Graphs'][0].get('Name')}")
                 
    if 'FunctionSignatures' in data:
        print(f"FunctionSignatures found: {len(data['FunctionSignatures'])}")
        
    print("--- Variables ---")
    if 'Variables' in data:
        for v in data['Variables'][:3]:
            print(f"Variable: {v.get('Name')} Type: {v.get('Type')}")
            
    print("--- Description Search ---")
    # Check for description in top level or BlueprintClass
    print(f"BlueprintClass: {data.get('BlueprintClass')}")
    # Sometimes description is in a 'MetaData' field
    
except Exception as e:
    print(f"Error: {e}")
