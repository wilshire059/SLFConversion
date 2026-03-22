
import json

file_path = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\Blueprint\B_Soulslike_Enemy.json"

try:
    with open(file_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
        
    print(f"Name: {data.get('Name')}")
    print(f"Parent: {data.get('ParentClass')}")
    
    print("\n--- Variables ---")
    if 'Variables' in data and 'List' in data['Variables']:
        for v in data['Variables']['List']:
            v_name = v.get('Name')
            v_replicated = v.get('IsReplicated', False)
            v_type = "Unknown"
            if 'Type' in v and isinstance(v['Type'], dict):
                 v_type = v['Type'].get('SubCategoryObject', v['Type'].get('SubCategory'))
            print(f"- {v_name} (Replicated: {v_replicated}, Type: {v_type})")
            
    print("\n--- Functions (from FunctionSignatures) ---")
    if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
        for f in data['FunctionSignatures']['Functions']:
            print(f"- {f.get('Name')}")

    print("\n--- Events/Graphs ---")
    if 'Logic' in data and 'AllGraphs' in data['Logic']:
        for g in data['Logic']['AllGraphs']:
            print(f"- Graph: {g.get('GraphName')} Type: {g.get('GraphType')}")

    print("\n--- Properties/Components ---")
    if 'Components' in data and 'Hierarchy' in data['Components']:
        for c in data['Components']['Hierarchy']:
            print(f"- Component: {c.get('Name')} Class: {c.get('Class')}")

except Exception as e:
    print(f"Error: {e}")
