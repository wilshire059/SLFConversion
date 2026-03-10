"""
Apply data needed for throwing knives to work:
1. DA_Action_Projectile needs ActionMontage set to throw montage
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
OUTPUT_FILE = "C:/scripts/slfconversion/migration_cache/throwable_data_result.txt"

def log(msg):
    print(msg)
    unreal.log(msg)

def check_and_apply():
    results = []
    results.append("\n" + "="*60)
    results.append("THROWING KNIFE DATA CHECK AND FIX")
    results.append("="*60 + "\n")

    modified = []

    # 1. Check DA_Action_Projectile
    results.append("--- DA_Action_Projectile ---")
    action = unreal.EditorAssetLibrary.load_asset(ACTION_PATH)
    if action:
        results.append(f"Loaded: {action.get_name()}")

        # Check if ActionMontage is set
        try:
            action_montage = action.get_editor_property('action_montage')
            results.append(f"  ActionMontage: {action_montage}")

            # If ActionMontage is None or invalid, set it
            if not action_montage or str(action_montage) == "None" or "None" in str(action_montage):
                montage = unreal.EditorAssetLibrary.load_asset(THROW_MONTAGE_PATH)
                if montage:
                    action.set_editor_property('action_montage', montage)
                    results.append(f"  -> Applied ActionMontage: {THROW_MONTAGE_PATH}")
                    modified.append(ACTION_PATH)
                else:
                    results.append(f"  ERROR: Could not load throw montage from {THROW_MONTAGE_PATH}")
            else:
                results.append("  ActionMontage already set")

        except Exception as e:
            results.append(f"  ActionMontage error: {e}")
    else:
        results.append(f"NOT FOUND: {ACTION_PATH}")

    # 2. Check DA_ThrowingKnife
    results.append("\n--- DA_ThrowingKnife ---")
    knife = unreal.EditorAssetLibrary.load_asset(KNIFE_PATH)
    knife_path = KNIFE_PATH
    if not knife:
        knife = unreal.EditorAssetLibrary.load_asset(ALT_KNIFE_PATH)
        if knife:
            knife_path = ALT_KNIFE_PATH
        else:
            results.append(f"NOT FOUND: {KNIFE_PATH} or {ALT_KNIFE_PATH}")
            knife_path = None

    if knife:
        results.append(f"Loaded: {knife.get_name()}")

        try:
            item_info = knife.get_editor_property('item_information')

            # Check ItemClass
            try:
                item_class = item_info.get_editor_property('item_class')
                results.append(f"  ItemClass: {item_class}")

                if not item_class or str(item_class) == "None" or "None" in str(item_class):
                    # Load projectile class
                    proj_bp = unreal.EditorAssetLibrary.load_asset(PROJECTILE_CLASS_PATH)
                    if proj_bp:
                        # Get generated class
                        gen_class = proj_bp.generated_class()
                        if gen_class:
                            item_info.set_editor_property('item_class', gen_class)
                            results.append(f"  -> Applied ItemClass: {gen_class.get_name()}")
                            modified.append(knife_path)
                        else:
                            results.append(f"  ERROR: Could not get generated class from {PROJECTILE_CLASS_PATH}")
                    else:
                        results.append(f"  ERROR: Could not load projectile BP from {PROJECTILE_CLASS_PATH}")
                else:
                    results.append("  ItemClass already set")

            except Exception as e:
                results.append(f"  ItemClass error: {e}")

            # Check SubCategory
            try:
                category_info = item_info.get_editor_property('category')
                subcategory = category_info.get_editor_property('sub_category')
                results.append(f"  SubCategory: {subcategory}")

                # Check if it's Projectiles
                if "PROJECTILES" not in str(subcategory).upper():
                    results.append("  WARNING: SubCategory is NOT Projectiles - attempting to set...")
                    try:
                        # Try setting by enum value
                        category_info.set_editor_property('sub_category', unreal.ESLFItemSubCategory.PROJECTILES)
                        results.append("  -> Applied SubCategory: Projectiles")
                        if knife_path not in modified:
                            modified.append(knife_path)
                    except Exception as e:
                        results.append(f"  Could not set SubCategory: {e}")
                else:
                    results.append("  SubCategory is Projectiles - OK")

            except Exception as e:
                results.append(f"  SubCategory error: {e}")

        except Exception as e:
            results.append(f"  ItemInformation error: {e}")

    # 3. Save modified assets
    results.append("\n--- Saving ---")
    for path in modified:
        try:
            unreal.EditorAssetLibrary.save_asset(path)
            results.append(f"Saved: {path}")
        except Exception as e:
            results.append(f"Error saving {path}: {e}")

    results.append("\n" + "="*60)
    results.append(f"Done. Modified {len(modified)} assets.")
    results.append("="*60 + "\n")

    # Write to file and log
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

    for line in results:
        log(line)

    return modified

if __name__ == "__main__":
    check_and_apply()
