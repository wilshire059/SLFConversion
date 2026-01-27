"""
Migrate Settings menu widgets to C++ using smarter clearing:
- Use ClearEventGraphs for widgets with widget bindings (preserves function graphs)
- Use ClearAllBlueprintLogic for simpler widgets

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/migrate_settings_widgets_v2.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

# Widgets with simple logic - can use full clear
SIMPLE_WIDGETS = {
    "/Game/SoulslikeFramework/Widgets/_Generic/W_Navigable": "/Script/SLFConversion.W_Navigable",
    "/Game/SoulslikeFramework/Widgets/_Generic/W_Navigable_InputReader": "/Script/SLFConversion.W_Navigable_InputReader",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CenteredText": "/Script/SLFConversion.W_Settings_CenteredText",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry": "/Script/SLFConversion.W_Settings_CategoryEntry",
}

# Widgets with widget bindings - only clear EventGraph, keep function graphs
COMPLEX_WIDGETS = {
    "/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton": "/Script/SLFConversion.W_GenericButton",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry": "/Script/SLFConversion.W_Settings_Entry",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings": "/Script/SLFConversion.W_Settings",
}


def migrate_simple_widget(bp_path, cpp_path):
    """Migrate using full clear (no widget bindings)."""
    unreal.log_warning(f"")
    unreal.log_warning(f"=== Migrating {bp_path.split('/')[-1]} (simple) ===")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  SKIP: Could not load")
        return False

    # Full clear
    unreal.log_warning(f"  Clearing ALL logic...")
    unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
    unreal.EditorAssetLibrary.save_asset(bp_path)

    bp = unreal.load_asset(bp_path)
    if not bp:
        return False

    # Reparent
    unreal.log_warning(f"  Reparenting to C++...")
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
    if not ok:
        unreal.log_warning(f"  FAIL: Reparent failed")
        return False

    # Compile
    result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if result:
        unreal.log_warning(f"  SUCCESS")
        return True
    else:
        unreal.log_warning(f"  FAIL: Compile failed")
        return False


def migrate_complex_widget(bp_path, cpp_path):
    """Migrate using EventGraph-only clear (preserves widget binding functions)."""
    unreal.log_warning(f"")
    unreal.log_warning(f"=== Migrating {bp_path.split('/')[-1]} (complex - EventGraph only) ===")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"  SKIP: Could not load")
        return False

    # Clear only EventGraphs, keep function graphs (widget bindings point to functions)
    unreal.log_warning(f"  Clearing EventGraphs only...")
    try:
        count = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
        unreal.log_warning(f"    Cleared {count} event graphs")
    except Exception as e:
        unreal.log_warning(f"    Warning: {e}")

    unreal.EditorAssetLibrary.save_asset(bp_path)

    bp = unreal.load_asset(bp_path)
    if not bp:
        return False

    # Reparent
    unreal.log_warning(f"  Reparenting to C++...")
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
    if not ok:
        unreal.log_warning(f"  FAIL: Reparent failed")
        return False

    # Compile
    result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    if result:
        unreal.log_warning(f"  SUCCESS")
        return True
    else:
        unreal.log_warning(f"  FAIL: Compile failed")
        return False


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Migrating Settings Widgets to C++ (v2 - Smart Clearing)")
    unreal.log_warning("=" * 70)

    success_count = 0
    fail_count = 0

    # Simple widgets first
    for bp_path, cpp_path in SIMPLE_WIDGETS.items():
        if migrate_simple_widget(bp_path, cpp_path):
            success_count += 1
        else:
            fail_count += 1

    # Complex widgets (with widget bindings)
    for bp_path, cpp_path in COMPLEX_WIDGETS.items():
        if migrate_complex_widget(bp_path, cpp_path):
            success_count += 1
        else:
            fail_count += 1

    unreal.log_warning("")
    unreal.log_warning("=" * 70)
    unreal.log_warning(f"Results: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
