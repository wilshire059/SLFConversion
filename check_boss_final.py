import unreal

def log(msg):
    print(f"[BossFinal] {msg}")
    unreal.log_warning(f"[BossFinal] {msg}")

log("=" * 70)
log("FINAL BOSS COMPONENT CHECK")
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

log("=" * 70)
