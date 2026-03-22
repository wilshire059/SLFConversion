
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    print("\n--- Components Detailed Inspection ---")
    if 'Components' in data:
        comps = data['Components']
        if isinstance(comps, dict):
            # Check if it has a 'List' key or if keys are component names
            print(f"Components keys: {list(comps.keys())}")
            if 'List' in comps:
                for c in comps['List']:
                    print(f"Component in List: {c.get('Name')} Type: {c.get('Type')}")
            else:
                 # iterate first few keys
                 for k, v in list(comps.items())[:3]:
                     print(f"Key: {k} Value Type: {type(v)}")

except Exception as e:
    print(f"Error: {e}")
