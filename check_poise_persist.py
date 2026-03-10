import unreal

def log(msg):
    print(f"[PoisePersist] {msg}")
    unreal.log_warning(f"[PoisePersist] {msg}")

log("=" * 70)
log("CHECK POISE BREAK ASSET PERSISTENCE")
log("=" * 70)

# Force reload the boss Blueprint to check if value persisted
boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"

# First unload if loaded
#unreal.EditorAssetLibrary.unload_asset(boss_path)

# Now load fresh
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

if boss_bp:
    boss_class = boss_bp.generated_class()
    if boss_class:
        boss_cdo = unreal.get_default_object(boss_class)
        components = boss_cdo.get_components_by_class(unreal.ActorComponent)

        for comp in components:
            comp_class = comp.get_class().get_name()
            if "AC_AI_CombatManager" in comp_class:
                log(f"Found: {comp.get_name()} ({comp_class})")
                try:
                    poise = comp.get_editor_property("poise_break_asset")
                    log(f"  PoiseBreakAsset: {poise}")
                    if poise:
                        log(f"    Class: {poise.get_class().get_name()}")
                        log(f"    Path: {poise.get_path_name()}")
                except Exception as e:
                    log(f"  Error: {e}")

# Also check if we need to spawn an instance
log("")
log("Spawning test instance to check runtime value...")

# Get editor world
editor_world = unreal.EditorLevelLibrary.get_editor_world()
if editor_world:
    # Can't easily spawn in editor world from commandlet
    log("  (Cannot spawn in commandlet mode)")
else:
    log("  No editor world available")

log("=" * 70)
