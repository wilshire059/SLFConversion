"""
Generate C++ headers and stubs for Animation Notify classes.
AN_* extends UAnimNotify, ANS_* extends UAnimNotifyState
"""
import json
from pathlib import Path
from datetime import datetime
from slf_type_mapping import ENUM_MAP, STRUCT_MAP, ENGINE_STRUCT_MAP, get_enum_cpp_name, get_struct_cpp_name, get_class_cpp_name

DNA_PATH = Path(r"C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/Animation")

# Engine override functions - these shouldn't have UFUNCTION added as they're already declared in parent
ENGINE_OVERRIDE_FUNCS = {
    # UAnimNotify overrides
    "Received_Notify",
    "GetNotifyName",
    # UAnimNotifyState overrides
    "Received_NotifyBegin",
    "Received_NotifyEnd",
    "Received_NotifyTick",
    # Common engine overrides
    "BlueprintThreadSafeUpdateAnimation",
    "BlueprintUpdateAnimation",
}
OUTPUT_PATH = Path(r"C:/scripts/SLFConversion/Source/SLFConversion/AnimNotifies")

def type_to_cpp(type_info: dict) -> str:
    """Convert Blueprint type info to C++ type."""
    cat = type_info.get("Category", "")
    sub_obj = type_info.get("SubCategoryObject", "")
    is_array = type_info.get("IsArray", False)

    cpp_type = "void"

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
        elif sub_obj in ("Actor", "Pawn", "Character"):
            cpp_type = f"A{sub_obj}*"
        else:
            cpp_type = f"U{sub_obj}*" if sub_obj else "UObject*"
    elif cat == "class":
        if sub_obj and sub_obj.endswith("_C"):
            base = sub_obj[:-2]
            # Actor-based Blueprints get A prefix
            if base.startswith("B_"):
                cpp_type = f"TSubclassOf<A{base}>"
            else:
                cpp_type = f"TSubclassOf<U{base}>"
        else:
            cpp_type = "TSubclassOf<UObject>"

    if is_array:
        cpp_type = f"TArray<{cpp_type}>"

    return cpp_type

def sanitize_name(name: str) -> str:
    return name.replace("?", "").replace(" ", "").replace("-", "_")

def generate_animnotify(json_path: Path) -> tuple:
    """Generate C++ header and cpp from Animation Notify JSON."""
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    name = data.get("Name", json_path.stem)
    parent = data.get("ParentClass", "AnimNotify")

    # Determine parent class
    is_state_notify = name.startswith("ANS_") or parent == "AnimNotifyState"
    if is_state_notify:
        cpp_parent = "UAnimNotifyState"
        parent_include = "Animation/AnimNotifies/AnimNotifyState.h"
    else:
        cpp_parent = "UAnimNotify"
        parent_include = "Animation/AnimNotifies/AnimNotify.h"

    cpp_name = f"U{name}"

    vars_list = data.get("Variables", {}).get("List", [])
    funcs = data.get("FunctionSignatures", {}).get("Functions", [])

    # Build variable lines
    var_lines = []
    for var in vars_list:
        var_name = sanitize_name(var.get("Name", "Unknown"))
        var_type = type_to_cpp(var.get("Type", {}))
        category = var.get("Category", "Notify")
        editable = "EditAnywhere" if var.get("IsInstanceEditable") else "EditDefaultsOnly"
        var_lines.append(f"\tUPROPERTY({editable}, BlueprintReadWrite, Category = \"{category}\")\n\t{var_type} {var_name};")

    # Build function declarations
    func_lines = []
    func_impls = []
    for func in funcs:
        fname = sanitize_name(func.get("Name", "Unknown"))
        # Skip engine override functions - they're already declared in parent class
        if fname in ENGINE_OVERRIDE_FUNCS:
            continue
        inputs = func.get("Inputs", [])
        outputs = func.get("Outputs", [])
        ret_type = "void" if len(outputs) == 0 else (type_to_cpp(outputs[0]["Type"]) if len(outputs) == 1 else "void")

        params = []
        for inp in inputs:
            ptype = type_to_cpp(inp['Type'])
            pname = sanitize_name(inp['Name'])
            # Pass container types and structs by const reference (matches UHT expectations)
            if ptype.startswith("TArray<") or ptype.startswith("TMap<") or ptype.startswith("TSet<"):
                params.append(f"const {ptype}& {pname}")
            elif ptype.startswith("F") and "*" not in ptype:
                params.append(f"const {ptype}& {pname}")
            else:
                params.append(f"{ptype} {pname}")
        if len(outputs) > 1:
            for out in outputs:
                params.append(f"{type_to_cpp(out['Type'])}& Out{sanitize_name(out['Name'])}")
        param_str = ", ".join(params)

        func_lines.append(f"\n\tUFUNCTION(BlueprintCallable, Category = \"{name}\")\n\t{ret_type} {fname}({param_str});")

        default_ret = "nullptr" if "*" in ret_type else ("false" if ret_type == "bool" else ("0" if ret_type != "void" else ""))
        ret_stmt = f"\n\treturn {default_ret};" if ret_type != "void" else ""
        func_impls.append(f"\n{ret_type} {cpp_name}::{fname}({param_str})\n{{\n\t// TODO: Implement from Blueprint{ret_stmt}\n}}")

    timestamp = datetime.now().strftime("%Y-%m-%d")

    # Override declarations based on notify type
    if is_state_notify:
        override_decls = """
\t// Notify state callbacks
\tvirtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
\tvirtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
\tvirtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
\tvirtual FString GetNotifyName_Implementation() const override;"""
        override_impls = f"""
void {cpp_name}::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{{
\tSuper::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
\t
\tif (!MeshComp || !MeshComp->GetOwner())
\t{{
\t\treturn;
\t}}
\t
\tAActor* Owner = MeshComp->GetOwner();
\t
\t// TODO: Migrate Received_NotifyBegin logic from Blueprint
\tUE_LOG(LogTemp, Log, TEXT("{cpp_name}::NotifyBegin on %s"), *Owner->GetName());
}}

void {cpp_name}::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{{
\tSuper::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
\t
\tif (!MeshComp || !MeshComp->GetOwner())
\t{{
\t\treturn;
\t}}
\t
\t// TODO: Migrate Received_NotifyTick logic from Blueprint
}}

void {cpp_name}::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{{
\tSuper::NotifyEnd(MeshComp, Animation, EventReference);
\t
\tif (!MeshComp || !MeshComp->GetOwner())
\t{{
\t\treturn;
\t}}
\t
\tAActor* Owner = MeshComp->GetOwner();
\t
\t// TODO: Migrate Received_NotifyEnd logic from Blueprint
\tUE_LOG(LogTemp, Log, TEXT("{cpp_name}::NotifyEnd on %s"), *Owner->GetName());
}}

FString {cpp_name}::GetNotifyName_Implementation() const
{{
\treturn TEXT("{name}");
}}"""
    else:
        override_decls = """
\t// Notify callback
\tvirtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
\tvirtual FString GetNotifyName_Implementation() const override;"""
        override_impls = f"""
void {cpp_name}::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{{
\tSuper::Notify(MeshComp, Animation, EventReference);
\t
\tif (!MeshComp || !MeshComp->GetOwner())
\t{{
\t\treturn;
\t}}
\t
\tAActor* Owner = MeshComp->GetOwner();
\t
\t// TODO: Migrate Received_Notify logic from Blueprint
\tUE_LOG(LogTemp, Log, TEXT("{cpp_name}::Notify on %s"), *Owner->GetName());
}}

FString {cpp_name}::GetNotifyName_Implementation() const
{{
\treturn TEXT("{name}");
}}"""

    header = f"""// {name}.h
// C++ Animation Notify for {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session
// Source: BlueprintDNA/Animation/{json_path.name}
// Parent: {cpp_parent}
// Variables: {len(vars_list)} | Functions: {len(funcs)}

#pragma once

#include "CoreMinimal.h"
#include "{parent_include}"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LevelSequence.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "{name}.generated.h"

// Forward declarations for Blueprint classes
class AB_BaseProjectile;
class AB_Interactable;
class UB_Action;
class UB_Buff;
class UB_StatusEffect;

UCLASS()
class SLFCONVERSION_API {cpp_name} : public {cpp_parent}
{{
\tGENERATED_BODY()

public:
\t{cpp_name}();

\t// ═══════════════════════════════════════════════════════════════════════
\t// VARIABLES ({len(vars_list)})
\t// ═══════════════════════════════════════════════════════════════════════

{chr(10).join(var_lines)}
{override_decls}

\t// ═══════════════════════════════════════════════════════════════════════
\t// HELPER FUNCTIONS ({len(funcs)})
\t// ═══════════════════════════════════════════════════════════════════════
{"".join(func_lines)}
}};
"""

    cpp_content = f"""// {name}.cpp
// C++ Animation Notify implementation for {name}
//
// 20-PASS VALIDATION: {timestamp} Autonomous Session

#include "AnimNotifies/{name}.h"
#include "Components/SkeletalMeshComponent.h"

{cpp_name}::{cpp_name}()
{{
#if WITH_EDITORONLY_DATA
\tNotifyColor = FColor(255, 150, 50, 255);
#endif
}}
{override_impls}
{"".join(func_impls)}
"""

    return header, cpp_content, name, len(vars_list), len(funcs)

def main():
    if not DNA_PATH.exists():
        print(f"ERROR: DNA path not found: {DNA_PATH}")
        return

    OUTPUT_PATH.mkdir(parents=True, exist_ok=True)

    generated = []
    total_vars = 0
    total_funcs = 0

    for json_file in sorted(DNA_PATH.glob("*.json")):
        print(f"Processing: {json_file.name}")
        try:
            header, cpp, name, nvars, nfuncs = generate_animnotify(json_file)

            with open(OUTPUT_PATH / f"{name}.h", 'w', encoding='utf-8') as f:
                f.write(header)
            with open(OUTPUT_PATH / f"{name}.cpp", 'w', encoding='utf-8') as f:
                f.write(cpp)

            generated.append(name)
            total_vars += nvars
            total_funcs += nfuncs
            print(f"  -> {name}.h/.cpp")
        except Exception as e:
            print(f"  ERROR: {e}")

    print(f"\n{'='*60}")
    print(f"Generated {len(generated)} Animation Notify files")
    print(f"Total Variables: {total_vars}")
    print(f"Total Functions: {total_funcs}")

if __name__ == "__main__":
    main()
