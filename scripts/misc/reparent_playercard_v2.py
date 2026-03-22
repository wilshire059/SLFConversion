import unreal

# Reparent W_Settings_PlayerCard and W_Settings_QuitConfirmation to their C++ classes
# Using direct parent_class property set (mirrors SLFAutomationLibrary::ReparentBlueprint)

REPARENT_MAP = {
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_PlayerCard": "/Script/SLFConversion.W_Settings_PlayerCard",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_QuitConfirmation": "/Script/SLFConversion.W_Settings_QuitConfirmation",
}

for bp_path, cpp_class_path in REPARENT_MAP.items():
    name = bp_path.split("/")[-1]

    # Load Blueprint asset
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning("REPARENT: Could not load Blueprint: {}".format(bp_path))
        continue

    # Load C++ parent class
    new_parent = unreal.load_class(None, cpp_class_path)
    if not new_parent:
        unreal.log_warning("REPARENT: Could not load class: {}".format(cpp_class_path))
        continue

    # Check current parent
    try:
        current_parent = bp.get_editor_property("parent_class")
        unreal.log_warning("REPARENT: {} current parent: {}".format(name, current_parent.get_name() if current_parent else "None"))
    except:
        unreal.log_warning("REPARENT: {} could not get current parent".format(name))

    # Direct set parent_class (mirrors C++ Blueprint->ParentClass = NewParentClass)
    try:
        bp.set_editor_property("parent_class", new_parent)
        unreal.log_warning("REPARENT: {} set parent_class to: {}".format(name, new_parent.get_name()))
    except Exception as e:
        unreal.log_warning("REPARENT: {} set_editor_property failed: {}".format(name, str(e)))
        # Fallback: try BlueprintEditorLibrary
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent)
            unreal.log_warning("REPARENT: {} BlueprintEditorLibrary.reparent_blueprint returned: {}".format(name, result))
        except Exception as e2:
            unreal.log_warning("REPARENT: {} fallback also failed: {}".format(name, str(e2)))
            continue

    # Mark modified
    bp.modify()

    # Compile
    try:
        unreal.KismetSystemLibrary.flush_persistent_debug_lines(None)
    except:
        pass

    try:
        result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("REPARENT: {} compile result: {}".format(name, result))
    except Exception as e:
        unreal.log_warning("REPARENT: {} compile failed (may be ok): {}".format(name, str(e)))

    # Save
    saved = unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning("REPARENT: {} saved: {}".format(name, saved))

    # Verify
    bp2 = unreal.load_asset(bp_path)
    if bp2:
        gen_class = bp2.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            unreal.log_warning("REPARENT: {} VERIFY CDO type: {}".format(name, type(cdo).__name__))
        try:
            parent = bp2.get_editor_property("parent_class")
            unreal.log_warning("REPARENT: {} VERIFY parent_class: {}".format(name, parent.get_name() if parent else "None"))
        except:
            pass

unreal.log_warning("REPARENT: Done!")
