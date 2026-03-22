# E_ValueType → ESLFValueType Migration Checklist

Generated automatically from JSON exports.
Migrate in the order shown (top to bottom).

## Migration Order

| Priority | Asset | Functions | Variables | Pins |
|----------|-------|-----------|-----------|------|
| 0 | FLevelChangeData | 0 | 1 | 0 |
| 0 | FStatChange | 0 | 1 | 0 |
| 0 | FStatChangePercent | 0 | 1 | 0 |
| 1 | B_Stat | 3 | 0 | 18 |
| 2 | AC_StatManager | 3 | 0 | 20 |
| 3 | AC_AI_Boss | 0 | 0 | 1 |
| 3 | AC_AI_CombatManager | 0 | 0 | 6 |
| 3 | AC_ActionManager | 0 | 0 | 1 |
| 3 | AC_CombatManager | 0 | 0 | 12 |
| 3 | AC_EquipmentManager | 0 | 0 | 1 |
| 4 | B_Action | 0 | 0 | 2 |
| 4 | B_Action_Dodge | 0 | 0 | 2 |
| 4 | B_Action_DrinkFlask_HP | 0 | 0 | 4 |
| 4 | B_Buff_AttackPower | 0 | 0 | 2 |
| 4 | B_StatusEffect | 0 | 0 | 6 |
| 4 | B_StatusEffect_Frostbite | 0 | 0 | 2 |
| 4 | B_StatusEffect_Plague | 0 | 0 | 2 |
| 5 | AN_AdjustStat | 0 | 1 | 2 |
| 6 | W_Boss_Healthbar | 0 | 0 | 1 |
| 6 | W_HUD | 0 | 0 | 7 |
| 6 | W_LevelUp | 0 | 0 | 18 |
| 6 | W_StatEntry | 0 | 0 | 1 |
| 6 | W_StatEntry_LevelUp | 0 | 0 | 1 |
| 6 | W_StatEntry_Status | 0 | 0 | 1 |
| 7 | B_ShowcaseEnemy_StatDisplay | 0 | 0 | 1 |

---

## FLevelChangeData

**Path:** `/Game/.../FLevelChangeData`
**Type:** Struct

### Variables to Change

| Variable Name | Category |
|---------------|----------|
| ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | Struct Member (UserDefinedStruct member - may need struct migration) |

**Instructions:**
1. Select the variable in My Blueprint panel
2. In Details panel, change Variable Type from `E_ValueType_DEPRECATED` → `ESLFValueType`

---

## FStatChange

**Path:** `/Game/.../FStatChange`
**Type:** Struct

### Variables to Change

| Variable Name | Category |
|---------------|----------|
| ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | Struct Member (UserDefinedStruct member - may need struct migration) |

**Instructions:**
1. Select the variable in My Blueprint panel
2. In Details panel, change Variable Type from `E_ValueType_DEPRECATED` → `ESLFValueType`

---

## FStatChangePercent

**Path:** `/Game/.../FStatChangePercent`
**Type:** Struct

### Variables to Change

| Variable Name | Category |
|---------------|----------|
| ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | Struct Member (UserDefinedStruct member - may need struct migration) |

**Instructions:**
1. Select the variable in My Blueprint panel
2. In Details panel, change Variable Type from `E_ValueType_DEPRECATED` → `ESLFValueType`

---

## B_Stat

**Path:** `/Game/.../B_Stat`
**Type:** Blueprint

### Function Parameters to Change

| Function | Parameter | Direction |
|----------|-----------|-----------|
| AdjustValue | ValueType | Input |
| AdjustAffectedValue | ValueType | Input |

**Instructions:**
1. Open the function in the Blueprint editor
2. Select the function parameter
3. In Details panel, change type from `E_ValueType_DEPRECATED` → `ESLFValueType`

### Event Dispatcher Parameters to Change

| Dispatcher | Parameter |
|------------|-----------|
| OnStatUpdated | ValueType |

**Instructions:**
1. Find the Event Dispatcher in the My Blueprint panel
2. Click to edit its signature
3. Change the parameter type from `E_ValueType_DEPRECATED` → `ESLFValueType`

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Call On Stat Updated | ValueType | ← |
| Call On Stat Updated | ValueType | ← |

**Graph: AdjustValue**

| Node | Pin | Direction |
|------|-----|-----------|
| AdjustValue | ValueType | → |
| Switch on E_ValueType | Selection | ← |
| Call On Stat Updated | ValueType | ← |
| Call On Stat Updated | ValueType | ← |
| Get ValueType | ValueType | → |
| Get ValueType | ValueType | → |

**Graph: AdjustAffectedValue**

| Node | Pin | Direction |
|------|-----|-----------|
| AdjustAffectedValue | ValueType | → |
| Switch on E_ValueType | Selection | ← |
| Call On Stat Updated | ValueType | ← |
| Call On Stat Updated | ValueType | ← |
| Get ValueType | ValueType | → |
| Get ValueType | ValueType | → |

**Graph: UpdateStatInfo**

| Node | Pin | Direction |
|------|-----|-----------|
| Call On Stat Updated | ValueType | ← |
| Call On Stat Updated | ValueType | ← |

**Graph: InitializeBaseClassValue**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Value
Target is Stat/Attribute Ob | ValueType | ← |

**Graph: OnStatUpdated**

| Node | Pin | Direction |
|------|-----|-----------|
| OnStatUpdated | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Call On Stat Updated | ValueType | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator1 | MaxValue |
| AdjustValue | ValueType | NewEnumerator0 | CurrentValue |
| Switch on E_ValueType | Selection | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator0 | CurrentValue |
| Get ValueType | ValueType | NewEnumerator0 | CurrentValue |
| Get ValueType | ValueType | NewEnumerator0 | CurrentValue |
| AdjustAffectedValue | ValueType | NewEnumerator0 | CurrentValue |
| Switch on E_ValueType | Selection | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator0 | CurrentValue |
| Get ValueType | ValueType | NewEnumerator0 | CurrentValue |
| Get ValueType | ValueType | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator0 | CurrentValue |
| Call On Stat Updated | ValueType | NewEnumerator1 | MaxValue |
| Adjust Value
Target is Stat/At | ValueType | NewEnumerator0 | CurrentValue |
| OnStatUpdated | ValueType | NewEnumerator0 | CurrentValue |

---

## AC_StatManager

**Path:** `/Game/.../AC_StatManager`
**Type:** Component

### Function Parameters to Change

| Function | Parameter | Direction |
|----------|-----------|-----------|
| AdjustStat | ValueType | Input |
| AdjustAffected | ValueType | Input |
| AdjustAffectedStats | ValueType | Input |

**Instructions:**
1. Open the function in the Blueprint editor
2. Select the function parameter
3. In Details panel, change type from `E_ValueType_DEPRECATED` → `ESLFValueType`

### Pins to Reconnect

**Graph: EventGraph > ON STAT UPDATE / ON LEVEL UPDATE**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Affected Stats
Target is AC Stat  | ValueType | ← |
| Event OnStatUpdated
Custom Event | ValueType | → |

**Graph: ResetStat**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Value
Target is Stat/Attribute Ob | ValueType | ← |

**Graph: AdjustStat**

| Node | Pin | Direction |
|------|-----|-----------|
| AdjustStat | ValueType | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Adjust Value
Target is Stat/Attribute Ob | ValueType | ← |

**Graph: AdjustAffected**

| Node | Pin | Direction |
|------|-----|-----------|
| AdjustAffected | ValueType | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Adjust Affected Value
Target is Stat/Att | ValueType | ← |

**Graph: AdjustAffectedStats**

| Node | Pin | Direction |
|------|-----|-----------|
| AdjustAffectedStats | ValueType | → |
| Adjust Affected
Target is AC Stat Manage | ValueType | ← |
| Adjust Affected
Target is AC Stat Manage | ValueType | ← |
| Adjust Affected
Target is AC Stat Manage | ValueType | ← |
| Adjust Affected
Target is AC Stat Manage | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Affected Stats
Target i | ValueType | NewEnumerator0 | CurrentValue |
| Event OnStatUpdated
Custom Eve | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Value
Target is Stat/At | ValueType | NewEnumerator0 | CurrentValue |
| AdjustStat | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Value
Target is Stat/At | ValueType | NewEnumerator0 | CurrentValue |
| AdjustAffected | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Affected Value
Target i | ValueType | NewEnumerator0 | CurrentValue |
| AdjustAffectedStats | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Affected
Target is AC S | ValueType | NewEnumerator1 | MaxValue |
| Adjust Affected
Target is AC S | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Affected
Target is AC S | ValueType | NewEnumerator1 | MaxValue |
| Adjust Affected
Target is AC S | ValueType | NewEnumerator0 | CurrentValue |

---

## AC_AI_Boss

**Path:** `/Game/.../AC_AI_Boss`
**Type:** Component

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnStatUpdated
Custom Event | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnStatUpdated
Custom Eve | ValueType | NewEnumerator0 | CurrentValue |

---

## AC_AI_CombatManager

**Path:** `/Game/.../AC_AI_CombatManager`
**Type:** Component

### Pins to Reconnect

**Graph: EventGraph > INCOMING DAMAGE & POISE HANDLING**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnEnemyStatUpdated
Custom Event | ValueType | → |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Graph: HandleIncomingWeaponDamage_AI**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnEnemyStatUpdated
Custo | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## AC_ActionManager

**Path:** `/Game/.../AC_ActionManager`
**Type:** Component

### Pins to Reconnect

**Graph: EventGraph > SPRINTING RELATED**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## AC_CombatManager

**Path:** `/Game/.../AC_CombatManager`
**Type:** Component

### Pins to Reconnect

**Graph: EventGraph > ON ANY DAMAGE**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Event OnHealthChanged
Custom Event | ValueType | → |

**Graph: EventGraph > POISE BREAK**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnPoiseChanged
Custom Event | ValueType | → |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Graph: HandleIncomingWeaponDamage**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Event OnHealthChanged
Custom E | ValueType | NewEnumerator0 | CurrentValue |
| Event OnPoiseChanged
Custom Ev | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## AC_EquipmentManager

**Path:** `/Game/.../AC_EquipmentManager`
**Type:** Component

### Pins to Reconnect

**Graph: ApplyStatChanges**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_Action

**Path:** `/Game/.../B_Action`
**Type:** Blueprint

### Pins to Reconnect

**Graph: AdjustStatByRequirement**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_Action_Dodge

**Path:** `/Game/.../B_Action_Dodge`
**Type:** Blueprint

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_Action_DrinkFlask_HP

**Path:** `/Game/.../B_Action_DrinkFlask_HP`
**Type:** Blueprint

### Pins to Reconnect

**Graph: NewEventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Break FStat Change Percent | ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_Buff_AttackPower

**Path:** `/Game/.../B_Buff_AttackPower`
**Type:** Blueprint

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_StatusEffect

**Path:** `/Game/.../B_StatusEffect`
**Type:** Blueprint

### Pins to Reconnect

**Graph: EventGraph > TRIGGER LOGIC**

| Node | Pin | Direction |
|------|-----|-----------|
| Break FStat Change | ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | → |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Break FStat Change | ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | → |
| Break FStat Change | ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | → |
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_StatusEffect_Frostbite

**Path:** `/Game/.../B_StatusEffect_Frostbite`
**Type:** Blueprint

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Break FStat Change | ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## B_StatusEffect_Plague

**Path:** `/Game/.../B_StatusEffect_Plague`
**Type:** Blueprint

### Pins to Reconnect

**Graph: NewEventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Break FStat Change | ValueType_9_C8A7CE854087AD23DB2A9B9060253F51 | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |

---

## AN_AdjustStat

**Path:** `/Game/.../AN_AdjustStat`
**Type:** AnimNotify

### Variables to Change

| Variable Name | Category |
|---------------|----------|
| Value Type | Default |

**Instructions:**
1. Select the variable in My Blueprint panel
2. In Details panel, change Variable Type from `E_ValueType_DEPRECATED` → `ESLFValueType`

### Pins to Reconnect

**Graph: Received_Notify**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Stat
Target is AC Stat Manager | ValueType | ← |
| Get Value Type | Value Type | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Adjust Stat
Target is AC Stat  | ValueType | NewEnumerator0 | CurrentValue |
| Get Value Type | Value Type | NewEnumerator0 | CurrentValue |

---

## W_Boss_Healthbar

**Path:** `/Game/.../W_Boss_Healthbar`
**Type:** Widget

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnBossHealthUpdated
Custom Event | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnBossHealthUpdated
Cust | ValueType | NewEnumerator0 | CurrentValue |

---

## W_HUD

**Path:** `/Game/.../W_HUD`
**Type:** Widget

### Pins to Reconnect

**Graph: EventGraph > LISTENED EVENTS**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnStatUpdated
Custom Event | ValueType | → |
| Equal (Enum) | A | ← |
| Equal (Enum) | B | ← |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnStatUpdated
Custom Eve | ValueType | NewEnumerator0 | CurrentValue |
| Equal (Enum) | B | NewEnumerator1 | MaxValue |

---

## W_LevelUp

**Path:** `/Game/.../W_LevelUp`
**Type:** Widget

### Pins to Reconnect

**Graph: EventGraph > ENTRY RELATED**

| Node | Pin | Direction |
|------|-----|-----------|
| Make FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | ← |
| Select | Option 0 | ← |
| Select | Option 1 | ← |
| Select | ReturnValue | → |

**Graph: EventGraph > APPLY/DISREGARD CHANGES**

| Node | Pin | Direction |
|------|-----|-----------|
| Adjust Value
Target is Stat/Attribute Ob | ValueType | ← |
| Break FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Reroute Node | InputPin | ← |
| Reroute Node | OutputPin | → |
| Adjust Value
Target is Stat/Attribute Ob | ValueType | ← |
| Break FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | → |

**Graph: HandleStatChanges**

| Node | Pin | Direction |
|------|-----|-----------|
| Select | Option 0 | ← |
| Select | Option 1 | ← |
| Select | ReturnValue | → |
| Adjust Value
Target is Stat/Attribute Ob | ValueType | ← |
| Make FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | ← |
| Break FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Make FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | NewEnumerator0 | CurrentValue |
| Select | Option 0 | NewEnumerator0 | CurrentValue |
| Select | Option 1 | NewEnumerator1 | MaxValue |
| Adjust Value
Target is Stat/At | ValueType | NewEnumerator0 | CurrentValue |
| Adjust Value
Target is Stat/At | ValueType | NewEnumerator0 | CurrentValue |
| Select | Option 0 | NewEnumerator0 | CurrentValue |
| Select | Option 1 | NewEnumerator1 | MaxValue |
| Adjust Value
Target is Stat/At | ValueType | NewEnumerator0 | CurrentValue |
| Make FLevel Change Data | ValueType_6_DD5AF4FE43302E61E614B9B7135A155B | NewEnumerator0 | CurrentValue |

---

## W_StatEntry

**Path:** `/Game/.../W_StatEntry`
**Type:** Widget

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnStatUpdated
Custom Event | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnStatUpdated
Custom Eve | ValueType | NewEnumerator0 | CurrentValue |

---

## W_StatEntry_LevelUp

**Path:** `/Game/.../W_StatEntry_LevelUp`
**Type:** Widget

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnStatUpdated
Custom Event | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnStatUpdated
Custom Eve | ValueType | NewEnumerator0 | CurrentValue |

---

## W_StatEntry_Status

**Path:** `/Game/.../W_StatEntry_Status`
**Type:** Widget

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| Event OnStatUpdated
Custom Event | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| Event OnStatUpdated
Custom Eve | ValueType | NewEnumerator0 | CurrentValue |

---

## B_ShowcaseEnemy_StatDisplay

**Path:** `/Game/.../B_ShowcaseEnemy_StatDisplay`
**Type:** Blueprint

### Pins to Reconnect

**Graph: EventGraph**

| Node | Pin | Direction |
|------|-----|-----------|
| CustomEvent
Custom Event | ValueType | → |

**Instructions:**
1. After changing function signatures, pins will turn red
2. Delete the broken connection
3. Reconnect - the types should now match

### Pin Default Values to Update

| Node | Pin | Old Value | New Value |
|------|-----|-----------|-----------|
| CustomEvent
Custom Event | ValueType | NewEnumerator0 | CurrentValue |

---
