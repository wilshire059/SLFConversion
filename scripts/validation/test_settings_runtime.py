import unreal

# Create and show the W_Settings widget to trigger icon loading
# This simulates what happens when the Settings menu opens

settings_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"
settings_bp = unreal.load_asset(settings_path)

print(f"Loaded W_Settings Blueprint: {settings_bp}")

if settings_bp:
    gen_class = settings_bp.generated_class()
    print(f"Generated class: {gen_class}")

    if gen_class:
        # Create an instance of the widget
        try:
            widget = unreal.WidgetBlueprintLibrary.create(gen_class, None)
            print(f"Created widget instance: {widget}")

            if widget:
                # Try to find the category entry widgets
                for entry_name in ['W_Settings_CategoryEntry_Display', 'W_Settings_CategoryEntry_Camera',
                                   'W_Settings_CategoryEntry_Gameplay', 'W_Settings_CategoryEntry_Keybinds',
                                   'W_Settings_CategoryEntry_Sounds', 'W_Settings_CategoryEntry_Quit']:
                    # Use get_widget_from_name
                    try:
                        entry = widget.get_widget_from_name(entry_name)
                        if entry:
                            entry_type = entry.get_class().get_name()
                            print(f"\n{entry_name}:")
                            print(f"  Found: {entry} ({entry_type})")

                            # Try to get the Icon property
                            try:
                                icon = entry.get_editor_property('icon')
                                if icon and not icon.is_null():
                                    print(f"  Icon property: {icon.get_path_name()}")
                                else:
                                    print(f"  Icon property: NULL or empty")
                            except Exception as e:
                                print(f"  Icon property error: {e}")

                            # Check CategoryIcon image widget
                            try:
                                cat_icon = entry.get_widget_from_name("CategoryIcon")
                                if cat_icon:
                                    print(f"  CategoryIcon widget found: {cat_icon}")
                                    # Try to get brush info
                                    try:
                                        brush = cat_icon.get_brush()
                                        if brush:
                                            res = brush.get_resource_object()
                                            if res:
                                                print(f"  CategoryIcon brush resource: {res.get_path_name()}")
                                            else:
                                                print(f"  CategoryIcon brush resource: NONE (no texture set)")
                                    except Exception as e:
                                        print(f"  Brush error: {e}")
                                else:
                                    print(f"  CategoryIcon widget: NOT FOUND")
                            except Exception as e:
                                print(f"  CategoryIcon error: {e}")
                        else:
                            print(f"\n{entry_name}: NOT FOUND in widget tree")
                    except Exception as e:
                        print(f"\n{entry_name}: Error - {e}")

        except Exception as e:
            print(f"Failed to create widget: {e}")
