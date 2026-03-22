"""
TAE (TimeAct Event) Parser for FromSoftware Games (Elden Ring, DS3, Sekiro)
Extracts animation combat event data: i-frames, hitboxes, poise, parry windows, sounds, VFX

Supports:
- Raw .tae files
- .anibnd.dcx archives (auto-decompresses DCX + unpacks BND)

Output: JSON files mapping animation_id -> events for UE5 AnimNotify creation

Based on: DSAnimStudio (github.com/Meowmaritus/DSAnimStudio) SoulsFormats C# library
"""

import struct
import zlib
import json
import os
import sys
from dataclasses import dataclass, field, asdict
from typing import List, Dict, Tuple, Optional, BinaryIO
from enum import IntEnum
from io import BytesIO
from pathlib import Path


# =============================================================================
# ENUMS
# =============================================================================

class EventType(IntEnum):
    """TAE Event Types"""
    JumpTable = 0
    AttackBehavior = 1
    BulletBehavior = 2
    CommonBehavior = 5
    SwitchWeapon1 = 32
    SwitchWeapon2 = 33
    CreateSpEffect1 = 66
    CreateSpEffect2 = 67
    PlayFFX = 96
    HitEffect = 112
    PlaySound1 = 128
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
    Toughness = 795


class JumpTableFlag(IntEnum):
    """Flags for JumpTable event (type 0) - the most important combat flags"""
    DoNothing = 0
    CancelRHAttackInputCheck = 1
    Guarding = 3
    CancelRHAttack = 4
    ParryPossible = 5
    DisableTurning = 7
    Invincible = 8                          # DODGE I-FRAMES
    DeathState = 12
    CancelLHAttack = 16
    AnimCancelAfterDealingDamage = 17
    AnimCancelAfterSuccessfulParry = 18
    HyperArmor = 24                         # POISE ACTIVE
    CancelDodgeInputCheck = 25
    CancelDodge = 26
    IgnoreGravity = 27
    DisableFootIK = 28
    Vulnerable = 41                          # +50% damage taken
    WhiffAnimTransition = 42                 # +12.5% damage dealt (sweetspot)
    DisablePoise = 45
    WeakAttack = 59                          # -20% damage dealt
    InvincibleExcludingThrow = 67
    ParryPossibleState = 73                  # PARRY WINDOW
    CompletelyDisableMovement = 89
    PerfectInvincibility = 94                # FULL INVINCIBLE
    SetImmortality = 96


# UE5 AnimNotify mapping
TAE_TO_UE5_NOTIFY = {
    # JumpTable flags -> SLF AnimNotify classes
    (EventType.JumpTable, JumpTableFlag.Invincible): "ANS_InvincibilityFrame",
    (EventType.JumpTable, JumpTableFlag.PerfectInvincibility): "ANS_InvincibilityFrame",
    (EventType.JumpTable, JumpTableFlag.InvincibleExcludingThrow): "ANS_InvincibilityFrame",
    (EventType.JumpTable, JumpTableFlag.HyperArmor): "ANS_HyperArmor",
    (EventType.JumpTable, JumpTableFlag.ParryPossibleState): "ANS_ParryWindow",
    (EventType.JumpTable, JumpTableFlag.DisablePoise): "ANS_DisablePoise",
    # Direct event types -> SLF AnimNotify classes
    EventType.AttackBehavior: "ANS_WeaponTrace",
    EventType.Toughness: "ANS_HyperArmor",
    EventType.DisableStaminaRegen: "ANS_DisableStaminaRegen",
    EventType.PlayFFX: "AN_PlayNiagara",
    EventType.PlaySound1: "AN_PlaySound",
    EventType.PlaySound2: "AN_PlaySound",
    EventType.PlaySound3: "AN_PlaySound",
    EventType.PlaySound4: "AN_PlaySound",
}


# =============================================================================
# DATA CLASSES
# =============================================================================

@dataclass
class TAEEvent:
    """Base event data"""
    start_time: float
    end_time: float
    event_type: int
    event_type_name: str = ""
    params: dict = field(default_factory=dict)

    @property
    def start_frame_30(self) -> int:
        return round(self.start_time * 30)

    @property
    def end_frame_30(self) -> int:
        return round(self.end_time * 30)

    @property
    def duration(self) -> float:
        return self.end_time - self.start_time

    def to_dict(self) -> dict:
        d = {
            "type": self.event_type_name or str(self.event_type),
            "type_id": self.event_type,
            "start_time": round(self.start_time, 6),
            "end_time": round(self.end_time, 6),
            "start_frame": self.start_frame_30,
            "end_frame": self.end_frame_30,
            "duration_frames": self.end_frame_30 - self.start_frame_30,
        }
        if self.params:
            d["params"] = self.params
        # Add UE5 notify mapping
        ue5_notify = self._get_ue5_notify()
        if ue5_notify:
            d["ue5_notify"] = ue5_notify
        return d

    def _get_ue5_notify(self) -> Optional[str]:
        """Map this event to UE5 AnimNotify class name"""
        if self.event_type == EventType.JumpTable and "flag" in self.params:
            key = (EventType.JumpTable, self.params["flag"])
            return TAE_TO_UE5_NOTIFY.get(key)
        return TAE_TO_UE5_NOTIFY.get(self.event_type)


@dataclass
class TAEAnimation:
    """Single animation with events"""
    anim_id: int
    events: List[TAEEvent] = field(default_factory=list)
    anim_file_name: str = ""

    def get_iframes(self) -> List[TAEEvent]:
        """Get all invulnerability frame events"""
        return [e for e in self.events
                if e.event_type == EventType.JumpTable
                and e.params.get("flag") in (
                    JumpTableFlag.Invincible,
                    JumpTableFlag.PerfectInvincibility,
                    JumpTableFlag.InvincibleExcludingThrow)]

    def get_hitboxes(self) -> List[TAEEvent]:
        """Get all attack hitbox activation events"""
        return [e for e in self.events if e.event_type == EventType.AttackBehavior]

    def get_hyper_armor(self) -> List[TAEEvent]:
        """Get all hyper armor / poise events"""
        result = []
        for e in self.events:
            if e.event_type == EventType.JumpTable and e.params.get("flag") == JumpTableFlag.HyperArmor:
                result.append(e)
            elif e.event_type == EventType.Toughness:
                result.append(e)
        return result

    def get_parry_windows(self) -> List[TAEEvent]:
        """Get parry window events"""
        return [e for e in self.events
                if e.event_type == EventType.JumpTable
                and e.params.get("flag") == JumpTableFlag.ParryPossibleState]

    def to_dict(self) -> dict:
        return {
            "animation_id": self.anim_id,
            "anim_file": self.anim_file_name,
            "event_count": len(self.events),
            "events": [e.to_dict() for e in self.events],
            "summary": {
                "iframes": len(self.get_iframes()),
                "hitboxes": len(self.get_hitboxes()),
                "hyper_armor": len(self.get_hyper_armor()),
                "parry_windows": len(self.get_parry_windows()),
            }
        }


# =============================================================================
# DCX DECOMPRESSOR
# =============================================================================

class DCXDecompressor:
    """Decompress FromSoftware .dcx files (DFLT/zlib compression)"""

    @staticmethod
    def is_dcx(data: bytes) -> bool:
        return data[:4] == b'DCX\x00'

    @staticmethod
    def decompress(data: bytes) -> bytes:
        """Decompress DCX data, returns raw bytes"""
        if not DCXDecompressor.is_dcx(data):
            return data  # Not DCX, return as-is

        f = BytesIO(data)
        magic = f.read(4)  # DCX\0
        assert magic == b'DCX\x00', f"Not a DCX file: {magic}"

        # Read DCX header
        unk04 = struct.unpack('>I', f.read(4))[0]  # 0x10000 or 0x11000
        dcs_offset = struct.unpack('>I', f.read(4))[0]
        dcp_offset = struct.unpack('>I', f.read(4))[0]

        # DCS block
        f.seek(dcs_offset)
        dcs_magic = f.read(4)
        assert dcs_magic == b'DCS\x00', f"Expected DCS, got {dcs_magic}"
        uncompressed_size = struct.unpack('>I', f.read(4))[0]
        compressed_size = struct.unpack('>I', f.read(4))[0]

        # DCP block
        f.seek(dcp_offset)
        dcp_magic = f.read(4)
        assert dcp_magic == b'DCP\x00', f"Expected DCP, got {dcp_magic}"
        compression_type = f.read(4)  # DFLT, EDGE, KRAK, etc.

        # Skip DCP parameters to find DCA (compressed data)
        # DCP header is 32 bytes total
        f.seek(dcp_offset + 32)
        dca_magic = f.read(4)
        assert dca_magic == b'DCA\x00', f"Expected DCA, got {dca_magic}"
        dca_header_size = struct.unpack('>I', f.read(4))[0]

        # Compressed data follows DCA header
        compressed_data = f.read(compressed_size)

        if compression_type == b'DFLT':
            # zlib/deflate
            return zlib.decompress(compressed_data, -15)  # Raw deflate
        elif compression_type == b'KRAK':
            # Oodle Kraken - need oodle library
            try:
                return DCXDecompressor._decompress_oodle(compressed_data, uncompressed_size)
            except Exception as e:
                print(f"WARNING: Oodle decompression not available: {e}")
                print("Elden Ring uses Oodle Kraken compression.")
                print("Try using WitchyBND to decompress DCX files first.")
                raise
        else:
            raise ValueError(f"Unsupported compression type: {compression_type}")

    @staticmethod
    def _decompress_oodle(data: bytes, uncompressed_size: int) -> bytes:
        """Try to decompress Oodle Kraken using the game's DLL"""
        import ctypes

        # Try to find oo2core DLL (ships with Elden Ring)
        oodle_paths = [
            # Elden Ring game directory
            os.environ.get("ELDEN_RING_PATH", "") + "/Game/oo2core_6_win64.dll",
            "C:/Program Files (x86)/Steam/steamapps/common/ELDEN RING/Game/oo2core_6_win64.dll",
            "./oo2core_6_win64.dll",
        ]

        for path in oodle_paths:
            if os.path.exists(path):
                oodle = ctypes.cdll.LoadLibrary(path)
                output = ctypes.create_string_buffer(uncompressed_size)
                result = oodle.OodleLZ_Decompress(
                    data, len(data),
                    output, uncompressed_size,
                    0, 0, 0, None, 0, None, None, None, 0, 3
                )
                if result == uncompressed_size:
                    return output.raw
                raise RuntimeError(f"Oodle decompression returned {result}, expected {uncompressed_size}")

        raise FileNotFoundError("oo2core DLL not found. Set ELDEN_RING_PATH environment variable.")


# =============================================================================
# BND4 UNPACKER
# =============================================================================

class BND4Unpacker:
    """Unpack BND4 archives (.anibnd, .chrbnd, etc.)"""

    @staticmethod
    def is_bnd4(data: bytes) -> bool:
        return data[:4] == b'BND4'

    @staticmethod
    def unpack(data: bytes) -> Dict[str, bytes]:
        """Unpack BND4 archive, returns {filename: data} dict"""
        f = BytesIO(data)

        magic = f.read(4)
        assert magic == b'BND4', f"Not a BND4 file: {magic}"

        unk04 = f.read(1)[0]  # Flag1
        unk05 = f.read(1)[0]  # Flag2
        f.read(2)  # Padding
        f.read(4)  # BigEndian flag (0 = little)

        file_count = struct.unpack('<I', f.read(4))[0]
        header_size = struct.unpack('<Q', f.read(8))[0]
        version = f.read(8)  # Version string
        file_header_size = struct.unpack('<Q', f.read(8))[0]
        f.read(8)  # Data start offset

        unicode_names = (unk04 & 0x04) != 0  # Flag for unicode filenames
        has_long_offsets = file_header_size >= 0x24

        files = {}
        for i in range(file_count):
            entry_start = f.tell()

            flags = struct.unpack('<B', f.read(1))[0]
            f.read(3)  # Padding
            f.read(4)  # Unk04
            compressed_size = struct.unpack('<Q', f.read(8))[0]

            if has_long_offsets:
                data_offset = struct.unpack('<Q', f.read(8))[0]
            else:
                data_offset = struct.unpack('<I', f.read(4))[0]

            entry_id = struct.unpack('<I', f.read(4))[0]
            name_offset = struct.unpack('<I', f.read(4))[0]

            # Read uncompressed size if present
            uncompressed_size = compressed_size
            if file_header_size >= 0x24:
                uncompressed_size = struct.unpack('<Q', f.read(8))[0]
                if uncompressed_size == 0:
                    uncompressed_size = compressed_size

            # Skip to next entry
            next_entry = entry_start + file_header_size
            current = f.tell()

            # Read filename
            f.seek(name_offset)
            if unicode_names:
                name_chars = []
                while True:
                    c = struct.unpack('<H', f.read(2))[0]
                    if c == 0:
                        break
                    name_chars.append(chr(c))
                filename = ''.join(name_chars)
            else:
                name_bytes = []
                while True:
                    c = f.read(1)
                    if c == b'\x00':
                        break
                    name_bytes.append(c)
                filename = b''.join(name_bytes).decode('ascii', errors='replace')

            # Read file data
            f.seek(data_offset)
            file_data = f.read(compressed_size)

            # Decompress if needed
            if compressed_size != uncompressed_size and compressed_size > 0:
                try:
                    file_data = zlib.decompress(file_data, -15)
                except zlib.error:
                    try:
                        file_data = zlib.decompress(file_data)
                    except zlib.error:
                        pass  # Leave as-is

            # Use just the filename part
            short_name = filename.split('\\')[-1].split('/')[-1]
            files[short_name] = file_data

            f.seek(next_entry)

        return files


# =============================================================================
# TAE PARSER
# =============================================================================

class TAEParser:
    """Parse TAE3 format files (DS3, Bloodborne, Sekiro, Elden Ring)"""

    MAGIC = b'TAE '
    VERSION_TAE3 = 0x1000C

    def __init__(self):
        self.tae_id: int = 0
        self.skeleton_name: str = ""
        self.animations: Dict[int, TAEAnimation] = {}

    def parse_file(self, file_path: str) -> Dict[int, TAEAnimation]:
        """Parse a .tae file or .anibnd.dcx archive"""
        path = Path(file_path)

        with open(file_path, 'rb') as f:
            raw_data = f.read()

        # Check if DCX compressed
        if DCXDecompressor.is_dcx(raw_data):
            print(f"  Decompressing DCX: {path.name}")
            raw_data = DCXDecompressor.decompress(raw_data)

        # Check if BND4 archive
        if BND4Unpacker.is_bnd4(raw_data):
            print(f"  Unpacking BND4 archive: {path.name}")
            return self._parse_bnd4(raw_data)

        # Direct TAE file
        if raw_data[:4] == self.MAGIC:
            return self._parse_tae(BytesIO(raw_data))

        raise ValueError(f"Unknown file format: {raw_data[:4]}")

    def _parse_bnd4(self, bnd_data: bytes) -> Dict[int, TAEAnimation]:
        """Parse all TAE files inside a BND4 archive"""
        files = BND4Unpacker.unpack(bnd_data)
        all_animations = {}

        for filename, data in files.items():
            # Check for nested DCX
            if DCXDecompressor.is_dcx(data):
                data = DCXDecompressor.decompress(data)

            if data[:4] == self.MAGIC:
                print(f"    Parsing TAE: {filename}")
                try:
                    anims = self._parse_tae(BytesIO(data))
                    all_animations.update(anims)
                except Exception as e:
                    print(f"    WARNING: Failed to parse {filename}: {e}")

        return all_animations

    def _parse_tae(self, f: BinaryIO) -> Dict[int, TAEAnimation]:
        """Parse a raw TAE data stream"""
        # --- Header ---
        magic = f.read(4)
        if magic != self.MAGIC:
            raise ValueError(f"Invalid TAE magic: {magic}")

        f.read(4)  # version header bytes
        version = struct.unpack('<I', f.read(4))[0]
        file_size = struct.unpack('<I', f.read(4))[0]

        # Skip header pointers (offsets 0x10-0x4F)
        f.seek(0x50)
        self.tae_id = struct.unpack('<I', f.read(4))[0]
        anim_count = struct.unpack('<I', f.read(4))[0]
        anims_offset = struct.unpack('<Q', f.read(8))[0]
        anim_groups_offset = struct.unpack('<Q', f.read(8))[0]

        # Skip to skeleton name
        f.seek(0x90)
        skeleton_name_offset = struct.unpack('<Q', f.read(8))[0]
        sib_name_offset = struct.unpack('<Q', f.read(8))[0]

        if skeleton_name_offset > 0:
            self.skeleton_name = self._read_utf16(f, skeleton_name_offset)

        # --- Read Animations ---
        f.seek(anims_offset)
        anim_headers = []
        for i in range(anim_count):
            anim_id = struct.unpack('<Q', f.read(8))[0]
            anim_data_offset = struct.unpack('<Q', f.read(8))[0]
            anim_headers.append((anim_id, anim_data_offset))

        animations = {}
        for anim_id, data_offset in anim_headers:
            try:
                anim = self._read_animation(f, anim_id, data_offset)
                animations[anim_id] = anim
            except Exception as e:
                print(f"    WARNING: Failed to parse animation {anim_id}: {e}")

        self.animations = animations
        return animations

    def _read_animation(self, f: BinaryIO, anim_id: int, offset: int) -> TAEAnimation:
        """Read a single animation's event data"""
        f.seek(offset)

        event_headers_offset = struct.unpack('<Q', f.read(8))[0]
        event_groups_offset = struct.unpack('<Q', f.read(8))[0]
        times_offset = struct.unpack('<Q', f.read(8))[0]
        anim_file_offset = struct.unpack('<Q', f.read(8))[0]

        event_count = struct.unpack('<I', f.read(4))[0]
        event_group_count = struct.unpack('<I', f.read(4))[0]
        times_count = struct.unpack('<I', f.read(4))[0]
        f.read(4)  # padding

        # Read time table
        times = {}
        if times_count > 0 and times_offset > 0:
            f.seek(times_offset)
            for i in range(times_count):
                time_file_offset = times_offset + (i * 4)
                time_val = struct.unpack('<f', f.read(4))[0]
                times[time_file_offset] = time_val

        # Read events
        events = []
        if event_count > 0 and event_headers_offset > 0:
            f.seek(event_headers_offset)
            for i in range(event_count):
                try:
                    event = self._read_event(f, times)
                    if event:
                        events.append(event)
                except Exception:
                    pass  # Skip malformed events

        # Read anim file name
        anim_file_name = ""
        if anim_file_offset > 0:
            try:
                f.seek(anim_file_offset)
                _ref = struct.unpack('<Q', f.read(8))[0]
                f.read(8)  # self-pointer
                name_offset = struct.unpack('<Q', f.read(8))[0]
                if name_offset > 0:
                    anim_file_name = self._read_utf16(f, name_offset)
            except Exception:
                pass

        return TAEAnimation(anim_id=anim_id, events=events, anim_file_name=anim_file_name)

    def _read_event(self, f: BinaryIO, times: Dict[int, float]) -> Optional[TAEEvent]:
        """Read a single event from event header"""
        # Event header: 3 x int64 pointers
        start_time_ptr = struct.unpack('<Q', f.read(8))[0]
        end_time_ptr = struct.unpack('<Q', f.read(8))[0]
        event_data_ptr = struct.unpack('<Q', f.read(8))[0]

        # Resolve times
        start_time = times.get(start_time_ptr, 0.0)
        end_time = times.get(end_time_ptr, 0.0)

        # Save position and read event data
        saved_pos = f.tell()
        f.seek(event_data_ptr)

        raw_type = struct.unpack('<Q', f.read(8))[0]
        f.read(8)  # self-pointer

        # Try to get event type name
        try:
            event_type = EventType(raw_type)
            type_name = event_type.name
        except ValueError:
            type_name = f"Unknown_{raw_type}"

        # Parse type-specific parameters
        params = self._parse_params(f, raw_type)

        f.seek(saved_pos)

        return TAEEvent(
            start_time=start_time,
            end_time=end_time,
            event_type=raw_type,
            event_type_name=type_name,
            params=params
        )

    def _parse_params(self, f: BinaryIO, event_type: int) -> dict:
        """Parse event-specific parameters"""
        try:
            if event_type == EventType.JumpTable:
                flag = struct.unpack('<I', f.read(4))[0]
                unk04 = struct.unpack('<I', f.read(4))[0]
                unk08 = struct.unpack('<I', f.read(4))[0]
                try:
                    flag_name = JumpTableFlag(flag).name
                except ValueError:
                    flag_name = f"Flag_{flag}"
                return {"flag": flag, "flag_name": flag_name}

            elif event_type == EventType.AttackBehavior:
                attack_type = struct.unpack('<I', f.read(4))[0]
                attack_index = struct.unpack('<I', f.read(4))[0]
                behavior_judge_id = struct.unpack('<I', f.read(4))[0]
                direction_type = struct.unpack('<B', f.read(1))[0]
                return {
                    "attack_type": attack_type,
                    "attack_index": attack_index,
                    "behavior_judge_id": behavior_judge_id,
                    "direction_type": direction_type
                }

            elif event_type == EventType.BulletBehavior:
                dummy_poly_id = struct.unpack('<I', f.read(4))[0]
                behavior_judge_id = struct.unpack('<I', f.read(4))[0]
                return {
                    "dummy_poly_id": dummy_poly_id,
                    "behavior_judge_id": behavior_judge_id
                }

            elif event_type == EventType.PlayFFX:
                ffx_id = struct.unpack('<I', f.read(4))[0]
                dummy_poly_id = struct.unpack('<I', f.read(4))[0]
                slot_id = struct.unpack('<I', f.read(4))[0]
                return {
                    "ffx_id": ffx_id,
                    "dummy_poly_id": dummy_poly_id,
                    "slot_id": slot_id
                }

            elif event_type in (EventType.PlaySound1, EventType.PlaySound2,
                                EventType.PlaySound3, EventType.PlaySound4,
                                EventType.PlaySound5):
                sound_type = struct.unpack('<I', f.read(4))[0]
                sound_id = struct.unpack('<I', f.read(4))[0]
                return {"sound_type": sound_type, "sound_id": sound_id}

            elif event_type == EventType.Toughness:
                param_id = struct.unpack('<B', f.read(1))[0]
                is_effective = struct.unpack('<B', f.read(1))[0] != 0
                f.read(2)  # padding
                rate = struct.unpack('<f', f.read(4))[0]
                return {
                    "toughness_param_id": param_id,
                    "is_effective": is_effective,
                    "toughness_rate": rate
                }

            elif event_type == EventType.CreateSpEffect1 or event_type == EventType.CreateSpEffect2:
                sp_effect_id = struct.unpack('<I', f.read(4))[0]
                return {"sp_effect_id": sp_effect_id}

            elif event_type == EventType.DisableStaminaRegen:
                state = struct.unpack('<B', f.read(1))[0]
                return {"disabled": state != 0}

        except Exception:
            pass  # Return empty params on parse failure

        return {}

    def _read_utf16(self, f: BinaryIO, offset: int) -> str:
        """Read null-terminated UTF-16LE string"""
        saved = f.tell()
        f.seek(offset)
        chars = []
        while True:
            b = f.read(2)
            if len(b) < 2:
                break
            val = struct.unpack('<H', b)[0]
            if val == 0:
                break
            chars.append(chr(val))
        f.seek(saved)
        return ''.join(chars)


# =============================================================================
# BATCH EXPORT
# =============================================================================

def export_all_animations(tae_path: str, output_dir: str, combat_only: bool = True):
    """
    Export all animations from a TAE/anibnd file to JSON.

    Args:
        tae_path: Path to .tae or .anibnd.dcx file
        output_dir: Directory for JSON output
        combat_only: If True, only export animations that have combat events
    """
    os.makedirs(output_dir, exist_ok=True)

    parser = TAEParser()
    print(f"Parsing: {tae_path}")
    animations = parser.parse_file(tae_path)
    print(f"Found {len(animations)} animations")

    # Filter to combat-relevant animations if requested
    exported = 0
    combat_summary = {
        "source_file": os.path.basename(tae_path),
        "skeleton": parser.skeleton_name,
        "total_animations": len(animations),
        "animations": {}
    }

    for anim_id, anim in sorted(animations.items()):
        has_combat = (anim.get_iframes() or anim.get_hitboxes() or
                      anim.get_hyper_armor() or anim.get_parry_windows())

        if combat_only and not has_combat:
            continue

        anim_data = anim.to_dict()
        combat_summary["animations"][str(anim_id)] = {
            "file": anim.anim_file_name,
            "events": anim_data["summary"],
        }
        exported += 1

    # Write summary
    summary_path = os.path.join(output_dir, "combat_summary.json")
    with open(summary_path, 'w') as f:
        json.dump(combat_summary, f, indent=2)
    print(f"Exported combat summary: {summary_path}")

    # Write full data
    full_path = os.path.join(output_dir, "all_events.json")
    full_data = {
        "source": os.path.basename(tae_path),
        "skeleton": parser.skeleton_name,
        "animations": {
            str(aid): anim.to_dict()
            for aid, anim in sorted(animations.items())
        }
    }
    with open(full_path, 'w') as f:
        json.dump(full_data, f, indent=2)
    print(f"Exported full data: {full_path} ({len(animations)} animations)")

    # Print combat summary
    print(f"\n--- Combat Event Summary ---")
    print(f"Animations with combat events: {exported}/{len(animations)}")

    total_iframes = sum(len(a.get_iframes()) for a in animations.values())
    total_hitboxes = sum(len(a.get_hitboxes()) for a in animations.values())
    total_hyper = sum(len(a.get_hyper_armor()) for a in animations.values())
    total_parry = sum(len(a.get_parry_windows()) for a in animations.values())

    print(f"  I-frame windows:    {total_iframes}")
    print(f"  Attack hitboxes:    {total_hitboxes}")
    print(f"  Hyper armor:        {total_hyper}")
    print(f"  Parry windows:      {total_parry}")

    return animations


def print_animation_details(animations: Dict[int, TAEAnimation], anim_id: int):
    """Print detailed event info for a specific animation"""
    if anim_id not in animations:
        print(f"Animation {anim_id} not found")
        return

    anim = animations[anim_id]
    print(f"\n=== Animation {anim_id} ===")
    print(f"File: {anim.anim_file_name}")
    print(f"Events: {len(anim.events)}")

    for event in anim.events:
        ue5 = event._get_ue5_notify()
        ue5_str = f" -> {ue5}" if ue5 else ""
        params_str = ""
        if event.params:
            params_str = " | " + ", ".join(f"{k}={v}" for k, v in event.params.items())

        print(f"  [{event.start_frame_30:4d}-{event.end_frame_30:4d}] "
              f"{event.event_type_name}{params_str}{ue5_str}")


# =============================================================================
# MAIN
# =============================================================================

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Elden Ring TAE Parser")
        print("=" * 40)
        print()
        print("Usage:")
        print(f"  python {sys.argv[0]} <file.tae|file.anibnd.dcx> [output_dir]")
        print(f"  python {sys.argv[0]} <file.tae> --anim <animation_id>")
        print()
        print("Examples:")
        print(f"  python {sys.argv[0]} c0000.anibnd.dcx ./output")
        print(f"  python {sys.argv[0]} c0000.tae --anim 3000")
        sys.exit(1)

    file_path = sys.argv[1]

    if "--anim" in sys.argv:
        anim_idx = sys.argv.index("--anim")
        anim_id = int(sys.argv[anim_idx + 1])
        parser = TAEParser()
        animations = parser.parse_file(file_path)
        print_animation_details(animations, anim_id)
    else:
        output_dir = sys.argv[2] if len(sys.argv) > 2 else "./tae_output"
        export_all_animations(file_path, output_dir, combat_only=False)
