"""
Apply data needed for throwing knives to work:
1. DA_Action_Projectile needs RelevantData containing FSLFMontage with throw montage
2. DA_ThrowingKnife needs ItemClass = B_Projectile_ThrowingKnife
3. DA_ThrowingKnife needs SubCategory = Projectiles
"""
import unreal

# Paths
ACTION_PATH = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Projectile"
KNIFE_PATH = "/Game/SoulslikeFramework/Data/Items/DA_ThrowingKnife"
ALT_KNIFE_PATH = "/Game/SoulslikeFramework/Data/Items/Consumables/DA_ThrowingKnife"
THROW_MONTAGE_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Combat/OneHanded/AM_SLF_ThrowProjectile"
PROJECTILE_CLASS_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_Projectile_ThrowingKnife"

def check_and_apply():
    print("\n" + "="*60)
    print("THROWING KNIFE DATA CHECK AND FIX")
    print("="*60 + "\n")

    modified = []

    # 1. Check DA_Action_Projectile
    print("--- DA_Action_Projectile ---")
    action = unreal.EditorAssetLibrary.load_asset(ACTION_PATH)
    if action:
        print(f"Loaded: {action.get_name()}")

        # Check if ActionMontage is set
        try:
            action_montage = action.get_editor_property('action_montage')
            print(f"  ActionMontage: {action_montage}")

            # If ActionMontage is None or invalid, set it
            if not action_montage or str(action_montage) == "None":
                montage = unreal.EditorAssetLibrary.load_asset(THROW_MONTAGE_PATH)
                if montage:
                    action.set_editor_property('action_montage', montage)
                    print(f"  -> Applied ActionMontage: {THROW_MONTAGE_PATH}")
                    modified.append(ACTION_PATH)
                else:
                    print(f"  ERROR: Could not load throw montage from {THROW_MONTAGE_PATH}")
            else:
                print("  ActionMontage already set")

        except Exception as e:
            print(f"  ActionMontage error: {e}")
    else:
        print(f"NOT FOUND: {ACTION_PATH}")

    # 2. Check DA_ThrowingKnife
    print("\n--- DA_ThrowingKnife ---")
    knife = unreal.EditorAssetLibrary.load_asset(KNIFE_PATH)
    if not knife:
        knife = unreal.EditorAssetLibrary.load_asset(ALT_KNIFE_PATH)
        if knife:
            knife_path = ALT_KNIFE_PATH
        else:
            print(f"NOT FOUND: {KNIFE_PATH} or {ALT_KNIFE_PATH}")
            knife_path = None
    else:
        knife_path = KNIFE_PATH

    if knife:
        print(f"Loaded: {knife.get_name()}")

        try:
            item_info = knife.get_editor_property('item_information')

            # Check ItemClass
            try:
                item_class = item_info.get_editor_property('item_class')
                print(f"  ItemClass: {item_class}")

                if not item_class or str(item_class) == "None":
                    # Load projectile class
                    proj_class = unreal.EditorAssetLibrary.load_asset(PROJECTILE_CLASS_PATH)
                    if proj_class:
                        # Get generated class
                        gen_class = proj_class.generated_class()
                        if gen_class:
                            item_info.set_editor_property('item_class', gen_class)
                            print(f"  -> Applied ItemClass: {PROJECTILE_CLASS_PATH}")
                            modified.append(knife_path)
                    else:
                        print(f"  ERROR: Could not load projectile class from {PROJECTILE_CLASS_PATH}")
                else:
                    print("  ItemClass already set")

            except Exception as e:
                print(f"  ItemClass error: {e}")

            # Check SubCategory
            try:
                category_info = item_info.get_editor_property('category')
                subcategory = category_info.get_editor_property('sub_category')
                print(f"  SubCategory: {subcategory}")

                # Check if it's Projectiles (enum value 2)
                if "PROJECTILES" not in str(subcategory).upper():
                    print("  WARNING: SubCategory is NOT Projectiles - attempting to set...")
                    try:
                        # ESLFItemSubCategory.Projectiles = 2
                        category_info.set_editor_property('sub_category', 2)
                        print("  -> Applied SubCategory: Projectiles")
                        if knife_path not in modified:
                            modified.append(knife_path)
                    except Exception as e:
                        print(f"  Could not set SubCategory: {e}")
                else:
                    print("  SubCategory is Projectiles - OK")

            except Exception as e:
                print(f"  SubCategory error: {e}")

        except Exception as e:
            print(f"  ItemInformation error: {e}")

    # 3. Save modified assets
    print("\n--- Saving ---")
    for path in modified:
        try:
            unreal.EditorAssetLibrary.save_asset(path)
            print(f"Saved: {path}")
        except Exception as e:
            print(f"Error saving {path}: {e}")

    print("\n" + "="*60)
    print(f"Done. Modified {len(modified)} assets.")
    print("="*60 + "\n")

if __name__ == "__main__":
    check_and_apply()
