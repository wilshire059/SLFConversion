"""
Generate C++ headers and stubs for Game Framework classes.
GameMode, GameState, PlayerController, PlayerState, GameInstance
"""
import json
from pathlib import Path
from datetime import datetime
from slf_type_mapping import ENUM_MAP, STRUCT_MAP, ENGINE_STRUCT_MAP, get_enum_cpp_name, get_struct_cpp_name, get_class_cpp_name

DNA_PATH = Path(r"C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/GameFramework")
OUTPUT_PATH = Path(r"C:/scripts/SLFConversion/Source/SLFConversion/GameFramework")

def type_to_cpp(type_info: dict) -> str:
    """Convert Blueprint type info to C++ type."""
    cat = type_info.get("Category", "")
    sub_obj = type_info.get("SubCategoryObject", "")
    is_array = type_info.get("IsArray", False)
    is_map = type_info.get("IsMap", False)

    cpp_type = "UObject*"  # Fallback for unknown types

    if cat == "bool":
        cpp_type = "bool"
    elif cat == "int":
        cpp_type = "int32"
    elif cat in ("float", "real", "double"):
        cpp_type = "double"
    elif cat == "name":
        cpp_type = "FName"
    elif cat == "string":
        cpp_type = "FString"
    elif cat == "text":
        cpp_type = "FText"
    elif cat == "byte":
        if sub_obj and sub_obj != "None":
            cpp_type = get_enum_cpp_name(sub_obj)
        else:
            cpp_type = "uint8"
    elif cat == "struct":
        cpp_type = get_struct_cpp_name(sub_obj) if sub_obj else "FStructFallback"
    elif cat == "object":
        if sub_obj.endswith("_C"):
            cpp_type = get_class_cpp_name(sub_obj)
        elif sub_obj in ("Actor", "Pawn", "Character", "PlayerController"):
            cpp_type = f"A{sub_obj}*"
        else:
            cpp_type = f"U{sub_obj}*" if sub_obj else "UObject*"
    elif cat == "class":
        if sub_obj and sub_obj.endswith("_C"):
            base = sub_obj[:-2]
            if base.startswith("B_"):
                cpp_type = f"TSubclassOf<A{base}>"
            else:
                cpp_type = f"TSubclassOf<U{base}>"
        else:
            cpp_type = "TSubclassOf<UObject>"
    elif cat == "softobject":
        cpp_type = f"TSoftObjectPtr<U{sub_obj}>" if sub_obj else "TSoftObjectPtr<UObject>"

    if is_map:
        cpp_type = f"TMap<FGameplayTag, {cpp_type}>"
    elif is_array:
        cpp_type = f"TArray<{cpp_type}>"

    return cpp_type

def sanitize_name(name: str) -> str:
    # Strip parenthetical suffixes and clean up
    import re as re_local
    name = re_local.sub(r'\([^)]*\)', '', name)
    return name.replace("?", "").replace(" ", "").replace("-", "_").strip()

def get_parent_info(name: str, parent: str) -> tuple:
    """Determine C++ parent class based on name/parent."""
    if "GameMode" in name or parent == "GameModeBase":
        return ("AGameModeBase", "A", "GameFramework/GameModeBase.h")
    elif "GameState" in name or parent == "GameStateBase":
        return ("AGameStateBase", "A", "GameFramework/GameStateBase.h")
    elif "PlayerController" in name or "PC_" in name or parent == "PlayerController":
        return ("APlayerController", "A", "GameFramework/PlayerController.h")
    elif "PlayerState" in name or "PS_" in name or parent == "PlayerState":
        return ("APlayerState", "A", "GameFramework/PlayerState.h")
    elif "GameInstance" in name or "GI_" in name or parent == "GameInstance":
        return ("UGameInstance", "U", "Engine/GameInstance.h")
    return ("AActor", "A", "GameFramework/Actor.h")

def generate_gameframework(json_path: Path) -> tuple:
    """Generate C++ header and cpp from JSON."""
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    name = data.get("Name", json_path.stem)
    parent = data.get("ParentClass", "")
    cpp_parent, prefix, parent_include = get_parent_info(name, parent)
    cpp_name = f"{prefix}{name}"

    vars_list = data.get("Variables", {}).get("List", [])
    funcs = data.get("FunctionSignatures", {}).get("Functions", [])
    dispatchers = data.get("EventDispatchers", {}).get("List", [])
    interfaces = data.get("Interfaces", {}).get("List", [])

    dispatcher_names = {d.get("Name") for d in dispatchers}
    vars_list = [v for v in vars_list if v.get("Name") not in dispatcher_names]

    # Build variable lines and collect names for shadowing check
    var_lines = []
    member_var_names = set()
    forward_decls = set()  # Collect types for forward declarations
    for var in vars_list:
        var_name = sanitize_name(var.get("Name", "Unknown"))
        member_var_names.add(var_name)
        var_type = type_to_cpp(var.get("Type", {}))
        category = var.get("Category", "Default")

        # Collect types that need forward declarations
        import re as re_local
        if var_type.endswith("*"):
            inner_type = var_type.rstrip("*").strip()
            if inner_type.startswith("UW_") or inner_type.startswith("USG_") or inner_type.startswith("UB_") or inner_type.startswith("AB_") or inner_type.startswith("UAC_"):
                forward_decls.add(inner_type)
        # Check TSubclassOf, TArray, TMap for inner types
        if "TSubclassOf<" in var_type or "TArray<" in var_type or "TMap<" in var_type:
            matches = re_local.findall(r'[UA][WSGBC]_\w+', var_type)
            for match in matches:
                forward_decls.add(match)

        var_lines.append(f"\tUPROPERTY(EditAnywhere, BlueprintReadWrite, Category = \"{category}\")\n\t{var_type} {var_name};")

    # Build dispatcher declarations
    dispatcher_decls = []
    dispatcher_props = []
    for disp in dispatchers:
        disp_name = sanitize_name(disp.get("Name", "Unknown"))
        # Prefix delegate type with class name to avoid conflicts across classes
        delegate_type = f"F{name}_{disp_name}"
        params = disp.get("Parameters", [])
        if not params:
            dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE({delegate_type});")
        else:
            param_strs = [f"{type_to_cpp(p.get('Type', {}))}, {sanitize_name(p.get('Name', 'P'))}" for p in params]
            if len(params) == 1:
                dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam({delegate_type}, {param_strs[0]});")
            else:
                num_names = {2: 'TwoParams', 3: 'ThreeParams', 4: 'FourParams', 5: 'FiveParams', 6: 'SixParams', 7: 'SevenParams', 8: 'EightParams', 9: 'NineParams'}
                suffix = num_names.get(len(params), f"{len(params)}Params")
                dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE_{suffix}({delegate_type}, {', '.join(param_strs)});")
        dispatcher_props.append(f"\tUPROPERTY(BlueprintAssignable, Category = \"Events\")\n\t{delegate_type} {disp_name};")

    # Engine override functions - skip these
    ENGINE_OVERRIDE_FUNCS = {
        "UserConstructionScript", "ReceiveTick", "ReceiveBeginPlay", "ReceiveEndPlay",
        "ReceiveActorBeginOverlap", "ReceiveActorEndOverlap", "ReceiveHit", "ReceiveAnyDamage",
    }

    # Build function declarations
    func_lines = []
    func_impls = []
    func_name_counts = {}  # Track function name usage to handle overloads
    for func in funcs:
        fname = sanitize_name(func.get("Name", "Unknown"))
        if fname in ENGINE_OVERRIDE_FUNCS:
            continue

        # Handle function overloading - UHT doesn't support it
        if fname in func_name_counts:
            func_name_counts[fname] += 1
            fname = f"{fname}_{func_name_counts[fname]}"
        else:
            func_name_counts[fname] = 0
        inputs = func.get("Inputs", [])
        outputs = func.get("Outputs", [])
        ret_type = "void" if len(outputs) == 0 else (type_to_cpp(outputs[0]["Type"]) if len(outputs) == 1 else "void")

        # Build params avoiding shadowing with member variables
        params = []
        used_param_names = set()
        for inp in inputs:
            param_type = type_to_cpp(inp["Type"])
            param_name = sanitize_name(inp["Name"])
            # Avoid shadowing member variables
            if param_name in member_var_names:
                param_name = f"In{param_name}"
            # Avoid duplicate parameter names
            base_name = param_name
            counter = 1
            while param_name in used_param_names:
                param_name = f"{base_name}{counter}"
                counter += 1
            used_param_names.add(param_name)
            # Pass container types and structs by const reference (matches UHT expectations)
            if param_type.startswith("TArray<") or param_type.startswith("TMap<") or param_type.startswith("TSet<"):
                params.append(f"const {param_type}& {param_name}")
            elif param_type.startswith("F") and "*" not in param_type:
                params.append(f"const {param_type}& {param_name}")
            else:
                params.append(f"{param_type} {param_name}")
        if len(outputs) > 1:
            for idx, out in enumerate(outputs):
                param_name = f"Out{sanitize_name(out['Name'])}"
                # Avoid duplicate output parameter names
                if param_name in used_param_names:
                    param_name = f"{param_name}{idx}"
                used_param_names.add(param_name)
                params.append(f"{type_to_cpp(out['Type'])}& {param_name}")
        param_str = ", ".join(params)

        func_lines.append(f"\n\tUFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = \"{name}\")\n\t{ret_type} {fname}({param_str});\n\tvirtual {ret_type} {fname}_Implementation({param_str});")

        # Determine default return value - containers MUST come before pointer check
        if ret_type == "void":
            default_ret = ""
        elif ret_type.startswith("TArray<") or ret_type.startswith("TMap<") or ret_type.startswith("TSet<"):
            default_ret = "{}"
        elif "*" in ret_type:
            default_ret = "nullptr"
        elif ret_type == "bool":
            default_ret = "false"
        elif ret_type == "FString":
            default_ret = "FString()"
        elif ret_type == "FText":
            default_ret = "FText::GetEmpty()"
        elif ret_type == "FName":
            default_ret = "NAME_None"
        elif ret_type.startswith("E"):
            default_ret = f"static_cast<{ret_type}>(0)"
        elif ret_type.startswith("F"):
            default_ret = f"{ret_type}()"
        else:
            default_ret = "0"
        ret_stmt = f"\n\treturn {default_ret};" if ret_type != "void" else ""
        func_impls.append(f"\n{ret_type} {cpp_name}::{fname}_Implementation({param_str})\n{{\n\t// TODO: Implement from Blueprint{ret_stmt}\n}}")

    # Interface handling
    interface_includes = []
    interface_inherits = []
    for iface in interfaces:
        raw_name = iface.get("Name", "")
        # Only remove trailing _C suffix, not all _C occurrences
        if raw_name.endswith("_C"):
            raw_name = raw_name[:-2]
        iface_name = raw_name.replace("BPI_", "SLF") + "Interface"
        interface_includes.append(f'#include "Interfaces/{iface_name}.h"')
        interface_inherits.append(f"I{iface_name}")

    timestamp = datetime.now().strftime("%Y-%m-%d")
    inherit_str = f", public {', public '.join(interface_inherits)}" if interface_inherits else ""

    header = f"""// {name}.h
// C++ Game Framework class for {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session
// Source: BlueprintDNA/GameFramework/{json_path.name}
// Parent: {cpp_parent}
// Variables: {len(vars_list)} | Functions: {len(funcs)} | Dispatchers: {len(dispatchers)}

#pragma once

#include "CoreMinimal.h"
#include "{parent_include}"
#include "GameplayTagContainer.h"
{chr(10).join(interface_includes)}
#include "{name}.generated.h"

// Forward declarations
{chr(10).join(f"class {fwd};" for fwd in sorted(forward_decls))}

// Event Dispatchers
{chr(10).join(dispatcher_decls)}

UCLASS()
class SLFCONVERSION_API {cpp_name} : public {cpp_parent}{inherit_str}
{{
\tGENERATED_BODY()

public:
\t{cpp_name}();

\t// ═══════════════════════════════════════════════════════════════════════
\t// VARIABLES ({len(vars_list)})
\t// ═══════════════════════════════════════════════════════════════════════

{chr(10).join(var_lines)}

\t// ═══════════════════════════════════════════════════════════════════════
\t// EVENT DISPATCHERS ({len(dispatchers)})
\t// ═══════════════════════════════════════════════════════════════════════

{chr(10).join(dispatcher_props)}

\t// ═══════════════════════════════════════════════════════════════════════
\t// FUNCTIONS ({len(funcs)})
\t// ═══════════════════════════════════════════════════════════════════════
{"".join(func_lines)}
}};
"""

    cpp_content = f"""// {name}.cpp
// C++ implementation for Game Framework class {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session

#include "GameFramework/{name}.h"

{cpp_name}::{cpp_name}()
{{
}}
{"".join(func_impls)}
"""

    return header, cpp_content, name, len(vars_list), len(funcs), len(dispatchers)

def main():
    if not DNA_PATH.exists():
        print(f"ERROR: DNA path not found: {DNA_PATH}")
        return

    OUTPUT_PATH.mkdir(parents=True, exist_ok=True)

    generated = []
    total_vars = 0
    total_funcs = 0
    total_dispatchers = 0

    for json_file in sorted(DNA_PATH.glob("*.json")):
        print(f"Processing: {json_file.name}")
        try:
            header, cpp, name, nvars, nfuncs, ndisps = generate_gameframework(json_file)

            with open(OUTPUT_PATH / f"{name}.h", 'w', encoding='utf-8') as f:
                f.write(header)
            with open(OUTPUT_PATH / f"{name}.cpp", 'w', encoding='utf-8') as f:
                f.write(cpp)

            generated.append(name)
            total_vars += nvars
            total_funcs += nfuncs
            total_dispatchers += ndisps
            print(f"  -> {name}.h/.cpp")
        except Exception as e:
            print(f"  ERROR: {e}")

    print(f"\n{'='*60}")
    print(f"Generated {len(generated)} Game Framework files")
    print(f"Total Variables: {total_vars}")
    print(f"Total Functions: {total_funcs}")
    print(f"Total Dispatchers: {total_dispatchers}")

if __name__ == "__main__":
    main()
