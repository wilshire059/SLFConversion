import json
import re
import os
import sys

def normalize_name(name):
    """Normalize names for comparison (remove spaces, ? suffix, etc)."""
    name = name.replace(" ", "").replace("?", "")
    return name

def parse_json_blueprint(json_path):
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    variables = {}
    functions = set()
    dispatchers = set()

    # Extract Variables
    if 'Variables' in data:
        var_source = []
        if isinstance(data['Variables'], list):
            var_source = data['Variables']
        elif isinstance(data['Variables'], dict) and 'List' in data['Variables']:
            var_source = data['Variables']['List']

        for var in var_source:

            name = var.get('Name', '')
            type_info = var.get('Type', {}).get('BaseType', 'Unknown')
            variables[name] = type_info

    # Extract Functions
    if 'FunctionSignatures' in data:
        func_source = []
        if isinstance(data['FunctionSignatures'], list):
            func_source = data['FunctionSignatures']
        elif isinstance(data['FunctionSignatures'], dict) and 'Functions' in data['FunctionSignatures']:
            func_source = data['FunctionSignatures']['Functions']

        for func in func_source:
            functions.add(func.get('Name', ''))

    # Extract Dispatchers (often listed in function signatures or separate depending on export)
    # The export tool 'BlueprintDNA' likely puts them in FunctionSignatures or I need to check properties
    # For now, let's treat them as functions or separate check if possible.
    
    return variables, functions

def parse_cpp_header(header_path):
    if not os.path.exists(header_path):
        return None, None, None

    with open(header_path, 'r', encoding='utf-8') as f:
        content = f.read()

    variables = set()
    functions = set()
    dispatchers = set()

    # Regex for properties: UPROPERTY(...) Type Name;
    # This is a basic regex and might miss complex types or formatting, but good for a first pass
    # Matches: UPROPERTY(...) \n \t Type Name;
    prop_pattern = re.compile(r'UPROPERTY\s*\(.*?\)\s*(?:[\w<>:*,]*)\s+([\w<>:*]+)\s+(\w+)\s*;', re.DOTALL | re.MULTILINE)
    
    # Simpler line-by-line scan might be more robust for irregular formatting
    lines = content.split('\n')
    is_uproperty = False
    
    for i, line in enumerate(lines):
        line = line.strip()
        if line.startswith('UPROPERTY'):
            is_uproperty = True
            continue
        
        if is_uproperty and line and not line.startswith('//'):
            # This line should contain the variable declaration
            # Type Name;
            match = re.search(r'[\w<>:*]+\s+(\w+)\s*;', line)
            if match:
                variables.add(match.group(1))
            is_uproperty = False
            continue

        if line.startswith('UFUNCTION'):
            # Look ahead for function name
            # void FuncName(...); or Type FuncName(...);
            # We assume functional declarations follow UFUNCTION
            next_line_idx = i + 1
            if next_line_idx < len(lines):
                func_decl = lines[next_line_idx].strip()
                # Extract 'FuncName' before '('
                match = re.search(r'\w+\s+(\w+)\s*\(', func_decl)
                if match:
                    functions.add(match.group(1))
        
        if line.startswith('DECLARE_DYNAMIC_MULTICAST_DELEGATE'):
            # DECLARE_DYNAMIC_MULTICAST_DELEGATE...(DelegateName, ...);
             match = re.search(r'DELEGATE\w*\s*\(\s*(\w+)', line)
             if match:
                 dispatchers.add(match.group(1))

    return variables, functions, dispatchers

def compare(bp_json, cpp_header):
    if not os.path.exists(cpp_header):
        print(f"MISSING_HEADER: {cpp_header}")
        return

    bp_vars, bp_funcs = parse_json_blueprint(bp_json)
    cpp_vars, cpp_funcs, cpp_dispatchers = parse_cpp_header(cpp_header)

    print(f"--- Analysis for {os.path.basename(bp_json)} vs {os.path.basename(cpp_header)} ---")
    
    # Variable Check
    missing_vars = []
    for var_name in bp_vars:
        norm_var = normalize_name(var_name)
        if norm_var not in cpp_vars and var_name not in cpp_vars:
            # Try fuzzy match or known conversions (e.g. bBoolean)
            if f"b{norm_var}" not in cpp_vars:
                 missing_vars.append(var_name)
    
    if missing_vars:
        print(f"MISSING VARIABLES ({len(missing_vars)}):")
        for v in missing_vars:
            print(f"  - {v}")
    else:
        print("VARIABLES: OK")

    # Function Check
    # Note: C++ often prefixes events with 'Event' or handles them differently. 
    # This check identifies functions strictly by name match.
    missing_funcs = []
    for func in bp_funcs:
        norm_func = normalize_name(func)
        if norm_func not in cpp_funcs and func not in cpp_funcs:
            # Check for _Implementation or On... variants?
            # For now, strict check to highlight gaps
             missing_funcs.append(func)
    
    if missing_funcs:
        print(f"MISSING /RENAMED FUNCTIONS ({len(missing_funcs)}):")
        # Filter out obvious getters/setters possibly?
        for f in missing_funcs:
            print(f"  - {f}")
    else:
        print("FUNCTIONS: OK")
    
    print("\n")


# Main execution block
if __name__ == "__main__":
    components_dir = r"c:\scripts\SLFConversion\Exports\BlueprintDNA\Component"
    source_dir = r"c:\scripts\SLFConversion\Source\SLFConversion\Components"
    
    # List of critical components to check
    targets = [
        ("AC_LootDropManager", "AC_LootDropManager"),
        ("AC_StatManager", "AC_StatManager"),
        ("AC_AI_BehaviorManager", "AC_AI_BehaviorManager"),
        ("AC_EquipmentManager", "AC_EquipmentManager"),
        ("AC_ProgressManager", "AC_ProgressManager"),
        ("AC_CombatManager", "AC_CombatManager"),
        ("AC_AI_CombatManager", "AC_AI_CombatManager"),
        ("AC_ActionManager", "AC_ActionManager"),
    ]

    for json_name, cpp_name in targets:
        json_path = os.path.join(components_dir, f"{json_name}.json")
        header_path = os.path.join(source_dir, f"{cpp_name}.h")
        
        if os.path.exists(json_path):
            compare(json_path, header_path)
        else:
            print(f"Skipping {json_name} (JSON not found)")
