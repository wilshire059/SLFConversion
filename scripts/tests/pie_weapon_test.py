"""
PIE Test - Verify enemy weapons are spawned correctly
"""
import unreal
import time

print("")
print("=" * 70)
print("PIE WEAPON SPAWN TEST")
print("=" * 70)

# Get the editor subsystem for PIE
editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

# Get current world
world = unreal.EditorLevelLibrary.get_editor_world()
if not world:
    print("ERROR: No editor world found")
else:
    print(f"Current Level: {world.get_name()}")

# Find all enemy actors in the level
print("")
print("=" * 70)
print("1. CHECKING ENEMY BLUEPRINTS IN EDITOR")
print("=" * 70)

enemy_classes = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
]

for bp_path in enemy_classes:
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        bp_name = bp.get_name()
        print(f"\n[{bp_name}]")

        # Check parent class
        if hasattr(bp, 'parent_class') and bp.parent_class:
            print(f"  Parent: {bp.parent_class.get_path_name()}")

        # Check for SCS components
        if hasattr(bp, 'simple_construction_script'):
            scs = bp.simple_construction_script
            if scs:
                all_nodes = scs.get_all_nodes()
                print(f"  SCS Nodes: {len(all_nodes)}")
                for node in all_nodes:
                    comp_template = node.component_template
                    if comp_template:
                        comp_class = comp_template.get_class()
                        comp_name = node.get_variable_name()
                        print(f"    - {comp_name}: {comp_class.get_name()}")

                        # If it's a ChildActorComponent, check the child class
                        if comp_class.get_name() == "ChildActorComponent":
                            child_class = comp_template.get_editor_property("child_actor_class")
                            if child_class:
                                print(f"        ChildActorClass: {child_class.get_path_name()}")
                            else:
                                print(f"        ChildActorClass: NONE (not set)")
            else:
                print("  SCS: None")

print("")
print("=" * 70)
print("2. CHECKING WEAPON BLUEPRINTS")
print("=" * 70)

weapon_classes = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
]

for bp_path in weapon_classes:
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        bp_name = bp.get_name()
        gen_class = bp.generated_class()

        print(f"\n[{bp_name}]")
        if hasattr(bp, 'parent_class') and bp.parent_class:
            print(f"  Parent: {bp.parent_class.get_path_name()}")

        # Check SCS for weapon components (StaticMesh/WeaponMesh)
        if hasattr(bp, 'simple_construction_script'):
            scs = bp.simple_construction_script
            if scs:
                all_nodes = scs.get_all_nodes()
                print(f"  SCS Nodes: {len(all_nodes)}")
                for node in all_nodes:
                    comp_template = node.component_template
                    if comp_template:
                        comp_class = comp_template.get_class()
                        comp_name = node.get_variable_name()
                        print(f"    - {comp_name}: {comp_class.get_name()}")

                        # Check if StaticMeshComponent has a mesh assigned
                        if "StaticMesh" in comp_class.get_name():
                            mesh = comp_template.get_editor_property("static_mesh")
                            if mesh:
                                print(f"        Mesh: {mesh.get_name()}")
                            else:
                                print(f"        Mesh: NONE (not set)")

print("")
print("=" * 70)
print("3. CHECKING LEVEL ENEMY INSTANCES")
print("=" * 70)

# Get all actors in level
all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
for actor in all_actors:
    actor_name = actor.get_name()
    if "Enemy" in actor_name or "Boss" in actor_name:
        actor_class = actor.get_class()
        print(f"\n[{actor_name}]")
        print(f"  Class: {actor_class.get_name()}")

        # Get all components
        components = actor.get_components_by_class(unreal.ActorComponent)
        child_actor_comps = []
        weapon_comps = []

        for comp in components:
            comp_class = comp.get_class().get_name()
            if "ChildActor" in comp_class:
                child_actor_comps.append(comp)
            if "Weapon" in comp.get_name() or "weapon" in comp.get_name().lower():
                weapon_comps.append(comp)

        print(f"  ChildActorComponents: {len(child_actor_comps)}")
        for cac in child_actor_comps:
            child_actor = cac.get_editor_property("child_actor")
            child_class = cac.get_editor_property("child_actor_class")
            print(f"    - {cac.get_name()}")
            print(f"        ChildActor: {child_actor.get_name() if child_actor else 'None'}")
            print(f"        ChildActorClass: {child_class.get_path_name() if child_class else 'None'}")

        print(f"  Weapon-related Components: {len(weapon_comps)}")
        for wc in weapon_comps:
            print(f"    - {wc.get_name()}: {wc.get_class().get_name()}")

print("")
print("=" * 70)
print("TEST COMPLETE")
print("=" * 70)
