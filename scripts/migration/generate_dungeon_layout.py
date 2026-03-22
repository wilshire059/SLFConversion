#!/usr/bin/env python3
"""Generate dungeon layout JSON from cell graph data.

Reads dungeon_02_cellgraph.json, applies Souls-like room designations,
computes corridor wall segments, and outputs dungeon_02_layout.json.

This is a design tool, NOT a UE Python script.
"""

import json
import math
import os

CELL_GRAPH_PATH = "MapCapture/dungeon_02_cellgraph.json"
LAYOUT_OUTPUT_PATH = "MapCapture/dungeon_02_layout.json"

# ═══════════════════════════════════════════════════════════════════════════════
# Room Design Assignments
# ═══════════════════════════════════════════════════════════════════════════════
# Based on cell graph analysis:
# - Southern rooms (low Y): Entrance area
# - Northern rooms (high Y): Boss area
# - Central hub: Group 9 (most connected, 7 connections)
# - Far west outlier: Group 7 (only 2 connections) = Hidden room
# - Groups 0 and 11 are southernmost = Entrance
# - Groups 3 and 32 are northernmost = Boss / Shortcut

ROOM_DESIGNATIONS = {
    11: "Entrance",        # South-center, good starting point
    0:  "Treasure",        # Southeast, upper level, detour reward
    19: "Corridor",        # South-center, first junction after entrance
    13: "PuzzleRoom",      # Southwest, holds key to boss gate
    28: "Junction",        # Central hub east, connects many paths
    9:  "Junction",        # Central hub west, MOST connected (7)
    22: "AmbushRoom",      # West, ambush on the way to hidden room
    5:  "Corridor",        # Central, passage toward north
    7:  "HiddenRoom",      # Far west, only 2 connections, secret treasure
    18: "TrapRoom",        # Northeast, traps guard the approach
    34: "AmbushRoom",      # West-north, another ambush
    36: "Corridor",        # Central-north, passage
    8:  "Corridor",        # North-center, approach to boss
    32: "ShortcutHub",     # Far north-west, shortcut back to entrance
    3:  "BossArena",       # Far northeast, deepest room
}

# ═══════════════════════════════════════════════════════════════════════════════
# Corridor Definitions (which connections get narrow walled corridors)
# ═══════════════════════════════════════════════════════════════════════════════
# Format: (from_group, to_group, width, is_shortcut)
# Main path: 11 → 19 → 9 → 5 → 8 → 3 (boss)
# Side paths: 28→0 (treasure), 9→22→7 (hidden), 19→13 (puzzle), 9→34 (ambush)
# Shortcut: 32 → 36 → 9 (loops back)

CORRIDORS = [
    # Main path
    (11, 19, 500, False),   # Entrance to first junction
    (11, 28, 500, False),   # Entrance to central hub
    (19, 9,  450, False),   # Through center
    (9,  5,  400, False),   # Toward north
    (5,  8,  400, False),   # Continue north
    (8,  3,  350, False),   # Final approach to boss (narrower = tension)

    # Side paths
    (28, 0,  400, False),   # Hub to treasure room
    (28, 18, 400, False),   # Hub to trap room
    (9,  22, 450, False),   # Junction to ambush
    (22, 7,  350, False),   # Ambush to hidden room (narrow)
    (19, 13, 400, False),   # To puzzle room
    (9,  34, 400, False),   # To second ambush
    (9,  36, 450, False),   # Junction to north corridor
    (36, 32, 400, True),    # Shortcut path
    (34, 36, 400, False),   # Connect ambush to corridor
    (8,  32, 400, False),   # Boss area to shortcut
    (3,  32, 400, False),   # Boss to shortcut hub
    (18, 8,  400, False),   # Trap room to boss approach
]


def normalize_2d(dx, dy):
    length = math.sqrt(dx*dx + dy*dy)
    if length < 0.001:
        return 0.0, 0.0
    return dx / length, dy / length


def compute_corridor_walls(room_a, room_b, width):
    """Compute two parallel wall segments forming a corridor between rooms."""
    ax, ay = room_a["center"][0], room_a["center"][1]
    bx, by = room_b["center"][0], room_b["center"][1]
    ar, br = room_a["radius"], room_b["radius"]
    az, bz = room_a["center"][2], room_b["center"][2]

    # Direction from A to B (2D)
    dx, dy = bx - ax, by - ay
    dir_x, dir_y = normalize_2d(dx, dy)

    # Perpendicular (rotate 90 degrees)
    perp_x, perp_y = -dir_y, dir_x

    # Wall starts at edge of room A, ends at edge of room B
    # Use 0.7 of radius to leave room openings
    start_x = ax + dir_x * ar * 0.7
    start_y = ay + dir_y * ar * 0.7
    end_x = bx - dir_x * br * 0.7
    end_y = by - dir_y * br * 0.7

    # Z is average of the two rooms
    wall_z = (az + bz) / 2.0

    half_w = width / 2.0

    left_wall = {
        "start": [start_x + perp_x * half_w, start_y + perp_y * half_w, wall_z],
        "end":   [end_x + perp_x * half_w, end_y + perp_y * half_w, wall_z],
        "thickness": 200,
        "naturalRock": True,
        "tag": f"corridor_{room_a['groupId']}_{room_b['groupId']}_left"
    }

    right_wall = {
        "start": [start_x - perp_x * half_w, start_y - perp_y * half_w, wall_z],
        "end":   [end_x - perp_x * half_w, end_y - perp_y * half_w, wall_z],
        "thickness": 200,
        "naturalRock": True,
        "tag": f"corridor_{room_a['groupId']}_{room_b['groupId']}_right"
    }

    return left_wall, right_wall


def compute_room_perimeter_walls(room, connected_directions, num_segments=12):
    """Create arc wall segments around a room perimeter with gaps for corridors.

    connected_directions: list of (dx, dy) normalized directions toward connected rooms
    """
    cx, cy, cz = room["center"]
    radius = room["radius"]
    walls = []

    # Create wall arcs every 30 degrees, skip arcs that face a corridor
    for i in range(num_segments):
        angle_start = (2 * math.pi * i) / num_segments
        angle_end = (2 * math.pi * (i + 1)) / num_segments
        angle_mid = (angle_start + angle_end) / 2.0

        # Check if this arc faces a corridor (within 35 degrees of a connection direction)
        arc_dir_x = math.cos(angle_mid)
        arc_dir_y = math.sin(angle_mid)

        skip = False
        for cdx, cdy in connected_directions:
            dot = arc_dir_x * cdx + arc_dir_y * cdy
            if dot > 0.82:  # cos(35 degrees) ≈ 0.82
                skip = True
                break

        if skip:
            continue

        # Create wall segment along this arc
        wall_radius = radius * 0.85  # Slightly inside the radius
        sx = cx + math.cos(angle_start) * wall_radius
        sy = cy + math.sin(angle_start) * wall_radius
        ex = cx + math.cos(angle_end) * wall_radius
        ey = cy + math.sin(angle_end) * wall_radius

        walls.append({
            "start": [sx, sy, cz],
            "end": [ex, ey, cz],
            "thickness": 250,
            "naturalRock": True,
            "tag": f"perimeter_{room['groupId']}_{i}"
        })

    return walls


def main():
    # Load cell graph
    with open(CELL_GRAPH_PATH, "r") as f:
        cell_graph = json.load(f)

    origin = cell_graph["origin"]
    groups = cell_graph["groups"]

    # Build lookup by groupId
    room_lookup = {}
    for g in groups:
        room_lookup[g["groupId"]] = g

    # ── Build rooms array ──
    rooms = []
    for g in groups:
        gid = g["groupId"]
        rooms.append({
            "groupId": gid,
            "designation": ROOM_DESIGNATIONS.get(gid, "Corridor"),
            "center": g["center"],
            "radius": g["radius"],
            "connectedRoomIds": g["connections"]
        })

    # ── Build corridors array ──
    corridors = []
    for from_id, to_id, width, is_shortcut in CORRIDORS:
        if from_id not in room_lookup or to_id not in room_lookup:
            print(f"WARNING: Corridor {from_id}->{to_id} references missing room")
            continue

        # Compute waypoints (for now just direct line)
        corridors.append({
            "from": from_id,
            "to": to_id,
            "width": width,
            "waypoints": [],
            "isShortcut": is_shortcut
        })

    # ── Compute wall segments ──
    walls = []

    # 1. Corridor walls (two parallel walls per corridor)
    for from_id, to_id, width, _ in CORRIDORS:
        if from_id not in room_lookup or to_id not in room_lookup:
            continue
        left, right = compute_corridor_walls(room_lookup[from_id], room_lookup[to_id], width)
        walls.append(left)
        walls.append(right)

    # 2. Room perimeter walls (arc segments with corridor gaps)
    # Build connected directions for each room
    connected_dirs = {}
    for from_id, to_id, _, _ in CORRIDORS:
        if from_id in room_lookup and to_id in room_lookup:
            a = room_lookup[from_id]
            b = room_lookup[to_id]
            dx = b["center"][0] - a["center"][0]
            dy = b["center"][1] - a["center"][1]
            ndx, ndy = normalize_2d(dx, dy)

            if from_id not in connected_dirs:
                connected_dirs[from_id] = []
            connected_dirs[from_id].append((ndx, ndy))

            if to_id not in connected_dirs:
                connected_dirs[to_id] = []
            connected_dirs[to_id].append((-ndx, -ndy))

    # Only add perimeter walls to important rooms (boss, treasure, puzzle, hidden, entrance)
    perimeter_rooms = [gid for gid, desig in ROOM_DESIGNATIONS.items()
                       if desig in ("BossArena", "Treasure", "HiddenRoom", "PuzzleRoom", "Entrance")]

    for gid in perimeter_rooms:
        if gid in room_lookup:
            dirs = connected_dirs.get(gid, [])
            perimeter = compute_room_perimeter_walls(room_lookup[gid], dirs, num_segments=16)
            walls.extend(perimeter)

    # 3. Blocking walls — seal off direct connections we don't want traversable
    # Block: 0↔18 (force long route), 5↔28 (no shortcut), 22↔13 (force through 19)
    BLOCKED_CONNECTIONS = [
        (0, 18),   # Don't let players go directly from treasure to trap room
        (5, 28),   # Force through the junction
    ]

    for a_id, b_id in BLOCKED_CONNECTIONS:
        if a_id in room_lookup and b_id in room_lookup:
            a = room_lookup[a_id]
            b = room_lookup[b_id]
            # Place a thick wall at the midpoint perpendicular to the connection
            mx = (a["center"][0] + b["center"][0]) / 2
            my = (a["center"][1] + b["center"][1]) / 2
            mz = (a["center"][2] + b["center"][2]) / 2

            dx = b["center"][0] - a["center"][0]
            dy = b["center"][1] - a["center"][1]
            ndx, ndy = normalize_2d(dx, dy)

            # Wall perpendicular to connection, 2000 units wide
            wall_half = 1000
            walls.append({
                "start": [mx + (-ndy) * wall_half, my + ndx * wall_half, mz],
                "end":   [mx - (-ndy) * wall_half, my - ndx * wall_half, mz],
                "thickness": 400,
                "naturalRock": True,
                "tag": f"blocker_{a_id}_{b_id}"
            })

    # ── Assemble layout ──
    layout = {
        "dungeonIndex": 1,
        "origin": origin,
        "extent": [7000, 6000, 1500],
        "rooms": rooms,
        "corridors": corridors,
        "walls": walls
    }

    # Write output
    os.makedirs(os.path.dirname(LAYOUT_OUTPUT_PATH), exist_ok=True)
    with open(LAYOUT_OUTPUT_PATH, "w") as f:
        json.dump(layout, f, indent="\t")

    print(f"Layout generated: {len(rooms)} rooms, {len(corridors)} corridors, {len(walls)} walls")
    print(f"Output: {LAYOUT_OUTPUT_PATH}")

    # Print summary
    print("\n── Room Designations ──")
    for r in rooms:
        print(f"  Group {r['groupId']:2d}: {r['designation']:15s} at ({r['center'][0]:.0f}, {r['center'][1]:.0f}, {r['center'][2]:.0f}) r={r['radius']:.0f}")

    print(f"\n── Corridors ({len(corridors)}) ──")
    for c in corridors:
        shortcut = " [SHORTCUT]" if c["isShortcut"] else ""
        print(f"  {c['from']:2d} → {c['to']:2d} (w={c['width']}){shortcut}")

    print(f"\n── Walls ({len(walls)}) ──")
    corridor_walls = sum(1 for w in walls if w["tag"].startswith("corridor_"))
    perimeter_walls = sum(1 for w in walls if w["tag"].startswith("perimeter_"))
    blocker_walls = sum(1 for w in walls if w["tag"].startswith("blocker_"))
    print(f"  Corridor walls: {corridor_walls}")
    print(f"  Perimeter walls: {perimeter_walls}")
    print(f"  Blocker walls: {blocker_walls}")


if __name__ == "__main__":
    main()
