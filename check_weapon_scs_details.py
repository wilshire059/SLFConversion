#!/usr/bin/env python3
"""Extract detailed StaticMeshComponent settings from weapon Blueprints"""

import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def check_weapons():
    log("=" * 70)
    log("WEAPON SCS COMPONENT DETAILS")
    log("=" * 70)

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load Blueprint")
            continue

        # Spawn a temporary actor to get component details
        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  ERROR: No generated class")
            continue

        # Get CDO to check component templates
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"  CDO: {cdo.get_name()}")

            # Get all components from CDO
            components = cdo.get_components_by_class(unreal.StaticMeshComponent)
            log(f"  Found {len(components)} StaticMeshComponent(s)")

            for i, comp in enumerate(components):
                log(f"\n  --- Component {i}: {comp.get_name()} ---")

                # Get mesh
                mesh = comp.get_editor_property("static_mesh")
                log(f"    StaticMesh: {mesh.get_name() if mesh else 'None'}")

                # Get transforms
                rel_loc = comp.get_editor_property("relative_location")
                rel_rot = comp.get_editor_property("relative_rotation")
                rel_scale = comp.get_editor_property("relative_scale3d")

                log(f"    RelativeLocation: X={rel_loc.x}, Y={rel_loc.y}, Z={rel_loc.z}")
                log(f"    RelativeRotation: P={rel_rot.pitch}, Y={rel_rot.yaw}, R={rel_rot.roll}")
                log(f"    RelativeScale3D: X={rel_scale.x}, Y={rel_scale.y}, Z={rel_scale.z}")

                # Get attachment info
                attach_parent = comp.get_attach_parent()
                attach_socket = comp.get_attach_socket_name()
                log(f"    AttachParent: {attach_parent.get_name() if attach_parent else 'None'}")
                log(f"    AttachSocket: {attach_socket}")

                # Get visibility settings
                visible = comp.get_editor_property("visible")
                hidden_in_game = comp.get_editor_property("hidden_in_game")
                log(f"    Visible: {visible}, HiddenInGame: {hidden_in_game}")

                # Get collision settings
                collision_enabled = comp.get_editor_property("collision_enabled")
                log(f"    CollisionEnabled: {collision_enabled}")

                # Get component mobility
                mobility = comp.get_editor_property("mobility")
                log(f"    Mobility: {mobility}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    check_weapons()
