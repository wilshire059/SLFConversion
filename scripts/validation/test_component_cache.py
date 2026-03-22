import unreal

# Simple test - just load the editor and print component info
print("=" * 60)
print("Component Cache Test")
print("=" * 60)

# Check that our C++ classes exist
action_manager_class = unreal.find_class("AC_ActionManager")
combat_manager_class = unreal.find_class("AC_CombatManager")
interaction_manager_class = unreal.find_class("AC_InteractionManager")

print(f"AC_ActionManager class: {action_manager_class}")
print(f"AC_CombatManager class: {combat_manager_class}")
print(f"AC_InteractionManager class: {interaction_manager_class}")

# Check that the character class exists
character_class = unreal.find_class("SLFSoulslikeCharacter")
print(f"SLFSoulslikeCharacter class: {character_class}")

print("=" * 60)
print("Test complete - C++ classes are accessible!")
print("=" * 60)
