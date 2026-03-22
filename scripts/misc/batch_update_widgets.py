# Batch script to add events to multiple widgets
import json
import os

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
                                    'is_array': pin_type.get('IsArray', False)
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
    is_array = param.get('is_array', False)

    base_type = {
        'bool': 'bool',
        'int': 'int32',
        'float': 'float',
        'real': 'double',
        'byte': 'uint8',
        'text': 'FText',
        'string': 'FString',
        'name': 'FName',
        'object': f'U{subtype.replace("_C", "")}*' if subtype else 'UObject*',
        'struct': f'F{subtype.replace("F", "", 1)}' if subtype.startswith('F') else f'FSL{subtype}',
    }.get(category, 'void')

    if is_array:
        return f'TArray<{base_type}>'
    return base_type

def clean_param_name(name):
    """Clean parameter name to be C++ compatible."""
    # Remove trailing ? and _
    name = name.rstrip('?').rstrip('_')
    # Convert to valid C++ identifier
    name = name.replace(' ', '').replace('?', '').replace('.', '')
    # Prefix with b for booleans, In for shadowing prevention
    if name == 'Slot':
        return 'InSlot'
    return name

def generate_event_declarations(events, class_name, category_prefix):
    """Generate UFUNCTION declarations for events."""
    declarations = []
    for event_name, params in sorted(events.items()):
        # Convert event name to function name
        func_name = event_name.replace(' ', '')

        # Build parameter list
        param_list = []
        for p in params:
            cpp_type = type_to_cpp(p)
            param_name = clean_param_name(p['name'])
            param_list.append(f'{cpp_type} {param_name}')

        param_str = ', '.join(param_list)

        decl = f'''	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "{category_prefix}")
	void {func_name}({param_str});
	virtual void {func_name}_Implementation({param_str});
'''
        declarations.append(decl)

    return '\n'.join(declarations)

def generate_event_implementations(events, class_name):
    """Generate function implementations for events."""
    implementations = []
    for event_name, params in sorted(events.items()):
        func_name = event_name.replace(' ', '')

        # Build parameter list
        param_list = []
        for p in params:
            cpp_type = type_to_cpp(p)
            param_name = clean_param_name(p['name'])
            param_list.append(f'{cpp_type} {param_name}')

        param_str = ', '.join(param_list)

        impl = f'''
void {class_name}::{func_name}_Implementation({param_str})
{{
	UE_LOG(LogTemp, Log, TEXT("{class_name}::{func_name}_Implementation"));
}}
'''
        implementations.append(impl)

    return '\n'.join(implementations)

# Process widgets
widgets = [
    ('W_StatBlock', 'UW_StatBlock'),
    ('W_ItemWheelSlot', 'UW_ItemWheelSlot'),
    ('W_DebugWindow', 'UW_DebugWindow'),
    ('W_VendorAction', 'UW_VendorAction'),
    ('W_Crafting', 'UW_Crafting'),
    ('W_LevelUp', 'UW_LevelUp'),
]

for widget_name, class_name in widgets:
    json_path = f'C:/scripts/SLFConversion/Exports/BlueprintDNA_v2/WidgetBlueprint/{widget_name}.json'
    h_path = f'C:/scripts/SLFConversion/Source/SLFConversion/Widgets/{widget_name}.h'
    cpp_path = f'C:/scripts/SLFConversion/Source/SLFConversion/Widgets/{widget_name}.cpp'

    if not os.path.exists(h_path):
        print(f'SKIP: {widget_name} - header not found')
        continue

    events = extract_events(json_path)
    if not events:
        print(f'SKIP: {widget_name} - no events')
        continue

    print(f'Processing {widget_name} with {len(events)} events...')

    # Generate declarations
    decls = generate_event_declarations(events, class_name, widget_name)

    # Update header
    with open(h_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Check if events already added (look for first event name)
    first_event = list(events.keys())[0].replace(' ', '')
    if first_event in content:
        print(f'  Events already in header')
    else:
        # Insert before 'protected:'
        insertion_point = content.find('protected:')
        if insertion_point != -1:
            # Add a comment before events
            event_section = f'\n\t// Event Handlers ({len(events)} events)\n{decls}\n'
            content = content[:insertion_point] + event_section + content[insertion_point:]

            with open(h_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f'  Added {len(events)} events to header')

    # Generate and add implementations
    impls = generate_event_implementations(events, class_name)

    with open(cpp_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Check if implementations already added
    if first_event in content:
        print(f'  Implementations already in cpp')
    else:
        content = content.rstrip() + impls

        with open(cpp_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f'  Added {len(events)} implementations to cpp')

print('\nDone!')
