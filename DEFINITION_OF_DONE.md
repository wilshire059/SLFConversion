# Definition of Done - SLF Blueprint to C++ Migration

## What is "Done"?

A Blueprint item is **DONE** when:

1. **ALL logic is in C++** - No execution nodes remain in Blueprint graphs
2. **Blueprint is an empty shell** - Only contains class identity, component instances, and property defaults
3. **20-pass validation complete** - Every variable, function, graph, macro, interface function verified
4. **Compiles and runs** - PIE test passes with expected behavior
5. **Documented** - Tracker shows all passes complete with validation notes

---

## Complete Scope - ALL Item Types

**CRITICAL: Do NOT skip any of these!**

| Item Type | JSON Location | C++ Implementation |
|-----------|---------------|-------------------|
| **Variables** | `Variables.List` | `UPROPERTY` declarations |
| **Functions** | `Functions`, `Graphs` | `UFUNCTION` + `_Implementation` |
| **Event Dispatchers** | `EventDispatchers.List` | `DECLARE_DYNAMIC_MULTICAST_DELEGATE` |
| **Interface Functions** | `Interfaces`, `ImplementedInterfaces` | `UINTERFACE` + `I<Name>` class |
| **Macros** | `MacroGraphs` | Inline C++ or helper functions |
| **Event Graphs** | `Graphs[EventGraph]` | `BeginPlay`, `Tick`, event handlers |
| **Function Graphs** | `Graphs[FunctionName]` | `_Implementation` function bodies |
| **Collapsed Graphs** | `Graphs[CollapsedGraph]` | Inline C++ or helper functions |
| **Animation Graphs** | AnimBP `AnimGraph` | Custom `FAnimNode_*` classes |

---

## What is "Good"?

A migration is **GOOD** when:

1. **Behavior matches original** - Side-by-side comparison with original Blueprint logic
2. **No inverted logic** - Branch conditions verified TRUE/FALSE match JSON
3. **All callers work** - Dependent Blueprints compile without "pin no longer exists" errors
4. **Debug logging present** - UE_LOG for function entry, key decisions, errors
5. **Type-safe** - No implicit conversions, proper null checks

---

## The 20-Pass Validation Protocol

### Phase 1: JSON Analysis (Passes 1-5)

| Pass | Focus | Deliverable |
|------|-------|-------------|
| 1 | **Extract All Items** | List every variable, function, dispatcher, graph from JSON |
| 2 | **Document Types** | Record exact types for every item (Blueprint naming) |
| 3 | **Map Dependencies** | List all referenced structs, enums, interfaces, components |
| 4 | **Trace Logic Flow** | For each function, document node-by-node execution |
| 5 | **Identify Edge Cases** | Note branches, loops, timers, async operations |

### Phase 2: Implementation (Passes 6-10)

| Pass | Focus | Deliverable |
|------|-------|-------------|
| 6 | **Write Header** | UPROPERTY declarations for all variables |
| 7 | **Write Declarations** | UFUNCTION declarations for all functions |
| 8 | **Write Implementations** | Function bodies matching JSON logic EXACTLY |
| 9 | **Add Dispatchers** | DECLARE_DYNAMIC_MULTICAST_DELEGATE for all events |
| 10 | **Add Debug Logging** | UE_LOG for function entry, decisions, errors |

### Phase 3: Verification (Passes 11-15)

| Pass | Focus | Deliverable |
|------|-------|-------------|
| 11 | **Re-read JSON** | Compare C++ to JSON node-by-node |
| 12 | **Verify Branches** | Check every if/else matches TRUE/FALSE from JSON |
| 13 | **Verify Types** | Ensure parameter types match caller expectations |
| 14 | **Verify Names** | Function and variable names match Blueprint exactly |
| 15 | **Verify Signatures** | Output parameters, return types match callers |

### Phase 4: Testing (Passes 16-20)

| Pass | Focus | Deliverable |
|------|-------|-------------|
| 16 | **Compile C++** | Build succeeds with no errors |
| 17 | **Run Migration Script** | Blueprint reparenting succeeds |
| 18 | **Compile Blueprints** | All dependent Blueprints compile |
| 19 | **PIE Test** | Play in Editor, verify functionality |
| 20 | **Final Review** | Update tracker, mark complete |

---

## Concrete Example: Migrating `GetBuffsWithTag` Function

### Source: AC_BuffManager.json

```json
{
  "Name": "GetBuffsWithTag",
  "Parameters": [
    {"Name": "Tag", "Type": "GameplayTag"},
    {"Name": "OutBuffs", "Type": "Array<Object>", "Direction": "Out"}
  ],
  "ReturnType": "void",
  "Graph": {
    "Nodes": [
      {"Type": "K2Node_FunctionEntry", "Outputs": ["exec", "Tag"]},
      {"Type": "K2Node_MakeArray", "Id": "TempArray"},
      {"Type": "K2Node_MacroInstance", "Name": "ForEachLoop", "Array": "ActiveBuffs"},
      {"Type": "K2Node_CallFunction", "Name": "GetBuffTag", "Target": "ArrayElement"},
      {"Type": "K2Node_IfThenElse", "Condition": "MatchesTag(Tag, BuffTag)"},
      {"Type": "K2Node_AddToArray", "Array": "TempArray", "Element": "ArrayElement"},
      {"Type": "K2Node_SetVariable", "Variable": "OutBuffs", "Value": "TempArray"},
      {"Type": "K2Node_FunctionResult"}
    ]
  }
}
```

### Pass 1-5: JSON Analysis

```
Pass 1 - Items:
  - Function: GetBuffsWithTag
  - Parameters: Tag (FGameplayTag), OutBuffs (TArray<UObject*>&)
  - Return: void

Pass 2 - Types:
  - Tag: FGameplayTag (read-only input)
  - OutBuffs: TArray<UObject*>& (output reference)
  - ActiveBuffs: TArray<UObject*> (member variable)

Pass 3 - Dependencies:
  - Uses: ActiveBuffs (member variable)
  - Calls: GetBuffTag (on each buff object)
  - Calls: MatchesTag (FGameplayTag comparison)

Pass 4 - Logic Flow:
  1. Create empty temp array
  2. For each buff in ActiveBuffs:
     a. Get buff's tag via GetBuffTag()
     b. If tag matches input Tag: add to temp array
  3. Set OutBuffs = temp array
  4. Return

Pass 5 - Edge Cases:
  - Empty ActiveBuffs array: returns empty OutBuffs
  - No matches: returns empty OutBuffs
  - Null buff in array: GetBuffTag may return invalid tag
```

### Pass 6-10: Implementation

```cpp
// Pass 6: Header - Variable already exists
// UPROPERTY(BlueprintReadWrite, Category = "Runtime")
// TArray<UObject*> ActiveBuffs;

// Pass 7: Declaration
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buff Manager|Query")
void GetBuffsWithTag(FGameplayTag Tag, TArray<UObject*>& OutBuffs);
virtual void GetBuffsWithTag_Implementation(FGameplayTag Tag, TArray<UObject*>& OutBuffs);

// Pass 8: Implementation
void UBuffManagerComponent::GetBuffsWithTag_Implementation(FGameplayTag Tag, TArray<UObject*>& OutBuffs)
{
    // Pass 10: Debug logging
    UE_LOG(LogTemp, Log, TEXT("[BuffManager] GetBuffsWithTag: %s"), *Tag.ToString());

    OutBuffs.Empty();

    for (UObject* Buff : ActiveBuffs)
    {
        if (!Buff) continue;  // Pass 5 edge case

        // Get buff's tag - assuming IBuff interface
        FGameplayTag BuffTag;
        if (Buff->Implements<UBuffInterface>())
        {
            BuffTag = IBuffInterface::Execute_GetBuffTag(Buff);
        }

        // Pass 4 logic: if matches, add to output
        if (Tag.MatchesTag(BuffTag))
        {
            OutBuffs.Add(Buff);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[BuffManager] GetBuffsWithTag found %d buffs"), OutBuffs.Num());
}

// Pass 9: No dispatcher for this function
```

### Pass 11-15: Verification

```
Pass 11 - Re-read JSON:
  [x] Function entry matches
  [x] ForEachLoop over ActiveBuffs
  [x] GetBuffTag call on each element
  [x] MatchesTag comparison
  [x] Add to temp array if match
  [x] Set OutBuffs at end

Pass 12 - Verify Branches:
  [x] if (Tag.MatchesTag(BuffTag)) - TRUE adds, FALSE skips
  [x] Matches JSON: K2Node_IfThenElse TRUE -> AddToArray

Pass 13 - Verify Types:
  [x] Tag: FGameplayTag (matches)
  [x] OutBuffs: TArray<UObject*>& (matches)

Pass 14 - Verify Names:
  [x] Function: GetBuffsWithTag (exact match)
  [x] Parameter: Tag (exact match)
  [x] Parameter: OutBuffs (exact match)

Pass 15 - Verify Signatures:
  [x] Callers expect void return
  [x] Callers expect OutBuffs as output reference
  [x] BlueprintCallable for direct calls
```

### Pass 16-20: Testing

```
Pass 16 - Compile C++:
  [x] Build.bat succeeds
  [x] No errors, no warnings

Pass 17 - Run Migration Script:
  [x] AC_BuffManager reparented to UBuffManagerComponent
  [x] Function available in Blueprint

Pass 18 - Compile Blueprints:
  [x] All callers of GetBuffsWithTag compile
  [x] No "pin no longer exists" errors

Pass 19 - PIE Test:
  [x] Add buff with tag "Buff.Heal"
  [x] Call GetBuffsWithTag("Buff.Heal")
  [x] Returns array with 1 element
  [x] Call GetBuffsWithTag("Buff.Damage")
  [x] Returns empty array

Pass 20 - Final Review:
  [x] Tracker updated
  [x] All passes marked complete
  [x] Implementation notes recorded
```

---

## Tracker Entry Format

For each item (variable, function, graph, dispatcher):

```json
{
  "name": "GetBuffsWithTag",
  "type": "function",
  "passes": {
    "pass1_items": true,
    "pass2_types": true,
    "pass3_deps": true,
    "pass4_logic": true,
    "pass5_edges": true,
    "pass6_header": true,
    "pass7_decl": true,
    "pass8_impl": true,
    "pass9_disp": true,
    "pass10_log": true,
    "pass11_json": true,
    "pass12_branch": true,
    "pass13_types": true,
    "pass14_names": true,
    "pass15_sig": true,
    "pass16_cpp": true,
    "pass17_script": true,
    "pass18_bp": true,
    "pass19_pie": true,
    "pass20_final": true
  },
  "notes": "Filters ActiveBuffs by tag, returns matching buffs",
  "verified_behavior": "Empty input returns empty. Null buffs skipped."
}
```

---

## Failure Modes to Watch For

### 1. Inverted Logic (CRITICAL)
```
WRONG: if (bIsOpen) { ProcessNow(); }
RIGHT: if (bIsOpen) { QueueForLater(); } else { ProcessNow(); }

ALWAYS verify TRUE/FALSE branches from JSON!
```

### 2. Wrong Parameter Direction
```
WRONG: void GetItem(UObject* OutItem);  // Caller expects modification
RIGHT: void GetItem(UObject*& OutItem); // Reference allows modification
```

### 3. Missing Null Checks
```
WRONG: Buff->GetTag();  // Crashes if Buff is null
RIGHT: if (Buff) { Buff->GetTag(); }
```

### 4. Type Mismatch
```
WRONG: int32 Count;  // Blueprint uses float
RIGHT: float Count;  // Match Blueprint exactly
```

### 5. Name Mismatch
```
WRONG: bIsSprinting  // C++ convention
RIGHT: IsSprinting   // Blueprint name (no 'b' prefix in BP)
```

---

## Non-Negotiable Rules

1. **NO STUB IMPLEMENTATIONS** - Every function must have full logic, not `// TODO`
2. **NO ASSUMPTIONS** - If unsure, read the JSON again
3. **NO SHORTCUTS** - All 20 passes required for every item
4. **NO LEAVING LOGIC IN BLUEPRINT** - Everything moves to C++
5. **NO GIVING UP** - If it seems impossible, find a way

---

## When to Mark DONE

Only mark an item DONE when:

- [ ] All 20 passes complete with checkmarks
- [ ] Compiles in C++ (pass 16)
- [ ] Blueprint callers compile (pass 18)
- [ ] PIE test passes (pass 19)
- [ ] Tracker updated with notes (pass 20)

If ANY pass fails, the item is NOT DONE.
