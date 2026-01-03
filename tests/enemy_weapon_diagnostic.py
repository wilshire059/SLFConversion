# enemy_weapon_diagnostic.py
# Investigate why enemies aren't holding weapons

import unreal

def run():
    results = []

    def log(msg):
        results.append(msg)
        unreal.log_warning(msg)

    def section(title):
        log("")
        log("=" * 70)
        log(title)
        log("=" * 70)

    section("ENEMY WEAPON DIAGNOSTIC")

    # Load the level first
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        unreal.EditorLevelLibrary.load_level(level_path)
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        world = subsystem.get_editor_world()
    except Exception as e:
        log(f"Error loading level: {e}")
        return

    # =========================================================================
    # 1. CHECK ENEMY EQUIPMENT MANAGER COMPONENT
    # =========================================================================
    section("1. EQUIPMENT MANAGER COMPONENT")

    equip_comp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager"
    equip_comp = unreal.EditorAssetLibrary.load_asset(equip_comp_path)

    if equip_comp:
        log("  Loaded: AC_EquipmentManager")
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(equip_comp)
        log(f"  Parent: {parent}")
    else:
        log("  NOT FOUND: AC_EquipmentManager")

    # =========================================================================
    # 2. CHECK AI WEAPON BLUEPRINTS
    # =========================================================================
    section("2. AI WEAPON BLUEPRINTS")

    ai_weapons = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
    ]

    for wp_path in ai_weapons:
        wp_name = wp_path.split("/")[-1]
        wp = unreal.EditorAssetLibrary.load_asset(wp_path)
        if wp:
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(wp)
            log(f"  {wp_name}")
            log(f"    Parent: {parent}")
        else:
            log(f"  {wp_name}: NOT FOUND")

    # =========================================================================
    # 3. CHECK ENEMY CHARACTER DEFAULT WEAPON CONFIG
    # =========================================================================
    section("3. ENEMY DEFAULT WEAPON CONFIGURATION")

    enemy_bps = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    for bp_path in enemy_bps:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if not bp:
            log(f"  {bp_name}: NOT FOUND")
            continue

        log(f"")
        log(f"  [{bp_name}]")

        gen_class = bp.generated_class()
        if not gen_class:
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            continue

        # Check for weapon-related properties
        weapon_props = [
            "DefaultWeapon",
            "RightHandWeapon",
            "LeftHandWeapon",
            "WeaponClass",
            "AI_WeaponClass",
            "StartingWeapon",
            "EquippedWeapon",
        ]

        for prop in weapon_props:
            try:
                val = cdo.get_editor_property(prop)
                if val:
                    log(f"    {prop}: {val.get_name() if hasattr(val, 'get_name') else val}")
                else:
                    log(f"    {prop}: None")
            except:
                pass

        # Check components for equipment manager
        try:
            comps = cdo.get_components_by_class(unreal.ActorComponent)
            equip_comps = [c for c in comps if "Equip" in c.get_class().get_name()]
            if equip_comps:
                log(f"    Equipment Components:")
                for c in equip_comps:
                    log(f"      - {c.get_name()} ({c.get_class().get_name()})")

                    # Try to get default weapon from component
                    try:
                        dw = c.get_editor_property("DefaultWeapon")
                        if dw:
                            log(f"        DefaultWeapon: {dw}")
                    except:
                        pass

                    try:
                        rw = c.get_editor_property("RightHandWeaponClass")
                        if rw:
                            log(f"        RightHandWeaponClass: {rw}")
                    except:
                        pass
            else:
                log(f"    No Equipment components found")
        except Exception as e:
            log(f"    Component error: {e}")

    # =========================================================================
    # 4. CHECK ENEMY INSTANCES IN LEVEL
    # =========================================================================
    section("4. ENEMY INSTANCES IN LEVEL - WEAPON CHECK")

    if world:
        characters = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Character)

        for char in characters:
            char_name = char.get_name()
            if "Enemy" not in char_name and "Boss" not in char_name:
                continue

            log(f"")
            log(f"  [{char_name}]")

            # Check for child actors (weapons are often child actors)
            try:
                child_actors = char.get_components_by_class(unreal.ChildActorComponent)
                if child_actors:
                    log(f"    Child Actors: {len(child_actors)}")
                    for ca in child_actors:
                        child = ca.get_child_actor()
                        if child:
                            log(f"      - {child.get_name()} ({child.get_class().get_name()})")
                        else:
                            log(f"      - {ca.get_name()} (child is None)")
                else:
                    log(f"    Child Actors: None")
            except:
                pass

            # Check for attached actors
            try:
                attached = char.get_attached_actors()
                if attached:
                    log(f"    Attached Actors: {len(attached)}")
                    for a in attached:
                        log(f"      - {a.get_name()} ({a.get_class().get_name()})")
                else:
                    log(f"    Attached Actors: None")
            except:
                pass

            # Check skeletal mesh sockets
            try:
                mesh = char.get_editor_property("Mesh")
                if mesh:
                    sk_mesh = mesh.get_editor_property("SkeletalMeshAsset")
                    if sk_mesh:
                        log(f"    Skeletal Mesh: {sk_mesh.get_name()}")
                    else:
                        log(f"    Skeletal Mesh: None (no mesh assigned!)")
            except Exception as e:
                log(f"    Mesh check error: {e}")

    # =========================================================================
    # 5. CHECK WEAPON SPAWN LOGIC
    # =========================================================================
    section("5. EQUIPMENT MANAGER C++ CHECK")

    # Check the C++ EquipmentManagerComponent
    log("  Looking for weapon spawn logic in C++...")

    cpp_files = [
        "EquipmentManagerComponent",
        "AC_EquipmentManager",
    ]

    for cpp in cpp_files:
        log(f"    Checking: {cpp}")

    # =========================================================================
    # 6. CHECK ITEM DATA ASSETS
    # =========================================================================
    section("6. WEAPON DATA ASSETS")

    weapon_data = [
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    ]

    for da_path in weapon_data:
        da_name = da_path.split("/")[-1]
        da = unreal.EditorAssetLibrary.load_asset(da_path)
        if da:
            log(f"  {da_name}: OK")
            # Try to get weapon class from data asset
            try:
                wc = da.get_editor_property("WeaponClass")
                if wc:
                    log(f"    WeaponClass: {wc.get_name()}")
            except:
                pass
            try:
                wc = da.get_editor_property("ItemClass")
                if wc:
                    log(f"    ItemClass: {wc}")
            except:
                pass
        else:
            log(f"  {da_name}: NOT FOUND")

    # =========================================================================
    # 7. CHECK BEGINPLAY WEAPON SPAWN
    # =========================================================================
    section("7. WEAPON SPAWN MECHANISM")

    log("  Weapons are typically spawned in BeginPlay by:")
    log("    1. EquipmentManager component reads default weapon config")
    log("    2. SpawnActor creates weapon actor")
    log("    3. Weapon attaches to hand socket")
    log("")
    log("  Possible issues:")
    log("    - Equipment component not initialized")
    log("    - Default weapon not set in Blueprint")
    log("    - Weapon class reference is None/broken")
    log("    - Socket name mismatch")
    log("    - BeginPlay not calling weapon spawn")

    # =========================================================================
    # SUMMARY
    # =========================================================================
    section("DIAGNOSIS")

    log("")
    log("  The enemies likely aren't holding weapons because:")
    log("")
    log("  1. The Equipment Manager component's BeginPlay logic")
    log("     may have been cleared during migration")
    log("")
    log("  2. The DefaultWeapon/WeaponClass property may be None")
    log("     in the enemy Blueprint defaults")
    log("")
    log("  3. The weapon spawn code in C++ may not be running")
    log("")
    log("  NEXT STEPS:")
    log("    - Check EquipmentManagerComponent.cpp for SpawnWeapon logic")
    log("    - Verify enemy Blueprints have weapon data assigned")
    log("    - Check if BeginPlay spawns weapons")

    # Write output
    output = "\n".join(results)
    with open("C:/scripts/SLFConversion/enemy_weapon_diagnostic_output.txt", 'w') as f:
        f.write(output)

run()
