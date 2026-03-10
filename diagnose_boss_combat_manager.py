import unreal

def log(msg):
    print(f"[DiagBoss] {msg}")
    unreal.log_warning(f"[DiagBoss] {msg}")

log("=" * 70)
log("DIAGNOSING BOSS COMBAT MANAGER COMPONENT")
log("=" * 70)

# 1. Check what components the boss has
boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
log(f"\n1. Loading boss: {boss_path}")
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
                full_name = comp.get_class().get_full_name()
                log(f"  - {comp.get_name()}: {class_name}")
                if "combat" in class_name.lower() or "ai" in class_name.lower():
                    log(f"    FULL: {full_name}")
                    # Check parent class
                    parent = comp.get_class().get_super_class()
                    if parent:
                        log(f"    PARENT: {parent.get_name()}")
                        grandparent = parent.get_super_class()
                        if grandparent:
                            log(f"    GRANDPARENT: {grandparent.get_name()}")
else:
    log("ERROR: Could not load boss Blueprint")

# 2. Check AC_AI_CombatManager Blueprint parent
log("\n2. Checking AC_AI_CombatManager Blueprint:")
ac_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager"
ac_bp = unreal.EditorAssetLibrary.load_asset(ac_path)
if ac_bp:
    log(f"Loaded: {ac_bp.get_name()}")
    log(f"Class: {ac_bp.get_class().get_name()}")

    # Get parent class
    try:
        parent_class = unreal.SLFAutomationLibrary.get_blueprint_parent_class(ac_bp)
        log(f"Parent class: {parent_class}")
    except:
        pass

    # Try generated class
    try:
        gen = ac_bp.generated_class()
        if gen:
            log(f"Generated class: {gen.get_name()}")
            parent = gen.get_super_class()
            if parent:
                log(f"Generated parent: {parent.get_name()}")
    except Exception as e:
        log(f"Error: {e}")
else:
    log("ERROR: Could not load AC_AI_CombatManager")

# 3. Check C++ class exists
log("\n3. Checking C++ classes:")
try:
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.AC_AI_CombatManager")
    log(f"UAC_AI_CombatManager exists: {cpp_class is not None}")
    if cpp_class:
        log(f"  Class name: {cpp_class.get_name()}")
except Exception as e:
    log(f"UAC_AI_CombatManager error: {e}")

try:
    cpp_class2 = unreal.load_class(None, "/Script/SLFConversion.AICombatManagerComponent")
    log(f"UAICombatManagerComponent exists: {cpp_class2 is not None}")
    if cpp_class2:
        log(f"  Class name: {cpp_class2.get_name()}")
except Exception as e:
    log(f"UAICombatManagerComponent error: {e}")

log("\n" + "=" * 70)
log("DONE")
log("=" * 70)
