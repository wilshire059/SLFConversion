# TAE Format Quick Reference Guide

## What is TAE?

**TAE (TimeAct Event)** files define frame-precise animation events in FromSoftware games:
- When invulnerability frames (I-frames) activate
- When attack hitboxes spawn
- When sounds and visual effects play
- When hyper armor (poise) is active
- And 100+ other animation events

## File Structure Overview

```
TAE File (.tae)
├── Header (208 bytes)
│   ├── Magic: "TAE "
│   ├── Version: 0x1000C (DS3/BB/Sekiro/ER)
│   ├── ID: TAE file identifier
│   ├── Skeleton Name: .hkt file reference
│   └── Flags: Unknown purpose
├── Animations (list)
│   └── For each animation:
│       ├── Animation ID
│       ├── Events (list)
│       │   └── For each event:
│       │       ├── Start Time (float, seconds)
│       │       ├── End Time (float, seconds)
│       │       ├── Event Type (enum, 108+ types)
│       │       └── Parameters (varies by type)
│       └── Event Groups (optional)
└── Time Table (shared float values)
```

## Critical Event Types for Combat

| ID | Name | Purpose | Use Case |
|----|------|---------|----------|
| **0** | JumpTable | General flags | I-frames, hyper armor, cancels |
| **1** | AttackBehavior | Hitbox activation | Damage dealing |
| **795** | Toughness | Poise control | Hyper armor strength |

### JumpTable Flags (Event Type 0)

Most important flags:

| Flag ID | Name | Purpose |
|---------|------|---------|
| **8** | Invincible | Standard I-frames (dodges, etc.) |
| **24** | HyperArmor | Poise active |
| **73** | ParryPossibleState | Parry window |
| **94** | PerfectInvincibility | Cannot be damaged at all |

## Event Time Format

**Times are stored as floats in SECONDS, not frames.**

To convert to frames (30 FPS):
```python
frame_number = round(time_in_seconds * 30)
```

Example:
- `start_time = 0.133333` → Frame 4
- `end_time = 0.400000` → Frame 12
- Duration: 8 frames

## Reading TAE Files in Python

### Basic Parser

```python
import struct

class SimpleTAEParser:
    def __init__(self, file_path):
        with open(file_path, 'rb') as f:
            # Check magic
            magic = f.read(4)
            assert magic == b'TAE '

            # Skip to animation count
            f.seek(0x50)
            self.tae_id = struct.unpack('<I', f.read(4))[0]
            anim_count = struct.unpack('<I', f.read(4))[0]
            anims_offset = struct.unpack('<Q', f.read(8))[0]

            # Read animations
            self.animations = self.read_animations(f, anims_offset, anim_count)
```

### Find I-Frames

```python
def find_iframes(tae_file, animation_id):
    """Returns list of (start_frame, end_frame) tuples"""
    parser = TAE3Parser(tae_file)
    animations = parser.parse()

    anim = animations[animation_id]
    iframes = []

    for event in anim.events:
        if event.event_type == EventType.JumpTable:
            # Flag 8 = Invincible, 94 = Perfect Invincibility
            if event.flag_type in [8, 94]:
                start_frame = round(event.start_time * 30)
                end_frame = round(event.end_time * 30)
                iframes.append((start_frame, end_frame))

    return iframes
```

## Common Event Parameters

### AttackBehavior (Type 1)
```c#
struct AttackBehavior {
    int32 AttackType;       // 0=Standard, 62=Plunging, 64=Parry
    int32 AttackIndex;      // Which attack (0-7)
    int32 BehaviorJudgeID;  // References attack params
    byte DirectionType;     // Attack direction
    int16 StateInfo;        // Unknown
}
```

### Toughness (Type 795)
```c#
struct Toughness {
    byte ToughnessParamID;      // Poise parameter ID
    bool IsToughnessEffective;  // Enable/disable
    float ToughnessRate;        // Damage resistance multiplier
}
```

### PlayFFX (Type 96)
```c#
struct PlayFFX {
    int32 FFXID;            // Visual effect ID
    int32 DummyPolyID;      // Attachment point
    int32 SlotID;           // Effect slot
    sbyte State0;           // Spawn condition
    sbyte State1;           // Follow dummy poly
    sbyte GhostCondition;   // Ghost-specific
}
```

### PlaySound (Type 128-132)
```c#
struct PlaySound {
    int32 SoundType;  // 0=Env, 1=Char, 5=SFX, 7=Voice
    int32 SoundID;    // Sound to play
}
```

## Binary Format Details

### Event Header (24 bytes)
```
Offset | Size | Type  | Description
-------|------|-------|------------------
0x00   | 8    | int64 | Start time offset (pointer)
0x08   | 8    | int64 | End time offset (pointer)
0x10   | 8    | int64 | Event data offset (pointer)
```

### Event Data (16+ bytes, aligned)
```
Offset | Size | Type   | Description
-------|------|--------|------------------
0x00   | 8    | uint64 | Event type enum
0x08   | 8    | int64  | Self-pointer (always offset+8)
0x10   | N    | varies | Event parameters (16-byte aligned)
```

## Parsing Workflow

1. **Read TAE header** → Get animation count and offset
2. **Read animation list** → Get each animation's data offset
3. **For each animation:**
   - Read time table (shared float values)
   - Read event headers (24 bytes each)
   - For each event:
     - Resolve start/end time from offsets
     - Read event type and parameters
4. **Parse event parameters** based on type

## Integration with Unreal Engine

### Export to JSON

```python
def export_to_unreal(tae_file, anim_id, output_json):
    """Export TAE events to JSON for Unreal AnimNotify creation"""
    import json

    parser = TAE3Parser(tae_file)
    anim = parser.parse()[anim_id]

    notifies = []
    for event in anim.events:
        notify = {
            'time': event.start_time,
            'duration': event.end_time - event.start_time,
            'type': event.event_type.name,
            'params': {}
        }

        # Add type-specific data
        if isinstance(event, JumpTableEvent):
            notify['params']['flag'] = event.flag_type
        elif isinstance(event, AttackBehaviorEvent):
            notify['params']['attack_index'] = event.attack_index

        notifies.append(notify)

    with open(output_json, 'w') as f:
        json.dump({'notifies': notifies}, f, indent=2)
```

### Create Unreal AnimNotify Classes

```cpp
// Example: Invulnerability frame notify
UCLASS()
class UAN_InvulnerabilityFrame : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
                            UAnimSequenceBase* Animation,
                            float TotalDuration) override
    {
        // Enable I-frames
        if (ASLFCharacter* Character = Cast<ASLFCharacter>(MeshComp->GetOwner()))
        {
            Character->SetInvulnerable(true);
        }
    }

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,
                          UAnimSequenceBase* Animation) override
    {
        // Disable I-frames
        if (ASLFCharacter* Character = Cast<ASLFCharacter>(MeshComp->GetOwner()))
        {
            Character->SetInvulnerable(false);
        }
    }
};
```

## Most Useful Events for Soulslike Games

| Event Type | ID | What It Does | How to Use |
|------------|----|--------------|--------------|
| JumpTable (Invincible) | 0 (flag 8) | I-frames | Dodge rolls, backsteps |
| JumpTable (HyperArmor) | 0 (flag 24) | Poise | Heavy attack wind-up |
| JumpTable (ParryPossible) | 0 (flag 73) | Parry window | Shield parry animations |
| AttackBehavior | 1 | Hitbox activation | All attacks |
| Toughness | 795 | Poise multiplier | Boss attacks, heavy weapons |
| PlayFFX | 96 | VFX spawn | Weapon trails, spell effects |
| PlaySound | 128 | Audio | Sword swoosh, footsteps |
| DisableStaminaRegen | 225 | Block stamina | During attacks/blocks |

## Event Type Categories

### Combat (20+ events)
- 0: JumpTable (flags)
- 1: AttackBehavior
- 2: BulletBehavior
- 5: CommonBehavior
- 66-67: CreateSpEffect
- 795: Toughness

### Audio (5+ events)
- 128-132: PlaySound variants

### Visual (10+ events)
- 96: PlayFFX
- 112: HitEffect
- 138: CreateDecal
- 710-713: Model visibility

### Movement (10+ events)
- 760: BoostRootMotion
- 770: BonePos
- 781: TurnLowerBody

## Resources

- **Full Analysis**: `TAE_FORMAT_ANALYSIS.md`
- **Python Parser**: `tae_parser_outline.py`
- **Source Code**: https://github.com/Meowmaritus/DSAnimStudio
- **Format Library**: SoulsFormats (C#)

## Quick Start Checklist

- [ ] Clone DSAnimStudio repo
- [ ] Extract sample .tae file from game
- [ ] Parse with Python script
- [ ] Export I-frames and hitboxes to JSON
- [ ] Create Unreal AnimNotify classes
- [ ] Import JSON and apply notifies
- [ ] Test in Soulslike Framework

## Next Steps

1. **Test Parser**: Run on actual .tae files from Dark Souls 3
2. **Validate Events**: Confirm I-frames/hitboxes match in-game behavior
3. **Extend Coverage**: Add remaining 88 event types
4. **Unreal Pipeline**: Build automated import tool
5. **Integration**: Map events to SLFConversion components

---

**Note:** TAE files are often inside .anibnd.dcx archives. You'll need to:
1. Decompress .dcx wrapper (zlib)
2. Extract .anibnd container (BND format)
3. Parse individual .tae files

See DSAnimStudio's BND/DCX parsers for reference.
