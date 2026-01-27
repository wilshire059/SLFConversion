# IK Weight / Finger IK Investigation Report

## Summary

After a thorough investigation comparing SLFConversion (current) with bp_only (working original), here are the findings:

## Key Findings

### 1. Two Separate IK Systems in AnimBP

The AnimBP `ABP_SoulslikeCharacter_Additive` has TWO different IK nodes:

| Node | Type | Alpha Source | Purpose |
|------|------|--------------|---------|
| **Control Rig** | AnimGraphNode_ControlRig | Alpha = 1.0 (hardcoded, not connected) | Hand/Foot IK positioning |
| **IK Rig** | AnimGraphNode_IKRig | Alpha = `IkWeight` variable | Hit reaction animation |

### 2. IkWeight Variable Purpose

The `IkWeight` variable in C++ (`AC_CombatManager::IKWeight`) is for **hit reaction animation**, NOT finger gripping:
- Initialized to 0.0 in constructor
- Set to 1.0 when hit reaction occurs
- Decays back to 0 over time in TickComponent

```cpp
// AC_CombatManager.cpp
IKWeight = 0.0;  // Constructor
IKWeight = 1.0;  // When hit
IKWeight = FMath::Max(0.0, IKWeight - (DeltaTime * IKDecayRate));  // Decay
```

### 3. Control Rig Configuration

The Control Rig handles hand/foot IK but **does NOT handle finger gripping**:
- Has `hand_l`, `hand_r`, `foot_l`, `foot_r` bones/controls
- No finger-specific controls found
- `ShouldDoIKTrace` input connected to `NOT bIsFalling` (does IK when not falling)

### 4. No Finger IK System Found

After searching all exports:
- No finger-specific IK controls in Control Rig
- Hand pose animations exist (`AS_Manny_hand_r_pose`, etc.) but aren't referenced in AnimBP
- No finger gripping logic found anywhere

### 5. Asset Comparison Results

| Asset | SLFConversion | bp_only | Difference |
|-------|---------------|---------|------------|
| AnimBP | 1,369,625 bytes | 1,347,273 bytes | +22KB (C++ reparenting overhead) |
| CR_SoulslikeFramework | 4,939,151 bytes | 4,939,151 bytes | **Identical** |
| IK_HitReact | 82,053 bytes | 82,053 bytes | **Identical** |
| IK_Mannequin | 142,398 bytes | 142,398 bytes | **Identical** |

### 6. C++ Property Mapping Working Correctly

The C++ AnimBP parent class properties are exposed correctly:
- `ik_weight` - Available (snake_case in Python)
- `is_falling`, `is_blocking`, etc. - Available
- `left_hand_overlay_state`, `right_hand_overlay_state` - Available

## CRITICAL FINDING: Blend Poses Enum Not Connected!

The `Blend Poses (E_OverlayState)` node in the AnimGraph has its `ActiveEnumValue` pin **NOT CONNECTED**:

```json
{
  "Name": "ActiveEnumValue",
  "Direction": "Input",
  "DefaultValue": "0",
  "LinkedTo": []  // <-- NOT CONNECTED!
}
```

This means:
- The Blend Poses node always uses default value 0 (Unarmed)
- Overlay state animations (LL_OneHanded_Right, LL_TwoHanded_Right, etc.) are NEVER selected
- Finger poses baked into weapon hold animations are never applied

The `LeftHandOverlayState` and `RightHandOverlayState` variables ARE being set correctly by C++ NativeUpdateAnimation(), but they are NOT being read by the AnimGraph's Blend Poses nodes.

## Root Cause

The AnimGraph's Property Access nodes may have broken during C++ reparenting:
- Blueprint variables had names like `LeftHandOverlayState`
- AnimGraph nodes read these via Property Access
- When reparented to C++, the Property Access binding may have broken

## Conclusions

### The Issue is NOT in IkWeight

The `IkWeight` variable and IK Rig node are for **hit reactions only**, not finger gripping. The Control Rig (for hand/foot IK) has Alpha=1.0 always and should work correctly.

### The Real Issue: Broken Property Access Binding

The AnimGraph's Blend Poses (E_OverlayState) nodes are not receiving the overlay state values from the C++ properties. This causes:
1. Always selecting default pose (index 0 = Unarmed)
2. Weapon hold animations never applied
3. Finger poses (baked into weapon animations) never shown

### Possible Causes for Missing Finger Wrapping

1. **Property Access binding broken** - Most likely cause
2. **Finger wrapping may be baked into overlay animations** - Not procedural IK
3. **Hand pose additive animations not being blended** - AS_*_hand_pose files exist but may not be connected
4. **Weapon attachment issue** - Weapon may not be attached to correct socket for hand to wrap around

## Recommended Fix

### Option 1: Re-wire AnimGraph Blend Poses nodes (Manual)

The Blend Poses (E_OverlayState) nodes need their `ActiveEnumValue` pins connected to the overlay state variables:

1. Open ABP_SoulslikeCharacter_Additive in UE Editor
2. Find the two Blend Poses (E_OverlayState) nodes in AnimGraph
3. For the LEFT arm blend node: Connect `LeftHandOverlayState` to `ActiveEnumValue`
4. For the RIGHT arm blend node: Connect `RightHandOverlayState` to `ActiveEnumValue`
5. Compile and save

### Option 2: Use Python automation script

Create a script to fix the Property Access bindings programmatically using unreal.BlueprintEditorLibrary.

## Next Steps to Investigate

1. **Verify the blend nodes need re-wiring** - Open AnimBP in editor and check
2. **Check if Property Access nodes exist but are broken** - May just need reconnection
3. **Check overlay state animations** - Do they include proper finger poses?
4. **Compare runtime behavior** - Add logging to confirm AnimBP variables are updating
5. **Check if this is an animation asset issue** - Not a code/IK system issue

## Files Examined

- `/Source/SLFConversion/Animation/ABP_SoulslikeCharacter_Additive.h/cpp`
- `/Source/SLFConversion/Components/AC_CombatManager.h/cpp`
- `/Exports/BlueprintDNA_v2/AnimBlueprint/ABP_SoulslikeCharacter_Additive.json`
- `/Exports/BlueprintDNA_v2/ControlRig/CR_SoulslikeFramework.json`
- `/Exports/BlueprintDNA_v2/IKRigDefinition/IK_HitReact.json`
