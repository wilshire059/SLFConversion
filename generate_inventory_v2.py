
import json
import os
import collections

ROOT_DIR = r"c:\scripts\SLFConversion\Exports\BlueprintDNA"
OUTPUT_FILE = r"c:\scripts\SLFConversion\blueprints_inventory.yaml"

def get_description_from_metadata(obj):
    if 'MetaData' in obj:
        md = obj['MetaData']
        if 'Tooltip' in md and md['Tooltip']:
            return md['Tooltip'].strip().replace('\n', ' ')
        if 'Description' in md and md['Description']:
            return md['Description'].strip().replace('\n', ' ')
    if 'Description' in obj and obj['Description']:
        return obj['Description'].strip().replace('\n', ' ')
    return None

def get_blueprint_details(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
            
        # Basic Info
        name = data.get('Name', os.path.basename(file_path).replace('.json', ''))
        bp_type = data.get('BlueprintType', 'Unknown')
        parent_class = data.get('ParentClass', 'Unknown')
        
        # Description
        description = data.get('BlueprintDescription')
        if not description:
             # Try metadata
             pass # heuristic usually in BlueprintDescription or we construct it
        if not description:
             description = f"Type: {bp_type}, Parent: {parent_class}"
        else:
             description = description.replace('\n', ' ')

        # Functions
        functions = []
        if 'FunctionSignatures' in data and 'Functions' in data['FunctionSignatures']:
            for f_obj in data['FunctionSignatures']['Functions']:
                f_name = f_obj.get('Name')
                if f_name:
                    f_desc = get_description_from_metadata(f_obj)
                    functions.append({'name': f_name, 'desc': f_desc})

        # Variables
        variables = []
        if 'Variables' in data and 'List' in data['Variables']:
            for v_obj in data['Variables']['List']:
                v_name = v_obj.get('Name')
                v_type = "Unknown"
                if 'Type' in v_obj:
                    # Type can be complex dict
                    t = v_obj['Type']
                    if isinstance(t, dict):
                         v_type = t.get('SubCategoryObject', t.get('SubCategory', 'Unknown'))
                    else:
                         v_type = str(t)
                variables.append({'name': v_name, 'type': v_type})

        # Components
        components = []
        if 'Components' in data and 'Hierarchy' in data['Components']:
             for c_obj in data['Components']['Hierarchy']:
                 c_name = c_obj.get('Name')
                 c_class = c_obj.get('Class')
                 components.append({'name': c_name, 'type': c_class})

        # Interfaces
        interfaces = []
        if 'Interfaces' in data and 'List' in data['Interfaces']:
             for i_obj in data['Interfaces']['List']:
                 i_name = i_obj.get('Name')
                 interfaces.append(i_name)

        # Event Dispatchers
        dispatchers = []
        if 'EventDispatchers' in data and 'List' in data['EventDispatchers']:
             for d_obj in data['EventDispatchers']['List']:
                 d_name = d_obj.get('Name')
                 dispatchers.append(d_name)

        return {
            'name': name,
            'description': description,
            'functions': functions,
            'variables': variables,
            'components': components,
            'interfaces': interfaces,
            'dispatchers': dispatchers
        }

    except Exception as e:
        return None

def main():
    inventory_by_category = collections.defaultdict(list)
    total_count = 0
    
    print(f"Scanning {ROOT_DIR}...")
    for root, dirs, files in os.walk(ROOT_DIR):
        rel_path = os.path.relpath(root, ROOT_DIR)
        category = "Root" if rel_path == '.' else rel_path.split(os.sep)[0]
        
        for file in files:
            if file.endswith('.json'):
                path = os.path.join(root, file)
                details = get_blueprint_details(path)
                if details:
                    inventory_by_category[category].append(details)
                    total_count += 1
                    
    print(f"Found {total_count} items. Writing to {OUTPUT_FILE}...")
    
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        for category in sorted(inventory_by_category.keys()):
            f.write(f"# Category: {category}\n")
            
            # Sort items by name
            items = sorted(inventory_by_category[category], key=lambda x: x['name'])
            
            for item in items:
                f.write(f"- name: {item['name']}\n")
                f.write(f"  category: {category}\n")
                f.write(f"  description: {item['description']}\n")
                
                # Functions
                f.write("  functions:\n")
                if item['functions']:
                    for func in item['functions']:
                        if func['desc']:
                            f.write(f"    - {func['name']}: {func['desc']}\n")
                        else:
                            f.write(f"    - {func['name']}\n")
                else:
                    f.write("    []\n")
                    
                # Variables
                f.write("  variables:\n")
                if item['variables']:
                    for var in item['variables']:
                         f.write(f"    - {var['name']} ({var['type']})\n")
                else:
                    f.write("    []\n")
                    
                # Components
                f.write("  components:\n")
                if item['components']:
                     for comp in item['components']:
                         f.write(f"    - {comp['name']} ({comp['type']})\n")
                else:
                     f.write("    []\n")
                     
                # Interfaces
                f.write("  interfaces:\n")
                if item['interfaces']:
                     for i in item['interfaces']:
                         f.write(f"    - {i}\n")
                else:
                     f.write("    []\n")
                
                # Dispatchers
                f.write("  event_dispatchers:\n")
                if item['dispatchers']:
                     for d in item['dispatchers']:
                         f.write(f"    - {d}\n")
                else:
                     f.write("    []\n")
                     
                f.write("\n")

    print("Done.")

if __name__ == '__main__':
    main()
