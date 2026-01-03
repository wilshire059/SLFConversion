# Blueprint JSON Extraction Guide

## Overview

This guide explains how to extract all relevant information from Blueprint JSON exports for migration tracking.

## JSON Export Structure

Blueprint JSON exports have this structure:

```json
{
  "Name": "AC_ActionManager",
  "Path": "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager.AC_ActionManager",
  "BlueprintClass": "Blueprint",
  "ParentClass": "ActorComponent",
  "ParentClassPath": "/Script/Engine.ActorComponent",
  "BlueprintType": "BPTYPE_Normal",

  "Variables": {
    "List": [
      {
        "Name": "IsSprinting",
        "Category": "Runtime",
        "DefaultValue": "...",
        "IsInstanceEditable": true,
        "IsBlueprintReadOnly": false,
        "IsExposeOnSpawn": false,
        "IsPrivate": false,
        "IsReplicated": false,
        "Type": {
          "Category": "bool",
          "SubCategory": "None",
          "IsArray": false,
          "IsSet": false,
          "IsMap": false,
          "IsReference": false,
          "IsConst": false
        }
      }
    ]
  },

  "EventDispatchers": {
    "List": [
      {
        "Name": "OnBuffDetected",
        "Parameters": [
          {"Name": "Buff", "Type": "object"},
          {"Name": "Added", "Type": "bool"}
        ]
      }
    ]
  },

  "Interfaces": {
    "List": [
      {
        "Name": "BPI_GenericCharacter_C",
        "Path": "/Game/...",
        "Functions": []
      }
    ]
  },

  "Components": {
    "Hierarchy": [
      {
        "Name": "AC_StatManager",
        "Class": "AC_StatManager_C",
        "ClassPath": "/Game/..."
      }
    ]
  },

  "Functions": [
    {
      "Name": "GetStat",
      "Category": "Stat Manager",
      "AccessSpecifier": "Public",
      "IsPure": true,
      "IsConst": false,
      "ReturnType": {...},
      "Parameters": [...],
      "LocalVariables": [...],
      "Graphs": [...]
    }
  ],

  "Graphs": [
    {
      "Name": "EventGraph",
      "Type": "EventGraph",
      "Nodes": [...]
    }
  ]
}
```

---

## Extracting Variables

### From JSON:

```json
"Variables": {
  "List": [
    {
      "Name": "MovementDirection",
      "Category": "Runtime",
      "Type": {
        "Category": "byte",
        "SubCategory": "None",
        "SubCategoryObject": "E_Direction",
        "SubCategoryObjectPath": "/Game/SoulslikeFramework/Enums/E_Direction.E_Direction"
      }
    }
  ]
}
```

### To C++:

```cpp
UPROPERTY(BlueprintReadWrite, Category = "Runtime")
ESLFDirection MovementDirection = ESLFDirection::Idle;
```

### Type Mapping Table

| JSON Type | JSON SubCategory | C++ Type |
|-----------|------------------|----------|
| bool | None | bool |
| byte | None | uint8 |
| byte | E_Direction | ESLFDirection |
| int | None | int32 |
| int64 | None | int64 |
| real | float | float |
| real | double | double |
| struct | FVector | FVector |
| struct | FRotator | FRotator |
| struct | FGameplayTag | FGameplayTag |
| struct | FStatInfo | FSLFStatInfo |
| object | Actor | AActor* |
| object | ActorComponent | UActorComponent* |
| object | AnimMontage | UAnimMontage* |
| object | DataTable | UDataTable* |
| softobject | AnimMontage | TSoftObjectPtr<UAnimMontage> |
| softclass | Actor | TSoftClassPtr<AActor> |
| class | Actor | TSubclassOf<AActor> |

### Array/Map/Set Handling

```json
// Array
"Type": {
  "Category": "object",
  "IsArray": true,
  "SubCategoryObject": "Actor"
}
// C++: TArray<AActor*>

// Map
"Type": {
  "Category": "struct",
  "IsMap": true,
  "SubCategoryObject": "GameplayTag",
  "KeyType": {
    "Category": "struct",
    "SubCategoryObject": "GameplayTag"
  }
}
// C++: TMap<FGameplayTag, FSLFStatInfo>

// Set
"Type": {
  "Category": "struct",
  "IsSet": true,
  "SubCategoryObject": "GameplayTag"
}
// C++: TSet<FGameplayTag>
```

---

## Extracting Functions

### From JSON:

```json
{
  "Name": "AdjustStat",
  "Category": "Stat Manager",
  "AccessSpecifier": "Public",
  "IsPure": false,
  "IsConst": false,
  "ReturnType": {
    "Category": "bool"
  },
  "Parameters": [
    {
      "Name": "StatTag",
      "Type": {
        "Category": "struct",
        "SubCategoryObject": "GameplayTag"
      }
    },
    {
      "Name": "ValueType",
      "Type": {
        "Category": "byte",
        "SubCategoryObject": "E_ValueType"
      }
    },
    {
      "Name": "Change",
      "Type": {
        "Category": "real",
        "SubCategory": "double"
      }
    }
  ]
}
```

### To C++:

```cpp
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Stat Manager")
bool AdjustStat(FGameplayTag StatTag, ESLFValueType ValueType, double Change);
virtual bool AdjustStat_Implementation(FGameplayTag StatTag, ESLFValueType ValueType, double Change);
```

### Function Specifier Decision Tree

```
Is this a pure function (no exec pins)?
  YES -> UFUNCTION(BlueprintPure, Category = "...")
  NO  -> Is this a custom event in Blueprint?
           YES -> UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "...")
           NO  -> Is this called from Blueprint?
                    YES -> UFUNCTION(BlueprintCallable, Category = "...")
                    NO  -> Regular C++ function (no UFUNCTION)
```

---

## Extracting Event Dispatchers

### From JSON:

```json
"EventDispatchers": {
  "List": [
    {
      "Name": "OnStatChanged",
      "Parameters": [
        {"Name": "StatTag", "Type": {"Category": "struct", "SubCategoryObject": "GameplayTag"}},
        {"Name": "NewValue", "Type": {"Category": "real", "SubCategory": "double"}}
      ]
    }
  ]
}
```

### To C++:

```cpp
// In header, before UCLASS:
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, FGameplayTag, StatTag, double, NewValue);

// In class:
UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
FOnStatChanged OnStatChanged;
```

### Delegate Naming Pattern

| Param Count | Macro |
|-------------|-------|
| 0 | DECLARE_DYNAMIC_MULTICAST_DELEGATE(FName) |
| 1 | DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FName, Type1, Name1) |
| 2 | DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FName, Type1, Name1, Type2, Name2) |
| 3 | DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(...) |
| 4+ | DECLARE_DYNAMIC_MULTICAST_DELEGATE_<N>Params(...) |

---

## Extracting Graph Logic

### Graph Node Types

| Node Class | Purpose | C++ Equivalent |
|------------|---------|----------------|
| K2Node_Event | Event entry point | Function entry |
| K2Node_CustomEvent | Custom event | BlueprintNativeEvent |
| K2Node_CallFunction | Function call | FunctionName() |
| K2Node_IfThenElse | Branch | if (condition) |
| K2Node_MacroInstance | Macro call | Inline the macro logic |
| K2Node_VariableGet | Read variable | variable |
| K2Node_VariableSet | Write variable | variable = value |
| K2Node_DynamicCast | Cast | Cast<Type>(Object) |
| K2Node_CommutativeAssociativeBinaryOperator | Math op | a + b, a * b, etc |
| K2Node_Select | Switch/Select | switch or ternary |

### Tracing Execution Flow

```json
"Nodes": [
  {
    "NodeClass": "K2Node_Event",
    "NodeGuid": "ABC123",
    "EventName": "BeginPlay",
    "Pins": [
      {"PinName": "then", "LinkedTo": ["DEF456:execute"]}
    ]
  },
  {
    "NodeClass": "K2Node_CallFunction",
    "NodeGuid": "DEF456",
    "FunctionName": "InitializeStats",
    "Pins": [
      {"PinName": "execute", "LinkedTo": []},
      {"PinName": "then", "LinkedTo": ["GHI789:execute"]}
    ]
  }
]
```

**Execution flow:** BeginPlay -> InitializeStats -> (next node at GHI789)

### Branch Analysis

```json
{
  "NodeClass": "K2Node_IfThenElse",
  "NodeGuid": "XYZ999",
  "Pins": [
    {"PinName": "Condition", "LinkedTo": ["VAR001:ReturnValue"]},
    {"PinName": "Then", "LinkedTo": ["ACTION_TRUE:execute"]},
    {"PinName": "Else", "LinkedTo": ["ACTION_FALSE:execute"]}
  ]
}
```

**C++ equivalent:**
```cpp
if (/* value from VAR001 */) {
    // ACTION_TRUE logic
} else {
    // ACTION_FALSE logic
}
```

---

## Extracting Dependencies

### Struct Dependencies

Look for all Type.SubCategoryObject values that reference `/Game/SoulslikeFramework/Structures/`

### Enum Dependencies

Look for all Type.SubCategoryObject values that reference `/Game/SoulslikeFramework/Enums/`

### Interface Dependencies

Check Interfaces.List for all implemented interfaces

### Component Dependencies

1. Check Components.Hierarchy for owned components
2. Check function calls for GetComponent<> or FindComponent patterns

### Blueprint Dependencies

Look for:
- SpawnActor calls referencing Blueprint classes
- Cast nodes referencing Blueprint classes
- Variable types referencing Blueprint classes

---

## Complete Extraction Checklist

For each Blueprint JSON:

- [ ] Extract blueprint name and path
- [ ] Identify parent class
- [ ] List all implemented interfaces
- [ ] Extract ALL variables with exact names and types
- [ ] Extract ALL event dispatchers with signatures
- [ ] Extract ALL functions with signatures
- [ ] For each function, analyze graph nodes
- [ ] Document execution flow for complex functions
- [ ] List all struct dependencies
- [ ] List all enum dependencies
- [ ] List all interface dependencies
- [ ] List all component dependencies
- [ ] List all other Blueprint dependencies

---

## Validation Questions

After extraction, verify:

1. Does the variable count match JSON?
2. Does the function count match JSON?
3. Does the event dispatcher count match JSON?
4. Are all parameter names exact matches?
5. Are all types correctly mapped?
6. Are all graph entry points accounted for?
