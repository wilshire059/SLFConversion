import unreal

def log(msg):
    print(f"[FullBoss] {msg}")
    unreal.log_warning(f"[FullBoss] {msg}")

log("=" * 70)
log("CHECKING BOSS COMPONENTS IN DETAIL")
log("=" * 70)

boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

if boss_bp:
    gen_class = boss_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            components = cdo.get_components_by_class(unreal.ActorComponent)
            log(f"Boss has {len(components)} components:")
            for comp in components:
                class_name = comp.get_class().get_name()
                log(f"  - {comp.get_name()}: {class_name}")
                if "combat" in class_name.lower() or "Combat" in comp.get_name():
                    log(f"    *** FOUND COMBAT ***")

# Try spawning boss to check runtime components
log("\n--- Attempting to spawn boss at runtime ---")
try:
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        log("Got editor world")
        # Get actor class
        actor_class = boss_bp.generated_class()
        if actor_class:
            log(f"Actor class: {actor_class.get_name()}")
except Exception as e:
    log(f"Error: {e}")

log("=" * 70)
