# Validation Strategy - Before and After Migration

## CORE PRINCIPLE

**When you encounter a pin or node error, the natural instinct should be:**

> "I need to migrate this or the dependency that is the root cause of that error!"

**NOT:** "How do I fix this pin?"

The goal is to REMOVE all Blueprint logic, not to fix pins. Every error is a signal that something needs to be migrated to C++.

---

## Pre-Reparent Validation

### BEFORE switching anything to C++ or reparenting:

**Capture ALL settings that could be affected:**

```python
def capture_blueprint_state(bp_path):
    """Capture complete Blueprint state BEFORE migration"""
    state = {
        "path": bp_path,
        "timestamp": datetime.now(),

        # Class settings
        "parent_class": get_parent_class(bp),
        "interfaces": get_implemented_interfaces(bp),
        "class_flags": get_class_flags(bp),

        # Variables with FULL details
        "variables": [
            {
                "name": var.name,
                "type": var.type,
                "default_value": var.default,
                "category": var.category,
                "replication": var.replication_settings,
                "editable": var.is_editable,
                "expose_on_spawn": var.expose_on_spawn,
            }
            for var in bp.variables
        ],

        # Components with ALL properties
        "components": [
            {
                "name": comp.name,
                "class": comp.class_path,
                "parent": comp.parent_name,
                "properties": get_all_property_values(comp),
                "transform": comp.relative_transform,
            }
            for comp in bp.components
        ],

        # Animation settings (for characters/AnimBPs)
        "animation": {
            "anim_class": get_anim_class(bp),
            "skeletal_mesh": get_skeletal_mesh(bp),
            "anim_mode": get_anim_mode(bp),
        },

        # Data asset references
        "data_assets": find_all_data_asset_refs(bp),

        # Event dispatchers
        "dispatchers": [
            {
                "name": d.name,
                "signature": d.signature,
                "bindings": get_dispatcher_bindings(d),
            }
            for d in bp.event_dispatchers
        ],

        # Default object values
        "defaults": get_all_default_values(bp.cdo),
    }
    return state
```

### What to Capture for Each Blueprint Type:

| Type | Capture |
|------|---------|
| **Actor Component** | Owner ref, tick settings, replication, component tags |
| **Character** | Skeletal mesh, anim class, capsule size, movement mode, default pawn data |
| **Widget** | Slot settings, anchors, widget hierarchy, style assets |
| **AnimBP** | Skeleton, preview mesh, linked anim layers, montage slots |
| **Data Asset refs** | All soft/hard references to PDA_*, DT_* |

---

## Post-Reparent Validation

### AFTER reparenting, verify EVERYTHING is preserved:

```python
def validate_migration(before_state, after_bp):
    """Compare before/after state - report ANY differences"""
    after_state = capture_blueprint_state(after_bp)

    errors = []

    # Check parent class changed correctly
    if after_state["parent_class"] != expected_cpp_class:
        errors.append(f"Parent class wrong: {after_state['parent_class']}")

    # Check NO variables were lost
    for var in before_state["variables"]:
        if not find_variable(after_state, var["name"]):
            errors.append(f"LOST variable: {var['name']}")

    # Check NO component settings changed
    for comp in before_state["components"]:
        after_comp = find_component(after_state, comp["name"])
        if not after_comp:
            errors.append(f"LOST component: {comp['name']}")
        else:
            diff = compare_properties(comp["properties"], after_comp["properties"])
            if diff:
                errors.append(f"Component {comp['name']} changed: {diff}")

    # Check animation settings preserved
    if before_state["animation"] != after_state["animation"]:
        errors.append(f"Animation settings changed!")

    # Check data asset references preserved
    for ref in before_state["data_assets"]:
        if ref not in after_state["data_assets"]:
            errors.append(f"LOST data asset ref: {ref}")

    # Check defaults preserved
    for key, value in before_state["defaults"].items():
        if after_state["defaults"].get(key) != value:
            errors.append(f"Default changed: {key} was {value}, now {after_state['defaults'].get(key)}")

    return errors
```

---

## Error Response Strategy

### When You See a Pin Error:

```
ERROR: "In use pin 'SomeProperty' no longer exists"
```

**WRONG Response:**
"I need to fix this pin by adding a property called SomeProperty"

**RIGHT Response:**
"This caller Blueprint has logic that depends on SomeProperty. I need to:
1. Identify what Blueprint is calling this
2. Migrate THAT Blueprint's logic to C++
3. The pin will naturally go away when the caller is migrated"

### When You See a Node Error:

```
ERROR: "Cannot find function 'GetSomeValue' in class"
```

**WRONG Response:**
"I need to add GetSomeValue function to make this compile"

**RIGHT Response:**
"This Blueprint is calling GetSomeValue. The goal is to:
1. REMOVE this call by migrating the calling logic to C++
2. Not to preserve the call by adding the function"

---

## The Migration Cascade

When migrating, think in terms of **dependency chains**:

```
A calls B calls C

If C changes, B gets an error.
If B has an error, A might also fail.

Solution: Migrate the ENTIRE CHAIN to C++, not just patch errors.
```

### Example:

```
WBP_HUD → calls → AC_StatManager.GetStat()
                          ↓
                   returns B_Stat object
                          ↓
               WBP_HUD reads B_Stat.CurrentValue
```

**If you migrate AC_StatManager but leave WBP_HUD in Blueprint:**
- WBP_HUD will have pin errors trying to access B_Stat

**Solution:**
1. Migrate AC_StatManager to C++ ✓
2. Migrate B_Stat to C++ ✓
3. Migrate WBP_HUD logic to C++ ✓
4. NOW everything compiles

---

## Rigorous 20-Pass Validation

### Each Pass Must Actually COMPARE to JSON

**Pass 1: Items Extracted**
```
Read JSON.Variables.List
For EACH variable in JSON:
  [ ] Name matches exactly
  [ ] Listed in tracker
Count: JSON has N variables, tracker has N variables
```

**Pass 4: Logic Traced**
```
Read JSON.Graphs[FunctionName].Nodes
For EACH node:
  [ ] Identify node type (K2Node_CallFunction, K2Node_IfThenElse, etc.)
  [ ] Document input pins and their sources
  [ ] Document output pins and their targets
  [ ] Document execution flow (which pin leads where)

Write out the COMPLETE flow:
1. Entry → Node A (input: X from variable Y)
2. Node A → Branch (condition: Z)
3. Branch TRUE → Node B
4. Branch FALSE → Node C
5. etc.
```

**Pass 12: Branches Verified**
```
For EACH K2Node_IfThenElse in JSON:
  [ ] What variable/expression is the condition?
  [ ] What happens on TRUE exec pin?
  [ ] What happens on FALSE exec pin?

Compare to C++ implementation:
  [ ] Same condition checked?
  [ ] TRUE path does same thing?
  [ ] FALSE path does same thing?

SIGN OFF: "I verified Branch at line X matches JSON node Y"
```

---

## Animation Blueprint Special Handling

### AnimBP Strategy: Custom K2 Anim Graph Node

1. **EventGraph logic → C++ via NativeUpdateAnimation**
2. **AnimGraph stays in Blueprint** (state machines, blendspaces)
3. **Custom logic → Custom FAnimNode that calls C++ functions**

### What to Preserve in AnimBP:
- State machine structure
- Blend space references
- Montage slot assignments
- Linked anim layers
- Preview settings

### What to Migrate:
- EventGraph variable updates
- Custom calculations (direction, speed, etc.)
- Event handling logic

---

## Validation Checklist Before Each Migration

```markdown
## Pre-Migration Checklist for [BlueprintName]

### State Capture
- [ ] Captured parent class
- [ ] Captured all variables with defaults
- [ ] Captured all components with properties
- [ ] Captured animation settings (if applicable)
- [ ] Captured data asset references
- [ ] Captured event dispatcher bindings
- [ ] Saved state to JSON file

### Dependency Analysis
- [ ] Listed all callers of this Blueprint
- [ ] Listed all Blueprints this calls
- [ ] Identified migration order for chain

### Ready to Migrate
- [ ] C++ base class written
- [ ] All functions implemented (no stubs)
- [ ] All variables declared
- [ ] Logged state capture file path
```

## Validation Checklist After Each Migration

```markdown
## Post-Migration Checklist for [BlueprintName]

### State Verification
- [ ] Compared parent class (correct C++ base)
- [ ] Compared all variables (none lost)
- [ ] Compared all components (settings preserved)
- [ ] Compared animation settings (unchanged)
- [ ] Compared data asset references (all present)
- [ ] Compared event dispatcher bindings (preserved)

### Functionality Test
- [ ] C++ compiles
- [ ] Blueprint callers compile
- [ ] PIE test passes
- [ ] No runtime errors in log

### Sign-Off
- [ ] State diff shows only expected changes
- [ ] All unexpected differences investigated
- [ ] Documented any intentional changes
```

---

## Summary

1. **Capture state BEFORE migration** - Everything that could be affected
2. **Validate AFTER migration** - Nothing should be lost except logic nodes
3. **Pin errors = migration needed** - Not pin fixes
4. **Node errors = dependency migration needed** - Trace the chain
5. **20 passes are THOROUGH** - Actually compare to JSON, don't rubber stamp
6. **AnimBP strategy** - Custom K2 nodes call C++ functions
