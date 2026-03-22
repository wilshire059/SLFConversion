# extract_abilities_v2.py
# Extract enemy abilities - check all component types

import unreal

def log(msg):
    unreal.log_warning(f"[EXTRACT] {msg}")

ENEMY_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
]

def extract_abilities(path):
    log(f"")
    log(f"=== {path.split('/')[-1]} ===")

    bp = unreal.load_asset(path)
    if not bp:
        log(f"  ERROR: Could not load")
        return

    log(f"  Blueprint class: {bp.get_class().get_name()}")

    # Check SCS nodes (Blueprint component defaults)
    if hasattr(unreal, 'SLFAutomationLibrary'):
        nodes = unreal.SLFAutomationLibrary.get_scs_nodes(bp)
        log(f"  SCS nodes: {len(nodes) if nodes else 0}")
        if nodes:
            for node in nodes:
                log(f"    - {node}")

    # Get generated class
    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  ERROR: No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log(f"  ERROR: No CDO")
        return

    # List ALL components
    components = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"  Total components: {len(components)}")
    for comp in components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        if "Combat" in comp_name or "Combat" in comp_class or "AI" in comp_name:
            log(f"    * {comp_name} ({comp_class})")

            # List all properties
            try:
                # Check for Abilities with different cases
                for prop_name in ["Abilities", "abilities", "AI_Abilities", "AiAbilities"]:
                    try:
                        val = comp.get_editor_property(prop_name)
                        log(f"      {prop_name}: {len(val) if val else 0} items")
                        if val:
                            for i, item in enumerate(val):
                                log(f"        [{i}] {item}")
                    except:
                        pass
            except Exception as e:
                log(f"      Error: {e}")

def main():
    log("=" * 70)
    log("Extract Abilities v2")
    log("=" * 70)

    for path in ENEMY_PATHS:
        extract_abilities(path)

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
