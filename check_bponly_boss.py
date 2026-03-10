import unreal

def log(msg):
    print(f"[BPOnlyBoss] {msg}")
    unreal.log_warning(f"[BPOnlyBoss] {msg}")

log("=" * 70)
log("CHECKING BP_ONLY BOSS COMPONENTS")
log("=" * 70)

boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
log(f"Loading boss: {boss_path}")
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
else:
    log("ERROR: Could not load boss Blueprint")

log("=" * 70)
