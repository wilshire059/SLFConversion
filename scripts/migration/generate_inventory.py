
import json
import os
import sys

# Directories to look for blueprints
TARGET_DIRS = {
    'Blueprint',
    'WidgetBlueprint',
    'AnimBlueprint',
    'EditorUtilityBlueprint',
    'EditorUtilityWidgetBlueprint'
}

ROOT_DIR = r"c:\scripts\SLFConversion\Exports\BlueprintDNA_v2"
OUTPUT_FILE = r"c:\scripts\SLFConversion\blueprints_inventory.yaml"

def find_description(data):
    # Try common fields
    if isinstance(data, dict):
        if 'BlueprintDescription' in data:
            return data['BlueprintDescription']
        if 'MetaData' in data and 'Tooltip' in data['MetaData']:
             return data['MetaData']['Tooltip']
             
    # Recursive search for 'Tooltip' or 'Description'
    # Limiting depth/width to avoid performance hit on huge files, but these are JSONs
    # Actually, simplest is just to return "No description available" if not found easily.
    return "No description available"

def get_blueprint_info(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
            
        name = data.get('Name', os.path.basename(file_path).replace('.json', ''))
        bp_type = data.get('BlueprintType', 'Unknown')
        parent_class = data.get('ParentClass', 'Unknown')
        
        description = find_description(data)
        if description == "No description available":
            # Try to construct a useful description
            description = f"Type: {bp_type}, Parent: {parent_class}"
            
        functions = []
        if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
            funcs = data['FunctionSignatures']['Functions']
            if isinstance(funcs, list):
                functions = [f.get('Name') for f in funcs if f.get('Name')]
                
        variables = []
        if 'Variables' in data and 'List' in data['Variables']:
            vars_list = data['Variables']['List']
            if isinstance(vars_list, list):
                variables = [v.get('Name') for v in vars_list if v.get('Name')]
                
        return {
            'name': name,
            'description': description,
            'functions': functions,
            'variables': variables
        }
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return None

def main():
    inventory = []
    
    print(f"Scanning {ROOT_DIR}...")
    
    for root, dirs, files in os.walk(ROOT_DIR):
        dir_name = os.path.basename(root)
        if dir_name in TARGET_DIRS:
            print(f"Processing directory: {dir_name}")
            for file in files:
                if file.endswith('.json'):
                    file_path = os.path.join(root, file)
                    info = get_blueprint_info(file_path)
                    if info:
                        inventory.append(info)
                        
    print(f"Found {len(inventory)} blueprints.")
    
    # Write to YAML manually to avoid dependencies
    print(f"Writing to {OUTPUT_FILE}...")
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        for item in inventory:
            f.write(f"- name: {item['name']}\n")
            f.write(f"  description: {item['description']}\n")
            
            f.write("  functions:\n")
            if item['functions']:
                for func in item['functions']:
                    f.write(f"    - {func}\n")
            else:
                f.write("    []\n")
                
            f.write("  variables:\n")
            if item['variables']:
                for var in item['variables']:
                    f.write(f"    - {var}\n")
            else:
                f.write("    []\n")
            
            f.write("\n")
            
    print("Done.")

if __name__ == '__main__':
    main()
