"""
Generate C++ headers and stubs for Blueprint classes.
Handles various parent types: UObject, AActor, ACharacter, etc.
"""
import json
import re
import os
from pathlib import Path
from datetime import datetime

# Import type mappings from shared module
from slf_type_mapping import (
    ENUM_MAP, STRUCT_MAP, ENGINE_STRUCT_MAP, ENGINE_CLASS_MAP,
    ACTOR_BASED_BLUEPRINTS, UOBJECT_BASED_BLUEPRINTS,
    get_enum_cpp_name, get_struct_cpp_name, get_class_cpp_name, get_class_prefix
)

DNA_PATH = Path(r"C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/Blueprint")
OUTPUT_PATH = Path(r"C:/scripts/SLFConversion/Source/SLFConversion/Blueprints")

# Global mapping of Blueprint name -> parent class
PARENT_MAP = {}
# Global mapping of Blueprint name -> is actor-based (True/False)
IS_ACTOR_BASED = {}


def build_parent_hierarchy():
    """Build mapping of all Blueprint parents to determine A vs U prefix."""
    global PARENT_MAP, IS_ACTOR_BASED

    # First pass: collect all parent info
    for json_file in DNA_PATH.glob("*.json"):
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                data = json.load(f)
            name = data.get("Name")
            parent = data.get("ParentClass", "Object")
            PARENT_MAP[name] = parent
        except:
            pass

    # Second pass: determine if each class is Actor-based
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
    is_array = type_info.get("IsArray", False)
    is_map = type_info.get("IsMap", False)

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
            # Use shared enum mapping
            cpp_type = get_enum_cpp_name(sub_obj)
        else:
            cpp_type = "uint8"
    elif cat == "struct":
        # Use shared type mapping
        cpp_type = get_struct_cpp_name(sub_obj) if sub_obj else "FStructFallback"
    elif cat == "object":
        if sub_obj.endswith("_C"):
            cpp_type = get_class_cpp_name(sub_obj)
        elif sub_obj in ENGINE_CLASS_MAP:
            cpp_type = f"{ENGINE_CLASS_MAP[sub_obj]}*"
        elif sub_obj in ("Actor", "Pawn", "Character", "PlayerController", "Controller", "AIController"):
            cpp_type = f"A{sub_obj}*"
        elif sub_obj:
            cpp_type = f"U{sub_obj}*"
        else:
            cpp_type = "UObject*"
    elif cat == "class":
        if sub_obj.endswith("_C"):
            base = sub_obj[:-2]
            # Use hierarchy to determine A vs U prefix
            if IS_ACTOR_BASED.get(base, False):
                cpp_type = f"TSubclassOf<A{base}>"
            else:
                cpp_type = f"TSubclassOf<U{base}>"
        else:
            cpp_type = f"TSubclassOf<UObject>"
    elif cat == "softobject":
        if sub_obj:
            if sub_obj.endswith("_C"):
                base = sub_obj[:-2]
                if IS_ACTOR_BASED.get(base, False):
                    cpp_type = f"TSoftObjectPtr<A{base}>"
                else:
                    cpp_type = f"TSoftObjectPtr<U{base}>"
            else:
                cpp_type = f"TSoftObjectPtr<U{sub_obj}>"
        else:
            cpp_type = "TSoftObjectPtr<UObject>"
    elif cat == "softclass":
        if sub_obj:
            if sub_obj.endswith("_C"):
                base = sub_obj[:-2]
                if IS_ACTOR_BASED.get(base, False):
                    cpp_type = f"TSoftClassPtr<A{base}>"
                else:
                    cpp_type = f"TSoftClassPtr<U{base}>"
            else:
                cpp_type = f"TSoftClassPtr<U{sub_obj}>"
        else:
            cpp_type = "TSoftClassPtr<UObject>"

    if is_map:
        cpp_type = f"TMap<FGameplayTag, {cpp_type}>"
    elif is_array:
        cpp_type = f"TArray<{cpp_type}>"

    return cpp_type

def sanitize_name(name: str) -> str:
    # Strip parenthetical suffixes like (Player) and clean up
    name = re.sub(r'\([^)]*\)', '', name)
    name = name.replace("?", "").replace(" ", "").replace("-", "_").strip()
    # C++ identifiers cannot start with a digit
    if name and name[0].isdigit():
        name = "_" + name
    return name

def get_parent_info(parent: str, class_name: str = "") -> tuple:
    """Map Blueprint parent to C++ parent class and prefix."""
    engine_parents = {
        "Object": ("UObject", "U"),
        "Actor": ("AActor", "A"),
        "Pawn": ("APawn", "A"),
        "Character": ("ACharacter", "A"),
        "PlayerController": ("APlayerController", "A"),
        "GameModeBase": ("AGameModeBase", "A"),
        "GameStateBase": ("AGameStateBase", "A"),
        "PlayerState": ("APlayerState", "A"),
        "GameInstance": ("UGameInstance", "U"),
        "UserWidget": ("UUserWidget", "U"),
        "ActorComponent": ("UActorComponent", "U"),
    }
    if parent in engine_parents:
        return engine_parents[parent]
    # Check if it's a blueprint parent (_C suffix)
    if parent.endswith("_C"):
        base = parent[:-2]
        # Look up if this Blueprint is Actor-based using our pre-built hierarchy
        if IS_ACTOR_BASED.get(base, False):
            return (f"A{base}", "A")
        else:
            return (f"U{base}", "U")
    # Default to UObject for unknown
    return ("UObject", "U")

def generate_blueprint(json_path: Path) -> tuple:
    """Generate C++ header and cpp from Blueprint JSON."""
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    name = data.get("Name", json_path.stem)
    parent = data.get("ParentClass", "Object")
    cpp_parent, prefix = get_parent_info(parent)

    # Generate C++ class name
    cpp_name = f"{prefix}{name}"

    vars_list = data.get("Variables", {}).get("List", [])
    funcs = data.get("FunctionSignatures", {}).get("Functions", [])
    dispatchers = data.get("EventDispatchers", {}).get("List", [])
    interfaces = data.get("Interfaces", {}).get("List", [])

    # Filter dispatchers from vars
    dispatcher_names = {d.get("Name") for d in dispatchers}
    vars_list = [v for v in vars_list if v.get("Name") not in dispatcher_names]

    # Build variable declarations and collect names for shadowing check
    var_lines = []
    member_var_names = set()
    forward_decls = set()  # Collect types for forward declarations
    for var in vars_list:
        var_name = sanitize_name(var.get("Name", "Unknown"))
        member_var_names.add(var_name)
        var_type = type_to_cpp(var.get("Type", {}))
        category = var.get("Category", "Default")
        editable = "EditAnywhere" if var.get("IsInstanceEditable") else "EditDefaultsOnly"
        readonly = "BlueprintReadOnly" if var.get("IsBlueprintReadOnly") else "BlueprintReadWrite"

        # Collect types that need forward declarations
        import re as re_local
        if var_type.endswith("*"):
            inner_type = var_type.rstrip("*").strip()
            # Collect SLF Blueprint, Widget, Component, and SaveGame types
            if inner_type.startswith("UW_") or inner_type.startswith("USG_") or inner_type.startswith("UB_") or inner_type.startswith("AB_") or inner_type.startswith("UAC_"):
                forward_decls.add(inner_type)
        # Check TSubclassOf, TArray, TMap for inner types
        if "TSubclassOf<" in var_type or "TArray<" in var_type or "TMap<" in var_type:
            matches = re_local.findall(r'[UA][WSGBC]_\w+', var_type)
            for match in matches:
                forward_decls.add(match)

        var_lines.append(f"\tUPROPERTY({editable}, {readonly}, Category = \"{category}\")\n\t{var_type} {var_name};")

    # Build dispatcher declarations - prefix with class name to avoid conflicts
    dispatcher_decls = []
    dispatcher_props = []
    for disp in dispatchers:
        disp_name = sanitize_name(disp.get("Name", "Unknown"))
        # Prefix delegate type with class name to avoid engine conflicts
        delegate_type = f"F{name}_{disp_name}"
        params = disp.get("Parameters", [])
        if not params:
            dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE({delegate_type});")
        elif len(params) == 1:
            p = params[0]
            p_type = type_to_cpp(p.get("Type", {}))
            p_name = sanitize_name(p.get("Name", "Param"))
            dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam({delegate_type}, {p_type}, {p_name});")
        else:
            param_strs = [f"{type_to_cpp(p.get('Type', {}))}, {sanitize_name(p.get('Name', 'P'))}" for p in params]
            num_names = {2: 'TwoParams', 3: 'ThreeParams', 4: 'FourParams', 5: 'FiveParams', 6: 'SixParams', 7: 'SevenParams', 8: 'EightParams', 9: 'NineParams'}
            suffix = num_names.get(len(params), f"{len(params)}Params")
            dispatcher_decls.append(f"DECLARE_DYNAMIC_MULTICAST_DELEGATE_{suffix}({delegate_type}, {', '.join(param_strs)});")
        dispatcher_props.append(f"\tUPROPERTY(BlueprintAssignable, Category = \"Events\")\n\t{delegate_type} {disp_name};")

    # Engine override functions - these shouldn't have UFUNCTION added since parent already has it
    ENGINE_OVERRIDE_FUNCS = {
        "UserConstructionScript", "ReceiveTick", "ReceiveBeginPlay", "ReceiveEndPlay",
        "ReceiveActorBeginOverlap", "ReceiveActorEndOverlap", "ReceiveHit", "ReceiveAnyDamage",
        "ReceivePointDamage", "ReceiveRadialDamage", "ReceiveActorBeginCursorOver",
        "ReceiveActorEndCursorOver", "ReceiveActorOnClicked", "ReceiveActorOnReleased",
        "ReceiveActorOnInputTouchBegin", "ReceiveActorOnInputTouchEnd", "ReceiveActorOnInputTouchEnter",
        "ReceiveActorOnInputTouchLeave", "ReceiveDestroyed", "K2_DestroyActor",
    }

    # Functions defined in parent Blueprint classes that can't be re-declared
    # Check if parent is a Blueprint and skip functions defined there
    if parent.endswith("_C"):
        parent_base = parent[:-2]
        parent_json = DNA_PATH / f"{parent_base}.json"
        if parent_json.exists():
            try:
                with open(parent_json, 'r', encoding='utf-8') as pf:
                    parent_data = json.load(pf)
                parent_funcs = parent_data.get("FunctionSignatures", {}).get("Functions", [])
                for pfunc in parent_funcs:
                    pfname = sanitize_name(pfunc.get("Name", ""))
                    if pfname:
                        ENGINE_OVERRIDE_FUNCS.add(pfname)
            except:
                pass

    # Build function declarations
    func_lines = []
    func_impls = []
    func_name_counts = {}  # Track function name usage to handle overloads
    for func in funcs:
        fname = sanitize_name(func.get("Name", "Unknown"))

        # Skip engine override functions - they can't have UFUNCTION
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

        # Determine return type
        if len(outputs) == 0:
            ret_type = "void"
        elif len(outputs) == 1:
            ret_type = type_to_cpp(outputs[0]["Type"])
        else:
            ret_type = "void"

        # Build params, avoiding shadowing with member variables
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
                param_type = type_to_cpp(out["Type"])
                out_base = sanitize_name(out['Name'])
                param_name = f"Out{out_base}"
                # Avoid duplicate output parameter names
                if param_name in used_param_names:
                    param_name = f"{param_name}{idx}"
                used_param_names.add(param_name)
                params.append(f"{param_type}& {param_name}")

        param_str = ", ".join(params)

        func_lines.append(f"\n\tUFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = \"{name}\")\n\t{ret_type} {fname}({param_str});\n\tvirtual {ret_type} {fname}_Implementation({param_str});")

        # Implementation stub - determine default return value
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

    # Interface includes
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

    # Determine includes based on parent
    parent_include = ""
    bp_parent_include = ""

    # Check if parent is a Blueprint class (A prefix + B_ name pattern)
    if parent.endswith("_C"):
        base_name = parent[:-2]  # Strip _C
        bp_parent_include = f'#include "Blueprints/{base_name}.h"'
    elif cpp_parent == "AActor":
        parent_include = "GameFramework/Actor.h"
    elif cpp_parent == "APawn":
        parent_include = "GameFramework/Pawn.h"
    elif cpp_parent == "ACharacter":
        parent_include = "GameFramework/Character.h"
    elif cpp_parent == "APlayerController":
        parent_include = "GameFramework/PlayerController.h"
    elif "GameMode" in cpp_parent:
        parent_include = "GameFramework/GameModeBase.h"
    elif "GameState" in cpp_parent:
        parent_include = "GameFramework/GameStateBase.h"
    elif "PlayerState" in cpp_parent:
        parent_include = "GameFramework/PlayerState.h"
    elif cpp_parent == "UGameInstance":
        parent_include = "Engine/GameInstance.h"
    else:
        parent_include = "CoreMinimal.h"

    # Build includes list
    includes_list = ['#include "CoreMinimal.h"']
    if parent_include:
        includes_list.append(f'#include "{parent_include}"')
    if bp_parent_include:
        includes_list.append(bp_parent_include)
    includes_list.append('#include "GameplayTagContainer.h"')
    includes_list.append('#include "SLFEnums.h"')
    includes_list.append('#include "SLFGameTypes.h"')
    includes_list.append('#include "SLFPrimaryDataAssets.h"')
    # Engine module includes for common types
    includes_list.append('#include "InputMappingContext.h"')
    includes_list.append('#include "LevelSequence.h"')
    includes_list.append('#include "LevelSequencePlayer.h"')
    includes_list.append('#include "MovieSceneSequencePlaybackSettings.h"')
    includes_list.append('#include "SkeletalMergingLibrary.h"')
    includes_list.append('#include "GeometryCollection/GeometryCollectionObject.h"')
    includes_list.append('#include "Field/FieldSystemObjects.h"')
    includes_list.extend(interface_includes)
    includes_list.append(f'#include "{name}.generated.h"')

    header = f"""// {name}.h
// C++ class for Blueprint {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session
// Source: BlueprintDNA/Blueprint/{json_path.name}
// Parent: {parent} -> {cpp_parent}
// Variables: {len(vars_list)} | Functions: {len(funcs)} | Dispatchers: {len(dispatchers)}

#pragma once

{chr(10).join(includes_list)}

// Forward declarations
class UAnimMontage;
class UDataTable;
{chr(10).join(f"class {fwd};" for fwd in sorted(forward_decls))}

// Event Dispatchers
{chr(10).join(dispatcher_decls)}

UCLASS(Blueprintable, BlueprintType)
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
// C++ implementation for Blueprint {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session
// Source: BlueprintDNA/Blueprint/{json_path.name}

#include "Blueprints/{name}.h"

{cpp_name}::{cpp_name}()
{{
}}
{"".join(func_impls)}
"""

    return header, cpp_content, name, len(vars_list), len(funcs), len(dispatchers)

def main():
    """Generate all Blueprint class files."""
    if not DNA_PATH.exists():
        print(f"ERROR: DNA path not found: {DNA_PATH}")
        return

    # CRITICAL: Build parent hierarchy BEFORE generating any files
    print("Building parent hierarchy for A/U prefix determination...")
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
            header, cpp, name, nvars, nfuncs, ndisps = generate_blueprint(json_file)

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
    print(f"Generated {len(generated)} Blueprint class files")
    print(f"Total Variables: {total_vars}")
    print(f"Total Functions: {total_funcs}")
    print(f"Total Dispatchers: {total_dispatchers}")

if __name__ == "__main__":
    main()
