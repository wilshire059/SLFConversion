# TAE (TimeAct Event) Format Analysis
## Analysis of DSAnimStudio TAE Parser

**Date:** 2026-02-08
**Source:** https://github.com/Meowmaritus/DSAnimStudio.git
**Key Library:** SoulsFormats (C# parsing library for FromSoftware game formats)

---

## Executive Summary

TAE (TimeAct Event) files are the binary format used by FromSoftware games (Dark Souls, Bloodborne, Elden Ring, etc.) to control frame-precise animation events. These files define when invulnerability frames activate, when hitboxes spawn, when VFX play, when sounds trigger, and much more during character animations.

**Key Findings:**
- Binary format with clear structure and 108+ distinct event types
- Events are frame-timed with start/end times (stored as floats in seconds)
- Each event type has specific parameters (ints, floats, bytes, bools)
- Format is highly parseable and could be replicated in Python
- Used across all modern FromSoftware games with minor variations

---

## 1. Binary Format Structure

### File Header (TAE3 format)

```
Offset | Size | Type   | Name              | Description
-------|------|--------|-------------------|---------------------------
0x00   | 4    | ASCII  | Magic             | "TAE " (with space)
0x04   | 4    | bytes  | Version Header    | 0x00, 0x00, 0x00, 0xFF
0x08   | 4    | int32  | Version           | 0x1000C (for DS3/BB/Sekiro)
0x0C   | 4    | int32  | File Size         | Total file size in bytes
0x10   | 8    | int64  | Header Offset 1   | 0x40
0x18   | 8    | int64  | Unknown           | Always 1
0x20   | 8    | int64  | Header Offset 2   | 0x50
0x28   | 8    | int64  | Anim Table Offset | 0x80
0x30   | 8    | int64  | Unk30             | Varies by TAE type
0x38   | 8    | int64  | Unknown           | Always 0
0x40   | 8    | bytes  | Flags             | Unknown flags
0x48   | 8    | int64  | Unknown           | Always 1
0x50   | 4    | int32  | ID                | TAE file ID number
0x54   | 4    | int32  | Animation Count   | Number of animations
0x58   | 8    | int64  | Animations Offset | Pointer to animation list
0x60   | 8    | int64  | Anim Groups Offset| Pointer to groups
0x68   | 8    | int64  | Unknown           | 0xA0
0x70   | 8    | int64  | Animation Count 2 | Same as offset 0x54
0x78   | 8    | int64  | First Anim Offset | Pointer to first animation data
0x80   | 8    | int64  | Unknown           | Always 1
0x88   | 8    | int64  | Unknown           | 0x90
0x90   | 4    | int32  | ID Repeat 1       | Same as offset 0x50
0x94   | 4    | int32  | ID Repeat 2       | Same as offset 0x50
0x98   | 8    | int64  | Unknown           | 0x50
0xA0   | 8    | int64  | Unknown           | Always 0
0xA8   | 8    | int64  | Unknown           | 0xB0
0xB0   | 8    | int64  | Skeleton Name Ptr | UTF-16 string pointer (.hkt file)
0xB8   | 8    | int64  | Sib Name Ptr      | UTF-16 string pointer (.sib file)
0xC0   | 8    | int64  | Unknown           | Always 0
0xC8   | 8    | int64  | Unknown           | Always 0
```

### Animation Entry Structure

Each animation in the TAE contains:

```c#
public class Animation
{
    long ID;                        // Animation ID (matches .hkx animation ID)
    List<Event> Events;             // Timed events in this animation
    List<EventGroup> EventGroups;   // Unknown groups of events
    bool AnimFileReference;         // Unknown
    int AnimFileUnk18;              // Unknown
    int AnimFileUnk1C;              // Unknown
    string AnimFileName;            // Optional .hkt/.hkx filename
}
```

**Animation Data Block:**
```
Offset | Size | Type   | Name                 | Description
-------|------|--------|----------------------|-------------------------
0x00   | 8    | int64  | Animation ID         | Animation identifier
0x08   | 8    | int64  | Data Offset          | Pointer to animation data
```

**Animation Data Structure (at Data Offset):**
```
Offset | Size | Type   | Name                 | Description
-------|------|--------|----------------------|-------------------------
0x00   | 8    | int64  | Event Headers Offset | Pointer to event list
0x08   | 8    | int64  | Event Groups Offset  | Pointer to groups
0x10   | 8    | int64  | Times Offset         | Pointer to time values
0x18   | 8    | int64  | Anim File Offset     | Pointer to anim file info
0x20   | 4    | int32  | Event Count          | Number of events
0x24   | 4    | int32  | Event Group Count    | Number of groups
0x28   | 4    | int32  | Times Count          | Number of unique times
0x2C   | 4    | int32  | Unknown              | Always 0
```

### Event Structure

**Event Header (24 bytes):**
```
Offset | Size | Type   | Name              | Description
-------|------|--------|-------------------|---------------------------
0x00   | 8    | int64  | Start Time Offset | Pointer to start time float
0x08   | 8    | int64  | End Time Offset   | Pointer to end time float
0x10   | 8    | int64  | Event Data Offset | Pointer to event-specific data
```

**Event Data Block:**
```
Offset | Size | Type   | Name              | Description
-------|------|--------|-------------------|---------------------------
0x00   | 8    | uint64 | Event Type        | Event type enum (see below)
0x08   | 8    | int64  | Data Offset       | Pointer to parameter data (self+8)
0x10   | N    | varies | Parameters        | Event-specific parameters
```

**Time Storage:**
- Times are stored as **32-bit floats** (seconds, NOT frames)
- Each unique time value appears once in a time table
- Events reference times via offsets into this table
- To convert to frames: `frame = round(time * 30)` for 30 FPS games

---

## 2. Event Types Catalog

The format supports **108+ distinct event types** across all games. Here are the major categories:

### Combat & Damage Events

| ID  | Name                  | Purpose                              | Key Parameters                          |
|-----|-----------------------|--------------------------------------|-----------------------------------------|
| 0   | JumpTable/ChrActionFlag | General flags (invincibility, etc.) | FlagType (int32), ArgA (float), ArgB (int32), ArgC/ArgD (bytes) |
| 1   | AttackBehavior        | Activates attack hitbox              | AttackType (int32), AttackIndex (int32), BehaviorJudgeID (int32) |
| 2   | BulletBehavior        | Fires projectile                     | DummyPolyID (int32), BehaviorJudgeID (int32), AttachmentType (byte) |
| 5   | CommonBehavior        | Simple damage (fall damage, etc.)    | AttackIndex (int32), BehaviorJudgeID (int32) |
| 304 | ThrowAttackBehavior   | Throw/grab damage                    | Index (int16), BehaviorJudgeID (int32) |

**JumpTable Flag Values (Event Type 0):**
- **8**: Invincible (I-frames)
- **24**: Hyper Armor (poise)
- **45**: Disable Poise
- **67**: Invincible Excluding Throw Damage Taken
- **68**: Invincible Excluding Throw Damage Dealt
- **73**: Parry Possible State
- **94**: Perfect Invincibility

### Poise/Toughness Events

| ID  | Name      | Purpose                 | Parameters                                    |
|-----|-----------|-------------------------|-----------------------------------------------|
| 795 | Toughness | Set poise during attack | ToughnessParamID (byte), IsToughnessEffective (bool), ToughnessRate (float) |

**Toughness Event Structure:**
```c#
public class Toughness : Event // 795
{
    byte ToughnessParamID;      // ID referencing poise parameter table
    bool IsToughnessEffective;  // Enable/disable toughness
    float ToughnessRate;        // Damage resistance multiplier
}
```

### Visual & Audio Events

| ID  | Name                 | Purpose                  | Key Parameters                                  |
|-----|----------------------|--------------------------|------------------------------------------------|
| 96  | PlayFFX              | Spawn VFX (one-shot)     | FFXID (int32), Unk04 (int32), State0/1 (sbyte) |
| 128 | PlaySound1           | Play sound (center body) | SoundType (int32), SoundID (int32)             |
| 129 | PlaySound2           | Play sound (by slot)     | SoundType (int32), SoundID (int32), DummyPolyID|
| 130 | PlaySound3           | Play sound (dummy poly)  | SoundType (int32), SoundID (int32), DummyPolyID|
| 131 | PlaySound4           | Play sound (by dummy)    | SoundType (int32), SoundID (int32)             |

**Sound Types:**
- 0: (a) Environment
- 1: (c) Character
- 2: (f) Menu SE
- 3: (o) Object
- 4: (p) Cutscene SE
- 5: (s) SFX
- 6: (m) BGM
- 7: (v) Voice
- 8: (x) Floor Material Determined

**PlayFFX Event Structure:**
```c#
public class PlayFFX : Event // 096
{
    int FFXID;              // VFX ID to spawn
    int Unk04;              // Unknown (likely dummy poly ID)
    int Unk08;              // Unknown (likely slot ID)
    sbyte State0;           // Spawn condition state
    sbyte State1;           // Follow dummy poly flag
    sbyte GhostFFXCondition;// Ghost-specific condition
    byte Unk0F;             // Unknown
}
```

### Special Effect Events

| ID  | Name                  | Purpose                      | Key Parameters                    |
|-----|-----------------------|------------------------------|-----------------------------------|
| 66  | CreateSpEffect1       | Apply special effect (type 1)| SpEffectID, effect parameters     |
| 67  | CreateSpEffect2       | Apply special effect (type 2)| SpEffectID, effect parameters     |
| 401 | CreateSpEffect        | Apply special effect         | SpEffectID, effect parameters     |
| 302 | AddSpEffectDragonForm | Apply dragon form effect     | SpEffectID, effect parameters     |

### Animation Control Events

| ID  | Name                  | Purpose                      | Key Parameters                    |
|-----|-----------------------|------------------------------|-----------------------------------|
| 303 | PlayAnimation         | Trigger additional animation | AnimID, blend parameters          |
| 225 | DisableStaminaRegen   | Disable stamina recovery     | None (timing only)                |
| 228 | RagdollReviveTime     | Set ragdoll revival time     | Time parameter                    |
| 231 | SetEventMessageID     | Display message              | MessageID                         |

### Model & Rendering Events

| ID  | Name            | Purpose                   | Key Parameters                    |
|-----|-----------------|---------------------------|-----------------------------------|
| 233 | ChangeDrawMask  | Toggle model visibility   | DrawMask bits                     |
| 710 | HideWeapon      | Hide weapon model         | WeaponSlot                        |
| 711 | HideModelMask   | Hide model parts          | MaskID                            |
| 713 | ModelMask       | Set model mask            | MaskID, parameters                |

### Movement & Positioning Events

| ID  | Name                         | Purpose                      | Key Parameters                    |
|-----|------------------------------|------------------------------|-----------------------------------|
| 760 | BoostRootMotionToReachTarget | Boost movement to target     | IsEnable (bool), ReferenceDist, EnableRange, ArriveAngle, ArriveDist |
| 770 | BonePos                      | Set bone position            | BoneID (int32), Position (float)  |
| 781 | TurnLowerBody                | Enable lower body turning    | TurnState (byte)                  |

### Advanced Events

| ID  | Name                  | Purpose                      | Key Parameters                    |
|-----|-----------------------|------------------------------|-----------------------------------|
| 236 | RollDistanceReduction | Reduce roll distance         | ReductionAmount                   |
| 237 | CreateAISound         | Create AI-detectable sound   | SoundRadius, SoundType            |
| 603 | DebugAnimSpeed        | Override animation speed     | SpeedMultiplier                   |
| 703 | EnableTurningDirection| Allow directional turning    | None                              |
| 705 | FacingAngleCorrection | Correct facing angle         | AngleCorrection                   |

---

## 3. Event Parameter Data Types

Events use a variety of parameter types:

### Common Parameter Patterns

```c#
// Simple flag event (no parameters, timing only)
public class Unk016 : Event // 016
{
    // No parameters - just start/end time
}

// Single-parameter event
public class TurnLowerBody : Event // 781
{
    byte TurnState;  // 1 byte parameter + 12 bytes padding (16 byte alignment)
}

// Multi-parameter event
public class Toughness : Event // 795
{
    byte ToughnessParamID;      // 1 byte
    bool IsToughnessEffective;  // 1 byte (stored as byte, not bit)
    // 2 bytes padding
    float ToughnessRate;        // 4 bytes
    // 8 bytes padding (total 16 bytes)
}

// Complex event
public class PlayFFX : Event // 096
{
    int FFXID;              // 4 bytes - VFX ID
    int Unk04;              // 4 bytes
    int Unk08;              // 4 bytes
    sbyte State0;           // 1 byte
    sbyte State1;           // 1 byte
    sbyte GhostFFXCondition;// 1 byte
    byte Unk0F;             // 1 byte
    // Total: 16 bytes
}
```

### Data Alignment Rules

- Parameters are **16-byte aligned** (padded with zeros)
- Multi-byte values are **little-endian**
- Booleans are stored as **full bytes** (0x00 or 0x01)
- Strings are **UTF-16** encoded

---

## 4. Example Event Parsing (C# Code)

### Reading an Event

```c#
internal static Event Read(BinaryReaderEx br)
{
    // Read event header (24 bytes)
    long startTimeOffset = br.ReadInt64();
    long endTimeOffset = br.ReadInt64();
    long eventDataOffset = br.ReadInt64();

    // Get timing values from offset table
    float startTime = br.GetSingle(startTimeOffset);
    float endTime = br.GetSingle(endTimeOffset);

    // Read event data
    Event result;
    br.StepIn(eventDataOffset);
    {
        EventType type = br.ReadEnum64<EventType>();
        br.AssertInt64(br.Position + 8);  // Next 8 bytes always point to self+8

        // Parse event-specific parameters based on type
        switch (type)
        {
            case EventType.Toughness:
                result = new Toughness(startTime, endTime, br);
                break;
            case EventType.PlayFFX:
                result = new PlayFFX(startTime, endTime, br);
                break;
            // ... 106 more event types
        }
    }
    br.StepOut();

    return result;
}
```

### Reading Toughness Event Parameters

```c#
internal Toughness(float startTime, float endTime, BinaryReaderEx br)
    : base(startTime, endTime)
{
    ToughnessParamID = br.ReadByte();
    IsToughnessEffective = br.ReadBoolean();
    br.AssertByte(0);  // Padding
    br.AssertByte(0);  // Padding
    ToughnessRate = br.ReadSingle();
}
```

---

## 5. Python Replication Feasibility

**Verdict: HIGHLY FEASIBLE**

### Why Python is Suitable

1. **Binary parsing is straightforward** with `struct` module
2. **Format is well-documented** through C# code
3. **No complex compression** (DCX wrapper uses standard zlib)
4. **Clear data structures** map easily to Python classes

### Recommended Python Approach

```python
import struct
from dataclasses import dataclass
from typing import List, Dict
from enum import IntEnum

class EventType(IntEnum):
    JumpTable = 0
    AttackBehavior = 1
    BulletBehavior = 2
    # ... 105 more types
    Toughness = 795

@dataclass
class Event:
    """Base class for all TAE events"""
    start_time: float
    end_time: float
    event_type: EventType

@dataclass
class ToughnessEvent(Event):
    toughness_param_id: int
    is_toughness_effective: bool
    toughness_rate: float

class TAEParser:
    def __init__(self, file_path: str):
        self.file_path = file_path
        self.animations: Dict[int, Animation] = {}

    def parse(self):
        with open(self.file_path, 'rb') as f:
            # Read header
            magic = f.read(4)
            assert magic == b'TAE ', "Invalid TAE file"

            # Read version info
            version_header = struct.unpack('<4B', f.read(4))
            version = struct.unpack('<I', f.read(4))[0]
            file_size = struct.unpack('<I', f.read(4))[0]

            # ... continue parsing structure

    def read_event(self, f, event_header_offset: int) -> Event:
        """Read a single event from file"""
        f.seek(event_header_offset)

        # Read event header
        start_time_offset = struct.unpack('<Q', f.read(8))[0]
        end_time_offset = struct.unpack('<Q', f.read(8))[0]
        event_data_offset = struct.unpack('<Q', f.read(8))[0]

        # Get timing
        f.seek(start_time_offset)
        start_time = struct.unpack('<f', f.read(4))[0]
        f.seek(end_time_offset)
        end_time = struct.unpack('<f', f.read(4))[0]

        # Read event type and parameters
        f.seek(event_data_offset)
        event_type = EventType(struct.unpack('<Q', f.read(8))[0])
        f.read(8)  # Skip self-pointer

        # Parse based on type
        if event_type == EventType.Toughness:
            return self.parse_toughness_event(f, start_time, end_time)
        # ... handle other types

    def parse_toughness_event(self, f, start_time: float, end_time: float) -> ToughnessEvent:
        """Parse Toughness event (type 795)"""
        param_id = struct.unpack('<B', f.read(1))[0]
        is_effective = struct.unpack('<B', f.read(1))[0] != 0
        f.read(2)  # Padding
        rate = struct.unpack('<f', f.read(4))[0]

        return ToughnessEvent(
            start_time=start_time,
            end_time=end_time,
            event_type=EventType.Toughness,
            toughness_param_id=param_id,
            is_toughness_effective=is_effective,
            toughness_rate=rate
        )
```

### External Dependencies Needed

1. **DCX decompression**: Use `pyzlib` or similar for .dcx file extraction
2. **BND parsing**: May need to parse .anibnd container format first
3. **XML templates**: Can parse the TAE.Template.DS3.xml for event metadata

### Challenges to Address

| Challenge                  | Solution                                                     |
|----------------------------|--------------------------------------------------------------|
| 108+ event types           | Start with most important events, add others incrementally   |
| Unknown event parameters   | Label as "Unk00", "Unk04" like C# code does                 |
| Game version differences   | Support TAE3 format first (DS3/BB/Sekiro/ER), add others later |
| Binary offset complexity   | Use `struct.unpack_from()` and careful offset tracking       |

---

## 6. Key C# Source Files

### Primary Format Files

| File Path | Purpose | Lines |
|-----------|---------|-------|
| `SoulsAssetPipeline/SoulsFormats/Formats/TAE3/TAE3.cs` | Main TAE file parser | ~500 |
| `SoulsAssetPipeline/SoulsFormats/Formats/TAE3/TAE3.Event.cs` | Event class definitions (108 types) | ~3188 |
| `SoulsAssetPipeline/SoulsAssetPipeline/Animation/TAE/TAE.cs` | High-level TAE wrapper | ~300 |
| `SoulsAssetPipeline/SoulsAssetPipeline/Animation/TAE/TAE.Template.cs` | XML template reader | ~157 |

### Template Files (Event Metadata)

| File | Game | Event Count |
|------|------|-------------|
| `DSAnimStudioNETCore/Res/TAE.Template.DS3.xml` | Dark Souls 3 | ~100+ |
| `DSAnimStudioNETCore/Res/TAE.Template.BB.xml` | Bloodborne | ~100+ |
| `DSAnimStudioNETCore/Res/TAE.Template.ER.xml` | Elden Ring | ~100+ |
| `DSAnimStudioNETCore/Res/TAE.Template.DS1.xml` | Dark Souls 1 | ~80+ |

### Helper Classes

| File | Purpose |
|------|---------|
| `SoulsFormats/IO/BinaryReaderEx.cs` | Enhanced binary reader with offset tracking |
| `SoulsFormats/IO/BinaryWriterEx.cs` | Enhanced binary writer with offset reservations |

---

## 7. Practical Example: Parsing Invulnerability Frames

### Finding I-Frames in a TAE

```python
def find_iframes(tae_file: str, animation_id: int) -> List[Tuple[float, float]]:
    """
    Find all invulnerability frame windows in an animation.
    Returns list of (start_time, end_time) tuples in seconds.
    """
    parser = TAEParser(tae_file)
    parser.parse()

    anim = parser.animations[animation_id]
    iframes = []

    for event in anim.events:
        # Check for JumpTable events with invincibility flags
        if event.event_type == EventType.JumpTable:
            if event.flag_type in [8, 67, 68, 94]:  # Various invincibility types
                iframes.append((event.start_time, event.end_time))

    return iframes

# Example usage
iframes = find_iframes("c0000.anibnd.dcx/a00_0000.tae", animation_id=3000)
for start, end in iframes:
    start_frame = round(start * 30)
    end_frame = round(end * 30)
    duration_frames = end_frame - start_frame
    print(f"I-frames: Frame {start_frame}-{end_frame} ({duration_frames} frames)")
```

**Output Example:**
```
I-frames: Frame 4-12 (8 frames)
I-frames: Frame 15-18 (3 frames)
```

---

## 8. Event Type Reference by Category

### Combat Mechanics (20 events)

- **0**: JumpTable/ChrActionFlag - General animation flags
- **1**: AttackBehavior - Primary attack hitbox
- **2**: BulletBehavior - Projectile spawning
- **5**: CommonBehavior - Simple attacks (fall damage, etc.)
- **66-67**: CreateSpEffect1/2 - Apply special effects
- **304**: ThrowAttackBehavior - Grab/throw attacks
- **401**: CreateSpEffect - Generic special effect
- **795**: Toughness/Poise - Hyper armor control

### Audio (5+ events)

- **128**: PlaySound1 - Center body sound
- **129**: PlaySound2 - By slot sound
- **130**: PlaySound3 - Dummy poly sound (player voice)
- **131**: PlaySound4 - By dummy poly
- **132**: PlaySound5 - Additional sound type

### Visual Effects (10+ events)

- **96**: PlayFFX - Spawn one-shot VFX
- **112**: HitEffect - Hit impact effect
- **138**: CreateDecal - Create decal on surface
- **710**: HideWeapon - Toggle weapon visibility
- **711**: HideModelMask - Toggle model parts
- **713**: ModelMask - Set model render mask

### Animation Control (15+ events)

- **225**: DisableStaminaRegen - Block stamina recovery
- **228**: RagdollReviveTime - Ragdoll physics timing
- **303**: PlayAnimation - Layer additional animation
- **603**: DebugAnimSpeed - Override animation speed
- **703**: EnableTurningDirection - Allow turning
- **705**: FacingAngleCorrection - Adjust facing

### Movement (10+ events)

- **760**: BoostRootMotionToReachTarget - Boost movement
- **770**: BonePos - Position bone
- **771-772**: BoneFixOn1/2 - Fix bone position
- **781**: TurnLowerBody - Lower body rotation

### Special Systems (20+ events)

- **193**: FadeOut - Screen fade effect
- **231**: SetEventMessageID - Display message
- **233**: ChangeDrawMask - Model visibility
- **236**: RollDistanceReduction - Modify roll
- **237**: CreateAISound - AI detection sound

### Weapon/Equipment (5+ events)

- **32**: SwitchWeapon1 - Weapon style change
- **33**: SwitchWeapon2 - Weapon style change
- **710**: HideWeapon - Toggle weapon model

---

## 9. Complete Event Type Enum (108 Types)

```c#
public enum EventType : ulong
{
    JumpTable = 000,
    Unk001 = 001,
    Unk002 = 002,
    Unk005 = 005,
    Unk016 = 016,
    Unk017 = 017,
    Unk024 = 024,
    SwitchWeapon1 = 032,
    SwitchWeapon2 = 033,
    Unk034 = 034,
    Unk035 = 035,
    Unk064 = 064,
    Unk065 = 065,
    CreateSpEffect1 = 066,
    CreateSpEffect2 = 067,
    PlayFFX = 096,
    Unk110 = 110,
    HitEffect = 112,
    Unk113 = 113,
    Unk114 = 114,
    Unk115 = 115,
    Unk116 = 116,
    Unk117 = 117,
    Unk118 = 118,
    Unk119 = 119,
    Unk120 = 120,
    Unk121 = 121,
    PlaySound1 = 128,
    PlaySound2 = 129,
    PlaySound3 = 130,
    PlaySound4 = 131,
    PlaySound5 = 132,
    Unk136 = 136,
    Unk137 = 137,
    CreateDecal = 138,
    Unk144 = 144,
    Unk145 = 145,
    Unk150 = 150,
    Unk151 = 151,
    Unk161 = 161,
    Unk192 = 192,
    FadeOut = 193,
    Unk194 = 194,
    Unk224 = 224,
    DisableStaminaRegen = 225,
    Unk226 = 226,
    Unk227 = 227,
    RagdollReviveTime = 228,
    Unk229 = 229,
    SetEventMessageID = 231,
    Unk232 = 232,
    ChangeDrawMask = 233,
    RollDistanceReduction = 236,
    CreateAISound = 237,
    Unk300 = 300,
    Unk301 = 301,
    AddSpEffectDragonForm = 302,
    PlayAnimation = 303,
    BehaviorThing = 304,
    Unk306 = 306,
    CreateBehaviorPC = 307,
    Unk308 = 308,
    Unk310 = 310,
    Unk311 = 311,
    Unk312 = 312,
    Unk317 = 317,
    Unk320 = 320,
    Unk330 = 330,
    EffectDuringThrow = 331,
    Unk332 = 332,
    CreateSpEffect = 401,
    Unk500 = 500,
    Unk510 = 510,
    Unk520 = 520,
    KingOfTheStorm = 522,
    Unk600 = 600,
    Unk601 = 601,
    DebugAnimSpeed = 603,
    Unk605 = 605,
    Unk606 = 606,
    Unk700 = 700,
    EnableTurningDirection = 703,
    FacingAngleCorrection = 705,
    Unk707 = 707,
    HideWeapon = 710,
    HideModelMask = 711,
    DamageLevelModule = 712,
    ModelMask = 713,
    DamageLevelFunction = 714,
    Unk715 = 715,
    CultStart = 720,
    Unk730 = 730,
    Unk740 = 740,
    IFrameState = 760,
    BonePos = 770,
    BoneFixOn1 = 771,
    BoneFixOn2 = 772,
    TurnLowerBody = 781,
    Unk782 = 782,
    SpawnBulletByCultSacrifice1 = 785,
    Unk786 = 786,
    Unk790 = 790,
    Unk791 = 791,
    HitEffect2 = 792,
    CultSacrifice1 = 793,
    SacrificeEmpty = 794,
    Toughness = 795,
    BringCultMenu = 796,
    CeremonyParamID = 797,
    CultSingle = 798,
    CultEmpty2 = 799,
    Unk800 = 800,
    Unk900 = 900,
}
```

---

## 10. Next Steps for Python Implementation

### Phase 1: Basic Parser (Week 1)
1. Implement TAE3 header reading
2. Parse animation list
3. Parse event headers (type, start/end time)
4. Test with sample .tae files

### Phase 2: Event Types (Week 2-3)
1. Implement 20 most common events:
   - JumpTable (0)
   - AttackBehavior (1)
   - PlayFFX (96)
   - PlaySound1-5 (128-132)
   - Toughness (795)
   - CreateSpEffect (66, 67, 401)
2. Add XML template parsing for event metadata
3. Create event visualization/export

### Phase 3: Full Coverage (Week 4)
1. Implement remaining 88 event types
2. Add write support (modify TAE files)
3. Create CLI tool for TAE analysis
4. Add Unreal integration

### Phase 4: Unreal Integration (Week 5+)
1. Export TAE data to JSON/YAML
2. Create Unreal AnimNotify classes for each event type
3. Build import pipeline for TAE → Unreal AnimSequence
4. Map TAE events to Soulslike Framework components

---

## 11. Resources

### Official Tools
- **DSAnimStudio**: https://github.com/Meowmaritus/DSAnimStudio
- **SoulsFormats**: https://github.com/soulsmods/SoulsFormatsNEXT

### Community Resources
- Event mappings by Pav: Embedded in TAE.Template.*.xml files
- Reverse engineering documentation: Comments in TAE3.Event.cs

### Recommended Reading Order
1. `TAE3.cs` - File structure
2. `TAE3.Event.cs` (lines 1-320) - Base event class and reading logic
3. `TAE.Template.DS3.xml` - Event metadata and flag meanings
4. `TAE3.Event.cs` (lines 321+) - Individual event implementations

---

## Conclusion

The TAE format is **well-structured, fully reversible, and suitable for Python parsing**. The DSAnimStudio codebase provides complete documentation through working C# code. A Python implementation would enable:

1. **Automated analysis** of animation timing data
2. **Extraction** of combat mechanics (hitboxes, I-frames, poise)
3. **Integration** with Unreal Engine animation systems
4. **Tooling** for Soulslike game development

**Recommended approach:** Start with the 20 most critical event types, validate against known .tae files, then expand coverage incrementally.
