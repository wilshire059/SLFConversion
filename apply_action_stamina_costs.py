"""
apply_action_stamina_costs.py
Sets stamina costs on action data assets for Souls-like gameplay.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_action_stamina_costs.py" ^
  -stdout -unattended -nosplash
"""

import unreal

# Stamina costs based on user requirements (Souls-like balance)
# Dodge: 20, Light Attack: 15, Heavy Attack: 30, Roll: 25, Sprint Start: 0 (sprint drain is continuous)
STAMINA_COSTS = {
    "DA_Action_Dodge": 20.0,
    "DA_Action_ComboLight_L": 15.0,
    "DA_Action_ComboLight_R": 15.0,
    "DA_Action_ComboHeavy": 30.0,
    "DA_Action_SprintAttack": 25.0,
    "DA_Action_JumpAttack": 25.0,
    "DA_Action_DualWieldAttack": 20.0,
    "DA_Action_WeaponAbility": 30.0,
    "DA_Action_Backstab": 20.0,
    "DA_Action_Execute": 0.0,  # Executions are free
    "DA_Action_GuardStart": 0.0,
    "DA_Action_GuardEnd": 0.0,
    "DA_Action_GuardCancel": 0.0,
    "DA_Action_Jump": 10.0,  # Small cost for jumping
    "DA_Action_Crouch": 0.0,
    "DA_Action_StartSprinting": 0.0,  # Sprint drain is handled separately
    "DA_Action_StopSprinting": 0.0,
    "DA_Action_DrinkFlask_HP": 0.0,  # Healing is free but has animation commitment
    "DA_Action_Projectile": 10.0,
    "DA_Action_PickupItemMontage": 0.0,
    "DA_Action_UseEquippedTool": 5.0,
    "DA_Action_ScrollWheel_LeftHand": 0.0,
    "DA_Action_ScrollWheel_RightHand": 0.0,
    "DA_Action_ScrollWheel_Tools": 0.0,
    "DA_Action_TwoHandedStance_L": 0.0,
    "DA_Action_TwoHandedStance_R": 0.0,
}

def apply_stamina_costs():
    """Apply stamina costs to action data assets."""

    base_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/"

    success_count = 0
    fail_count = 0

    for asset_name, stamina_cost in STAMINA_COSTS.items():
        asset_path = base_path + asset_name

        # Load the asset
        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"SKIP: {asset_name} - Not found at {asset_path}")
            continue

        # Check if it has stamina_cost property
        try:
            current_cost = asset.get_editor_property("stamina_cost")

            if current_cost != stamina_cost:
                asset.set_editor_property("stamina_cost", stamina_cost)

                # Save the asset
                if unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False):
                    unreal.log_warning(f"OK: {asset_name} - Set StaminaCost to {stamina_cost} (was {current_cost})")
                    success_count += 1
                else:
                    unreal.log_error(f"FAIL: {asset_name} - Could not save")
                    fail_count += 1
            else:
                unreal.log_warning(f"SKIP: {asset_name} - Already has StaminaCost = {stamina_cost}")
                success_count += 1

        except Exception as e:
            unreal.log_error(f"ERROR: {asset_name} - {str(e)}")
            fail_count += 1

    unreal.log_warning("")
    unreal.log_warning(f"=== STAMINA COSTS APPLIED ===")
    unreal.log_warning(f"Success: {success_count}")
    unreal.log_warning(f"Failed: {fail_count}")
    unreal.log_warning(f"Total: {len(STAMINA_COSTS)}")

# Run directly (UE Python doesn't always respect __name__ == "__main__")
unreal.log_warning("=== STARTING STAMINA COST APPLICATION ===")
try:
    apply_stamina_costs()
except Exception as e:
    unreal.log_error(f"Script failed with exception: {str(e)}")
    import traceback
    unreal.log_error(traceback.format_exc())
