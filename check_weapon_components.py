import unreal

print("")
print("=" * 70)
print("WEAPON COMPONENT CHECK (Post-Migration)")
print("=" * 70)

# Check weapon Blueprints for components
weapon_bps = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
]

for bp_path in weapon_bps:
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        bp_name = bp.get_name()
        gen_class = bp.generated_class()
        cdo = gen_class.get_class_default_object() if gen_class else None
        
        print(f"\n[{bp_name}]")
        print(f"  Parent: {bp.parent_class.get_path_name() if bp.parent_class else 'None'}")
        print(f"  GeneratedClass: {gen_class.get_name() if gen_class else 'None'}")
        
        if cdo:
            # Check for components from C++ parent
            has_weapon_mesh = hasattr(cdo, 'weapon_mesh') or hasattr(cdo, 'WeaponMesh')
            has_trail = hasattr(cdo, 'trail_component') or hasattr(cdo, 'TrailComponent')
            has_default_root = hasattr(cdo, 'default_scene_root') or hasattr(cdo, 'DefaultSceneRoot')
            
            print(f"  CDO has WeaponMesh: {has_weapon_mesh}")
            print(f"  CDO has TrailComponent: {has_trail}")
            print(f"  CDO has DefaultSceneRoot: {has_default_root}")
            
            # Check for SCS components (Blueprint-defined)
            scs = bp.simple_construction_script
            if scs:
                all_nodes = scs.get_all_nodes()
                print(f"  SCS Nodes: {len(all_nodes)}")
                for node in all_nodes:
                    comp_template = node.component_template
                    if comp_template:
                        comp_class = comp_template.get_class()
                        print(f"    - {node.get_variable_name()}: {comp_class.get_name()}")

print("")
print("=" * 70)
print("CHECK COMPLETE")
print("=" * 70)
print("")
print("Weapons should now inherit WeaponMesh and TrailComponent from C++")
print("The SCS nodes (StaticMesh, Trail) may still exist but won't conflict")
print("")
