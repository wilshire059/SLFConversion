"""Test if SLFAutomationLibrary functions are available."""
import unreal

# Check what's available
automation = unreal.SLFAutomationLibrary
unreal.log_warning(f"SLFAutomationLibrary type: {type(automation)}")

# List available methods
methods = [m for m in dir(automation) if not m.startswith('_')]
unreal.log_warning(f"Available methods: {len(methods)}")

# Check for our specific function
if hasattr(automation, 'migrate_all_dialog_data_tables'):
    unreal.log_warning("[OK] migrate_all_dialog_data_tables EXISTS")
    result = automation.migrate_all_dialog_data_tables()
    unreal.log_warning(f"Result: {result[:500] if result else 'None'}...")
else:
    unreal.log_error("[ERROR] migrate_all_dialog_data_tables NOT FOUND")
    # Show what IS available
    for m in methods:
        if 'dialog' in m.lower() or 'data' in m.lower() or 'table' in m.lower():
            unreal.log_warning(f"  Found related: {m}")
