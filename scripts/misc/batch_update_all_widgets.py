# Batch script to add events to ALL remaining widgets
import json
import os
import glob

def extract_events(json_path):
    """Extract all custom events from a widget JSON."""
    if not os.path.exists(json_path):
        return {}

    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    events = {}

    def find_events(obj):
        if isinstance(obj, dict):
            if obj.get('NodeClass') == 'K2Node_CustomEvent':
                event_name = obj.get('CustomEventName', '')
                if event_name and event_name.startswith('Event '):
                    params = []
                    for pin in obj.get('Pins', []):
                        name = pin.get('Name', '')
                        if name not in ['OutputDelegate', 'then', 'self'] and pin.get('Direction') == 'Output':
                            pin_type = pin.get('Type', {})
                            category = pin_type.get('Category', '')
                            if category and category not in ['exec', 'delegate']:
                                params.append({
                                    'name': name,
                                    'type': category,
                                    'subtype': pin_type.get('SubCategoryObject', ''),
                                    'subtype_path': pin_type.get('SubCategoryObjectPath', ''),
                                    'is_array': pin_type.get('IsArray', False),
                                    'is_ref': pin_type.get('IsReference', False)
                                })
                    if event_name not in events:
                        events[event_name] = params
            for v in obj.values():
                find_events(v)
        elif isinstance(obj, list):
            for item in obj:
                find_events(item)

    find_events(data)
    return events

def type_to_cpp(param):
    """Convert Blueprint type to C++ type."""
    category = param['type']
    subtype = param['subtype']
    subtype_path = param.get('subtype_path', '')
    is_array = param.get('is_array', False)
    is_ref = param.get('is_ref', False)

    # Handle common types
    type_map = {
        'bool': 'bool',
        'int': 'int32',
        'int64': 'int64',
        'float': 'float',
        'real': 'double',
        'byte': 'uint8',
        'text': 'FText',
        'string': 'FString',
        'name': 'FName',
        'vector': 'FVector',
        'rotator': 'FRotator',
        'transform': 'FTransform',
        'linearcolor': 'FLinearColor',
        'color': 'FColor',
    }

    if category in type_map:
        base_type = type_map[category]
    elif category == 'object':
        if subtype:
            # Clean up class name
            clean_name = subtype.replace('_C', '')
            # Add U prefix for UObject types, A for Actors
            if 'Actor' in clean_name or clean_name.startswith('A') or clean_name.startswith('B_'):
                if not clean_name.startswith('A'):
                    clean_name = 'A' + clean_name
            elif not clean_name.startswith('U'):
                clean_name = 'U' + clean_name
            base_type = f'{clean_name}*'
        else:
            base_type = 'UObject*'
    elif category == 'struct':
        if subtype:
            # Handle struct naming
            if subtype.startswith('F'):
                base_type = subtype
            elif 'SLF' in subtype or 'Soulslike' in subtype:
                base_type = f'FSLF{subtype}'
            else:
                base_type = f'F{subtype}'
            # Fix common struct names
            base_type = base_type.replace('FGameplayTag', 'FGameplayTag')
            base_type = base_type.replace('FFGameplayTag', 'FGameplayTag')
            base_type = base_type.replace('FSLFGameplayTag', 'FGameplayTag')
            base_type = base_type.replace('FItemInfo', 'FSLFItemInfo')
            base_type = base_type.replace('FItemWheelSaveInfo', 'FSLFItemWheelSaveInfo')
            base_type = base_type.replace('FCurrentEquipment', 'FSLFCurrentEquipment')
            base_type = base_type.replace('FSaveGameInfo', 'FSLFSaveGameInfo')
        else:
            base_type = 'FStructOnScope'
    elif category == 'interface':
        base_type = 'TScriptInterface<IInterface>'
    else:
        base_type = 'int32'  # Fallback

    if is_array:
        return f'const TArray<{base_type}>&'
    elif is_ref and category in ['struct', 'text', 'string', 'name']:
        return f'const {base_type}&'
    return base_type

def clean_param_name(name):
    """Clean parameter name to be C++ compatible."""
    # Remove trailing ? and _
    name = name.rstrip('?').rstrip('_')
    # Convert to valid C++ identifier
    name = name.replace(' ', '').replace('?', '').replace('.', '').replace('-', '')
    # Handle shadowing prevention
    shadowed_names = ['Slot', 'SelectedSlot', 'Widget', 'Parent', 'Child', 'Content', 'Value']
    if name in shadowed_names:
        return f'In{name}'
    # Add b prefix for bool-like names that don't have it
    if name.endswith('d') and not name.startswith('b'):
        pass  # Keep as is
    return name

def generate_event_declarations(events, class_name, category_prefix):
    """Generate UFUNCTION declarations for events."""
    declarations = []
    for event_name, params in sorted(events.items()):
        func_name = event_name.replace(' ', '')
        param_list = []
        for p in params:
            cpp_type = type_to_cpp(p)
            param_name = clean_param_name(p['name'])
            param_list.append(f'{cpp_type} {param_name}')

        param_str = ', '.join(param_list)

        decl = f'''\tUFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "{category_prefix}")
\tvoid {func_name}({param_str});
\tvirtual void {func_name}_Implementation({param_str});
'''
        declarations.append(decl)

    return '\n'.join(declarations)

def generate_event_implementations(events, class_name):
    """Generate function implementations for events."""
    implementations = []
    for event_name, params in sorted(events.items()):
        func_name = event_name.replace(' ', '')
        param_list = []
        for p in params:
            cpp_type = type_to_cpp(p)
            param_name = clean_param_name(p['name'])
            param_list.append(f'{cpp_type} {param_name}')

        param_str = ', '.join(param_list)

        impl = f'''
void {class_name}::{func_name}_Implementation({param_str})
{{
\tUE_LOG(LogTemp, Log, TEXT("{class_name}::{func_name}_Implementation"));
}}
'''
        implementations.append(impl)

    return '\n'.join(implementations)

# Already processed widgets
processed = ['W_HUD', 'W_Inventory', 'W_Equipment', 'W_StatBlock', 'W_ItemWheelSlot',
             'W_DebugWindow', 'W_VendorAction', 'W_Crafting', 'W_LevelUp']

json_dir = 'C:/scripts/SLFConversion/Exports/BlueprintDNA_v2/WidgetBlueprint'
cpp_dir = 'C:/scripts/SLFConversion/Source/SLFConversion/Widgets'

total_events = 0
widgets_updated = 0

for json_file in sorted(glob.glob(f'{json_dir}/*.json')):
    widget_name = os.path.basename(json_file).replace('.json', '')

    if widget_name in processed:
        continue

    h_path = f'{cpp_dir}/{widget_name}.h'
    cpp_path = f'{cpp_dir}/{widget_name}.cpp'

    if not os.path.exists(h_path):
        continue

    events = extract_events(json_file)
    if not events:
        continue

    # Determine class name (U prefix for widgets)
    class_name = f'U{widget_name}'

    # Check if events already added
    with open(h_path, 'r', encoding='utf-8') as f:
        h_content = f.read()

    first_event = list(events.keys())[0].replace(' ', '')
    if first_event + '_Implementation' in h_content:
        continue  # Already processed

    print(f'Processing {widget_name} ({len(events)} events)...')

    # Generate declarations
    decls = generate_event_declarations(events, class_name, widget_name)

    # Insert before 'protected:'
    insertion_point = h_content.find('protected:')
    if insertion_point != -1:
        event_section = f'\n\t// Event Handlers ({len(events)} events)\n{decls}\n'
        h_content = h_content[:insertion_point] + event_section + h_content[insertion_point:]

        with open(h_path, 'w', encoding='utf-8') as f:
            f.write(h_content)

    # Generate and add implementations
    impls = generate_event_implementations(events, class_name)

    with open(cpp_path, 'r', encoding='utf-8') as f:
        cpp_content = f.read()

    cpp_content = cpp_content.rstrip() + impls

    with open(cpp_path, 'w', encoding='utf-8') as f:
        f.write(cpp_content)

    total_events += len(events)
    widgets_updated += 1

print(f'\nDone! Updated {widgets_updated} widgets with {total_events} events.')
