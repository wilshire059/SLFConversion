# check_bt_services.py
# Check what services are attached to BT_Combat

import unreal

def main():
    paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
    ]

    for bt_path in paths:
        print(f"\n=== {bt_path.split('/')[-1]} ===")
        bt = unreal.load_asset(bt_path)

        if not bt:
            print(f"  ERROR: Could not load")
            continue

        print(f"  Class: {bt.get_class().get_name()}")

        # Try to get export text
        export = unreal.ExporterText.run_asset_export_task(
            unreal.AssetExportTask(
                object=bt,
                filename="",
                exporter=None,
                automated=True,
                prompt=False,
                write_empty_files=True
            )
        )

        # Try export_text from asset library
        try:
            export_text = unreal.SystemLibrary.export_text(bt)
            if export_text:
                # Look for service references
                for line in export_text.split('\n'):
                    if 'Service' in line or 'BTS_' in line:
                        print(f"  {line.strip()[:100]}")
        except:
            pass

        # List services using reflection
        if hasattr(bt, 'get_editor_property'):
            try:
                root_node = bt.get_editor_property('root_node')
                print(f"  RootNode: {root_node}")
            except:
                pass

if __name__ == "__main__":
    main()
