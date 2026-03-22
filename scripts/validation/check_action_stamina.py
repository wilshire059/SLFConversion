"""Check stamina costs on action data assets"""
import unreal

# Find all action data assets
action_paths = [
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_ComboLight_L",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_ComboLight_R",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_ComboHeavy",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Jump",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_JumpAttack",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_SprintAttack",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Backstab",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_DualWieldAttack",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Projectile",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_UseEquippedTool",
    "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_WeaponAbility",
]

output_lines = []
output_lines.append("=== ACTION STAMINA COSTS ===")
output_lines.append("")

for path in action_paths:
    asset = unreal.load_asset(path)
    if asset:
        name = asset.get_name()
        stamina_cost = 0.0
        try:
            stamina_cost = asset.get_editor_property("stamina_cost")
        except:
            pass
        output_lines.append(f"  {name}: StaminaCost = {stamina_cost}")
        unreal.log(f"  {name}: StaminaCost = {stamina_cost}")
    else:
        output_lines.append(f"  {path.split('/')[-1]}: NOT FOUND")
        unreal.log(f"  {path.split('/')[-1]}: NOT FOUND")

output_lines.append("")
output_lines.append("=== END ===")

# Write to file
with open("C:/scripts/SLFConversion/migration_cache/action_stamina_costs.txt", "w") as f:
    f.write("\n".join(output_lines))

unreal.log("Wrote results to migration_cache/action_stamina_costs.txt")
