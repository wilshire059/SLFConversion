"""
Generate C++ Component headers and stubs from Blueprint JSON exports.
Breadth-first: Generate all headers, then implementations.
"""
import json
import os
import re
from pathlib import Path
from datetime import datetime

DNA_PATH = Path(r"C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/Component")
OUTPUT_PATH = Path(r"C:/scripts/SLFConversion/Source/SLFConversion/Components")
BP_DNA_PATH = Path(r"C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/Blueprint")

# Forward declare these instead of including (to avoid circular dependencies)
FORWARD_DECLARE_TYPES = {
    # Component types
    "UAC_ActionManager", "UAC_BuffManager", "UAC_CombatManager", "UAC_EquipmentManager",
    "UAC_InputBuffer", "UAC_InteractionManager", "UAC_InventoryManager", "UAC_RadarManager",
    "UAC_StatManager", "UAC_StatusEffectManager", "UAC_CollisionManager", "UAC_DebugCentral",
    "UAC_LadderManager", "UAC_LootDropManager", "UAC_ProgressManager", "UAC_SaveLoadManager",
    "UAC_AI_BehaviorManager", "UAC_AI_Boss", "UAC_AI_CombatManager", "UAC_AI_InteractionManager",
    "UAC_RadarElement",
    # Widget types
    "UW_Debug_HUD", "UW_Radar", "UW_Radar_TrackedElement", "UW_HUD", "UW_Inventory",
    "UW_Equipment", "UW_InventorySlot", "UW_EquipmentSlot",
    # Blueprint actor types
    "AB_RestingPoint", "AB_Interactable", "AB_PatrolPath", "AB_Container",
    # Save game types
    "USG_SoulslikeFramework", "USG_SaveSlots",
}

# Global mapping for Blueprint actor-based detection
PARENT_MAP = {}
IS_ACTOR_BASED = {}

# Import mappings from shared module
from slf_type_mapping import (
    ENUM_MAP, STRUCT_MAP, ENGINE_STRUCT_MAP, ENGINE_CLASS_MAP,
    ENGINE_ENUMS_NEEDING_WRAPPER, ACTOR_BASED_BLUEPRINTS, UOBJECT_BASED_BLUEPRINTS,
    TYPE_INCLUDES,
    get_enum_cpp_name, get_struct_cpp_name, get_class_cpp_name, get_class_prefix
)

def build_parent_hierarchy():
    """Build mapping of all Blueprint parents to determine A vs U prefix."""
    global PARENT_MAP, IS_ACTOR_BASED

    for json_file in BP_DNA_PATH.glob("*.json"):
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                data = json.load(f)
            name = data.get("Name")
            parent = data.get("ParentClass", "Object")
            PARENT_MAP[name] = parent
        except:
            pass

    def is_actor_based(name, visited=None):
        if visited is None:
            visited = set()
        if name in visited:
            return False
        visited.add(name)
        parent = PARENT_MAP.get(name, "Object")
        if parent in ["Actor", "Pawn", "Character", "PlayerController", "GameModeBase", "GameStateBase", "PlayerState"]:
            return True
        elif parent in ["Object", "GameInstance", "UserWidget", "ActorComponent"]:
            return False
        elif parent.endswith("_C"):
            return is_actor_based(parent[:-2], visited)
        return False

    for name in PARENT_MAP:
        IS_ACTOR_BASED[name] = is_actor_based(name)

def type_to_cpp(type_info: dict) -> str:
    """Convert Blueprint type info to C++ type."""
    cat = type_info.get("Category", "")
    sub_obj = type_info.get("SubCategoryObject", "")
    sub_path = type_info.get("SubCategoryObjectPath", "")
    is_array = type_info.get("IsArray", False)
    is_map = type_info.get("IsMap", False)
    is_set = type_info.get("IsSet", False)

    cpp_type = "UObject*"  # Fallback for unknown types

    if cat == "bool":
        cpp_type = "bool"
    elif cat == "int":
        cpp_type = "int32"
    elif cat == "int64":
        cpp_type = "int64"
    elif cat == "float":
        cpp_type = "float"
    elif cat in ("real", "double"):
        cpp_type = "double"
    elif cat == "name":
        cpp_type = "FName"
    elif cat == "string":
        cpp_type = "FString"
    elif cat == "text":
        cpp_type = "FText"
    elif cat == "byte":
        if sub_obj and sub_obj != "None":
            # Use shared enum name conversion (includes TEnumAsByte for engine enums)
            cpp_type = get_enum_cpp_name(sub_obj)
        else:
            cpp_type = "uint8"
    elif cat == "struct":
        # Use shared struct name conversion
        cpp_type = get_struct_cpp_name(sub_obj) if sub_obj else "FStructFallback"
    elif cat == "object":
        if sub_obj.endswith("_C"):
            cpp_type = get_class_cpp_name(sub_obj)
        elif sub_obj in ENGINE_CLASS_MAP:
            cpp_type = f"{ENGINE_CLASS_MAP[sub_obj]}*"
        elif sub_obj in ACTOR_BASED_BLUEPRINTS:
            cpp_type = f"A{sub_obj}*"
        elif sub_obj in UOBJECT_BASED_BLUEPRINTS:
            cpp_type = f"U{sub_obj}*"
        elif IS_ACTOR_BASED.get(sub_obj, False):
            cpp_type = f"A{sub_obj}*"
        elif sub_obj:
            cpp_type = f"U{sub_obj}*"
        else:
            cpp_type = "UObject*"
    elif cat == "class":
        if sub_obj.endswith("_C"):
            base = sub_obj[:-2]
            # Use shared class prefix detection
            prefix = get_class_prefix(base)
            cpp_type = f"TSubclassOf<{prefix}{base}>"
        elif sub_obj in ENGINE_CLASS_MAP:
            cpp_type = f"TSubclassOf<{ENGINE_CLASS_MAP[sub_obj]}>"
        elif sub_obj in ACTOR_BASED_BLUEPRINTS or sub_obj in ("Actor", "Pawn", "Character", "PlayerController"):
            cpp_type = f"TSubclassOf<A{sub_obj}>"
        elif sub_obj:
            cpp_type = f"TSubclassOf<U{sub_obj}>"
        else:
            cpp_type = "TSubclassOf<UObject>"
    elif cat == "softobject":
        if sub_obj:
            if sub_obj.endswith("_C"):
                base = sub_obj[:-2]
                prefix = get_class_prefix(base)
                cpp_type = f"TSoftObjectPtr<{prefix}{base}>"
            elif sub_obj in ENGINE_CLASS_MAP:
                cpp_type = f"TSoftObjectPtr<{ENGINE_CLASS_MAP[sub_obj]}>"
            elif sub_obj in ACTOR_BASED_BLUEPRINTS or sub_obj in ("Actor", "Pawn", "Character"):
                cpp_type = f"TSoftObjectPtr<A{sub_obj}>"
            else:
                cpp_type = f"TSoftObjectPtr<U{sub_obj}>"
        else:
            cpp_type = "TSoftObjectPtr<UObject>"
    elif cat == "softclass":
        if sub_obj:
            if sub_obj.endswith("_C"):
                base = sub_obj[:-2]
                prefix = get_class_prefix(base)
                cpp_type = f"TSoftClassPtr<{prefix}{base}>"
            elif sub_obj in ENGINE_CLASS_MAP:
                cpp_type = f"TSoftClassPtr<{ENGINE_CLASS_MAP[sub_obj]}>"
            elif sub_obj in ACTOR_BASED_BLUEPRINTS or sub_obj in ("Actor", "Pawn", "Character"):
                cpp_type = f"TSoftClassPtr<A{sub_obj}>"
            else:
                cpp_type = f"TSoftClassPtr<U{sub_obj}>"
        else:
            cpp_type = "TSoftClassPtr<UObject>"

    # Handle containers
    if is_map:
        # Maps need key and value types - simplified for now
        cpp_type = f"TMap<FGameplayTag, {cpp_type}>"
    elif is_set:
        cpp_type = f"TSet<{cpp_type}>"
    elif is_array:
        cpp_type = f"TArray<{cpp_type}>"

    return cpp_type

def sanitize_name(name: str) -> str:
    """Clean up name for C++."""
    name = re.sub(r'\([^)]*\)', '', name)
    # Replace all invalid characters with underscores or remove
    name = name.replace("?", "").replace(" ", "").replace("-", "_")
    name = name.replace("|", "_").replace("&", "_").replace("*", "").replace("+", "_")
    name = name.replace("/", "_").replace("\\", "_").replace(":", "_")
    name = name.strip()
    # Remove any remaining non-alphanumeric/underscore characters
    name = re.sub(r'[^a-zA-Z0-9_]', '', name)
    # C++ identifiers cannot start with a digit
    if name and name[0].isdigit():
        name = "_" + name
    return name

def extract_types_from_cpp(cpp_type: str) -> set:
    """Extract type names from a C++ type string for include/forward declaration generation."""
    types = set()
    # Remove template wrappers
    cleaned = cpp_type.replace("TArray<", "").replace("TMap<", "").replace("TSet<", "")
    cleaned = cleaned.replace("TSubclassOf<", "").replace("TSoftObjectPtr<", "").replace("TSoftClassPtr<", "")
    cleaned = cleaned.replace("TEnumAsByte<", "").replace(">", "").replace("*", "").replace(",", " ")

    for part in cleaned.split():
        part = part.strip()
        if not part:
            continue
        # Handle nested enum types like EDrawDebugTrace::Type
        if "::" in part:
            part = part.split("::")[0]
        # Check if it's a class/struct/enum we care about
        if part.startswith("U") or part.startswith("A") or part.startswith("F") or part.startswith("E"):
            types.add(part)
    return types

def get_uproperty_specifiers(var: dict) -> str:
    """Generate UPROPERTY specifiers from variable info."""
    specs = []

    if var.get("IsInstanceEditable", False):
        specs.append("EditAnywhere")
    else:
        specs.append("EditDefaultsOnly")

    if var.get("IsBlueprintReadOnly", False):
        specs.append("BlueprintReadOnly")
    else:
        specs.append("BlueprintReadWrite")

    category = var.get("Category", "Default")
    specs.append(f'Category = "{category}"')

    if var.get("IsReplicated", False):
        specs.append("Replicated")

    return ", ".join(specs)

def generate_component(json_path: Path) -> tuple:
    """Generate C++ component header and cpp from JSON."""
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    name = data.get("Name", json_path.stem)
    parent = data.get("ParentClass", "ActorComponent")

    # Map parent to C++ class
    parent_map = {
        "ActorComponent": "UActorComponent",
        "SceneComponent": "USceneComponent",
        "PrimitiveComponent": "UPrimitiveComponent",
    }
    cpp_parent = parent_map.get(parent, f"U{parent}")

    # Generate C++ class name
    cpp_name = f"U{name}"

    vars_list = data.get("Variables", {}).get("List", [])
    funcs = data.get("FunctionSignatures", {}).get("Functions", [])
    dispatchers = data.get("EventDispatchers", {}).get("List", [])

    # Filter out dispatchers from variables (they appear in both)
    dispatcher_names = {d.get("Name") for d in dispatchers}
    vars_list = [v for v in vars_list if v.get("Name") not in dispatcher_names]

    # Collect all types used for include/forward declaration generation
    all_types_used = set()

    # Build variable declarations and collect names for shadowing check
    var_lines = []
    member_var_names = set()
    replicated_vars = []  # Track replicated properties for GetLifetimeReplicatedProps
    for var in vars_list:
        var_name = sanitize_name(var.get("Name", "Unknown"))
        member_var_names.add(var_name)
        var_type = type_to_cpp(var.get("Type", {}))
        all_types_used.update(extract_types_from_cpp(var_type))
        specs = get_uproperty_specifiers(var)
        var_lines.append(f"\tUPROPERTY({specs})\n\t{var_type} {var_name};")
        # Track replicated properties
        if var.get("IsReplicated", False):
            replicated_vars.append(var_name)

    # Build event dispatcher declarations - prefix with component name to avoid conflicts
    dispatcher_lines = []
    dispatcher_decls = []
    for disp in dispatchers:
        disp_name = sanitize_name(disp.get("Name", "Unknown"))
        # Prefix delegate type with component name to avoid conflicts across components
        delegate_type = f"F{name}_{disp_name}"
        # Get parameters if any
        params = disp.get("Parameters", [])
        if not params:
            dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE({delegate_type});")
        elif len(params) == 1:
            p = params[0]
            p_type = type_to_cpp(p.get("Type", {}))
            p_name = sanitize_name(p.get("Name", "Param"))
            dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam({delegate_type}, {p_type}, {p_name});")
        else:
            # Multiple params
            param_strs = []
            for p in params:
                p_type = type_to_cpp(p.get("Type", {}))
                p_name = sanitize_name(p.get("Name", "Param"))
                param_strs.append(f"{p_type}, {p_name}")
            num_names = {2: 'TwoParams', 3: 'ThreeParams', 4: 'FourParams', 5: 'FiveParams', 6: 'SixParams', 7: 'SevenParams', 8: 'EightParams', 9: 'NineParams'}
            suffix = num_names.get(len(params), f"{len(params)}Params")
            macro = f"DECLARE_DYNAMIC_MULTICAST_DELEGATE_{suffix}"
            dispatcher_decls.append(f"{macro}({delegate_type}, {', '.join(param_strs)});")

        dispatcher_lines.append(f"\tUPROPERTY(BlueprintAssignable, Category = \"Events\")\n\t{delegate_type} {disp_name};")

    # Build function declarations
    func_lines = []
    func_impl_lines = []
    for func in funcs:
        fname = sanitize_name(func.get("Name", "Unknown"))
        inputs = func.get("Inputs", [])
        outputs = func.get("Outputs", [])

        # Determine return type
        if len(outputs) == 0:
            ret_type = "void"
        elif len(outputs) == 1:
            ret_type = type_to_cpp(outputs[0]["Type"])
            all_types_used.update(extract_types_from_cpp(ret_type))
        else:
            ret_type = "void"

        # Build parameters, avoiding shadowing with member variables
        params = []
        seen_param_names = set()
        for inp in inputs:
            param_type = type_to_cpp(inp["Type"])
            all_types_used.update(extract_types_from_cpp(param_type))
            param_name = sanitize_name(inp["Name"])
            # Avoid shadowing member variables
            if param_name in member_var_names:
                param_name = f"In{param_name}"
            # Avoid duplicate param names
            if param_name in seen_param_names:
                param_name = f"{param_name}_{len([p for p in seen_param_names if p.startswith(param_name)])}"
            seen_param_names.add(param_name)
            # Pass container types and large structs by const reference (matches UHT expectations)
            if param_type.startswith("TArray<") or param_type.startswith("TMap<") or param_type.startswith("TSet<"):
                params.append(f"const {param_type}& {param_name}")
            elif param_type.startswith("F") and "*" not in param_type:
                # Structs passed by const reference for efficiency
                params.append(f"const {param_type}& {param_name}")
            else:
                params.append(f"{param_type} {param_name}")

        # Add out params for multiple outputs
        if len(outputs) > 1:
            for out in outputs:
                param_type = type_to_cpp(out["Type"])
                all_types_used.update(extract_types_from_cpp(param_type))
                out_name = sanitize_name(out['Name'])
                param_name = f"Out{out_name}"
                # Avoid duplicate param names
                if param_name in seen_param_names:
                    param_name = f"{param_name}_{len([p for p in seen_param_names if p.startswith(param_name)])}"
                seen_param_names.add(param_name)
                params.append(f"{param_type}& {param_name}")

        param_str = ", ".join(params) if params else ""

        func_lines.append(f"""
\tUFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "{name}")
\t{ret_type} {fname}({param_str});
\tvirtual {ret_type} {fname}_Implementation({param_str});""")

        # Generate implementation stub
        if ret_type == "void":
            func_impl_lines.append(f"""
{ret_type} {cpp_name}::{fname}_Implementation({param_str})
{{
\t// TODO: Implement from Blueprint graph
\tUE_LOG(LogTemp, Warning, TEXT("{cpp_name}::{fname} - Not yet implemented"));
}}""")
        else:
            # Determine default return value based on type
            # IMPORTANT: Check containers FIRST because TArray<UObject*> contains "*"
            if ret_type.startswith("TArray<") or ret_type.startswith("TMap<") or ret_type.startswith("TSet<"):
                default_return = "{}"  # Empty container
            elif "*" in ret_type:
                default_return = "nullptr"
            elif ret_type == "bool":
                default_return = "false"
            elif ret_type == "FString":
                default_return = "FString()"
            elif ret_type == "FText":
                default_return = "FText::GetEmpty()"
            elif ret_type == "FName":
                default_return = "NAME_None"
            elif ret_type.startswith("TEnumAsByte<"):
                # Extract inner enum type and cast
                inner = ret_type.replace("TEnumAsByte<", "").rstrip(">")
                default_return = f"TEnumAsByte<{inner}>(static_cast<{inner}>(0))"
            elif ret_type.startswith("E"):
                # Enum type - need explicit cast
                default_return = f"static_cast<{ret_type}>(0)"
            elif ret_type.startswith("F"):
                default_return = f"{ret_type}()"  # Struct default constructor
            elif ret_type in ("int32", "int64", "float", "double", "uint8", "int8", "uint16", "int16", "uint32", "uint64"):
                default_return = "0"
            else:
                default_return = "0"
            func_impl_lines.append(f"""
{ret_type} {cpp_name}::{fname}_Implementation({param_str})
{{
\t// TODO: Implement from Blueprint graph
\tUE_LOG(LogTemp, Warning, TEXT("{cpp_name}::{fname} - Not yet implemented"));
\treturn {default_return};
}}""")

    timestamp = datetime.now().strftime("%Y-%m-%d")

    # Build dynamic includes based on types used
    extra_includes = set()
    forward_decls = {"UAnimMontage", "UDataTable", "UPrimaryDataAsset"}

    for type_name in all_types_used:
        if type_name in TYPE_INCLUDES:
            extra_includes.add(TYPE_INCLUDES[type_name])
        elif type_name in FORWARD_DECLARE_TYPES:
            forward_decls.add(type_name)

    # Sort for consistent output
    include_lines = '\n'.join(f'#include "{inc}"' for inc in sorted(extra_includes))
    forward_decl_lines = '\n'.join(f'class {fwd};' for fwd in sorted(forward_decls))

    # Generate header
    header = f"""// {name}.h
// C++ component for {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session
// Source: BlueprintDNA/Component/{json_path.name}
// Variables: {len(vars_list)} | Functions: {len(funcs)} | Dispatchers: {len(dispatchers)}

#pragma once

#include "CoreMinimal.h"
#include "Components/{cpp_parent.replace('U', '')}.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
{include_lines}
#include "{name}.generated.h"

// Forward declarations
{forward_decl_lines}

// Event Dispatcher Declarations
{chr(10).join(dispatcher_decls)}

UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API {cpp_name} : public {cpp_parent}
{{
\tGENERATED_BODY()

public:
\t{cpp_name}();

protected:
\tvirtual void BeginPlay() override;

public:
\tvirtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
{"" if not replicated_vars else chr(10) + "\tvirtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;"}

\t// ═══════════════════════════════════════════════════════════════════════
\t// VARIABLES ({len(vars_list)})
\t// ═══════════════════════════════════════════════════════════════════════

{chr(10).join(var_lines)}

\t// ═══════════════════════════════════════════════════════════════════════
\t// EVENT DISPATCHERS ({len(dispatchers)})
\t// ═══════════════════════════════════════════════════════════════════════

{chr(10).join(dispatcher_lines)}

\t// ═══════════════════════════════════════════════════════════════════════
\t// FUNCTIONS ({len(funcs)})
\t// ═══════════════════════════════════════════════════════════════════════
{"".join(func_lines)}
}};
"""

    # Generate GetLifetimeReplicatedProps implementation if needed
    replicated_props_impl = ""
    network_include = ""
    if replicated_vars:
        network_include = '#include "Net/UnrealNetwork.h"\n'
        doreplifetime_lines = '\n'.join(f'\tDOREPLIFETIME({cpp_name}, {var});' for var in replicated_vars)
        replicated_props_impl = f"""
void {cpp_name}::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{{
\tSuper::GetLifetimeReplicatedProps(OutLifetimeProps);
{doreplifetime_lines}
}}
"""

    # Generate cpp stub
    cpp_content = f"""// {name}.cpp
// C++ component implementation for {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session
// Source: BlueprintDNA/Component/{json_path.name}

#include "{name}.h"
{network_include}
{cpp_name}::{cpp_name}()
{{
\tPrimaryComponentTick.bCanEverTick = true;
}}

void {cpp_name}::BeginPlay()
{{
\tSuper::BeginPlay();
\tUE_LOG(LogTemp, Log, TEXT("{cpp_name}::BeginPlay"));
}}

void {cpp_name}::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{{
\tSuper::TickComponent(DeltaTime, TickType, ThisTickFunction);
}}
{replicated_props_impl}

// ═══════════════════════════════════════════════════════════════════════
// FUNCTION IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════
{"".join(func_impl_lines)}
"""

    return header, cpp_content, name

def main():
    """Generate all component headers and cpp files."""
    if not DNA_PATH.exists():
        print(f"ERROR: DNA path not found: {DNA_PATH}")
        return

    # Build parent hierarchy for correct A/U prefix determination
    print("Building Blueprint parent hierarchy...")
    build_parent_hierarchy()
    print(f"  Mapped {len(PARENT_MAP)} Blueprints, {sum(IS_ACTOR_BASED.values())} are Actor-based")

    OUTPUT_PATH.mkdir(parents=True, exist_ok=True)

    generated = []
    total_vars = 0
    total_funcs = 0
    total_dispatchers = 0

    for json_file in sorted(DNA_PATH.glob("*.json")):
        print(f"Processing: {json_file.name}")
        try:
            header_content, cpp_content, name = generate_component(json_file)

            header_file = OUTPUT_PATH / f"{name}.h"
            cpp_file = OUTPUT_PATH / f"{name}.cpp"

            with open(header_file, 'w', encoding='utf-8') as f:
                f.write(header_content)

            with open(cpp_file, 'w', encoding='utf-8') as f:
                f.write(cpp_content)

            generated.append(name)
            print(f"  -> Generated: {name}.h and {name}.cpp")

            # Count items
            with open(json_file, 'r') as f:
                data = json.load(f)
            total_vars += len(data.get("Variables", {}).get("List", []))
            total_funcs += len(data.get("FunctionSignatures", {}).get("Functions", []))
            total_dispatchers += len(data.get("EventDispatchers", {}).get("List", []))

        except Exception as e:
            print(f"  ERROR: {e}")
            import traceback
            traceback.print_exc()

    # Generate master include
    master = """// SLFComponents.h
// Master include for all SLF components
// Auto-generated

#pragma once

"""
    for name in sorted(generated):
        master += f'#include "{name}.h"\n'

    with open(OUTPUT_PATH / "SLFComponents.h", 'w', encoding='utf-8') as f:
        f.write(master)

    print(f"\n{'='*60}")
    print(f"Generated {len(generated)} component files")
    print(f"Total Variables: {total_vars}")
    print(f"Total Functions: {total_funcs}")
    print(f"Total Dispatchers: {total_dispatchers}")
    print(f"Master include: SLFComponents.h")

if __name__ == "__main__":
    main()
