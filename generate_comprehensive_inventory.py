
import json
import os
import sys

# We will scan ALL subdirectories.
ROOT_DIR = r"c:\scripts\SLFConversion\Exports\BlueprintDNA"
OUTPUT_FILE = r"c:\scripts\SLFConversion\blueprints_inventory_comprehensive.yaml"

def find_description(data):
    if isinstance(data, dict):
        if 'BlueprintDescription' in data:
            return data['BlueprintDescription']
        if 'MetaData' in data and 'Tooltip' in data['MetaData']:
             return data['MetaData']['Tooltip']
    return "No description available"

def is_likely_blueprint(data):
    # Heuristics to determine if a JSON file represents a Blueprint
    if 'BlueprintClass' in data:
        return True
    if 'ParentClass' in data:
        return True
    if 'BlueprintType' in data:
        return True
    # Interfaces often look like blueprints
    if 'Type' in data:
        t = data['Type']
        if isinstance(t, str) and 'BPTYPE' in t:
             return True
    return False

def get_blueprint_info(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
            
        if not is_likely_blueprint(data):
            return None
            
        name = data.get('Name', os.path.basename(file_path).replace('.json', ''))
        bp_type = data.get('BlueprintType', 'Unknown')
        parent_class = data.get('ParentClass', 'Unknown')
        
        description = find_description(data)
        if description == "No description available":
            description = f"Type: {bp_type}, Parent: {parent_class}"
            
        functions = []
        # Check FunctionSignatures
        if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
            funcs = data['FunctionSignatures']['Functions']
            if isinstance(funcs, list):
                functions.extend([f.get('Name') for f in funcs if f.get('Name')])
        
        # Check Logic graphs for events that act like functions or entry points
        if 'Logic' in data and 'AllGraphs' in data['Logic']:
            for g in data['Logic']['AllGraphs']:
                g_name = g.get('GraphName')
                if g_name and g_name not in functions:
                     # Some graphs are just internal, but EventGraph is main. 
                     # We might want to list graphs as "functions" loosely or separate them.
                     # The prompt asked for "all function names". 
                     # Often in UE BPs, graphs contain logic but aren't functions. 
                     # But UserConstructionScript is important.
                     if g_name != "EventGraph": # EventGraph is generic
                         functions.append(g_name)

        variables = []
        if 'Variables' in data and 'List' in data['Variables']:
            vars_list = data['Variables']['List']
            if isinstance(vars_list, list):
                variables = [v.get('Name') for v in vars_list if v.get('Name')]
                
        return {
            'name': name,
            'description': description,
            'functions': list(set(functions)), # dedupe
            'variables': variables,
            'file_path': file_path
        }
    except Exception as e:
        # print(f"Error processing {file_path}: {e}")
        return None

def main():
    inventory_by_category = {}
    total_count = 0
    
    print(f"Scanning {ROOT_DIR} recursively...")
    
    for root, dirs, files in os.walk(ROOT_DIR):
        # Determine category from the direct child of ROOT_DIR
        rel_path = os.path.relpath(root, ROOT_DIR)
        if rel_path == '.':
            category = "Root"
        else:
             # Take the top-level folder name as category
             category = rel_path.split(os.sep)[0]
             
        for file in files:
            if file.endswith('.json'):
                file_path = os.path.join(root, file)
                info = get_blueprint_info(file_path)
                if info:
                    if category not in inventory_by_category:
                        inventory_by_category[category] = []
                    inventory_by_category[category].append(info)
                    total_count += 1
                        
    print(f"Found {total_count} blueprints across {len(inventory_by_category)} categories.")
    
    print(f"Writing to {OUTPUT_FILE}...")
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        # Sort categories for cleaner output
        for category in sorted(inventory_by_category.keys()):
            f.write(f"# Category: {category}\n")
            
            # Sort items by name
            items = sorted(inventory_by_category[category], key=lambda x: x['name'])
            
            for item in items:
                f.write(f"- name: {item['name']}\n")
                f.write(f"  category: {category}\n")
                f.write(f"  description: {item['description']}\n")
                
                f.write("  functions:\n")
                if item['functions']:
                    for func in sorted(item['functions']):
                        f.write(f"    - {func}\n")
                else:
                    f.write("    []\n")
                    
                f.write("  variables:\n")
                if item['variables']:
                    for var in sorted(item['variables']):
                        f.write(f"    - {var}\n")
                else:
                    f.write("    []\n")
                
                f.write("\n")
            f.write("\n")
            
    print("Done.")

if __name__ == '__main__':
    main()
