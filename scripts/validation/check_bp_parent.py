# check_bp_parent.py
import unreal

def log(msg):
    unreal.log_warning(str(msg))

WEAPONS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
]

def check():
    log("=" * 70)

    for bp_path in WEAPONS:
        bp_name = bp_path.split("/")[-1]

        bp = unreal.load_asset(bp_path)
        if not bp:
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            continue

        # Use BlueprintEditorLibrary
        try:
            parent = unreal.BlueprintEditorLibrary.get_blueprint_parent(bp)
            log(f"[{bp_name}] Parent: {parent.get_name() if parent else 'None'}")
            if parent:
                log(f"  Parent path: {parent.get_path_name()}")
        except Exception as e:
            log(f"[{bp_name}] BlueprintEditorLibrary error: {e}")

        # Also try generated class super
        try:
            super_class = gen_class.static_class().get_super_class()
            if super_class:
                log(f"  SuperClass: {super_class.get_name()}")
        except:
            pass

if __name__ == "__main__":
    check()
