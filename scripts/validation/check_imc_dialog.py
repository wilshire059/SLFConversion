#!/usr/bin/env python
"""Check what input actions are mapped in IMC_Dialog."""
import unreal

def check_imc_dialog():
    """Dump IMC_Dialog mappings."""
    imc_path = "/Game/SoulslikeFramework/Input/IMC_Dialog.IMC_Dialog"

    imc = unreal.load_object(None, imc_path)
    if not imc:
        unreal.log_error(f"ERROR: Could not load {imc_path}")
        return

    unreal.log("=" * 60)
    unreal.log("IMC_Dialog Input Mappings")
    unreal.log("=" * 60)

    # Try new API first
    try:
        default_mappings = imc.get_editor_property("default_key_mappings")
        if hasattr(default_mappings, 'mappings'):
            mappings = default_mappings.mappings
        else:
            mappings = default_mappings
        unreal.log(f"Found {len(mappings)} mappings:")
        for i, mapping in enumerate(mappings):
            action = mapping.get_editor_property("action")
            key = mapping.get_editor_property("key")
            action_name = action.get_name() if action else "None"
            key_name = str(key) if key else "None"
            unreal.log(f"  [{i}] {action_name} -> {key_name}")
    except Exception as e:
        unreal.log(f"Error with new API: {e}")
        # Fall back to deprecated API
        try:
            mappings = imc.get_editor_property("mappings")
            unreal.log(f"Found {len(mappings)} mappings (deprecated API):")
            for i, mapping in enumerate(mappings):
                action = mapping.get_editor_property("action")
                key = mapping.get_editor_property("key")
                action_name = action.get_name() if action else "None"
                key_name = str(key) if key else "None"
                unreal.log(f"  [{i}] {action_name} -> {key_name}")
        except Exception as e2:
            unreal.log_error(f"Error reading mappings: {e2}")

    unreal.log("=" * 60)

if __name__ == "__main__":
    check_imc_dialog()
