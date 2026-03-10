"""
Check status effect data assets to verify icons and configurations.
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/status_effect_check.txt"

def check_status_effects():
    output = []

    status_effects = [
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite",
    ]

    output.append("=== STATUS EFFECT DATA CHECK ===\n")

    for path in status_effects:
        output.append(f"\n--- {path.split('/')[-1]} ---")

        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            output.append(f"  ERROR: Could not load asset")
            continue

        # Check Tag
        try:
            tag = asset.get_editor_property("tag")
            output.append(f"  Tag: {tag.tag_name if tag else '(None)'}")
        except:
            output.append(f"  Tag: (Error reading)")

        # Check Icon
        try:
            icon = asset.get_editor_property("icon")
            output.append(f"  Icon: {icon.get_asset_name() if icon else '(None)'}")
        except:
            output.append(f"  Icon: (Error reading)")

        # Check BarFillColor
        try:
            color = asset.get_editor_property("bar_fill_color")
            output.append(f"  BarFillColor: R={color.r:.2f}, G={color.g:.2f}, B={color.b:.2f}, A={color.a:.2f}")
        except:
            output.append(f"  BarFillColor: (Error reading)")

        # Check Effect class
        try:
            effect = asset.get_editor_property("effect")
            output.append(f"  Effect Class: {effect.get_asset_name() if effect else '(None)'}")
        except:
            output.append(f"  Effect Class: (Error reading)")

    # Also check boss weapon
    output.append("\n\n=== BOSS WEAPON CHECK ===\n")
    weapon_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace"
    weapon_bp = unreal.EditorAssetLibrary.load_asset(weapon_path)
    if weapon_bp:
        output.append(f"Weapon Blueprint: {weapon_bp.get_name()}")

        gen_class = weapon_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            output.append(f"  CDO: {cdo.get_name()}")

            # Check for static mesh components
            try:
                comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                output.append(f"  StaticMeshComponents: {len(comps)}")
                for comp in comps:
                    mesh = comp.get_editor_property("static_mesh")
                    output.append(f"    - {comp.get_name()}: Mesh={mesh.get_name() if mesh else '(None)'}")
            except Exception as e:
                output.append(f"  StaticMeshComponents: Error - {e}")

            # Check DefaultWeaponMesh property
            try:
                default_mesh = cdo.get_editor_property("default_weapon_mesh")
                output.append(f"  DefaultWeaponMesh: {default_mesh.get_asset_name() if default_mesh else '(None)'}")
            except Exception as e:
                output.append(f"  DefaultWeaponMesh: {e}")

    else:
        output.append(f"ERROR: Could not load weapon Blueprint at {weapon_path}")

    result = '\n'.join(output)

    with open(OUTPUT_FILE, 'w') as f:
        f.write(result)

    unreal.log_warning(result)
    unreal.log_warning(f"Results written to {OUTPUT_FILE}")

if __name__ == "__main__":
    check_status_effects()
