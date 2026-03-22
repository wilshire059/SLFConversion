"""
TAE (TimeAct Event) Parser for FromSoftware Animation Files
Based on DSAnimStudio's SoulsFormats library

This is a Python implementation outline for parsing TAE files.
See TAE_FORMAT_ANALYSIS.md for complete format documentation.

Author: Analysis from github.com/Meowmaritus/DSAnimStudio
Date: 2026-02-08
"""

import struct
from dataclasses import dataclass
from typing import List, Dict, Tuple, Optional, BinaryIO
from enum import IntEnum


class EventType(IntEnum):
    """TAE Event Types - see TAE_FORMAT_ANALYSIS.md for full list"""
    JumpTable = 0  # General flags (invincibility, hyper armor, etc.)
    AttackBehavior = 1  # Attack hitbox activation
    BulletBehavior = 2  # Projectile spawning
    CommonBehavior = 5  # Simple damage (fall damage, etc.)
    SwitchWeapon1 = 32
    SwitchWeapon2 = 33
    CreateSpEffect1 = 66
    CreateSpEffect2 = 67
    PlayFFX = 96  # Visual effects
    HitEffect = 112
    PlaySound1 = 128  # Audio
    PlaySound2 = 129
    PlaySound3 = 130
    PlaySound4 = 131
    PlaySound5 = 132
    CreateDecal = 138
    FadeOut = 193
    DisableStaminaRegen = 225
    RagdollReviveTime = 228
    SetEventMessageID = 231
    ChangeDrawMask = 233
    RollDistanceReduction = 236
    CreateAISound = 237
    AddSpEffectDragonForm = 302
    PlayAnimation = 303
    ThrowAttackBehavior = 304
    CreateSpEffect = 401
    DebugAnimSpeed = 603
    EnableTurningDirection = 703
    FacingAngleCorrection = 705
    HideWeapon = 710
    HideModelMask = 711
    ModelMask = 713
    IFrameState = 760
    BonePos = 770
    BoneFixOn1 = 771
    BoneFixOn2 = 772
    TurnLowerBody = 781
    Toughness = 795  # Poise/hyper armor


class JumpTableFlag(IntEnum):
    """Flags for JumpTable event (type 0)"""
    DoNothing = 0
    CancelRHAttackInputCheck = 1
    Guarding = 3
    CancelRHAttack = 4
    ParryPossible = 5
    ShieldPushPossible = 6
    DisableTurning = 7
    Invincible = 8  # I-FRAMES
    CancelLHAttackInputCheck = 9
    CancelMagicInputCheck = 10
    CancelLSMove = 11
    DeathState = 12
    CancelRHAttackInputReleaseCheck = 14
    CancelLHAttackInputReleaseCheck = 15
    CancelLHAttack = 16
    AnimCancelAfterDealingDamage = 17
    AnimCancelAfterSuccessfulParry = 18
    DisableMapObjectCollision = 19
    CancelLHGuardInputCheck = 21
    CancelLHGuard = 22
    CancelAIComboAttack = 23
    HyperArmor = 24  # POISE
    CancelDodgeInputCheck = 25
    CancelDodge = 26
    IgnoreGravity = 27
    DisableFootIK = 28
    CancelMagic = 29
    CancelItem = 31
    CancelGeneric = 32
    LeftRightMovementPossible = 37
    DisableCharacterRagdollCollision = 39
    TemporaryDeathState = 40
    Vulnerable = 41  # +50% damage taken
    WhiffAnimationTransitionAndSweetSpot = 42  # +12.5% damage dealt
    EnableFootIK = 43
    DisableEventObjectCollision = 44
    DisablePoise = 45
    PermanentlyDisableCharacter = 46
    DisableLockOn = 49
    DisableNPCParamHitRadiusPushCollision = 50
    DisableFallingAnimation = 51
    DisableOpeningMenu = 54
    BreakMyLockOn = 55
    DisableWallAttackClang = 56
    DisableNPCWallAttackClang = 57
    WeakAttack = 59  # -20% damage dealt
    DisableCharacterUpDownAiming = 62
    FlagAIForParryTiming = 63
    EnableChameleonSpellState = 64
    InvincibleExcludingThrowDamageTaken = 67
    InvincibleExcludingThrowDamageDealt = 68
    ThrowDeathAnimationTransition = 69
    ThrowEscapeAnimationTransition = 70
    StopPoiseRecoveryAfterPoiseBroken = 71
    AttackKnockbackCanPushSelf = 72
    ParryPossibleState = 73
    LadderCollisionStart = 74
    CancelGrabLoot = 75
    CancelAIMovement = 78
    CancelAIDodge = 79
    CancelGrabLootInputCheck = 80
    DisableGrabbedLootSuccessOkPrompt = 81
    DisableChameleonSpellState = 83
    PlungeAttackAnimTransition = 84
    LanternWeaponFlag = 85
    CancelCommonInputCheck = 87
    DisablePrecisionShooting = 88
    CompletelyDisableMovement = 89
    DisableDashRun = 90
    DisableDash = 91
    WarpAnimRootToMasterAfterDeath = 92
    PerfectInvincibility = 94
    DisableShadow = 95
    SetImmortality = 96
    DisableCancelMagicAndItem = 97


@dataclass
class Event:
    """Base class for all TAE events"""
    start_time: float  # In seconds
    end_time: float  # In seconds
    event_type: EventType

    @property
    def start_frame(self, fps: int = 30) -> int:
        """Convert start time to frame number"""
        return round(self.start_time * fps)

    @property
    def end_frame(self, fps: int = 30) -> int:
        """Convert end time to frame number"""
        return round(self.end_time * fps)

    @property
    def duration_frames(self, fps: int = 30) -> int:
        """Duration in frames"""
        return self.end_frame - self.start_frame


@dataclass
class JumpTableEvent(Event):
    """Event type 0 - General animation flags"""
    flag_type: int  # JumpTableFlag enum value
    arg_a: float
    arg_b: int
    arg_c: int
    arg_d: int
    state_info: int


@dataclass
class AttackBehaviorEvent(Event):
    """Event type 1 - Attack hitbox activation"""
    attack_type: int
    attack_index: int
    behavior_judge_id: int
    direction_type: int
    state_info: int


@dataclass
class BulletBehaviorEvent(Event):
    """Event type 2 - Projectile spawning"""
    dummy_poly_id: int
    behavior_judge_id: int
    attachment_type: int
    enable: bool
    state_info: int
    offset: int


@dataclass
class PlayFFXEvent(Event):
    """Event type 96 - Visual effects"""
    ffx_id: int  # VFX ID
    unk_04: int
    unk_08: int
    state_0: int
    state_1: int
    ghost_ffx_condition: int
    unk_0f: int


@dataclass
class PlaySoundEvent(Event):
    """Event types 128-132 - Audio playback"""
    sound_type: int  # 0=Environment, 1=Character, 5=SFX, 7=Voice, etc.
    sound_id: int
    dummy_poly_id: Optional[int] = None  # For types 129-131


@dataclass
class ToughnessEvent(Event):
    """Event type 795 - Poise/hyper armor control"""
    toughness_param_id: int  # ID referencing poise parameter table
    is_toughness_effective: bool  # Enable/disable toughness
    toughness_rate: float  # Damage resistance multiplier


@dataclass
class Animation:
    """A single animation with its events"""
    id: int  # Animation ID (matches .hkx animation ID)
    events: List[Event]
    event_groups: List['EventGroup']
    anim_file_reference: bool
    anim_file_unk18: int
    anim_file_unk1c: int
    anim_file_name: str


@dataclass
class EventGroup:
    """Group of events with associated type"""
    type: EventType
    indices: List[int]  # Indices into animation's event list


class TAE3Parser:
    """Parser for TAE3 format (DS3, Bloodborne, Sekiro, Elden Ring)"""

    def __init__(self, file_path: str):
        self.file_path = file_path
        self.id: int = 0
        self.flags: bytes = b''
        self.skeleton_name: str = ''
        self.sib_name: str = ''
        self.unk30: int = 0
        self.animations: Dict[int, Animation] = {}

    def parse(self) -> Dict[int, Animation]:
        """Parse TAE file and return animations dictionary"""
        with open(self.file_path, 'rb') as f:
            self._read_header(f)
            self._read_animations(f)
        return self.animations

    def _read_header(self, f: BinaryIO):
        """Read TAE file header"""
        # Magic and version
        magic = f.read(4)
        if magic != b'TAE ':
            raise ValueError(f"Invalid TAE file: magic = {magic}")

        version_header = struct.unpack('<4B', f.read(4))
        if version_header != (0, 0, 0, 0xFF):
            raise ValueError(f"Unexpected version header: {version_header}")

        version = struct.unpack('<I', f.read(4))[0]
        if version != 0x1000C:
            raise ValueError(f"Unsupported TAE version: {version:#x}")

        file_size = struct.unpack('<I', f.read(4))[0]

        # Read header pointers
        f.read(8)  # Skip 0x40
        f.read(8)  # Skip 1
        f.read(8)  # Skip 0x50
        f.read(8)  # Skip 0x80

        self.unk30 = struct.unpack('<Q', f.read(8))[0]
        f.read(8)  # Skip 0

        self.flags = f.read(8)
        f.read(8)  # Skip 1

        self.id = struct.unpack('<I', f.read(4))[0]
        anim_count = struct.unpack('<I', f.read(4))[0]

        anims_offset = struct.unpack('<Q', f.read(8))[0]
        anim_groups_offset = struct.unpack('<Q', f.read(8))[0]

        f.read(8)  # Skip 0xA0
        f.read(8)  # Skip anim_count
        first_anim_offset = struct.unpack('<Q', f.read(8))[0]

        f.read(8)  # Skip 1
        f.read(8)  # Skip 0x90
        f.read(4)  # Skip ID
        f.read(4)  # Skip ID
        f.read(8)  # Skip 0x50
        f.read(8)  # Skip 0
        f.read(8)  # Skip 0xB0

        skeleton_name_offset = struct.unpack('<Q', f.read(8))[0]
        sib_name_offset = struct.unpack('<Q', f.read(8))[0]

        # Read strings
        self.skeleton_name = self._read_utf16_at(f, skeleton_name_offset)
        self.sib_name = self._read_utf16_at(f, sib_name_offset)

        # Store offsets for animation reading
        self._anims_offset = anims_offset
        self._anim_count = anim_count

    def _read_utf16_at(self, f: BinaryIO, offset: int) -> str:
        """Read null-terminated UTF-16 string at offset"""
        if offset == 0:
            return ""

        current_pos = f.tell()
        f.seek(offset)

        chars = []
        while True:
            char_bytes = f.read(2)
            if len(char_bytes) < 2:
                break
            char_val = struct.unpack('<H', char_bytes)[0]
            if char_val == 0:
                break
            chars.append(chr(char_val))

        f.seek(current_pos)
        return ''.join(chars)

    def _read_animations(self, f: BinaryIO):
        """Read all animations from file"""
        f.seek(self._anims_offset)

        anim_headers = []
        for i in range(self._anim_count):
            anim_id = struct.unpack('<Q', f.read(8))[0]
            anim_data_offset = struct.unpack('<Q', f.read(8))[0]
            anim_headers.append((anim_id, anim_data_offset))

        # Read each animation's data
        for anim_id, anim_data_offset in anim_headers:
            anim = self._read_animation(f, anim_id, anim_data_offset)
            self.animations[anim_id] = anim

    def _read_animation(self, f: BinaryIO, anim_id: int, offset: int) -> Animation:
        """Read a single animation's data"""
        f.seek(offset)

        event_headers_offset = struct.unpack('<Q', f.read(8))[0]
        event_groups_offset = struct.unpack('<Q', f.read(8))[0]
        times_offset = struct.unpack('<Q', f.read(8))[0]
        anim_file_offset = struct.unpack('<Q', f.read(8))[0]

        event_count = struct.unpack('<I', f.read(4))[0]
        event_group_count = struct.unpack('<I', f.read(4))[0]
        times_count = struct.unpack('<I', f.read(4))[0]
        f.read(4)  # Skip 0

        # Read time table
        times = self._read_times(f, times_offset, times_count)

        # Read events
        events, event_header_offsets = self._read_events(
            f, event_headers_offset, event_count, times
        )

        # Read event groups
        event_groups = self._read_event_groups(
            f, event_groups_offset, event_group_count, event_header_offsets
        )

        # Read animation file info
        anim_file_ref, anim_file_unk18, anim_file_unk1c, anim_file_name = \
            self._read_anim_file_info(f, anim_file_offset)

        return Animation(
            id=anim_id,
            events=events,
            event_groups=event_groups,
            anim_file_reference=anim_file_ref,
            anim_file_unk18=anim_file_unk18,
            anim_file_unk1c=anim_file_unk1c,
            anim_file_name=anim_file_name
        )

    def _read_times(self, f: BinaryIO, offset: int, count: int) -> Dict[int, float]:
        """Read time value table"""
        if count == 0 or offset == 0:
            return {}

        f.seek(offset)
        times = {}
        for i in range(count):
            time_offset = offset + (i * 4)
            time_val = struct.unpack('<f', f.read(4))[0]
            times[time_offset] = time_val

        return times

    def _read_events(
        self, f: BinaryIO, offset: int, count: int, times: Dict[int, float]
    ) -> Tuple[List[Event], List[int]]:
        """Read all events and return (events, header_offsets)"""
        if count == 0 or offset == 0:
            return [], []

        f.seek(offset)
        events = []
        header_offsets = []

        for i in range(count):
            header_offset = f.tell()
            header_offsets.append(header_offset)

            event = self._read_event(f, times)
            events.append(event)

        return events, header_offsets

    def _read_event(self, f: BinaryIO, times: Dict[int, float]) -> Event:
        """Read a single event"""
        # Read event header (24 bytes)
        start_time_offset = struct.unpack('<Q', f.read(8))[0]
        end_time_offset = struct.unpack('<Q', f.read(8))[0]
        event_data_offset = struct.unpack('<Q', f.read(8))[0]

        # Get timing from time table
        start_time = times.get(start_time_offset, 0.0)
        end_time = times.get(end_time_offset, 0.0)

        # Read event data
        current_pos = f.tell()
        f.seek(event_data_offset)

        event_type = EventType(struct.unpack('<Q', f.read(8))[0])
        f.read(8)  # Skip self-pointer

        # Parse event-specific parameters
        event = self._parse_event_params(f, event_type, start_time, end_time)

        f.seek(current_pos)
        return event

    def _parse_event_params(
        self, f: BinaryIO, event_type: EventType, start: float, end: float
    ) -> Event:
        """Parse event-specific parameters based on type"""

        if event_type == EventType.JumpTable:
            return self._parse_jumptable_event(f, start, end)
        elif event_type == EventType.AttackBehavior:
            return self._parse_attack_behavior_event(f, start, end)
        elif event_type == EventType.PlayFFX:
            return self._parse_play_ffx_event(f, start, end)
        elif event_type in [EventType.PlaySound1, EventType.PlaySound2,
                            EventType.PlaySound3, EventType.PlaySound4]:
            return self._parse_play_sound_event(f, start, end, event_type)
        elif event_type == EventType.Toughness:
            return self._parse_toughness_event(f, start, end)
        else:
            # Generic event (unknown parameters)
            return Event(start_time=start, end_time=end, event_type=event_type)

    def _parse_jumptable_event(self, f: BinaryIO, start: float, end: float) -> JumpTableEvent:
        """Parse JumpTable event (type 0)"""
        flag_type = struct.unpack('<I', f.read(4))[0]
        unk_04 = struct.unpack('<I', f.read(4))[0]
        unk_08 = struct.unpack('<I', f.read(4))[0]
        unk_0c = struct.unpack('<h', f.read(2))[0]
        unk_0e = struct.unpack('<h', f.read(2))[0]

        return JumpTableEvent(
            start_time=start,
            end_time=end,
            event_type=EventType.JumpTable,
            flag_type=flag_type,
            arg_a=0.0,  # TODO: Read these from correct offset
            arg_b=unk_04,
            arg_c=unk_0c,
            arg_d=unk_0e,
            state_info=0
        )

    def _parse_attack_behavior_event(self, f: BinaryIO, start: float, end: float) -> AttackBehaviorEvent:
        """Parse AttackBehavior event (type 1)"""
        attack_type = struct.unpack('<I', f.read(4))[0]
        attack_index = struct.unpack('<I', f.read(4))[0]
        behavior_judge_id = struct.unpack('<I', f.read(4))[0]
        direction_type = struct.unpack('<B', f.read(1))[0]
        f.read(1)  # Padding
        state_info = struct.unpack('<h', f.read(2))[0]

        return AttackBehaviorEvent(
            start_time=start,
            end_time=end,
            event_type=EventType.AttackBehavior,
            attack_type=attack_type,
            attack_index=attack_index,
            behavior_judge_id=behavior_judge_id,
            direction_type=direction_type,
            state_info=state_info
        )

    def _parse_play_ffx_event(self, f: BinaryIO, start: float, end: float) -> PlayFFXEvent:
        """Parse PlayFFX event (type 96)"""
        ffx_id = struct.unpack('<I', f.read(4))[0]
        unk_04 = struct.unpack('<I', f.read(4))[0]
        unk_08 = struct.unpack('<I', f.read(4))[0]
        state_0 = struct.unpack('<b', f.read(1))[0]
        state_1 = struct.unpack('<b', f.read(1))[0]
        ghost_ffx_condition = struct.unpack('<b', f.read(1))[0]
        unk_0f = struct.unpack('<B', f.read(1))[0]

        return PlayFFXEvent(
            start_time=start,
            end_time=end,
            event_type=EventType.PlayFFX,
            ffx_id=ffx_id,
            unk_04=unk_04,
            unk_08=unk_08,
            state_0=state_0,
            state_1=state_1,
            ghost_ffx_condition=ghost_ffx_condition,
            unk_0f=unk_0f
        )

    def _parse_play_sound_event(
        self, f: BinaryIO, start: float, end: float, event_type: EventType
    ) -> PlaySoundEvent:
        """Parse PlaySound events (types 128-131)"""
        sound_type = struct.unpack('<I', f.read(4))[0]
        sound_id = struct.unpack('<I', f.read(4))[0]

        dummy_poly_id = None
        if event_type in [EventType.PlaySound2, EventType.PlaySound3, EventType.PlaySound4]:
            dummy_poly_id = struct.unpack('<I', f.read(4))[0]

        return PlaySoundEvent(
            start_time=start,
            end_time=end,
            event_type=event_type,
            sound_type=sound_type,
            sound_id=sound_id,
            dummy_poly_id=dummy_poly_id
        )

    def _parse_toughness_event(self, f: BinaryIO, start: float, end: float) -> ToughnessEvent:
        """Parse Toughness event (type 795)"""
        toughness_param_id = struct.unpack('<B', f.read(1))[0]
        is_toughness_effective = struct.unpack('<B', f.read(1))[0] != 0
        f.read(2)  # Padding
        toughness_rate = struct.unpack('<f', f.read(4))[0]

        return ToughnessEvent(
            start_time=start,
            end_time=end,
            event_type=EventType.Toughness,
            toughness_param_id=toughness_param_id,
            is_toughness_effective=is_toughness_effective,
            toughness_rate=toughness_rate
        )

    def _read_event_groups(
        self, f: BinaryIO, offset: int, count: int, event_header_offsets: List[int]
    ) -> List[EventGroup]:
        """Read event groups"""
        if count == 0 or offset == 0:
            return []

        f.seek(offset)
        groups = []

        for i in range(count):
            entry_count = struct.unpack('<Q', f.read(8))[0]
            values_offset = struct.unpack('<Q', f.read(8))[0]
            type_offset = struct.unpack('<Q', f.read(8))[0]
            f.read(8)  # Skip 0

            # Read group type
            current_pos = f.tell()
            f.seek(type_offset)
            group_type = EventType(struct.unpack('<Q', f.read(8))[0])

            # Read indices
            f.seek(values_offset)
            indices = []
            for j in range(entry_count):
                event_offset = struct.unpack('<I', f.read(4))[0]
                try:
                    index = event_header_offsets.index(event_offset)
                    indices.append(index)
                except ValueError:
                    pass  # Event offset not found

            groups.append(EventGroup(type=group_type, indices=indices))
            f.seek(current_pos)

        return groups

    def _read_anim_file_info(self, f: BinaryIO, offset: int) -> Tuple[bool, int, int, str]:
        """Read animation file reference info"""
        f.seek(offset)

        reference = struct.unpack('<Q', f.read(8))[0] == 1
        f.read(8)  # Skip self-pointer
        anim_file_name_offset = struct.unpack('<Q', f.read(8))[0]
        unk18 = struct.unpack('<I', f.read(4))[0]
        unk1c = struct.unpack('<I', f.read(4))[0]
        f.read(8)  # Skip 0
        f.read(8)  # Skip 0

        anim_file_name = self._read_utf16_at(f, anim_file_name_offset)

        return reference, unk18, unk1c, anim_file_name


# ============================================================================
# USAGE EXAMPLES
# ============================================================================

def find_invulnerability_frames(tae_file: str, animation_id: int) -> List[Tuple[float, float]]:
    """
    Find all invulnerability frame windows in an animation.
    Returns list of (start_time, end_time) tuples in seconds.
    """
    parser = TAE3Parser(tae_file)
    animations = parser.parse()

    if animation_id not in animations:
        raise ValueError(f"Animation {animation_id} not found in TAE file")

    anim = animations[animation_id]
    iframes = []

    for event in anim.events:
        # Check for JumpTable events with invincibility flags
        if isinstance(event, JumpTableEvent):
            if event.flag_type in [
                JumpTableFlag.Invincible,
                JumpTableFlag.InvincibleExcludingThrowDamageTaken,
                JumpTableFlag.InvincibleExcludingThrowDamageDealt,
                JumpTableFlag.PerfectInvincibility
            ]:
                iframes.append((event.start_time, event.end_time))

    return iframes


def find_hyper_armor_windows(tae_file: str, animation_id: int) -> List[Tuple[float, float]]:
    """
    Find all hyper armor (poise) windows in an animation.
    Returns list of (start_time, end_time) tuples in seconds.
    """
    parser = TAE3Parser(tae_file)
    animations = parser.parse()

    if animation_id not in animations:
        raise ValueError(f"Animation {animation_id} not found in TAE file")

    anim = animations[animation_id]
    hyper_armor_windows = []

    for event in anim.events:
        # JumpTable hyper armor flag
        if isinstance(event, JumpTableEvent) and event.flag_type == JumpTableFlag.HyperArmor:
            hyper_armor_windows.append((event.start_time, event.end_time))

        # Toughness events
        elif isinstance(event, ToughnessEvent) and event.is_toughness_effective:
            hyper_armor_windows.append((event.start_time, event.end_time))

    return hyper_armor_windows


def find_attack_hitbox_frames(tae_file: str, animation_id: int) -> List[Tuple[int, int, int]]:
    """
    Find all attack hitbox activations in an animation.
    Returns list of (start_frame, end_frame, attack_index) tuples.
    """
    parser = TAE3Parser(tae_file)
    animations = parser.parse()

    if animation_id not in animations:
        raise ValueError(f"Animation {animation_id} not found in TAE file")

    anim = animations[animation_id]
    hitboxes = []

    for event in anim.events:
        if isinstance(event, AttackBehaviorEvent):
            start_frame = round(event.start_time * 30)
            end_frame = round(event.end_time * 30)
            hitboxes.append((start_frame, end_frame, event.attack_index))

    return hitboxes


def export_animation_events_to_json(tae_file: str, animation_id: int, output_path: str):
    """Export animation events to JSON for Unreal Engine import"""
    import json

    parser = TAE3Parser(tae_file)
    animations = parser.parse()

    if animation_id not in animations:
        raise ValueError(f"Animation {animation_id} not found in TAE file")

    anim = animations[animation_id]

    # Convert to JSON-serializable format
    events_data = []
    for event in anim.events:
        event_dict = {
            'type': event.event_type.name,
            'type_id': event.event_type.value,
            'start_time': event.start_time,
            'end_time': event.end_time,
            'start_frame': round(event.start_time * 30),
            'end_frame': round(event.end_time * 30),
        }

        # Add type-specific data
        if isinstance(event, JumpTableEvent):
            event_dict['flag'] = event.flag_type
            event_dict['flag_name'] = JumpTableFlag(event.flag_type).name if event.flag_type in JumpTableFlag.__members__.values() else f"Unknown_{event.flag_type}"
        elif isinstance(event, AttackBehaviorEvent):
            event_dict['attack_index'] = event.attack_index
            event_dict['attack_type'] = event.attack_type
        elif isinstance(event, ToughnessEvent):
            event_dict['toughness_id'] = event.toughness_param_id
            event_dict['toughness_rate'] = event.toughness_rate
        elif isinstance(event, PlayFFXEvent):
            event_dict['ffx_id'] = event.ffx_id
        elif isinstance(event, PlaySoundEvent):
            event_dict['sound_id'] = event.sound_id
            event_dict['sound_type'] = event.sound_type

        events_data.append(event_dict)

    output = {
        'animation_id': animation_id,
        'anim_file': anim.anim_file_name,
        'event_count': len(events_data),
        'events': events_data
    }

    with open(output_path, 'w') as f:
        json.dump(output, f, indent=2)


if __name__ == '__main__':
    # Example usage
    import sys

    if len(sys.argv) < 2:
        print("Usage: python tae_parser_outline.py <tae_file_path> [animation_id]")
        sys.exit(1)

    tae_file = sys.argv[1]
    parser = TAE3Parser(tae_file)
    animations = parser.parse()

    print(f"Parsed {len(animations)} animations from {tae_file}")
    print(f"TAE ID: {parser.id}")
    print(f"Skeleton: {parser.skeleton_name}")
    print(f"Sib: {parser.sib_name}")
    print()

    if len(sys.argv) >= 3:
        anim_id = int(sys.argv[2])
        if anim_id in animations:
            anim = animations[anim_id]
            print(f"Animation {anim_id}:")
            print(f"  Events: {len(anim.events)}")
            print(f"  Event Groups: {len(anim.event_groups)}")
            print(f"  Anim File: {anim.anim_file_name}")
            print()

            # Find I-frames
            iframes = find_invulnerability_frames(tae_file, anim_id)
            if iframes:
                print("Invulnerability Frames:")
                for start, end in iframes:
                    print(f"  {round(start*30)}-{round(end*30)} frames")

            # Find hyper armor
            hyper_armor = find_hyper_armor_windows(tae_file, anim_id)
            if hyper_armor:
                print("\nHyper Armor Windows:")
                for start, end in hyper_armor:
                    print(f"  {round(start*30)}-{round(end*30)} frames")

            # Find hitboxes
            hitboxes = find_attack_hitbox_frames(tae_file, anim_id)
            if hitboxes:
                print("\nAttack Hitboxes:")
                for start, end, idx in hitboxes:
                    print(f"  Attack {idx}: {start}-{end} frames")
        else:
            print(f"Animation {anim_id} not found")
    else:
        print("Available animations:")
        for anim_id in sorted(animations.keys()):
            anim = animations[anim_id]
            print(f"  {anim_id}: {len(anim.events)} events")
