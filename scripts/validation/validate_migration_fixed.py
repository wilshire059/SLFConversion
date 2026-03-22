
import os
import re
import json
from collections import defaultdict

JSON_DIR = 'C:/scripts/slfconversion/Exports/BlueprintDNA_v2'
CPP_DIR = 'C:/scripts/slfconversion/Source/SLFConversion'

def load_json_files():
    blueprints = {}
    if not os.path.exists(JSON_DIR):
        print(f'WARNING: JSON directory not found: {JSON_DIR}')
        return blueprints
    # Search recursively for JSON files
    for root, dirs, files in os.walk(JSON_DIR):
        for f in files:
            if f.endswith('.json'):
                path = os.path.join(root, f)
                try:
                    with open(path, 'r', encoding='utf-8') as fp:
                        data = json.load(fp)
                        name = f.replace('.json', '')
                        blueprints[name] = data
                except Exception as e:
                    pass  # Skip invalid JSON files
    return blueprints

def extract_variables(bp_data):
    variables = []
    if 'Variables' in bp_data and isinstance(bp_data['Variables'], dict) and 'List' in bp_data['Variables']:
        for var in bp_data['Variables']['List']:
            if isinstance(var, dict) and 'Name' in var:
                variables.append(var['Name'])
    if 'Variables' in bp_data and isinstance(bp_data['Variables'], list):
        for var in bp_data['Variables']:
            if isinstance(var, dict) and 'Name' in var:
                variables.append(var['Name'])
    return variables

def extract_functions(bp_data):
    functions = []
    if 'Functions' in bp_data:
        funcs = bp_data['Functions']
        if isinstance(funcs, list):
            for func in funcs:
                if isinstance(func, dict) and 'Name' in func:
                    functions.append(func['Name'])
        elif isinstance(funcs, dict) and 'List' in funcs:
            for func in funcs['List']:
                if isinstance(func, dict) and 'Name' in func:
                    functions.append(func['Name'])
    if 'Graphs' in bp_data:
        for graph_name in bp_data['Graphs'].keys():
            if graph_name not in ['EventGraph', 'ConstructionScript']:
                functions.append(graph_name)
    return functions

def extract_dispatchers(bp_data):
    dispatchers = []
    if 'EventDispatchers' in bp_data:
        eds = bp_data['EventDispatchers']
        if isinstance(eds, dict) and 'List' in eds:
            for ed in eds['List']:
                if isinstance(ed, dict) and 'Name' in ed:
                    dispatchers.append(ed['Name'])
        elif isinstance(eds, list):
            for ed in eds:
                if isinstance(ed, dict) and 'Name' in ed:
                    dispatchers.append(ed['Name'])
    return dispatchers

def load_cpp_content():
    cpp_content = ''
    cpp_files = []
    for root, dirs, files in os.walk(CPP_DIR):
        for f in files:
            if f.endswith('.h') or f.endswith('.cpp'):
                path = os.path.join(root, f)
                cpp_files.append(path)
                try:
                    with open(path, 'r', encoding='utf-8', errors='ignore') as fp:
                        cpp_content += fp.read() + chr(10)
                except:
                    pass
    return cpp_content, cpp_files

def find_in_cpp(name, cpp_content):
    normalized = re.sub(r'[^a-zA-Z0-9_]', '', name)
    if re.search(rf'{re.escape(normalized)}', cpp_content, re.IGNORECASE):
        return True
    return False

def main():
    print('=' * 70)
    print('BLUEPRINT TO C++ MIGRATION VALIDATION')
    print('=' * 70)

    blueprints = load_json_files()
    print(f'Loaded {len(blueprints)} Blueprint definitions')

    cpp_content, cpp_files = load_cpp_content()
    print(f'Loaded {len(cpp_files)} C++ files')
    print(f'C++ content length: {len(cpp_content)} chars')

    # Quick sanity check
    test_names = ['ComboSection', 'IsDead', 'Abilities', 'Speed']
    print('Sanity check:')
    for name in test_names:
        if find_in_cpp(name, cpp_content):
            print(f'  {name}: FOUND')
        else:
            print(f'  {name}: NOT FOUND')
    
    gaps = defaultdict(list)
    stats = {'total_vars': 0, 'found_vars': 0, 'total_funcs': 0, 'found_funcs': 0, 'total_disp': 0, 'found_disp': 0}
    
    skip = {'UberGraphFrame', 'DefaultSceneRoot', 'RootComponent', 'Self', 'Target', 'ReturnValue', 'DeltaSeconds'}
    
    for bp_name, bp_data in sorted(blueprints.items()):
        for var in extract_variables(bp_data):
            if var in skip or var.startswith('K2Node_'):
                continue
            stats['total_vars'] += 1
            if find_in_cpp(var, cpp_content):
                stats['found_vars'] += 1
            else:
                gaps[bp_name].append(('var', var))
        
        for func in extract_functions(bp_data):
            if func in skip or func.startswith('K2Node_'):
                continue
            stats['total_funcs'] += 1
            if find_in_cpp(func, cpp_content):
                stats['found_funcs'] += 1
            else:
                gaps[bp_name].append(('func', func))
        
        for disp in extract_dispatchers(bp_data):
            if disp in skip:
                continue
            stats['total_disp'] += 1
            if find_in_cpp(disp, cpp_content):
                stats['found_disp'] += 1
            else:
                gaps[bp_name].append(('disp', disp))
    
    print()
    print('SUMMARY')
    print('=' * 70)
    v_pct = stats['found_vars']/stats['total_vars']*100 if stats['total_vars'] else 100
    f_pct = stats['found_funcs']/stats['total_funcs']*100 if stats['total_funcs'] else 100
    d_pct = stats['found_disp']/stats['total_disp']*100 if stats['total_disp'] else 100
    print(f"Variables:   {stats['found_vars']:4}/{stats['total_vars']:4} ({v_pct:.1f}%)")
    print(f"Functions:   {stats['found_funcs']:4}/{stats['total_funcs']:4} ({f_pct:.1f}%)")
    print(f"Dispatchers: {stats['found_disp']:4}/{stats['total_disp']:4} ({d_pct:.1f}%)")
    
    if gaps:
        print()
        print('TOP GAPS BY BLUEPRINT')
        print('=' * 70)
        sorted_gaps = sorted(gaps.items(), key=lambda x: len(x[1]), reverse=True)
        for bp_name, gap_list in sorted_gaps[:20]:
            vars_list = [g[1] for g in gap_list if g[0]=='var']
            funcs_list = [g[1] for g in gap_list if g[0]=='func']
            print(f"{bp_name}: {len(vars_list)} vars, {len(funcs_list)} funcs")
            if vars_list[:3]:
                print(f"  vars: {', '.join(vars_list[:3])}")
            if funcs_list[:3]:
                print(f"  funcs: {', '.join(funcs_list[:3])}")
    
    with open('C:/scripts/slfconversion/migration_gaps.txt', 'w') as fp:
        for bp, glist in sorted(gaps.items()):
            fp.write(bp + ":\n")
            for gtype, gname in glist:
                fp.write("  [" + gtype + "] " + gname + "\n")
    print()
    print('Full report: C:/scripts/slfconversion/migration_gaps.txt')

if __name__ == '__main__':
    main()
