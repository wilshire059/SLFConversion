
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2\Blueprint\B_BaseCharacter.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    if 'Components' in data and 'Hierarchy' in data['Components']:
        h = data['Components']['Hierarchy']
        if isinstance(h, list) and len(h) > 0:
            print(f"First component keys: {h[0].keys()}")
            print(f"First component Name: {h[0].get('Name')}")
            print(f"First component Type: {h[0].get('Type')}")
            # Check for properties that might be interesting?
            
except Exception as e:
    print(f"Error: {e}")
