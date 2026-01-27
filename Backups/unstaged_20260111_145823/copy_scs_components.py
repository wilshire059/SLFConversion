# copy_scs_components.py
# Copies SCS components from B_BaseCharacter to all child Blueprints
# This preserves component data after reparenting B_BaseCharacter to C++
#
# Run with:
# UnrealEditor-Cmd.exe "project.uproject" -run=pythonscript -script="copy_scs_components.py" -stdout -unattended

import unreal

# Child Blueprints that inherit from B_BaseCharacter and need the SCS components
CHILD_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

# Components to copy from B_BaseCharacter (empty = copy all)
COMPONENTS_TO_COPY = [
    "AC_StatManager",
    "AC_StatusEffectManager",
    "AC_BuffManager",
    "TargetLockonComponent",
    "ProjectileHomingPosition",
    "TargetLockonWidget",
    "ExecutionWidget",
]

def copy_scs_components():
    """Copy SCS components from B_BaseCharacter to child Blueprints"""

    print("=" * 60)
    print("SCS Component Copy - B_BaseCharacter to Children")
    print("=" * 60)

    # Load B_BaseCharacter
    parent_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter"
    parent_bp = unreal.load_asset(parent_path)

    if not parent_bp:
        print("ERROR: Could not load B_BaseCharacter")
        return

    # Show parent's SCS components
    print("\n--- B_BaseCharacter SCS Components ---")
    parent_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(parent_bp)
    for comp in parent_comps:
        print("  " + comp)

    print("\n--- Processing Child Blueprints ---")

    total_copied = 0

    # Process each child Blueprint
    for child_path in CHILD_BLUEPRINTS:
        child_bp = unreal.load_asset(child_path)
        if not child_bp:
            print("WARNING: Could not load " + child_path)
            continue

        child_name = child_path.split("/")[-1]
        print("\n" + child_name + ":")

        # Copy SCS components using the C++ function
        copied = unreal.SLFAutomationLibrary.copy_scs_components(
            parent_bp,
            child_bp,
            COMPONENTS_TO_COPY
        )

        print("  Copied " + str(copied) + " components")
        total_copied += copied

        # Compile and save if we copied anything
        if copied > 0:
            print("  Compiling and saving...")
            unreal.SLFAutomationLibrary.compile_and_save(child_bp)

    print("\n" + "=" * 60)
    print("Summary: Copied " + str(total_copied) + " total components")
    print("=" * 60)

    # Verify the result
    print("\n--- Verification ---")
    for child_path in CHILD_BLUEPRINTS:
        child_bp = unreal.load_asset(child_path)
        if child_bp:
            child_name = child_path.split("/")[-1]
            comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(child_bp)
            stat_count = sum(1 for c in comps if "StatManager" in c)
            print(child_name + ": " + str(len(comps)) + " SCS components (StatManager: " + ("YES" if stat_count > 0 else "NO") + ")")

copy_scs_components()
