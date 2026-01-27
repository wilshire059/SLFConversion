# check_component_classes.py
# Check the parent classes of the component Blueprints

import unreal

def log(msg):
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    log("=" * 70)
    log("COMPONENT CLASS CHECK")
    log("=" * 70)

    components = [
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager", "UAC_ActionManager"),
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager", "UAC_CombatManager"),
        ("/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager", "UAC_EquipmentManager"),
    ]

    for bp_path, expected_parent in components:
        log(f"\n{bp_path}:")
        try:
            bp = unreal.load_asset(bp_path)
            if bp:
                parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
                log(f"  Parent: {parent}")
                log(f"  Expected: /Script/SLFConversion.{expected_parent[1:]}")  # Remove U prefix
            else:
                log("  NOT FOUND")
        except Exception as e:
            log(f"  ERROR: {e}")

    # Also check the C++ classes exist
    log("\n")
    log("Checking C++ classes:")
    cpp_classes = [
        "/Script/SLFConversion.AC_ActionManager",
        "/Script/SLFConversion.AC_CombatManager",
        "/Script/SLFConversion.AC_EquipmentManager",
    ]
    for class_path in cpp_classes:
        try:
            cpp_class = unreal.load_class(None, class_path)
            if cpp_class:
                log(f"  {class_path}: EXISTS")
            else:
                log(f"  {class_path}: NOT FOUND")
        except Exception as e:
            log(f"  {class_path}: ERROR - {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
