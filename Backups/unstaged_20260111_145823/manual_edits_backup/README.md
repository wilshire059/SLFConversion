# Manual Edits Backup

This folder contains backups of assets that required **manual editor changes** and should NOT be overwritten by restores from `C:\scripts\bp_only`.

## Protected Assets

| Asset | Date | Description |
|-------|------|-------------|
| ABP_SoulslikeCharacter_Additive.uasset | 2026-01-10 | AnimBP reparented to C++, EventGraph cleared, AnimGraph manually fixed (transitions, blend poses, animation layers) |

## Why This Exists

The standard migration workflow restores from `bp_only` backup, which would overwrite manual AnimGraph fixes:
- State machine transition conditions (Speed > 10, bIsFalling, IsResting)
- Blend Poses (E_OverlayState) ActiveEnumValue bindings
- Animation Layer input→output connections (LL_Shield_Block_Left/Right, LL_Shield_Right)
- Variable bindings (bIsBlocking, bIsFalling, LeftHandOverlayState, RightHandOverlayState)

## Restore Instructions

If the AnimBP gets overwritten, copy back from here:
```powershell
Copy-Item "C:\scripts\slfconversion\manual_edits_backup\ABP_SoulslikeCharacter_Additive.uasset" "C:\scripts\slfconversion\Content\SoulslikeFramework\Demo\_Animations\Locomotion\AnimBP\" -Force
```

## Important

- NEVER delete this folder
- NEVER restore from bp_only over these files
- Update this README when adding new manually-edited assets
