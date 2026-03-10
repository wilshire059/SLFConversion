"""
Fix boss weapon mesh and check status effect icons from backup.
Run on bp_only to extract the data first.
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/boss_weapon_extract.txt"

def extract_boss_weapon_data():
    output = []
    output.append("=== EXTRACTING BOSS WEAPON DATA FROM BACKUP ===\n")

    # Check boss weapon in bp_only
    weapon_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace"
    weapon_bp = unreal.EditorAssetLibrary.load_asset(weapon_path)

    if weapon_bp:
        output.append(f"Weapon Blueprint: {weapon_bp.get_name()}")

        gen_class = weapon_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            output.append(f"  CDO: {cdo.get_name()}")

            # Check SCS for mesh configuration
            output.append("\n--- SCS Components ---")
            scs = weapon_bp.simple_construction_script
            if scs:
                nodes = scs.get_all_nodes()
                for node in nodes:
                    if node.component_template:
                        comp = node.component_template
                        comp_name = comp.get_name()
                        comp_class = comp.get_class().get_name()
                        output.append(f"  {comp_name} ({comp_class})")

                        if comp_class == "StaticMeshComponent":
                            try:
                                mesh = comp.get_editor_property("static_mesh")
                                output.append(f"    StaticMesh: {mesh.get_path_name() if mesh else '(None)'}")
                            except Exception as e:
                                output.append(f"    StaticMesh: Error - {e}")

            # Check CDO components
            output.append("\n--- CDO Components ---")
            try:
                comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                for comp in comps:
                    mesh = comp.get_editor_property("static_mesh")
                    output.append(f"  {comp.get_name()}: {mesh.get_path_name() if mesh else '(None)'}")
            except Exception as e:
                output.append(f"  Error: {e}")

    # Check other AI weapons for reference
    output.append("\n\n=== OTHER AI WEAPONS ===\n")
    ai_weapons = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_DemoSword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_DemoShield",
    ]

    for path in ai_weapons:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if bp:
            output.append(f"\n{bp.get_name()}:")
            gen_class = bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                try:
                    comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                    for comp in comps:
                        mesh = comp.get_editor_property("static_mesh")
                        output.append(f"  {comp.get_name()}: {mesh.get_path_name() if mesh else '(None)'}")
                except:
                    pass

                # Check SCS
                scs = bp.simple_construction_script
                if scs:
                    nodes = scs.get_all_nodes()
                    for node in nodes:
                        if node.component_template:
                            comp = node.component_template
                            if comp.get_class().get_name() == "StaticMeshComponent":
                                try:
                                    mesh = comp.get_editor_property("static_mesh")
                                    output.append(f"  SCS {comp.get_name()}: {mesh.get_path_name() if mesh else '(None)'}")
                                except:
                                    pass

    # Check status effect icons
    output.append("\n\n=== STATUS EFFECT ICONS ===\n")
    status_effects = [
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
    ]

    for path in status_effects:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            output.append(f"\n{asset.get_name()}:")
            # Try to get ItemInformation which may have the icon
            try:
                # Check all properties
                for prop_name in dir(asset):
                    if 'icon' in prop_name.lower():
                        val = getattr(asset, prop_name, None)
                        output.append(f"  {prop_name}: {val}")
            except:
                pass

            # Try direct property access
            try:
                icon = asset.get_editor_property("icon")
                if icon:
                    output.append(f"  Icon: {icon.get_path_name()}")
                else:
                    output.append(f"  Icon: (None)")
            except Exception as e:
                output.append(f"  Icon error: {e}")

    result = '\n'.join(output)

    with open(OUTPUT_FILE, 'w') as f:
        f.write(result)

    unreal.log_warning(result)
    unreal.log_warning(f"Results written to {OUTPUT_FILE}")

if __name__ == "__main__":
    extract_boss_weapon_data()
