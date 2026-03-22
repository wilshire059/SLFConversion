import unreal

def log(msg):
    print(f"[CheckEnemy] {msg}")
    unreal.log_warning(f"[CheckEnemy] {msg}")

log("=" * 70)
log("CHECKING ENEMY CLASS HIERARCHY COMPONENTS")
log("=" * 70)

# Check B_Soulslike_Enemy
enemy_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"
log(f"\n1. B_Soulslike_Enemy: {enemy_path}")
enemy_bp = unreal.EditorAssetLibrary.load_asset(enemy_path)

if enemy_bp:
    gen_class = enemy_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            components = cdo.get_components_by_class(unreal.ActorComponent)
            log(f"Enemy has {len(components)} components:")
            for comp in components:
                class_name = comp.get_class().get_name()
                log(f"  - {comp.get_name()}: {class_name}")
                if "combat" in class_name.lower():
                    log(f"    *** FOUND COMBAT MANAGER ***")

# Check B_BaseCharacter
base_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
log(f"\n2. B_BaseCharacter: {base_path}")
base_bp = unreal.EditorAssetLibrary.load_asset(base_path)

if base_bp:
    gen_class = base_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            components = cdo.get_components_by_class(unreal.ActorComponent)
            log(f"BaseCharacter has {len(components)} components:")
            for comp in components:
                class_name = comp.get_class().get_name()
                log(f"  - {comp.get_name()}: {class_name}")
                if "combat" in class_name.lower():
                    log(f"    *** FOUND COMBAT MANAGER ***")

log("\n" + "=" * 70)
