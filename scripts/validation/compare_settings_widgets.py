"""
Comprehensive comparison of W_Settings and W_Settings_CategoryEntry between bp_only and SLFConversion.
Run this on bp_only FIRST to export baseline, then on SLFConversion to compare.
"""
import unreal
import json
import os

# Determine which project we're in
project_name = unreal.Paths.get_project_file_path()
is_bp_only = "bp_only" in project_name.lower()
project_label = "bp_only" if is_bp_only else "SLFConversion"

output_dir = "C:/scripts/SLFConversion/migration_cache/settings_comparison"
os.makedirs(output_dir, exist_ok=True)

def safe_str(val):
    """Safely convert value to string."""
    if val is None:
        return "None"
    try:
        if hasattr(val, 'get_path_name'):
            return val.get_path_name()
        return str(val)
    except:
        return repr(val)

def get_class_hierarchy(cls):
    """Get class hierarchy as list of class names."""
    hierarchy = []
    current = cls
    while current:
        try:
            hierarchy.append(current.get_name())
            current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        except:
            break
    return hierarchy

def analyze_category_entry_blueprint(bp_path):
    """Analyze W_Settings_CategoryEntry blueprint."""
    result = {"path": bp_path}

    bp = unreal.load_asset(bp_path)
    if not bp:
        result["error"] = "Could not load blueprint"
        return result

    result["blueprint_class"] = bp.get_class().get_name()

    gen_class = bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class"
        return result

    result["generated_class"] = gen_class.get_name()

    # Try to get parent class
    try:
        parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
        result["parent_class"] = parent.get_name() if parent else "Unknown"
    except:
        result["parent_class"] = "Could not determine"

    # Get CDO
    try:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            result["cdo_class"] = cdo.get_class().get_name()

            # Get all UPROPERTY values
            cdo_props = {}

            # Common widget properties
            for prop_name in ['icon', 'selected', 'selected_color', 'deselected_color',
                              'setting_category', 'switcher_index']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    cdo_props[prop_name] = safe_str(val)
                except Exception as e:
                    cdo_props[prop_name] = f"Error: {e}"

            result["cdo_properties"] = cdo_props
    except Exception as e:
        result["cdo_error"] = str(e)

    return result

def create_runtime_widget_and_analyze():
    """Create widget at runtime and analyze its actual state."""
    result = {}

    # Load W_Settings
    settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
    settings_bp = unreal.load_asset(settings_path)

    if not settings_bp:
        result["error"] = "Could not load W_Settings"
        return result

    gen_class = settings_bp.generated_class()
    if not gen_class:
        result["error"] = "No generated class for W_Settings"
        return result

    result["w_settings_class"] = gen_class.get_name()

    # Try to get parent class
    try:
        parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
        result["w_settings_parent"] = parent.get_name() if parent else "Unknown"
    except:
        result["w_settings_parent"] = "Could not determine"

    # Try to create an instance
    try:
        # Get the game world
        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            result["world_error"] = "No editor world"
            # Try without world
            try:
                widget = unreal.WidgetBlueprintLibrary.create(None, gen_class)
            except:
                widget = None
        else:
            widget = unreal.WidgetBlueprintLibrary.create(world, gen_class)

        if not widget:
            result["error"] = "Could not create widget instance"
            return result

        result["widget_created"] = True
        result["widget_class"] = widget.get_class().get_name()

        # Find category entries
        entry_names = [
            "W_Settings_CategoryEntry_Display",
            "W_Settings_CategoryEntry_Camera",
            "W_Settings_CategoryEntry_Gameplay",
            "W_Settings_CategoryEntry_Keybinds",
            "W_Settings_CategoryEntry_Sounds",
            "W_Settings_CategoryEntry_Quit"
        ]

        entries_analysis = {}
        for entry_name in entry_names:
            try:
                entry = widget.get_widget_from_name(entry_name)
                if entry:
                    entry_data = {
                        "found": True,
                        "class": entry.get_class().get_name(),
                    }

                    # Get visibility
                    try:
                        entry_data["visibility"] = str(entry.get_visibility())
                    except:
                        pass

                    # Get render opacity
                    try:
                        entry_data["render_opacity"] = entry.get_render_opacity()
                    except:
                        pass

                    # Get Icon property
                    try:
                        icon = entry.get_editor_property('icon')
                        if icon:
                            if hasattr(icon, 'is_null') and icon.is_null():
                                entry_data["icon_property"] = "Null (is_null=True)"
                            elif hasattr(icon, 'get_path_name'):
                                entry_data["icon_property"] = icon.get_path_name()
                            else:
                                entry_data["icon_property"] = str(icon)
                        else:
                            entry_data["icon_property"] = "None"
                    except Exception as e:
                        entry_data["icon_property_error"] = str(e)

                    # Get Selected property
                    try:
                        entry_data["selected"] = entry.get_editor_property('selected')
                    except:
                        pass

                    # Get SelectedColor property
                    try:
                        sc = entry.get_editor_property('selected_color')
                        entry_data["selected_color"] = str(sc)
                    except:
                        pass

                    # Get DeselectedColor property
                    try:
                        dc = entry.get_editor_property('deselected_color')
                        entry_data["deselected_color"] = str(dc)
                    except:
                        pass

                    # Get CategoryIcon child widget
                    try:
                        cat_icon = entry.get_widget_from_name("CategoryIcon")
                        if cat_icon:
                            cat_icon_data = {
                                "found": True,
                                "class": cat_icon.get_class().get_name(),
                            }

                            # Get visibility
                            try:
                                cat_icon_data["visibility"] = str(cat_icon.get_visibility())
                            except:
                                pass

                            # Get render opacity
                            try:
                                cat_icon_data["render_opacity"] = cat_icon.get_render_opacity()
                            except:
                                pass

                            # Get color and opacity
                            try:
                                cat_icon_data["color_and_opacity"] = str(cat_icon.get_color_and_opacity())
                            except:
                                pass

                            # Get brush info
                            try:
                                brush = cat_icon.get_brush()
                                if brush:
                                    cat_icon_data["brush_image_size"] = str(brush.get_image_size())
                                    cat_icon_data["brush_tint_color"] = str(brush.tint_color)
                                    cat_icon_data["brush_draw_as"] = str(brush.draw_as)
                                    res = brush.get_resource_object()
                                    cat_icon_data["brush_resource"] = res.get_path_name() if res else "None"
                            except Exception as e:
                                cat_icon_data["brush_error"] = str(e)

                            entry_data["category_icon_widget"] = cat_icon_data
                        else:
                            entry_data["category_icon_widget"] = {"found": False}
                    except Exception as e:
                        entry_data["category_icon_widget_error"] = str(e)

                    # Get Button child widget
                    try:
                        button = entry.get_widget_from_name("Button")
                        if button:
                            button_data = {
                                "found": True,
                                "class": button.get_class().get_name(),
                            }
                            try:
                                button_data["visibility"] = str(button.get_visibility())
                            except:
                                pass
                            entry_data["button_widget"] = button_data
                        else:
                            entry_data["button_widget"] = {"found": False}
                    except Exception as e:
                        entry_data["button_widget_error"] = str(e)

                    # Get SelectedGlow child widget (Border)
                    try:
                        glow = entry.get_widget_from_name("SelectedGlow")
                        if glow:
                            glow_data = {
                                "found": True,
                                "class": glow.get_class().get_name(),
                            }
                            try:
                                glow_data["visibility"] = str(glow.get_visibility())
                            except:
                                pass
                            try:
                                glow_data["brush_color"] = str(glow.get_brush_color())
                            except:
                                pass
                            entry_data["selected_glow_widget"] = glow_data
                        else:
                            entry_data["selected_glow_widget"] = {"found": False}
                    except Exception as e:
                        entry_data["selected_glow_widget_error"] = str(e)

                    entries_analysis[entry_name] = entry_data
                else:
                    entries_analysis[entry_name] = {"found": False}
            except Exception as e:
                entries_analysis[entry_name] = {"error": str(e)}

        result["category_entries_runtime"] = entries_analysis

        # Clean up
        try:
            widget.remove_from_parent()
        except:
            pass

    except Exception as e:
        result["creation_error"] = str(e)
        import traceback
        result["traceback"] = traceback.format_exc()

    return result

# Main analysis
print(f"\n{'='*60}")
print(f"Analyzing Settings Widgets in {project_label}")
print(f"{'='*60}\n")

full_results = {
    "project": project_label,
    "w_settings_categoryentry": analyze_category_entry_blueprint(
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
    ),
    "runtime_analysis": create_runtime_widget_and_analyze()
}

# Save results
output_file = os.path.join(output_dir, f"settings_analysis_{project_label}.json")
with open(output_file, "w") as f:
    json.dump(full_results, f, indent=2)

print(f"\nResults saved to: {output_file}")
print(json.dumps(full_results, indent=2))
